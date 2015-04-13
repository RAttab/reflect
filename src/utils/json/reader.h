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
        AllowComments = 1 << 0,
        DecodeUnicode = 1 << 1,

        Default = DecodeUnicode,
    };

    Reader(std::istream& stream, Options options = Default) :
        stream(stream),
        buffer_(128),
        pos_(0), line_(0),
        options(options)
    {}

    bool operator() const { return error_ && stream; }

    template<typename... Args>
    void error(const char* fmt, Args&&... args);
    const Error& error() const { return error_; }

    char peek() { return stream.peek(); }
    char pop() { return stream.pop(); pos_++; }

    void save(char c) { buffer_.push_back(c); }
    std::string& buffer() { save('\0');  return buffer_; }

    size_t pos() const { return pos_; }
    size_t line() const { return line_; }
    void newline() { pos_ = 0; line_++; }

    bool allowComments() const { return options & AllowComments; }
    bool decodeUnicode() const { return options & DecodeUnicode; }

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
