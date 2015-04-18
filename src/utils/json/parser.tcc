/* parser.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
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
    return reader.expectToken(Token::Int).asInt();
}

double parseFloat(Reader& reader)
{
    Token token = reader.nextToken();

    if (token.type() == Token::Int);
    else reader.assertToken(token, Token::Float);

    return token.asFloat();
}

std::string parseString(Reader& reader)
{
    return reader.expectToken(Token::String).asString();
}

template<typename Fn>
void parseObject(Reader& reader, const Fn& fn)
{
    Token token = reader.nextToken();
    if (token.type() == Token::Null) return;
    reader.assertToken(token, Token::ObjectStart);

    token = reader.peekToken();
    if (token.type() == Token::ObjectEnd) {
        reader.expectToken(Token::ObjectEnd);
        return;
    }

    while (reader) {
        token = reader.expectToken(Token::String);
        const std::string& key = token.asString();
        token = reader.expectToken(Token::KeySeparator);

        fn(key);

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

    token = reader.peekToken();
    if (token.type() == Token::ArrayEnd) {
        reader.expectToken(Token::ArrayEnd);
        return;
    }

    for (size_t i = 0; reader; ++i) {
        fn(i);

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
    Value v = cast<Value>(value);
    parse(reader, v);
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
