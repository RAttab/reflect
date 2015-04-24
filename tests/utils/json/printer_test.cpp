/* printer_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 23 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "utils/json.h"

#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace reflect;
using namespace reflect::json;


/******************************************************************************/
/* OBJECT                                                                     */
/******************************************************************************/

template<typename... T>
std::vector<std::string> keys(T... keys) { return { keys... }; }

void print(Writer& writer)
{
    auto onLeafObject = [&] (const std::string& key) {
        if (key == "3") printInt(writer, 4);

        else reflectError("unknown key %s", key);
    };

    auto onLeafArray = [&] (size_t i) {
        if (i == 0) printInt(writer, 2);

        else reflectError("unknown index %d", i);
    };

    auto onObject = [&] (const std::string& key) {
        if (key == "a") printInt(writer, 1);
        else if (key == "b") printArray(writer, 1, onLeafArray);
        else if (key == "c") printObject(writer, keys("3"), onLeafObject);

        else reflectError("unknown key %s", key);
    };

    auto onArray = [&] (size_t i) {
        if (i == 0) printInt(writer, 1);
        else if (i == 1) printArray(writer, 1, onLeafArray);
        else if (i == 2) printObject(writer, keys("3"), onLeafObject);

        else reflectError("unknown index %d", i);
    };

    auto onField = [&] (const std::string& key) {
        if (key == "null") printNull(writer);
        else if (key == "bool") printBool(writer, true);
        else if (key == "int") printInt(writer, 123);
        else if (key == "float") printFloat(writer, 123.321);
        else if (key == "string") printString(writer, "abc");
        else if (key == "array") printArray(writer, 3, onArray);
        else if (key == "object")
            printObject(writer, keys("a", "b", "c"), onObject);

        else reflectError("unknown key <%s>", key);
    };
    printObject(
            writer,
            keys("null", "bool", "int", "float", "string", "array", "object"),
            onField);

    BOOST_CHECK(!writer.error());
    if (writer.error())
        std::cerr << "ERROR: " << writer.error().what() << std::endl;
}


/******************************************************************************/
/* PRINTERS                                                                   */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_compact)
{
    std::stringstream ss;
    Writer writer(ss);

    print(writer);

    const std::string exp =
        "{\"null\":null,\"bool\":true,\"int\":123,\"float\":123.321,\"string\":\"abc\",\"array\":[1,[2],{\"3\":4}],\"object\":{\"a\":1,\"b\":[2],\"c\":{\"3\":4}}}";

    BOOST_CHECK_EQUAL(ss.str(),exp);
}

BOOST_AUTO_TEST_CASE(test_pretty)
{
    std::stringstream ss;
    Writer writer(ss, Writer::Pretty);
    print(writer);

    const std::string exp =
        "{\n"
        "    \"null\": null,\n"
        "    \"bool\": true,\n"
        "    \"int\": 123,\n"
        "    \"float\": 123.321,\n"
        "    \"string\": \"abc\",\n"
        "    \"array\": [\n"
        "        1,\n"
        "        [ 2 ],\n"
        "        { \"3\": 4 }\n"
        "    ],\n"
        "    \"object\": {\n"
        "        \"a\": 1,\n"
        "        \"b\": [ 2 ],\n"
        "        \"c\": { \"3\": 4 }\n"
        "    }\n"
        "}";
    BOOST_CHECK_EQUAL(ss.str(), exp);
}
