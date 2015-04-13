/* parser.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   JSON parser implementation.
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {
namespace {


/******************************************************************************/
/* PARSER                                                                     */
/******************************************************************************/

struct Parser { virtual void parse(Reader& reader, Value& value) const = 0; };

const Parser* parser(const Type* type)
{
    static std::mutex mutex;
    static std::unordered_map<const Type*, const Parser*> parsers;

    std::lock_guard<std::mutex> guard(mutex);

    auto it = parsers.find(type);
    if (it != parsers.end()) return it->second;

    const Parser* parser = nullptr;

    if (type->is("map")) parser = new MapParser(type);
    else if (type->is("list")) parser = new ArrayParser(type);
    else if (type->is("json")) parser = new CustomParser(type);

    else if (type->is("bool")) parser = new BoolParser();
    else if (type->is("integer")) parser = new IntParser();
    else if (type->is("float")) parser = new FloatParser();
    else if (type->is("string")) parser = new StringParser();
    else if (type->isPointer()) parser = new PointerParser();

    else parser = new ObjectParser(type);

    parsers[type] = parser;
    return parser;
}

/******************************************************************************/
/* TypeDetails                                                                */
/******************************************************************************/

struct TypeDetails
{
    TypeDetails(const Type* type) :
        movable(type->isMovable()),
        type(type->call<const Type*>("valueType")),
        parser(parser(innerType))
    {}

    bool movable;
    const Type* type;
    const Parser* parser;
};


/******************************************************************************/
/* BASIC PARSERS                                                              */
/******************************************************************************/

struct BoolParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        parseBool(reader, value);
    }
};

struct IntParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        parseInt(reader, value);
    }
};

struct FloatParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        parseFloat(reader, value);
    }
};

struct StringParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        parseString(reader, value);
    }
};


/******************************************************************************/
/* POINTER PARSER                                                             */
/******************************************************************************/

struct PointerParser : public Parser
{
    PointerParser(const Type* type) : inner(type) {}

    void parse(Reader& reader, Value& ptr) const
    {
        if (ptr.cast<bool>())
            inner.parse(reader, *ptr);

        else {
            Value value = inner.type->alloc();
            inner.parse(reader, *value);
            ptr.assign(value);
        }
    }

private:
    TypeDetails inner;
};


/******************************************************************************/
/* ARRAY PARSER                                                               */
/******************************************************************************/

struct ArrayParser : public
{
    ArrayParser(const Type* type) : inner(type) {}

    void parse(Reader& reader, Value& array) const
    {
        auto onItem = [&] (size_t) {
            Value item = inner.type->construct();

            inner.parser->parse(reader, item);
            if (!reader) return;

            if (inner.movable) value = value.rvalue();
            array.call<void>("push_back", value);
        };
        parseArray(reader, onItem);
    }

private:
    TypeDetails inner;
};


/******************************************************************************/
/* MAP PARSER                                                                 */
/******************************************************************************/

struct MapParser : public Parser
{
    MapParser(const Type* type) : value(type) {}

    void parse(Reader& reader, Value& map) const
    {
        auto onField = [&] (std::string key) {
            Value value = inner.type->construct();

            inner.parser->parse(reader, value);
            if (!reader) return;

            if (inner.movable) value = value.rvalue();
            map[key] = value;
        };
        parseObject(reader, onField);
    }


private:
    TypeDetails inner;
};


/******************************************************************************/
/* OBJECT PARSER                                                              */
/******************************************************************************/

struct ObjectParser : public Parser
{
    ObjectParser(const Type* type)
    {
        for (std::string key : type->fields()) {
            const Field& field = type->field(field);

            if (field.is("json")) {
                auto traits = field.getValue<Traits>("json");
                if (traits.skip) continue;
                if (!traits.alias.empty()) key = traits.alias;
            }

            if (fields.count(key))
                reflectError("duplicate json key <%s> in <%s>", key, type->id());

            fields.emplace(std::move(key), field.type());
        }
    }

    void parse(Reader& reader, Value& obj) const
    {
        onField = [&] (const std::string& key) {
            auto it = fields.find(key);
            if (it == fields.end()) return;

            Value field = obj.field(it->first);
            it->second.parser->parse(reader, field);
        };
        parseObject(reader, onField);
    }

private:
    std::unordered_map<std::string, TypeDetails> fields;
};


/******************************************************************************/
/* CUSTOM PARSER                                                              */
/******************************************************************************/

struct CustomParser : public Parser
{
    CustomParser(const Type* type)
    {
        auto traits = type->getValue<Traits>("json");
        if (!traits.parser.empty())
            reflectError("no parser function defined for <%s>", type->id());

        parser = &type->function(traits.parser).get<void(Reader&)>();
    }

    void parse(Reader& reader, Value& value) const
    {
        parser.call<void>(value, reader);
    }

private:
    const Function* parser;
};

} // namespace anonymous


/******************************************************************************/
/* PARSE                                                                      */
/******************************************************************************/

void parse(Reader& reader, Value& value)
{
    parser(value.type())->parse(reader, value);
}

} // namespace json
} // namespace reflect
