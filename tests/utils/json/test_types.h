/* value_test_utils.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#pragma once

#include "reflect.h"
#include "utils/json.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

/******************************************************************************/
/* CUSTOM                                                                     */
/******************************************************************************/

struct Custom
{
    int a;

    Custom(int v = 0) : a(v) {}

    void parseJson(reflect::json::Reader& reader)
    {
        a = std::stoi(reflect::json::parseString(reader));
    }

    void printJson(reflect::json::Writer& writer) const
    {
        reflect::json::printString(writer, std::to_string(a));
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

reflectTypeDecl(Custom)


/******************************************************************************/
/* BASICS                                                                     */
/******************************************************************************/

struct Basics
{
    bool boolean;
    int64_t integer;
    double floating;

    std::string string;
    std::string* stringPtr;
    std::shared_ptr<std::string> stringShared;

    int64_t* nullPtr;
    std::shared_ptr<std::string> nullShared;

    int64_t skip;
    int64_t alias;
    Custom custom;

    std::vector<int64_t> vector;
    std::vector<int64_t*> vectorPtr;

    std::map<std::string, int64_t> map;
    std::map<std::string, int64_t*> mapPtr;

    Basics* next;

    Basics();
    ~Basics();
    static void construct(Basics& value);

    bool operator==(const Basics& other) const;

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

    std::string print(size_t indent = 0) const;
};

template<typename OStream>
OStream& operator<< (OStream& stream, const Basics& obj)
{
    return stream << obj.print();
}

reflectTypeDecl(Basics)
