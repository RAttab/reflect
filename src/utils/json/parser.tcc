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

template<typename T>
void parseBool(Reader& reader, T& value)
{
    value = expectToken(reader, Token::Bool).asBool();
}

template<typename T>
void parseInt(Reader& reader, T& value)
{
    value = expectToken(reader, Token::Number).asInt();
}

template<typename T>
void parseFloat(Reader& reader, T& value)
{
    value = expectToken(reader, Token::Number).asFloat();
}

template<typename T>
void parseString(Reader& reader, T& value)
{
    value = expectToken(reader, Token::String).asString();
}

template<typename Fn>
void parseObject(Reader& reader, const Fn& fn)
{
    expectToken(reader, Token::ObjectStart);

    Token token = nextToken(reader);
    if (token.type() == Token::ObjectEnd) return;

    while (reader) {
        assertToken(reader, token, Token::String);
        expectToken(reader, Token::KeySeparator);

        fn(reader, token.asString());

        token = nextToken(reader);
        if (token.type() == Token::ObjectEnd) return;
        assertToken(reader, token, Token::Separator);
    }
}

template<typename Fn>
void parseArray(Reader& reader, const Fn& fn)
{
    expectToken(reader, Token::ArrayStart);

    Token token = nextToken(reader);
    if (token.type() == Token::ArrayEnd) return;

    for (size_t i = 0; reader; ++i) {
        fn(reader, i);

        token = nextToken(reader);
        if (token.type() == Token::ArrayEnd) return;
        assertToken(reader, token, Token::Separator);
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
