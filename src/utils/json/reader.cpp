/* reader.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   JSON Reader implementation
*/

#include "json.h"

namespace reflect {
namespace json {

/******************************************************************************/
/* READER                                                                     */
/******************************************************************************/

Token
Reader::
peekToken()
{
    if (!ok()) return Token(Token::EOS);

    if (token.type() == Token::NoToken)
        token = details::nextToken(*this);

    return token;
}

Token
Reader::
nextToken()
{
    if (!ok()) return Token(Token::EOS);

    if (token.type() == Token::NoToken)
        return details::nextToken(*this);

    Token result;
    std::swap(token, result);
    return result;
}

Token
Reader::
expectToken(Token::Type exp)
{
    if (!ok()) return {};

    Token token = nextToken();
    assertToken(token, exp);
    return ok() ? token : Token(Token::EOS);
}

void
Reader::
assertToken(const Token& token, Token::Type exp)
{
    if (!ok()) return;
    if (token.type() == exp) return;
    error("unexpected token <%s>, expecting <%s>", token.print(), print(exp));
}

} // namespace json
} // namespace reflect


reflectTypeImpl(reflect::json::Reader)
{
    reflectPlumbing();
}
