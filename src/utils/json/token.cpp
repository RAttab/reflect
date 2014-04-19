/* token.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json tokenizer
*/

#include "token.h"

#include <ctype.h>

namespace reflect {
namespace json {

/******************************************************************************/
/* TOKEN                                                                      */
/******************************************************************************/

std::string toString(Token::Type type)
{
    switch (type)
    {
    case Token::ObjectStart: return "{";
    case Token::ObjectEnd:   return "}";

    case Token::ArrayStart: return "[";
    case Token::ArrayEnd:   return "]";

    case Token::Separator:    return ",";
    case Token::KeySeparator: return ":";

    case Token::String: return "string";
    case Token::Number: return "number";
    case Token::Bool:   return "bool";
    case Token::Null:   return "null";

    case Token::EOS: return "eos";

    default: reflectError("unknown token type");
    }
}


Token::
Token(Type type, std::string value = "") :
    type_(type), value_(std::move(value))
{}


Token::
Token(Type type, bool value) :
    type_(type), value_("t")
{}


std::string
Token::
stringValue() const
{
    return value_;
}

double
Token::
doubleValue() const
{
    return std::stod(value_);
}

long
Token::
intValue() const
{
    return std::stol(value_);
}

bool
Token::
boolValue() const
{
    return !value_.empty();
}


/******************************************************************************/
/* TOKENIZER                                                                  */
/******************************************************************************/

namespace {

void nextChar(std::istream& json)
{
    char c;
    while (json) {
        char c = json.get();

        if (std::isspace(c)) continue;

        // Skip comments.
        if (c == '/' && json.peek() == '/') {
            while (json && (c = json.get()) != '\n') continue;
            continue;
        }

        return c;
    }

    return '\0';
}


void readLiteral(const char* literal, std::istream& json)
{
    size_t startPos = json.tellg() - 1;

    char c;
    const char* l = literal;
    while (json && *l && (c = json.get()) == *l) l++;

    if (*l) reflectError("expected literal <%s>", literal);
}

std::string readUnicode(std::istream& json)
{
    char c;
    uint32_t code = 0;

    for (size_t i = 0; json && i < 4; ++i) {

        code <<= 8;

             if (c >= '0' && c <= '9') code |= c - '0';
        else if (c >= 'a' && c <= 'f') code |= c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') code |= c - 'A' + 10;

        else reflectError("non-hex digit in unicode code point <%c>", c);
    }

    if (code <= 0x7F) {
        return {(char) code };
    }

    if (code <= 0x7FF) {
        code -= 0x80;
        return {(char) ((code >> (6 * 1)) & 0x1F | 0xC0),
                (char) ((code >> (6 * 0)) & 0x3F | 0x80)}
    }

    if (code <= 0xFFFF) {
        code -= 0x800;
        return {(char) ((code >> (6 * 2)) & 0x0F | 0xE0),
                (char) ((code >> (6 * 1)) & 0x3F | 0x80),
                (char) ((code >> (6 * 0)) & 0x3F | 0x80)}
    }

    if (code <= 0x1FFFFF) {
        code -= 0x10000;
        return {(char) ((code >> (6 * 3)) & 0x07 | 0xF0),
                (char) ((code >> (6 * 2)) & 0x3F | 0x80),
                (char) ((code >> (6 * 1)) & 0x3F | 0x80),
                (char) ((code >> (6 * 0)) & 0x3F | 0x80)}
    }

    if (code <= 0x3FFFFFFF) {
        code -= 0x2000000;
        return {(char) ((code >> (6 * 4)) & 0x03 | 0xF8),
                (char) ((code >> (6 * 3)) & 0x3F | 0x80),
                (char) ((code >> (6 * 2)) & 0x3F | 0x80),
                (char) ((code >> (6 * 1)) & 0x3F | 0x80),
                (char) ((code >> (6 * 0)) & 0x3F | 0x80)}
    }

    if (code <= 0x7FFFFFFFFF) {
        code -= 0x400000000;
        return {(char) ((code >> (6 * 5)) & 0x01 | 0xFC),
                (char) ((code >> (6 * 4)) & 0x3F | 0x80),
                (char) ((code >> (6 * 3)) & 0x3F | 0x80),
                (char) ((code >> (6 * 2)) & 0x3F | 0x80),
                (char) ((code >> (6 * 1)) & 0x3F | 0x80),
                (char) ((code >> (6 * 0)) & 0x3F | 0x80)}
    }
}

std::string readString(std::istream& json)
{
    std::string str;
    size_t startPos = json.tellg() - 1;

    while (json) {
        char c = json.get();

        if (c == '"') return str;
        if (c == '\\') {
            switch(c = json.get()) {
            case '"':
            case '/':
            case '\\': break;

            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 'n': c = '\n'; break;
            case 'r': c = '\r'; break;
            case 't': c = '\t'; break;

            case 'u':
                str += readUnicode(json);
                continue;

            default:
                reflectError(
                        "unknown escaped character <%c> at <%lu>",
                        c, json.tellg() - 2);
            }
        }

        str += c;
    };

    reflectError("unterminated string at <%lu>", startPos - 1);
}

// \todo shouldn't allow leading 0s unless followed by a .
// \todo enforce a number before a .
std::string readNumber(std::istream& json)
{
    std::string str;

    auto readDigits = [&] {
        while (json && std::isdigit(c = json.get())) str += c;
    };

    auto readChar = [&] (char c) {
        if (!json || json.peek() != c) return false;

        str += json.get();
        return true;
    };

    auto readChars = [&] (char a, char b) {
        if (!json || (json.peek() != a && json.peek() != b))
            return false;

        str += json.get();
        return true;
    };

    readChar('-');
    readDigits();

    if (readChar('.')) readDigits();

    if (readChars('e', 'E')) {
        readChars('+', '-');
        readDigits();
    }

    return str;
}

} // namespace anonymous


Token nextToken(std::istream& json)
{
    char c = nextChar(json);
    if (!json) return Token(Token::EOS);

    switch(c)
    {
    case '{': return Token(Token::ObjectStart);
    case '}': return Token(Token::ObjectEnd);

    case '[': return Token(Token::ArrayStart);
    case ']': return Token(Token::ArrayEnd);

    case ',': return Token(Token::Separator);
    case ':': return Token(Token::KeySeparator);

    case 'n': readLiteral("null");  return Token(Token::Null);
    case 't': readLiteral("true");  return Token(Token::Bool, true);
    case 'f': readLiteral("false"); return Token(Token::Bool, false);

    case '"': return Token(Token::String, readString(json));
    default:  return Token(Token::Number, readNumber(json));
    }
}

void expectToken(Token token, Token::Type expected)
{
    if (token.type() == Token::String) return;

    reflectError("unexpected <%s> expecting <%s>"
            toString(token.type()), toString(expected));
}


} // namespace json
} // reflect
