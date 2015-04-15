/* parser.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Template implementation of the JSON parser.
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {


/******************************************************************************/
/* GENERIC PARSER                                                             */
/******************************************************************************/

bool parseBool(Reader& reader)
{
    return reader.expectToken(Token::Bool).asBool();
}

int64_t parseInt(Reader& reader)
{
    return reader.expectToken(Token::Number).asInt();
}

double parseFloat(Reader& reader)
{
    return reader.expectToken(Token::Number).asFloat();
}

std::string parseString(Reader& reader)
{
    return reader.expectToken(Token::String).asString();
}

template<typename Fn>
void parseObject(Reader& reader, const Fn& fn)
{
    Token token = reader.nextToken(reader);
    if (token.type() == Token::Null) return;
    reader.assertToken(token, Token::ObjectStart);

    token = reader.nextToken();
    if (token.type() == Token::ObjectEnd) return;

    while (reader) {
        reader.assertToken(token, Token::String);
        reader.expectToken(Token::KeySeparator);

        fn(reader, token.asString());

        token = reader.nextToken();
        if (token.type() == Token::ObjectEnd) return;
        reader.assertToken(token, Token::Separator);
    }
}

template<typename Fn>
void parseArray(Reader& reader, const Fn& fn)
{
    Token token = reader.nextToken();
    if (token.type() == Token::Null) return;
    reader.assertToken(token, Token::ArrayStart);

    Token token = reader.nextToken();
    if (token.type() == Token::ArrayEnd) return;

    for (size_t i = 0; reader; ++i) {
        fn(reader, i);

        token = reader.nextToken();
        if (token.type() == Token::ArrayEnd) return;
        reader.assertToken(token, Token::Separator);
    }
}


/******************************************************************************/
/* VALUE PARSER                                                               */
/******************************************************************************/

template<typename T> 
Error parse(Reader& reader, T& value)
{
    Value value = cast<Value>(value);
    parse(reader, value);
    return reader.error();
}

template<typename T> 
Error parse(std::istream& stream, T& value)
{
    Reader reader(stream);
    return parse(reader, value);
}

template<typename T> 
Error parse(const std::string& str, T& value)
{
    std::istringstream stream(str);
    return parse(stream, value);
}

} // namespace json
} // namespace reflect
