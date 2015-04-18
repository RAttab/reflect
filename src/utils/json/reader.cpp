/* reader.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
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
    if (token.type() == Token::NoToken)
        token = details::nextToken(*this);

    return token;
}

Token
Reader::
nextToken()
{
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
    Token token = nextToken();
    return assertToken(token, exp) ? token : Token(Token::EOS);
}

bool
Reader::
assertToken(const Token& token, Token::Type exp)
{
    if (token.type() == exp) return true;

    error("unexpected token <%s>, expecting <%s>", token.print(), print(exp));
    return false;
}

} // namespace json
} // namespace reflect


reflectTypeImpl(reflect::json::Reader)
{
    reflectPlumbing();
}
