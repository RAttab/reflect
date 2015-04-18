/* token.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply
*/

namespace reflect {
namespace json {

/******************************************************************************/
/* TOKEN                                                                      */
/******************************************************************************/

Token::
Token(Type type, const std::string& value) :
    type_(type), value_(&value)
{}

namespace  {
std::string trueValue = "true";
std::string falseValue = "false";
}

Token::
Token(Type type, bool value) :
    type_(type), value_(value ? &trueValue : &falseValue)
{}

bool
Token::
asBool() const
{
    return value_ == &trueValue;
}

int64_t
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
    case Token::NoToken: return "notok";

    case Token::ObjectStart: return "{";
    case Token::ObjectEnd:   return "}";

    case Token::ArrayStart: return "[";
    case Token::ArrayEnd:   return "]";

    case Token::Separator:    return ",";
    case Token::KeySeparator: return ":";

    case Token::String: return "string";
    case Token::Int:    return "int";
    case Token::Float:  return "float";
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
    if (value_) ss << ": " << *value_;
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
        if (c == '\n') reader.newline();

        if (c == '/' && reader.peek() == '/') {
            if (!reader.allowComments())
                reader.error("comments are not allowed");

            while (reader && (c = reader.pop()) != '\n') continue;
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
    while (reader && *l && (c = reader.pop()) == *l) l++;

    if (*l) reader.error("expected literal <%s>", literal);
}

void unescapeUnicode(Reader& reader)
{
    uint32_t code = 0;

    size_t i = 0;
    for (i = 0; reader && i < 4; ++i) {

        char c = reader.pop();
        code <<= 4;

             if (c >= '0' && c <= '9') code |= c - '0';
        else if (c >= 'a' && c <= 'f') code |= c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') code |= c - 'A' + 10;
        else reader.error("non-hex digit in unicode code point <%c>", c);
    }

    if (reader && i != 4) reader.error("\\u requires 4 hex digits", i);
    if (!reader) return;

    auto encode = [&] (unsigned pos, uint32_t mask, uint32_t head) {
        reader.save(((code >> (6 * pos)) & mask) | head);
    };

    if (code <= 0x7F)
        reader.save(code);

    else if (code <= 0x7FF) {
        encode(1, 0x1F, 0xC0);
        encode(0, 0x3F, 0x80);
    }

    else if (code <= 0xFFFF) {
        encode(2, 0x0F, 0xE0);
        encode(1, 0x3F, 0x80);
        encode(0, 0x3F, 0x80);
    }

    else if (code <= 0x1FFFFF) {
        encode(3, 0x07, 0xF0);
        encode(2, 0x3F, 0x80);
        encode(1, 0x3F, 0x80);
        encode(0, 0x3F, 0x80);
    }

    else reflectError("code point <%u> is too big", code);
}

void readUnicode(Reader& reader)
{
    if (reader.unescapeUnicode()) {
        unescapeUnicode(reader);
        return;
    }

    reader.save('\\');
    reader.save('u');

    size_t i = 0;
    for (i = 0; reader && i < 4; ++i) {
        char c = reader.pop();

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
    if (bytes > 4) reader.error("invalid UTF-8 header: %x", c);

    uint32_t mask = (1 << (7 - bytes)) - 1;
    uint32_t code = uint32_t(c) & mask;

    for (size_t i = 1; reader && i < bytes; i++) {
        reader.save(c = reader.pop());
        if ((c & 0xC0) != 0x80) reader.error("invalid UTF-8 encoding");
        code = (code << 6) | (c & 0x3F);
    }

    switch (bytes) {
    case 4: if (code > 0xFFFF) return; break;
    case 3: if (code > 0x7FF) return; break;
    case 2: if (code > 0x7F) return; break;
    }

    reader.error("invalid UTF-8 encoding");
}

void readString(Reader& reader)
{
    reader.resetBuffer();

    while (reader) {
        char c = reader.pop();

        if (c == '\n') {
            reader.error("invalid \\n character in a string");
            continue;
        }

        if ((c & 0x80) && reader.validateUnicode()) {
            validateUnicode(reader, c);
            continue;
        }

        if (c == '"') return;
        if (c == '\\') {
            switch(c = reader.pop()) {
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

Token::Type readNumber(Reader& reader, char c)
{
    Token::Type type = Token::Int;
    reader.resetBuffer();
    reader.save(c); // can be either - or [0-9].

    auto readDigits = [&] {
        while (reader && std::isdigit(c = reader.peek()))
            reader.save(reader.pop());
    };

    auto readChar = [&] (char c) {
        if (!reader || reader.peek() != c) return false;

        reader.save(reader.pop());
        return true;
    };

    auto readChars = [&] (char a, char b) {
        if (!reader || (reader.peek() != a && reader.peek() != b))
            return false;

        reader.save(reader.pop());
        return true;
    };

    readDigits();
    if (readChar('.')) {
        type = Token::Float;
        readDigits();
    }

    if (readChars('e', 'E')) {
        type = Token::Float;
        readChars('+', '-');
        readDigits();
    }

    return type;
}

} // namespace anonymous

namespace details {

Token nextToken(Reader& reader)
{
    if (!reader) return Token(Token::EOS);

    char c = nextChar(reader);
    if (!reader) return Token(Token::EOS);

    switch(c)
    {
    case '{': return Token(Token::ObjectStart);
    case '}': return Token(Token::ObjectEnd);

    case '[': return Token(Token::ArrayStart);
    case ']': return Token(Token::ArrayEnd);

    case ',': return Token(Token::Separator);
    case ':': return Token(Token::KeySeparator);

    case 'n': readLiteral(reader, "ull");  return Token(Token::Null);
    case 't': readLiteral(reader, "rue");  return Token(Token::Bool, true);
    case 'f': readLiteral(reader, "alse"); return Token(Token::Bool, false);

    case '"':
        readString(reader);
        return Token(Token::String, reader.buffer());

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
    {
        Token::Type type = readNumber(reader, c);
        return Token(type, reader.buffer());
    }
    default:
        reader.error("unexpected character <%c>", c);
        return Token(Token::EOS);
    }
}

} // namespace details
} // namespace json
} // reflect
