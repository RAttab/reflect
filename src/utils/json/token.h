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
        ObjectStart, ObjectEnd, // {}
        ArrayStart, ArrayEnd,   // []
        Separator,              // ,
        KeySeparator,           // :
        String, Number, Bool, Null,
        EOS
    };

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


/******************************************************************************/
/* TOKENIZER                                                                  */
/******************************************************************************/

Token nextToken(Reader& reader);
Token expectToken(Reader& reader, Token::Type expected);
void assertToken(Reader& reader, Token token, Token::Type expected);


/******************************************************************************/
/* PRINTERS                                                                   */
/******************************************************************************/

void printNull(std::ostream& json);
void printBool(bool value, std::ostream& json);
void printInteger(long value, std::ostream& json);
void printFloat(double value, std::ostream& json);
void printString(const std::string& value, std::ostream& json);


} // namespace json
} // reflect
