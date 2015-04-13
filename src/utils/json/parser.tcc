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
/* PARSER FN                                                                  */
/******************************************************************************/

template<typename Fn>
void object(Reader& reader, const Fn& fn)
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
void array(Reader& reader, const Fn& fn)
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


} // namespace json
} // namespace reflect
