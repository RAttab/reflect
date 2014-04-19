/* parser.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json parser implementation.
*/

#include "parser.h"

#include <sstream>

namespace reflect {
namespace json {


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

const Type* getContainerType(Value& value)
{
    Value typeVector = value.call<Value>("containedType");
    return typeVector[0].get<const Type*>();
}

/******************************************************************************/
/* PARSER                                                                     */
/******************************************************************************/

void parseInto(Value& value, Token token, std::istream& json);


void parseNull(Value& value)
{
    if (value.is("pointer"))
            value.assign(nullptr);

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

    Value typeVector = value.call<Value>("containedType");
    auto type = typeVector[0].get<const Type*>();

    Token token = nextToken();
    if (token == Token::ArrayEnd) return;

    while (json) {
        Value item = type.construct();
        parseInto(item, token, json);
        value.call<void>("push_back", item.rvalue());

        token = nextToken();
        if (token.type() == Token::Separator) continue;

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

    Token token = nextToken();
    if (token == Token::ObjectEnd) return;

    bool isDictionnary = value.is("dictionnary");

    while (json) {

        expectToken(token, Token::String);
        std::string key = token.stringValue();
        expectToken(nextToken(), Token::KeySeparator);

        const Type* type = isDictionnary ?
            containedType(value) : value.type()->fieldType(key);

        Value item = type->construct();
        parseInto(item, json);

        if (isDictionnary)
            value[key] = item;
        else value.set(key, item);

        token = nextToken();
        if (token.type() == Token::Separator) continue;

        expectToken(token, Token::ArrayEnd);
        return;
    }

    reflectError("unexpected end of object");
}


void parseInto(Value& value, Token token, std::istream& json)
{
    switch (token.type())
    {
    case Token::Null: parseNull(value, json); break;
    case Token::Bool: parseBool(value, json); break;
    case Token::Number: parseNumber(value, json); break;
    case Token::String: parseString(value, json); break;
    case Token::ArrayStart: parseArray(value, json); break;
    case Token::ObjectStart: parseObject(value, json); break;

    default: reflectError("unexpected token <%s>", toString(token.type());
    }
}


void parseInto(Value& value, std::istream& json)
{
    parseJson(value, nextToken(json), json);
}

void parseInto(Value& value, const std::string& json)
{
    parseInto(value, std::stringstream(json));
}

Value parse(const Type* type, std::istream& json)
{
    Value value = type->construct();
    parseInto(value, json);
    return value;
}

Value parse(const Type* type, const std::string& json)
{
    parse(type, std::stringstream(json));
}

} // namespace json
} // reflect
