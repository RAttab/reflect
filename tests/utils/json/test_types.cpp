/* value_test_utils.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "test_types.h"

#include "dsl/all.h"
#include "types/primitives.h"
#include "types/std/map.h"
#include "types/std/vector.h"
#include "types/std/string.h"
#include "types/std/smart_ptr.h"


/******************************************************************************/
/* CUSTOM                                                                     */
/******************************************************************************/

reflectTypeImpl(Custom)
{
    reflectPlumbing();

    reflectFn(parseJson);
    reflectTypeValue(json, reflect::json::custom("parseJson", "printJson"));
}


/******************************************************************************/
/* BASICS                                                                     */
/******************************************************************************/

Basics::
Basics() :
    boolean(false), integer(0), floating(0),
    stringPtr(nullptr), nullPtr(nullptr),
    skip(0), alias(0), next(nullptr)
{}

Basics::
~Basics()
{
    if (stringPtr) delete stringPtr;

    for (const auto& item : vectorPtr) delete item;
    for (const auto& entry : mapPtr) delete entry.second;

    if (next) delete next;
}

bool
Basics::
operator==(const Basics& other) const
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

    if (nullPtr) return err("nullPtr");
    if (nullShared) return err("nullShared");

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

std::string
Basics::
print(size_t indent) const
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
        << i1 << "nullPtr: " << printPtr(nullPtr) << '\n'
        << i1 << "nullShared: " << printPtr(nullShared) << '\n'
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

reflectTypeImpl(Basics)
{
    reflectPlumbing();
    reflectAlloc();

    reflectField(boolean);
    reflectField(integer);
    reflectField(floating);

    reflectField(string);
    reflectField(stringPtr);
    reflectField(stringShared);

    reflectField(nullPtr);
    reflectField(nullShared);

    reflectField(skip);
    reflectFieldValue(skip, json, reflect::json::skip());

    reflectField(alias);
    reflectFieldValue(alias, json, reflect::json::alias("bob"));

    reflectField(custom);

    reflectField(vector);
    reflectField(vectorPtr);

    reflectField(map);
    reflectField(mapPtr);

    reflectField(next);
}
