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

struct Basics
{
    bool boolean;
    int64_t integer;
    double floating;

    std::string string;
    std::string* stringPtr;
    std::shared_ptr<std::string> stringShared;

    std::vector<int64_t> vector;
    std::vector<int64_t*> vectorPtr;

    std::map<std::string, int64_t> map;
    std::map<std::string, int64_t*> mapPtr;

    Basics* next;

    Basics() :
        boolean(false), integer(0), floating(0),
        stringPtr(nullptr), next(nullptr)
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
            && string == other.string;
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
            << i1 << "stringShared: " << printPtr(stringShared) << '\n';

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

    reflectField(vector);
    reflectField(vectorPtr);

    reflectField(map);
    reflectField(mapPtr);

    reflectField(next);
}


/******************************************************************************/
/* PARSE FILE                                                                 */
/******************************************************************************/

Basics parseFile(const std::string& file)
{
    std::ifstream stream("tests/utils/json/" + file);

    Basics obj;
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

    Basics obj = parseFile("basics.json");
    BOOST_CHECK_EQUAL(obj, exp);
}
