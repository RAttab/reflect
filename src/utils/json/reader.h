/* reader.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   JSON buffered reader.
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {


/******************************************************************************/
/* READER                                                                     */
/******************************************************************************/

struct Reader
{
    enum Options
    {
        AllowComments   = 1 << 0,
        UnescapeUnicode = 1 << 1,
        ValidateUnicode = 1 << 2,

        Default = DecodeUnicode | ValidateUnicode,
    };

    Reader(std::istream& stream, Options options = Default) :
        stream(stream),
        pos_(0), line_(0),
        options(options)
    {
        buffer_.reserve(128);
    }

    bool operator() const { return error_ && stream; }

    template<typename... Args>
    void error(const char* fmt, Args&&... args);
    const Error& error() const { return error_; }

    char peek() { return stream.peek(); }
    char pop() { pos_++; return stream.pop(); }

    void save(char c) { buffer_.push_back(c); }
    const std::string& buffer() { return buffer_; }
    void resetBuffer() const { buffer_.reset(); }

    size_t pos() const { return pos_; }
    size_t line() const { return line_; }
    void newline() { pos_ = 0; line_++; }

    bool allowComments() const { return options & AllowComments; }
    bool unescapeUnicode() const { return options & DecodeUnicode; }
    bool validateUnicode() const { return options & ValidateUnicode; }

private:
    std::istream& stream;
    std::string buffer_;
    Error error_;

    size_t pos_;
    size_t line_;

    Options options;
};

} // namespace json
} // namespace reflect
