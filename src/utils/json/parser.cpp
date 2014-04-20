/* parser.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json parser implementation.
*/

#include "parser.h"
#include "token.h"
#include "reflect.h"
#include "types/primitives.h"
#include "types/std/string.h"
#include "types/reflect/type.h"

#include <sstream>

namespace reflect {
namespace json {


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

namespace {

const Type* getValueType(Value& value)
{
    return value.type()->call<const Type*>("valueType");
}

const Type* getFieldType(Value& value, const std::string& field)
{
    return value.type()->fieldType(field);
}

} // namespace anonymous


/******************************************************************************/
/* PARSER                                                                     */
/******************************************************************************/

void parseInto(Value& value, Token token, std::istream& json);


void parseNull(Value& value)
{
    if (value.is("pointer"))
        value.assign(value.type()->construct());

    else {
        reflectError("can't assign null to non-pointer type <%s>",
                value.typeId());
    }
}

void parseBool(Value& value, const Token& token)
{
    if (value.is("bool"))
        value.assign(token.boolValue());

    else {
        reflectError("can't assign <%d> to non-bool type <%s>",
                token.boolValue(), value.typeId());
    }
}

void parseNumber(Value& value, const Token& token)
{
    if (value.is("integer"))
        value.assign(token.intValue());

    else if (value.is("float"))
        value.assign(token.floatValue());

    else {
        reflectError("can't assign <%s> to non-number type <%s>",
                token.stringValue(), value.typeId());
    }
}

void parseString(Value& value, const Token& token)
{
    if (value.is("string"))
        value.assign(token.stringValue());

    else {
        reflectError("can't assign <%s> to non-string type <%s>",
                token.stringValue(), value.typeId());
    }
}

void parseArray(Value& value, std::istream& json)
{
    if (!value.is("list")) {
        reflectError("can't assign array to non-array type <%s>",
                value.typeId());
    }

    auto type = getValueType(value);

    Token token = nextToken(json);
    if (token.type() == Token::ArrayEnd) return;

    while (json) {

        Value item = type->construct();
        parseInto(item, token, json);
        value.call<void>("push_back", item.rvalue());

        token = nextToken(json);
        if (token.type() == Token::Separator) {
            token = nextToken(json);
            continue;
        }

        expectToken(token, Token::ArrayEnd);
        return;
    }

    reflectError("unexpected end of array");
}

void parseObject(Value& value, std::istream& json)
{
    if (value.is("primitive") || value.is("list") || value.is("string")) {
        reflectError("can't assign object to non-object type <%s>",
                value.typeId());
    }

    Token token = nextToken(json);
    if (token.type() == Token::ObjectEnd) return;

    bool isMap = value.is("map");
    const Type* tValue = nullptr;
    if (isMap) tValue = getValueType(value);

    while (json) {

        expectToken(token, Token::String);
        const std::string key = token.stringValue();

        expectToken(nextToken(json), Token::KeySeparator);

        const Type* type = isMap ? tValue : getFieldType(value, key);
        Value item = type->construct();
        parseInto(item, json);

        if (isMap) value[key].assign(item.rvalue());
        else value.set(key, item.rvalue());

        token = nextToken(json);
        if (token.type() == Token::Separator) {
            token = nextToken(json);
            continue;
        }

        expectToken(token, Token::ObjectEnd);
        return;
    }

    reflectError("unexpected end of object");
}


void parseInto(Value& value, Token token, std::istream& json)
{
    switch (token.type())
    {
    case Token::Null: parseNull(value); break;
    case Token::Bool: parseBool(value, token); break;
    case Token::Number: parseNumber(value, token); break;
    case Token::String: parseString(value, token); break;
    case Token::ArrayStart: parseArray(value, json); break;
    case Token::ObjectStart: parseObject(value, json); break;

    default: reflectError("unexpected token <%s>", print(token.type()));
    }
}


void parseInto(Value& value, std::istream& json)
{
    parseInto(value, nextToken(json), json);
}

void parseInto(Value& value, const std::string& json)
{
    std::stringstream ss(json);
    parseInto(value, ss);
}

Value parse(const Type* type, std::istream& json)
{
    Value value = type->construct();
    parseInto(value, json);
    return value;
}

Value parse(const Type* type, const std::string& json)
{
    std::stringstream ss(json);
    return parse(type, ss);
}

} // namespace json
} // reflect
