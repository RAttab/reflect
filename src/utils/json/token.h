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
    enum Type {
        ObjectStart, ObjectEnd, // {}
        ArrayStart, ArrayEnd,   // []
        Separator,              // ,
        KeySeparator,           // :
        String, Number, Bool, Null,
        EOS
    };

    Token(Type type, bool value);
    Token(Type type, std::string value = "");

    Type type() const { return type_; }

    std::string stringValue() const;

    double floatValue() const;
    long intValue() const;

    bool boolValue() const;

    std::string print() const;

private:
    Type type_;
    std::string value_;
};

std::string print(Token::Type type);


/******************************************************************************/
/* TOKENIZER                                                                  */
/******************************************************************************/

Token nextToken(std::istream& json);
void expectToken(Token token, Token::Type expected);


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
