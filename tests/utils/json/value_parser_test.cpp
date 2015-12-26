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
    Basics::construct(exp);
    auto obj = parseFile<Basics>("value_parser.json");
    BOOST_CHECK_EQUAL(obj, exp);
}


/******************************************************************************/
/* TEST VALUE PARSER                                                          */
/******************************************************************************/

void checkEqual(Value& obj, nullptr_t) { BOOST_CHECK(obj.isVoid()); }
void checkEqual(Value& obj, bool exp) { BOOST_CHECK_EQUAL(cast<bool>(obj), exp); }
void checkEqual(Value& obj, int exp) { BOOST_CHECK_EQUAL(cast<int64_t>(obj), exp); }
void checkEqual(Value& obj, double exp) { BOOST_CHECK_EQUAL(cast<double>(obj), exp); }
void checkEqual(Value& obj, const char* exp) {
    BOOST_CHECK_EQUAL(cast<std::string>(obj), std::string(exp));
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
