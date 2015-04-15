/* writer.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   JSON Writer interface.
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {


/******************************************************************************/
/* WRITER                                                                     */
/******************************************************************************/


struct Writer
{
    enum Options
    {
        Pretty        = 1 << 0,
        EscapeUnicode = 1 << 1,

        Default = EscapeUnicode,
    };

    Writer(std::ostream& stream, Options options = Default) :
        stream(stream), indent_(0), options(options)
    {
        buffer_.reserve(128);
    }
    
    bool operator() const { return error_ && stream; }

    template<typename... Args>
    void error(const char* fmt, Args&&... args);
    const Error& error() const { return error_; }

    void push(char c) { stream.put(c); }
    void push(const char* c, size_t n) { stream.write(c, n); }
    void push(const std::string& c) { stream.write(c.c_str(), c.size()); }

    std::string& buffer() { return buffer_; }

    bool pretty() const { return options & Pretty; }
    bool escapeUnicode() const { return options & EncodeUnicode; }

    void indent() { indent_++; }
    void unindent() { indent_--; }
    void newline();
    void seperator(char c);

private:
    std::ostream& stream;
    std::string buffer_;
    Error error_;

    size_t indent_;

    Options options;
};

} // namespace json
} // namespace reflect
