/* reader_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 16 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "utils/json.h"

#include <boost/test/unit_test.hpp>

using namespace reflect;
using namespace reflect::json;


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

void checkToken(
        const std::string& s, Token::Type type,
        Reader::Options options = Reader::Default)
{
    std::istringstream stream(s);
    Reader reader(stream, options);

    Token token = reader.expectToken(type);

    std::cerr << "<" << s << "> -> ";
    if (!reader.error()) std::cerr << token.print();
    else std::cerr << "<" << reader.error().what() << ">";
    std::cerr << std::endl;
}

void check(Token token, bool exp) { BOOST_CHECK_EQUAL(token.asBool(), exp); }
void check(Token token, int exp) { BOOST_CHECK_EQUAL(token.asInt(), exp); }
void check(Token token, double exp) { BOOST_CHECK_CLOSE(token.asFloat(), exp, 0.0000001); }
void check(Token token, const char* exp) { BOOST_CHECK_EQUAL(token.asString(), exp); }
void check(Token token, std::string exp) { BOOST_CHECK_EQUAL(token.asString(), exp); }

template<typename T>
void checkToken(
        const std::string& s, Token::Type type, T exp,
        Reader::Options options = Reader::Default)
{
    std::istringstream stream(s);
    Reader reader(stream, options);

    Token token = reader.expectToken(type);

    std::cerr << "<" << s << "> -> ";
    if (!reader.error()) std::cerr << token.print();
    else std::cerr << "<" << reader.error().what() << ">";
    std::cerr << std::endl;

    BOOST_CHECK(!reader.error());
    check(token, exp);
}

void errorToken(const std::string& s, Reader::Options options = Reader::Default)
{
    std::istringstream stream(s);
    Reader reader(stream, options);

    Token token = reader.nextToken();

    std::cerr << "<" << s << "> -> ";
    if (!reader.error()) std::cerr << token.print();
    else std::cerr << "expected<" << reader.error().what() << ">";
    std::cerr << std::endl;

    BOOST_CHECK(!!reader.error());
}


/******************************************************************************/
/* TOKENS                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_basics)
{
    checkToken("", Token::EOS);
    checkToken(" ", Token::EOS);
    checkToken("{", Token::ObjectStart);
    checkToken("}", Token::ObjectEnd);
    checkToken("[", Token::ArrayStart);
    checkToken("]", Token::ArrayEnd);
    checkToken(",", Token::Separator);
    checkToken(":", Token::KeySeparator);
    errorToken("<");

    checkToken("null", Token::Null);
    errorToken("nul");
    errorToken("nul l");

    checkToken("true", Token::Bool, true);
    checkToken("false", Token::Bool, false);

    checkToken("   ,", Token::Separator);
    checkToken(" \t  ,", Token::Separator);
    checkToken(" \n  ,", Token::Separator);

    checkToken(" // weeeeeeeee\n,", Token::Separator, Reader::AllowComments);
    errorToken(" // weeeeeeeee\n,");

}

BOOST_AUTO_TEST_CASE(test_number)
{
    checkToken("123", Token::Int, 123);
    checkToken("-123", Token::Int, -123);
    checkToken("000001", Token::Int, 1); // \todo Should fail

    checkToken("0.1", Token::Float, 0.1);
    checkToken("-0.1", Token::Float, -0.1);
    checkToken("123.321", Token::Float, 123.321);
    checkToken("123e10", Token::Float, 123e10);
    checkToken("123E10", Token::Float, 123e10);
    checkToken("123e-10", Token::Float, 123e-10);
    checkToken("123e+10", Token::Float, 123e+10);
    checkToken("1.23e4", Token::Float, 1.23e4);
    checkToken("123.456e78", Token::Float, 123.456e78);

    errorToken(".1");
    checkToken("1.1.1", Token::Float, 1.1);
}

BOOST_AUTO_TEST_CASE(test_string)
{
    auto s = [] (std::string str) { return '"' + str + '"'; };

    checkToken(s("abc"), Token::String, "abc");
    checkToken(s("\\/"), Token::String, "/");
    checkToken(s("\\b"), Token::String, "\b");
    checkToken(s("\\f"), Token::String, "\f");
    checkToken(s("\\n"), Token::String, "\n");
    checkToken(s("\\r"), Token::String, "\r");
    checkToken(s("\\t"), Token::String, "\t");
    checkToken(s("\\\\"), Token::String, "\\");
    checkToken(s("\\\""), Token::String, "\"");
    errorToken(s("\\g"));
    errorToken(s("\n"));

    checkToken(s("\u1234"), Token::String, "\u1234");
    checkToken(s("\uFFFF"), Token::String, "\uFFFF");
    checkToken(s("\u0FFF"), Token::String, "\u0FFF");
    checkToken(s("\u00FF"), Token::String, "\u00FF");
    checkToken(s("\u000F"), Token::String, "\u000F");
    checkToken(s("0\u12345"), Token::String, "0\u12345");
    checkToken(s("\u1234\u1234"), Token::String, "\u1234\u1234");

    checkToken(s("\\u1234"), Token::String, "\u1234");
    checkToken(s("\\uFFFF"), Token::String, "\uFFFF");
    checkToken(s("\\u0FFF"), Token::String, "\u0FFF");
    checkToken(s("\\u00FF"), Token::String, "\u00FF");
    checkToken(s("\\u000F"), Token::String, "\u000F");
    checkToken(s("0\\u12345"), Token::String, "0\u12345");
    errorToken(s("\\u123"));
    errorToken(s("\\u123G"));
    errorToken(s("\\uG234"));

    checkToken(s("\\u1234"), Token::String, "\\u1234", Reader::ValidateUnicode);
    errorToken(s("\\u123"), Reader::ValidateUnicode);
    errorToken(s("\\u123G"), Reader::ValidateUnicode);
    errorToken(s("\\uG234"), Reader::ValidateUnicode);

    auto u = [&] (std::initializer_list<int> c) {
        return std::string(c.begin(), c.end());
    };

    checkToken(s(u({ 0x0F })), Token::String, "\u000F");
    checkToken(s(u({ 0xCF, 0x8F })), Token::String, u({ 0xCF, 0x8F }));

    checkToken(s(u({ 0xF0 })), Token::String, u({ 0xF0 }), Reader::None);
    checkToken(s(u({ 0xC0, 0x8F })), Token::String, u({ 0xC0, 0x8F }), Reader::None);
    checkToken(s(u({ 0xE0, 0x8F })), Token::String, u({ 0xE0, 0x8F }), Reader::None);
    checkToken(s(u({ 0xE0, 0x8F, 0x0F })), Token::String, u({ 0xE0, 0x8F, 0x0F }), Reader::None);

    errorToken(s(u({ 0xF0 })));
    errorToken(s(u({ 0xC0, 0x8F })));
    errorToken(s(u({ 0xE0, 0x8F })));
    errorToken(s(u({ 0xE0, 0x8F, 0x0F })));
}
