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

using namespace std;
using namespace reflect;
using namespace reflect::json;


/******************************************************************************/
/* STRUCTS                                                                    */
/******************************************************************************/

struct Basics
{
    bool boolean;
    bool* booleanPtr;

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
        boolean(false), booleanPtr(nullptr),
        integer(0), floating(0),
        stringPtr(nullptr),
        next(nullptr)
    {}

    ~Basics()
    {
        if (booleanPtr) delete booleanPtr;
        if (stringPtr) delete stringPtr;

        for (const auto& item : vectorPtr) delete item;
        for (const auto& entry : mapPtr) delete entry.second;

        if (next) delete next;
    }

    bool operator==(const Basics& other) const
    {
        bool ok = boolean == other.boolean
            && *booleanPtr == *other.booleanPtr
            && integer == other.integer
            && floating == other.integer
            && string == other.string
            && *stringPtr == *other.stringPtr;

        if (!ok) return false;

        if (vector.size() != other.vector.size()) return false;
        for (size_t i = 0; i < vector.size(); ++i) {
            if (vector[i] != other.vector[i]) return false;
        }

        if (vectorPtr.size() != other.vectorPtr.size()) return false;
        for (size_t i = 0; i < vectorPtr.size(); ++i) {
            if (*vectorPtr[i] != *other.vectorPtr[i]) return false;
        }

        if (map.size() != other.map.size()) return false;
        for (const auto& entry : map) {
            auto it = other.map.find(entry.first);
            if (it == other.map.end() || entry.second != it->second) return false;
        }

        if (mapPtr.size() != other.mapPtr.size()) return false;
        for (const auto& entry : mapPtr) {
            auto it = other.mapPtr.find(entry.first);
            if (it == other.mapPtr.end() || *entry.second != *it->second) return false;
        }

        if ((next == nullptr) != (other.next == nullptr)) return false;
        return next != nullptr ? (*next) == (*other.next) : true;
    }

    template<typename Ptr>
    static std::string printPtr(Ptr ptr)
    {
        std::stringstream ss;
        ss << '<' << ptr << (ptr ? std::to_string(*ptr) : "nil") << '>';
        return ss.str();
    }

    std::string print(size_t indent = 0) const
    {

        std::string i0(indent * 4, ' ');
        std::string i1(++indent * 4, ' ');

        std::stringstream ss;

        ss << i0 << "{\n"
            << i1 << "boolean: " << boolean << '\n'
            << i1 << "booleanPtr: " << printPtr(booleanPtr) << '\n'
            << i1 << "integer: " << integer << '\n'
            << i1 << "floating: " << floating << '\n'
            << i1 << "string: " << string << '\n'
            << i1 << "stringPtr: " << stringPtr << '\n'
            << i1 << "stringShared: " << stringShared << '\n';

        ss << i1 << "vector: " << vector.size() << "[ ";
        for (const auto& item : vector) ss << item << ' ';
        ss << i1 << "]\n";

        ss << i1 << "vectorPtr: " << vectorPtr.size() << "[ ";
        for (const auto& item : vectorPtr) ss << printPtr(item) << ' ';
        ss << i1 << "]\n";

        ss << i1 << "map: " << map.size() << "[ ";
        for (const auto& entry : map) ss << entry.first << ':' << entry.second << ' ';
        ss << i1 << "]\n";

        ss << i1 << "mapPtr: " << mapPtr.size() << "[ ";
        for (const auto& entry : mapPtr) ss << entry.first << ':' << printPtr(entry.second) << ' ';
        ss << i1 << "]\n";

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
    reflectField(booleanPtr);

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
        exp.booleanPtr = new bool(true);

        exp.integer = 123;
        exp.floating = 123.321;

        exp.string = "abc";
        exp.stringPtr = new std::string("def");
        exp.stringShared = std::make_shared<std::string>("ghi");

        auto pInt = [](int64_t i) { return new int64_t(i); };

        exp.vector = std::vector<int64_t>({ 1, 2, 3 });
        exp.vectorPtr = std::vector<int64_t*>({ pInt(4), pInt(5), pInt(6) });

        exp.map = std::map<std::string, int64_t>({ {"abc", 10}, {"def", 20} });
        exp.mapPtr = std::map<std::string, int64_t*>({ {"hig", pInt(30)} });

        exp.next = new Basics;
        exp.next->integer = 10;

        exp.next->next = new Basics;
        exp.next->next->integer = 20;
    }

    Basics obj = parseFile("basics.json");

    BOOST_CHECK_EQUAL(exp, obj);
}
