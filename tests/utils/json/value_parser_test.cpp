/* value_parser_test.cc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "dsl/all.h"
#include "types/primitives.h"
#include "types/std/map.h"
#include "types/std/vector.h"
#include "types/std/string.h"
#include "types/std/smart_ptr.h"
#include "utils/json.h"

#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace reflect;
using namespace reflect::json;


/******************************************************************************/
/* STRUCTS                                                                    */
/******************************************************************************/

struct Custom
{
    int a;

    Custom(int v = 0) : a(v) {}

    void parseJson(Reader& reader)
    {
        a = std::stoi(parseString(reader));
    }

    bool operator==(const Custom& other) const
    {
        return a == other.a;
    }

    std::string print() const
    {
        return std::to_string(a);
    }
};

reflectType(Custom)
{
    reflectPlumbing();

    reflectFn(parseJson);
    reflectTypeValue(json, custom("parseJson", ""));
}

struct Basics
{
    bool boolean;
    int64_t integer;
    double floating;

    std::string string;
    std::string* stringPtr;
    std::shared_ptr<std::string> stringShared;

    int64_t skip;
    int64_t alias;
    Custom custom;

    std::vector<int64_t> vector;
    std::vector<int64_t*> vectorPtr;

    std::map<std::string, int64_t> map;
    std::map<std::string, int64_t*> mapPtr;

    Basics* next;

    Basics() :
        boolean(false), integer(0), floating(0),
        stringPtr(nullptr), skip(0), alias(0),
        next(nullptr)
    {}

    ~Basics()
    {
        if (stringPtr) delete stringPtr;

        for (const auto& item : vectorPtr) delete item;
        for (const auto& entry : mapPtr) delete entry.second;

        if (next) delete next;
    }

    bool operator==(const Basics& other) const
    {
        auto err = [] (const std::string& msg) {
            std::cerr << "ERROR: " << msg << std::endl;
            return false;
        };

        bool ok = boolean == other.boolean
            && integer == other.integer
            && floating == other.floating
            && string == other.string
            && skip == other.skip
            && alias == other.alias
            && custom == other.custom;
        if (!ok) return err("basics");

        if ((stringPtr == nullptr) != (other.stringPtr == nullptr)) return err("ptr.nil");
        if (stringPtr && (*stringPtr != *other.stringPtr)) return err("ptr.eq");

        if (vector.size() != other.vector.size()) return err("vec.size");
        for (size_t i = 0; i < vector.size(); ++i) {
            if (vector[i] != other.vector[i]) return err("vec.item");
        }

        if (vectorPtr.size() != other.vectorPtr.size()) return err("vecPtr.size");
        for (size_t i = 0; i < vectorPtr.size(); ++i) {
            if (*vectorPtr[i] != *other.vectorPtr[i]) return err("vecPtr.item");
        }

        if (map.size() != other.map.size()) return err("map.size");
        for (const auto& entry : map) {
            auto it = other.map.find(entry.first);
            if (it == other.map.end() || entry.second != it->second) return err("map.entry");
        }

        if (mapPtr.size() != other.mapPtr.size()) return err("map.size");
        for (const auto& entry : mapPtr) {
            auto it = other.mapPtr.find(entry.first);
            if (it == other.mapPtr.end() || *entry.second != *it->second) return err("map.entry");
        }

        if ((next == nullptr) != (other.next == nullptr)) return err("next");
        return next != nullptr ? (*next) == (*other.next) : true;
    }

    template<typename T>
    static std::string toString(T value) { return std::to_string(value); }
    static std::string toString(std::string value) { return value; }

    template<typename Ptr>
    static std::string printPtr(Ptr ptr)
    {
        std::stringstream ss;
        ss << '<' << ptr << ":" << (ptr ? toString(*ptr) : "nil") << '>';
        return ss.str();
    }

    std::string print(size_t indent = 0) const
    {
        std::string i0(indent * 4, ' ');
        std::string i1(++indent * 4, ' ');

        std::stringstream ss;

        ss << "{\n"
            << i1 << "boolean: " << boolean << '\n'
            << i1 << "integer: " << integer << '\n'
            << i1 << "floating: " << floating << '\n'
            << i1 << "string: " << string << '\n'
            << i1 << "stringPtr: " << printPtr(stringPtr) << '\n'
            << i1 << "stringShared: " << printPtr(stringShared) << '\n'
            << i1 << "skip: " << skip << '\n'
            << i1 << "alias: " << alias << '\n'
            << i1 << "custom: " << custom.print() << '\n';

        ss << i1 << "vector: " << vector.size() << "[ ";
        for (const auto& item : vector) ss << item << ' ';
        ss << "]\n";

        ss << i1 << "vectorPtr: " << vectorPtr.size() << "[ ";
        for (const auto& item : vectorPtr) ss << printPtr(item) << ' ';
        ss << "]\n";

        ss << i1 << "map: " << map.size() << "[ ";
        for (const auto& entry : map) ss << entry.first << ':' << entry.second << ' ';
        ss << "]\n";

        ss << i1 << "mapPtr: " << mapPtr.size() << "[ ";
        for (const auto& entry : mapPtr) ss << entry.first << ':' << printPtr(entry.second) << ' ';
        ss << "]\n";

        ss << i1 << "next: " << (next ? next->print(indent) : "nil") << '\n';

        ss << i0 << "}";
        return ss.str();
    }
};

template<typename OStream>
OStream& operator<< (OStream& stream, const Basics& obj)
{
    return stream << obj.print();
}

reflectType(Basics)
{
    reflectPlumbing();
    reflectAlloc();

    reflectField(boolean);
    reflectField(integer);
    reflectField(floating);

    reflectField(string);
    reflectField(stringPtr);
    reflectField(stringShared);

    reflectField(skip);
    reflectFieldValue(skip, json, skip());

    reflectField(alias);
    reflectFieldValue(alias, json, alias("bob"));

    reflectField(custom);

    reflectField(vector);
    reflectField(vectorPtr);

    reflectField(map);
    reflectField(mapPtr);

    reflectField(next);
}


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
