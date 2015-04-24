/* format_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 23 Apr 2015
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

void format(Writer& writer, nullptr_t) { formatNull(writer); }
void format(Writer& writer, bool value) { formatBool(writer, value); }
void format(Writer& writer, int value) { formatInt(writer, value); }
void format(Writer& writer, double value) { formatFloat(writer, value); }
void format(Writer& writer, const char* value) { formatString(writer, value); }
void format(Writer& writer, std::string value) { formatString(writer, value); }

template<typename T>
std::string doPrint(T value) { return std::to_string(value); }
std::string doPrint(const char* c) { return c; }
std::string doPrint(std::string s) { return s; }
std::string doPrint(nullptr_t) { return "null"; }

template<typename T>
void check(T value, std::string exp, Writer::Options options = Writer::Default)
{
    std::stringstream ss;
    Writer writer(ss, options);

    format(writer, value);

    std::cerr << "'" << doPrint(value) << "' -> '" << ss.str() << "'\n";

    BOOST_CHECK(!writer.error());
    if (writer.error())
        std::cerr << "ERROR: " << writer.error() << std::endl;

    BOOST_CHECK_EQUAL(ss.str(), exp);
}

template<typename T>
void checkError(T value, Writer::Options options = Writer::Default)
{
    std::stringstream ss;
    Writer writer(ss, options);

    format(writer, value);

    std::cerr << "'" << doPrint(value) << "' -> '" << writer.error().what() << "'\n";
    BOOST_CHECK(writer.error());
}

/******************************************************************************/
/* BASICS                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_basics)
{
    check(nullptr, "null");
    check(false, "false");
    check(true, "true");

    check(123, "123");
    check(-123, "-123");
    check(0.1, "0.1");
    check(-0.1, "-0.1");
    check(123.321, "123.321");
    check(123e10, "1.23e+12");
    check(123e-10, "1.23e-08");
    check(123e+10, "1.23e+12");
    check(1.23e10, "12300000000");
    check(0.123456789, "0.123456789");
    check(0.123456789123456789, "0.123456789123");
    check(0.000000123456789, "1.23456789e-07");
    check(0.000000123456789123456789, "1.23456789123e-07");
}

BOOST_AUTO_TEST_CASE(test_string)
{
    auto s = [] (std::string str) { return '"' + str + '"'; };

    check("abc", s("abc"));
    check("/", s("\\/"));
    check("\"", s("\\\""));
    check("\\", s("\\\\"));
    check("\b", s("\\b"));
    check("\f", s("\\f"));
    check("\n", s("\\n"));
    check("\r", s("\\r"));
    check("\t", s("\\t"));

    check("\u1234", s("\\u1234"));
    check("\u1234", s("\u1234"), Writer::None);
    check("\u1234", s("\u1234"), Writer::ValidateUnicode);
    check("\uFFFF", s("\\uffff"));
    check("\u0FFF", s("\\u0fff"));
    check("\u00FF", s("\\u00ff"));
    check("\u000F", s("\u000F"));
    check("0\u12345", s("0\\u12345"));
    check("\u1234\u1234", s("\\u1234\\u1234"));
    check("\\u1234", s("\\\\u1234"));

    auto u = [&] (std::initializer_list<int> c) {
        return std::string(c.begin(), c.end());
    };

    check(u({ 0x0F }), s("\u000F"));
    check(u({ 0xCF, 0x8F }), s(u({ 0xCF, 0x8F })), Writer::ValidateUnicode);

    check(u({ 0xF0 }), s(u({ 0xF0 })), Writer::None);
    check(u({ 0xC0, 0x8F }), s(u({ 0xC0, 0x8F })), Writer::None);
    check(u({ 0xE0, 0x8F }), s(u({ 0xE0, 0x8F })), Writer::None);
    check(u({ 0xE0, 0x8F, 0x0F }), s(u({ 0xE0, 0x8F, 0x0F })), Writer::None);

    checkError(u({ 0xF0 }), Writer::ValidateUnicode);
    checkError(u({ 0xC0, 0x8F }), Writer::ValidateUnicode);
    checkError(u({ 0xE0, 0x8F }), Writer::ValidateUnicode);
    checkError(u({ 0xE0, 0x8F, 0x0F }), Writer::ValidateUnicode);
}
