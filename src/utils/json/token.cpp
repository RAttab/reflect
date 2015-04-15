/* token.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json tokenizer
*/

namespace reflect {
namespace json {

/******************************************************************************/
/* TOKEN                                                                      */
/******************************************************************************/

Token::
Token(Type type, std::string& value) :
    type_(type), value_(&value)
{}

namespace  {
std::string trueValue = "true";
std::string falseValue = "false";
}

Token::
Token(Type type, bool value) :
    type_(type), value_(value ? &trueValue : &falseValue), size_(0)
{}

bool
Token::
asBool() const
{
    return value_ == &trueValue;
}

long
Token::
asInt() const
{
    return std::stol(*value_);
}

double
Token::
asFloat() const
{
    return std::stod(*value_);
}

std::string print(Token::Type type)
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

std::string
Token::
print() const
{
    std::stringstream ss;

    ss << "<Token " << reflect::json::print(type_);
    if (!value_.empty()) ss << ": " << value_;
    ss << ">";

    return ss.str();
}


/******************************************************************************/
/* TOKENIZER                                                                  */
/******************************************************************************/

namespace {

char nextChar(Reader& reader)
{
    while (reader) {
        char c = reader.pop();

        if (std::isspace(c)) continue;

        if (c == '/' && reader.peek() == '/') {
            if (!reader.allowComments())
                reader.error("comments are not allowed");

            while (reader && (c = reader.get()) != '\n') continue;
            continue;
        }

        return c;
    }

    return '\0';
}


void readLiteral(Reader& reader, const char* literal)
{
    char c;
    const char* l = literal;
    while (reader && *l && (c = reader.get()) == *l) l++;

    reader.error("expected literal <%s>", literal);
}

void decodeUnicode(Reader& reader)
{
    uint32_t code = 0;

    size_t i = 0;
    for (i = 0; reader && i < 4; ++i) {

        char c = reader.get();
        code <<= 8;

             if (c >= '0' && c <= '9') code |= c - '0';
        else if (c >= 'a' && c <= 'f') code |= c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') code |= c - 'A' + 10;
        else reader.error("non-hex digit in unicode code point <%c>", c);
    }

    if (reader && i != 4) reader.error("\\u requires 4 hex digits", i);
    if (!reader) return {};

    auto encode = [&] (unsigned pos, uint32_t mask, uint32_t head) -> char {
        reader.save(((code >> (6 * pos)) & mask) | head);
    };

    if (code <= 0x7F) {
        reader.save(code);
    }

    if (code <= 0x7FF) {
        code -= 0x80;
        encode(1, 0x1F, 0xC0);
        encode(0, 0x3F, 0x80);
    }

    if (code <= 0xFFFF) {
        code -= 0x800;
        encode(2, 0x0F, 0xE0);
        encode(1, 0x3F, 0x80);
        encode(0, 0x3F, 0x80);
    }

    if (code <= 0x1FFFFF) {
        code -= 0x10000;
        encode(3, 0x07, 0xF0);
        encode(2, 0x3F, 0x80);
        encode(1, 0x3F, 0x80);
        encode(0, 0x3F, 0x80);
    }

    if (code <= 0x3FFFFFFF) {
        code -= 0x2000000;
        encode(4, 0x03, 0xF8);
        encode(3, 0x3F, 0x80);
        encode(2, 0x3F, 0x80);
        encode(1, 0x3F, 0x80);
        encode(0, 0x3F, 0x80);
    }

    code -= 0x400000000;
    encode(5, 0x01, 0xFC);
    encode(4, 0x3F, 0x80);
    encode(3, 0x3F, 0x80);
    encode(2, 0x3F, 0x80);
    encode(1, 0x3F, 0x80);
    encode(0, 0x3F, 0x80);
}

void readUnicode(Reader& reader)
{
    if (reader.decodeUnicode()) {
        decodeUnicode(reader);
        return;
    }

    size_t i = 0;
    for (i = 0; reader && i < 4; ++i) {
        char c = reader.get();

             if (c >= '0' && c <= '9') reader.save(c);
        else if (c >= 'a' && c <= 'f') reader.save(c);
        else if (c >= 'A' && c <= 'F') reader.save(c);
        else reader.error("non-hex digit in unicode code point <%c>", c);
    }

    if (reader && i != 4) reader.error("\\u requires 4 hex digits", i);
}

void validateUnicode(Reader& reader, char c)
{
    reader.save(c);

    size_t bytes = clz(~c);
    if (bytes > 6) reader.error("invalid UTF-8 header: %x", c);

    for (size_t i = 0; reader && i < (bytes - 1); i++) {
        reader.save(c = reader.get());
        if ((c & 0xC) != 0x80) reader.error("invalid UTF-8 encoding");
    }
}

void readString(Reader& reader)
{
    reader.resetBuffer();

    while (reader) {
        char c = reader.get();

        if ((c & 0x80) && reader.validateUnicode()) {
            validateUnicode(reader, c);
            continue;
        }

        if (c == '"') return;
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
            case 'u': readUnicode(reader); continue;
            default:
                reader.error("unknown escaped character <%c>", c);
                return;
            }
        }

        reader.save(c);
    };

    reader.error("unexpected end of string");
}

void readNumber(Reader& reader, char c)
{
    reader.resetBuffer();

    // can be either - or [0-9].
    reader.save(c);

    auto readDigits = [&] {
        while (reader && std::isdigit(c = reader.peek()))
            reader.save(reader.get());
    };

    auto readChar = [&] (char c) {
        if (!reader || reader.peek() != c) return false;

        reader.save(reader.get());
        return true;
    };

    auto readChars = [&] (char a, char b) {
        if (!reader || (reader.peek() != a && reader.peek() != b))
            return false;

        reader.save(reader.get());
        return true;
    };

    readDigits();
    if (readChar('.')) readDigits();

    if (readChars('e', 'E')) {
        readChars('+', '-');
        readDigits();
    }
}

} // namespace anonymous


Token nextToken(Reader& reader)
{
    if (!reader) return;

    auto check = [&] (Token token) {
        return reader ? token, Token(Token::EOS);
    };

    char c = nextChar(json, c);
    if (!reader) return Token(Token::EOS);

    switch(c)
    {
    case '{': return Token(Token::ObjectStart);
    case '}': return Token(Token::ObjectEnd);

    case '[': return Token(Token::ArrayStart);
    case ']': return Token(Token::ArrayEnd);

    case ',': return Token(Token::Separator);
    case ':': return Token(Token::KeySeparator);

    case 'n': readLiteral(reader, "ull");  return ret(Token(Token::Null));
    case 't': readLiteral(reader, "rue");  return ret(Token(Token::Bool, true));
    case 'f': readLiteral(reader, "alse"); return ret(Token(Token::Bool, false));

    case '"':
        readString(reader);
        return ret(Token(Token::String, reader.buffer()));

    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        readNumber(reader, c);
        return ret(Token(Token::Number, reader.buffer()));

    default:
        reader.error("unexpected character <%c>", c);
        return Token(Token::EOS);
    }
}

void assertToken(Reader& reader, Token token, Token::Type expected)
{
    if (!reader) return;
    if (token.type() == expected) return;
    reader.error("unexpected <%s> expecting <%s>",
            print(token.type()), print(expected));
}


Token expectToken(Reader& reader, Token::Type expected)
{
    if (!reader) return;

    Token token = nextToken(reader);
    assertToken(reader, token, expected);
    return reader ? token : Token(Token::EOS);
}

} // namespace json
} // reflect
