/* token.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json token
*/

#pragma once

namespace reflect {

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

    Type type() const { return Type; }

    std::string stringValue() const;

    double floatValue() const;
    long intValue() const;

    bool boolValue() const;

private:
    Type type_;
    std::string value_;
};

std::string toString(Token::Type type);


/******************************************************************************/
/* TOKENIZER                                                                  */
/******************************************************************************/

Token nextToken(std::istream& json);


} // reflect
