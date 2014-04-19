/* token.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json tokenizer
*/

#include "token.h"

#include <ctype>

namespace reflect {
namespace json {

/******************************************************************************/
/* TOKEN                                                                      */
/******************************************************************************/

std::string toString(Token::Type type)
{

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

}

double
Token::
doubleValue() const
{

}

long
Token::
intValue() const
{

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

            case 'u': reflectError("\\u is not currently supported");

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


} // namespace json
} // reflect
