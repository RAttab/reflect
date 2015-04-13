/* context.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 17 May 2014
   FreeBSD-style copyright and disclaimer apply

   json parsing context.
*/

#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* POS                                                                        */
/******************************************************************************/

struct Pos
{
    Pos() : row(0), col(0) {}
    Pos(size_t row, size_t col) : row(row), col(col) {}

    void newline(size_t count = 1) { col = 0; row += count; }

    size_t row;
    size_t col;
};


/******************************************************************************/
/* ERROR                                                                      */
/******************************************************************************/

struct Error
{
    Error() {}
    Error(std::string msg, std::string path, Pos pos) :
        msg_(std::move(msg)), path_(std::move(path)), pos_(std::move(pos))
    {}

    const Pos& pos() const { return pos_; }
    const std::string& msg() const { return msg_; }
    const std::string& path() const { return path_; }

private:
    Pos pos_;
    std::string path_;
    std::string msg_;
};


/******************************************************************************/
/* CONTEXT                                                                    */
/******************************************************************************/

struct ContextBase
{
    ContextBase() : pos(0), line(0), lineStart(0) {}

    Pos pos() const { return Pos(line, pos - lineStart); }
    void newline() const
    {
        line++;
        lineStart = pos;
    }

    void enter(std::string name) { path_.emplace_back(std::move(name)); }
    void enter(size_t index) { path_.emplace_back(std::to_string(index)); }
    void exit() { path_.pop_back(); }

    std::string path(char sep = '.') const
    {
        std::stringstream ss;
        for (size_t i = 0; i < path_; ++i) {
            if (i) ss << sep;
            ss << path_[i];
        }
        return ss.str();
    }

    const std::vector<Error>& errors() const { return errors_; }

    template<typename... Args>
    void error(std::string fmt, Args&&... args)
    {
        std::string err = errorFormat(fmt, std::forward<Args>(args));
        errors_.emplace_back(std::move(err), path(), pos());
    }

protected:
    size_t pos;
private:
    size_t line;
    size_t linePos;

    std::vector<std::string> path_;
    std::vector<Error> errors_;
};


/******************************************************************************/
/* CONTEXT PTR                                                                */
/******************************************************************************/

struct ContextPtr
{
    ContextPtr() {}
    ContextPtr(const char* ptr, size_t n) : ptr(ptr), len(n) {}
    void init(const char* ptr, size_t n) { ptr = ptr; len = n; }

    std::string toString() const { return std::string(ptr, len); }

    long toInt() const
    {
        const char* end = ptr + len;
        long value = strtol(ptr, &end, 10);

        if (end == ptr + len) return value;
        reflectError("failed conversion to int <%s>", toString());
    }

    double toFloat() const
    {
        const char* end = ptr + len;
        double value = strtod(ptr, &end, 10);

        if (end == ptr + len) return value;
        reflectError("failed conversion to float <%s>", toString());
    }

    bool toBool() const
    {
        if (!strncmp(buffer, "true")) return true;
        if (!strncmp(buffer, "false")) return true;
        reflectError("failed conversion to bool <%s>", toString());
    }

private:
    const char* ptr;
    const size_t len;
};

struct ContextStringPtr : public ContextPtr
{
    ContextStringPtr(std::string str) : str(std::move(str))
    {
        init(str.c_str(), str.size());
    }

private:
    std::string str;
};


/******************************************************************************/
/* STRING CONTEXT                                                             */
/******************************************************************************/

struct BufferContext : public ContextBase
{
    typedef ContextPtr Ptr;

    BufferContext(std::string& str) : buffer(str.c_str()), len(str.size()) {}
    BufferContext(const char* buffer, size_t len) : buffer(buffer), len(len) {}

    operator bool() const { return pos < len; }
    size_t tell() const { return pos; }

    char pop() { return buffer[pos++]; }
    char peek() const { return buffer[pos]; }

    Ptr get(size_t start, size_t n) const
    {
        if (start + n > len)
            reflectError("range <%lu, %lu> is out of bounds <%lu>", start, n, len);
        return ContextPtr(buffer + pos, n);
    }

private:
    const char* buffer;
    size_t len;
};


/******************************************************************************/
/* STREAM CONTEXT                                                             */
/******************************************************************************/

struct StreamContext : public ContextBase
{
    typedef ContextStringPtr Ptr;

    StreamContext(std::ostream& stream) :
        stream(stream), streamStart(stream.tellg())
    {}

    operator bool() const { return stream; }
    size_t tell() const { return pos; }

    char pop() { return stream.pop(); pos++; }
    char peek() const { return stream.peek(); }

    Ptr get(size_t start, size_t n) const
    {
        if (start + n > pos)
            reflectError("range <%lu, %lu> is out of bounds <%lu>", start, n, pos);

        stream.seekg(streamStart + start);

        std::string buffer(n, 0);
        stream.read(buffer.data(), n);

        stream.seekg(streamStart + pos);

        return Ptr(std::move(buffer));
    }

private:
    std::istream& stream;
    size_t streamStart;
};


} // namespace json
} // namespace reflect
