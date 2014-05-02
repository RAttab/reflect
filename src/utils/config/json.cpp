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

using namespace json;

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
void load(Config& cfg, const Path& path, const Token& token, std::istream& json);

void loadLink(Config& cfg, const Path& path, std::istream& json);
void loadLink(Config& cfg, const Path& path, const Token& token, std::istream& json);


void loadNull(Config&, const Path&) {}

void loadBool(Config& cfg, const Path& path, Token token)
{
    cfg.set(path, Value(token.boolValue()));
}

void loadNumber(Config& cfg, const Path& path, Token token)
{
    cfg.set(path, Value(token.floatValue()));
}

void loadString(Config& cfg, const Path& path, Token token)
{
    cfg.set(path, Value(token.stringValue()));
}


void initArray(Config& cfg, const Path& path, const Token& token)
{
    const Type* tVector = nullptr;

    switch(token.type())
    {
    case Token::Number: tVector = type< std::vector<double> >(); break;
    case Token::String: tVector = type< std::vector<std::string> >(); break;
    default:
        reflectError("unexpected token <%s> for untyped array <%s>",
                token.print(), path.toString());
    }

    cfg.set(path, tVector->construct());
}

void loadArray(Config& cfg, const Path& path, std::istream& json)
{
    Token token = nextToken(json);
    if (token.type() == Token::ArrayEnd) return;

    if (path.size() == 1) initArray(cfg, path, token);

    for (size_t i = 0; json; ++i) {

        load(cfg, Path(path, i), token, json);

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


void loadLinkString(Config& cfg, const Path& path, Token token)
{
    cfg.link(path, token.stringValue());
}

void loadLinkArray(Config& cfg, const Path& path, std::istream& json)
{
    Token token = nextToken(json);
    if (token.type() == Token::ArrayEnd) return;

    for (size_t i = 0; json; ++i) {

        loadLink(cfg, Path(path, i), token, json);

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

void loadLink(Config& cfg, const Path& path, const Token& token, std::istream& json)
{
    switch (token.type())
    {
    case Token::String: loadLinkString(cfg, path, token); break;
    case Token::ArrayStart: loadLinkArray(cfg, path, json); break;
    default:
        reflectError("unexpected link token <%s> in <%s>",
                token.print(), path.toString());
    }
}

void loadLink(Config& cfg, const Path& path, std::istream& json)
{
    loadLink(cfg, path, nextToken(json), json);
}

void loadObject(Config& cfg, const Path& path, std::istream& json)
{
    Token token = nextToken(json);
    if (token.type() == Token::ObjectEnd) return;

    while (json) {

        expectToken(token, Token::String);
        expectToken(nextToken(json), Token::KeySeparator);

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


void load(Config& cfg, const Path& path, const Token& token, std::istream& json)
{
    switch(token.type())
    {
    case Token::Null: loadNull(cfg, path); break;
    case Token::Bool: loadBool(cfg, path, token); break;
    case Token::Number: loadNumber(cfg, path, token); break;
    case Token::String: loadString(cfg, path, token); break;
    case Token::ArrayStart: loadArray(cfg, path, json); break;
    case Token::ObjectStart: loadObject(cfg, path, json); break;

    default: reflectError("unexpected token <%s>", token.print());
    }
}

void load(Config& cfg, const Path& path, std::istream& json)
{
    load(cfg, path, nextToken(json), json);

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
