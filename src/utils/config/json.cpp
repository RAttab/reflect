/* json.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   json config serialization implementation.
*/

#include "includes.h"
#include "utils/json/token.h"

#include <sstream>

namespace reflect {
namespace config {

namespace {

/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

std::tuple<bool, std::string, std::string>
parseKey(const std::string& str)
{
    if (str.empty()) reflectError("empty key");

    size_t i = str.find('!');

    bool isLink = str[0] == '#';
    bool hasType = i == std::string::npos;

    size_t split = hasType ? str.size() : i;
    size_t next = hasType ? str.size() : i + 1;

    std::string key = str.substr(isLink, split);
    if (key.empty()) reflectError("empty key <%s>", str);

    return std::make_tuple(isLink, key, str.substr(next));
}


/******************************************************************************/
/* LOAD                                                                       */
/******************************************************************************/

void load(Config& cfg, const Path& path, std::istream& json);



void loadNull(Config&, const Path&) {}

void loadBool(Config& cfg, const Path& path, json::Token token)
{
    cfg.set(path, Value(token.boolValue()));
}

void loadNumber(Config& cfg, const Path& path, json::Token token)
{
    cfg.set(path, Value(token.floatValue()));
}

void loadString(Config& cfg, const Path& path, json::Token token)
{
    cfg.set(path, Value(token.stringValue()));
}

void loadArray(Config&, const Path&, std::istream&)
{
    reflectError("config doesn't currently support arrays");
}

void loadLink(Config& cfg, const Path& path, std::istream& json)
{
    auto token = json::nextToken(json);
    json::expectToken(token, json::Token::String);

    cfg.link(path, token.stringValue());
}

void loadObject(Config& cfg, const Path& path, std::istream& json)
{
    json::Token token = json::nextToken(json);
    if (token.type() == json::Token::ObjectEnd) return;

    while (json) {

        expectToken(token, json::Token::String);
        expectToken(json::nextToken(json), json::Token::KeySeparator);

        bool isLink;
        std::string key, type;
        std::tie(isLink, key, type) = parseKey(token.stringValue());

        Path sub(path, key);

        if (!type.empty()) {
            if (!path.empty())
                reflectError("type specifier <%s> only allowed at root", type);

            cfg.set(sub, reflect::type(type)->alloc());
        }

        if (isLink) loadLink(cfg, sub, json);
        else load(cfg, sub, json);


        token = json::nextToken(json);
        if (token.type() == json::Token::Separator) {
            token = json::nextToken(json);
            continue;
        }

        json::expectToken(token, json::Token::ObjectEnd);
        return;
    }

    reflectError("unexpected end of object");
}


void load(Config& cfg, const Path& path, std::istream& json)
{
    auto token = json::nextToken(json);

    switch(token.type())
    {
    case json::Token::Null: loadNull(cfg, path); break;
    case json::Token::Bool: loadBool(cfg, path, token); break;
    case json::Token::Number: loadNumber(cfg, path, token); break;
    case json::Token::String: loadString(cfg, path, token); break;
    case json::Token::ArrayStart: loadArray(cfg, path, json); break;
    case json::Token::ObjectStart: loadObject(cfg, path, json); break;

    default: reflectError("unexpected token<%s>", print(token.type()));
    }
}

} // namespace anonymous


void loadJson(Config& cfg, std::istream& json)
{
    load(cfg, Path(), json);
}

void loadJson(Config& cfg, const std::string& json)
{
    std::stringstream ss(json);
    loadJson(cfg, ss);
}


/******************************************************************************/
/* SAVE                                                                       */
/******************************************************************************/

void saveJson(const Config&, std::ostream&, const std::string&)
{
    reflectError("saveJson is not implemented yet");
}

std::string saveJson(const Config& cfg, const std::string& trait)
{
    std::stringstream ss;
    saveJson(cfg, ss, trait);
    return ss.str();
}

} // namespace config
} // namespace reflect
