/* printer.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json printer
*/

#include "printer.h"
#include "token.h"
#include "types/primitives.h"
#include "types/std/string.h"
#include "types/std/vector.h"
#include "types/reflect/type.h"

#include <sstream>

namespace reflect {
namespace json {

namespace {

/******************************************************************************/
/* PRINTER                                                                    */
/******************************************************************************/


void newline(std::ostream& json, int indent)
{
    if (indent < 0) return;
    json << '\n' << std::string(indent * 4, ' ');
}

void space(std::ostream& json, int indent)
{
    if (indent < 0) return;
    json << ' ';
}

int inc(int indent) { return indent < 0 ? -1 : (indent + 1); }


void print(const Value& value, std::ostream& json, int indent);

void printPointer(const Value& value, std::ostream& json, int indent)
{
    if (!value) printNull(json);
    else print(*value, json, indent);
}

void printArray(const Value& value, std::ostream& json, int indent)
{
    json << '[';
    size_t n = value.call<size_t>("size");

    for (size_t i = 0; i < n; ++i) {
        if (i) json << ',';
        newline(json, inc(indent));
        print(value[i], json, inc(indent));
    }

    if (n) newline(json, indent);
    json << ']';
}


void printMap(const Value& value, std::ostream& json, int indent)
{
    json << '{';
    size_t i = 0;

    auto keys = value.call<std::vector<std::string> >("keys");

    for (const auto& key : keys) {
        if (i++) json << ',';
        newline(json, inc(indent));

        printString(key, json);

        json << ':';
        space(json, indent);

        print(value[key], json, inc(indent));
    }

    if (i) newline(json, indent);
    json << '}';
}

void printObject(const Value& value, std::ostream& json, int indent)
{
    json << '{';
    size_t i = 0;

    for (const auto& field : value.type()->fields()) {
        if (!value.type()->hasField(field)) continue;

        if (i++) json << ',';
        newline(json, inc(indent));

        printString(field, json);

        json << ':';
        space(json, indent);

        print(value.get<Value>(field), json, inc(indent));
    }

    if (i) newline(json, indent);
    json << '}';
}


void print(const Value& value, std::ostream& json, int indent)
{
    // copy allows for converters to be called.
    if (value.is("bool")) printBool(value.copy<bool>(), json);
    else if (value.is("float")) printFloat(value.copy<double>(), json);
    else if (value.is("integer")) printInteger(value.copy<long>(), json);
    else if (value.is("string")) printString(value.copy<std::string>(), json);

    else if (value.is("map")) printMap(value, json, indent);
    else if (value.is("list")) printArray(value, json, indent);
    else if (value.is("pointer")) printPointer(value, json, indent);
    else if (!value.is("primitive")) printObject(value, json, indent);

    else reflectError("can't print value");
}

} // namespace anonymous


/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

void print(const Value& value, std::ostream& json, bool pretty)
{
    print(value, json, pretty ? 0 : -1);
}

std::string print(Value& value, bool pretty)
{
    std::stringstream ss;
    print(value, ss, pretty);
    return ss.str();
}

} // namespace json
} // reflect
