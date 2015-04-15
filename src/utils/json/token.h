/* token.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json token
*/

#pragma once

#include <string>

namespace reflect {
namespace json {

/******************************************************************************/
/* TOKEN                                                                      */
/******************************************************************************/

struct Token
{
    enum Type
    {
        NoToken,
        ObjectStart, ObjectEnd, // {}
        ArrayStart, ArrayEnd,   // []
        Separator,              // ,
        KeySeparator,           // :
        String, Number, Bool, Null,
        EOS
    };

    Token() : type_(NoToken) {}
    Token(Type type) : type_(type) {}
    Token(Type type, bool value);
    Token(Type type, std::string& value);

    Type type() const { return type_; }

    bool asBool() const;
    long asInt() const;
    double asFloat() const;
    std::string& asString() const { return value_; }

    std::string print() const;

private:
    Type type_;
    std::string* value_;
};

std::string print(Token::Type type);


namespace details {

Token nextToken(Reader& reader);

} // namespace details
} // namespace json
} // namespace reflect
