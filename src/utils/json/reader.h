/* reader.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
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

        Default = UnescapeUnicode | ValidateUnicode,
    };

    Reader(std::istream& stream, Options options = Default) :
        stream(stream),
        pos_(1), line_(1),
        options(options)
    {
        buffer_.reserve(128);
    }

    bool ok() const { return !error_ && stream; }
    operator bool() const { return ok(); }

    template<typename... Args>
    void error(const char* fmt, Args&&... args);
    const Error& error() const { return error_; }

    char peek() { return stream.peek(); }
    char pop() { pos_++; return stream.get(); }

    Token peekToken();
    Token nextToken();
    Token expectToken(Token::Type exp);
    bool assertToken(const Token& token, Token::Type exp);

    void save(char c) { buffer_.push_back(c); }
    const std::string& buffer() { return buffer_; }
    void resetBuffer() { buffer_.clear(); }

    size_t pos() const { return pos_; }
    size_t line() const { return line_; }
    void newline() { pos_ = 1; line_++; }

    bool allowComments() const { return options & AllowComments; }
    bool unescapeUnicode() const { return options & UnescapeUnicode; }
    bool validateUnicode() const { return options & ValidateUnicode; }

private:
    std::istream& stream;
    std::string buffer_;
    Error error_;

    size_t pos_;
    size_t line_;

    Options options;

    Token token;
};

} // namespace json
} // namespace reflect

reflectTypeDecl(reflect::json::Reader)
