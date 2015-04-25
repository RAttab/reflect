/* value_parser_test.cc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "test_types.h"
#include "types/std/map.h"
#include "types/std/vector.h"
#include "types/std/string.h"

#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace reflect;
using namespace reflect::json;


/******************************************************************************/
/* PARSE FILE                                                                 */
/******************************************************************************/

template<typename T>
T parseFile(const std::string& file)
{
    std::ifstream stream("tests/utils/json/" + file);

    T obj;
    auto err = parse(stream, obj);
    if (err) reflectError("unable to parse <%s>: %s", file, err.what());

    return obj;
}


/******************************************************************************/
/* TEST BASICS                                                                */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_basics)
{
    Basics exp;
    {
        exp.boolean = true;
        exp.integer = 123;
        exp.floating = 123.321;

        exp.string = "abc";
        exp.stringPtr = new std::string("def");
        exp.stringShared = std::make_shared<std::string>("ghi");

        exp.skip = 0;
        exp.alias = 654;
        exp.custom = { 789 };

        auto pInt = [](int64_t i) { return new int64_t(i); };

        exp.vector = { 1, 2, 3 };
        exp.vectorPtr = { pInt(4), pInt(5), pInt(6) };

        exp.map = { {"abc", 10}, {"def", 20} };
        exp.mapPtr = { {"hig", pInt(30)} };

        exp.next = new Basics;
        exp.next->integer = 10;

        exp.next->next = new Basics;
        exp.next->next->integer = 20;
    }

    auto obj = parseFile<Basics>("basics.json");
    BOOST_CHECK_EQUAL(obj, exp);
}


/******************************************************************************/
/* TEST VALUE PARSER                                                          */
/******************************************************************************/

void checkEqual(Value& obj, nullptr_t) { BOOST_CHECK(obj.isVoid()); }
void checkEqual(Value& obj, bool exp) { BOOST_CHECK_EQUAL(obj.cast<bool>(), exp); }
void checkEqual(Value& obj, int exp) { BOOST_CHECK_EQUAL(obj.cast<int64_t>(), exp); }
void checkEqual(Value& obj, double exp) { BOOST_CHECK_EQUAL(obj.cast<double>(), exp); }
void checkEqual(Value& obj, const char* exp) {
    BOOST_CHECK_EQUAL(obj.cast<std::string>(), std::string(exp));
}

template<typename Fn>
void checkObject(Value& obj, const Fn& fn)
{
    BOOST_CHECK(obj.is("map"));
    BOOST_CHECK_EQUAL(obj.type()->getValue<const Type*>("keyType"), type<std::string>());
    BOOST_CHECK_EQUAL(obj.type()->getValue<const Type*>("valueType"), type<Value>());

    typedef std::vector<std::string> Keys;
    for (const std::string& key : obj.call<Keys>("keys"))
        fn(key, obj.call<Value>("at", key));
}

template<typename Fn>
void checkArray(Value& array, const Fn& fn)
{
    BOOST_CHECK(array.is("list"));
    BOOST_CHECK_EQUAL(array.type()->getValue<const Type*>("valueType"), type<Value>());

    for (size_t i = 0; i < array.call<size_t>("size"); ++i)
        fn(i, array.call<Value>("at", i));
}

BOOST_AUTO_TEST_CASE(test_value_parser)
{
    auto obj = parseFile<Value>("generic.json");

    auto onLeafArray = [] (size_t i, Value item) {
        if (i == 0) checkEqual(item, 2);
        else reflectError("unexpected item <%d>", i);
    };

    auto onLeafObject = [] (std::string key, Value field) {
        if (key == "3") checkEqual(field, 4);
        else reflectError("unexpected field <%s>", key);
    };

    auto onArrayField = [&] (size_t i, Value item) {
        if (i == 0) checkEqual(item, 1);
        else if (i == 1) checkArray(item, onLeafArray);
        else if (i == 2) checkObject(item, onLeafObject);
        else reflectError("unexpected item <%d>", i);
    };

    auto onObjectField = [&] (std::string key, Value field) {
        if (key == "a") checkEqual(field, 1);
        else if (key == "b") checkArray(field, onLeafArray);
        else if (key == "c") checkObject(field, onLeafObject);
        else reflectError("unexpected field <%s>", key);
    };

    auto onTopField = [&] (std::string key, Value field) {
        if (key == "null")        checkEqual(field, nullptr);
        else if (key == "bool")   checkEqual(field, true);
        else if (key == "int")    checkEqual(field, 123);
        else if (key == "float")  checkEqual(field, 123.321);
        else if (key == "string") checkEqual(field, "abc");
        else if (key == "array")  checkArray(field, onArrayField);
        else if (key == "object") checkObject(field, onObjectField);
        else reflectError("unexpected field <%s>", key);
    };
    checkObject(obj, onTopField);
}
