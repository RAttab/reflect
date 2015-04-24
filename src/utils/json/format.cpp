/* format.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

namespace reflect {
namespace json {

/******************************************************************************/
/* FORMATTER                                                                  */
/******************************************************************************/

void formatNull(Writer& writer)
{
    static const std::string sNull = "null";
    writer.push(sNull);
}

void formatBool(Writer& writer, bool value)
{
    static const std::string sTrue = "true";
    static const std::string sFalse = "false";

    if (value) writer.push(sTrue);
    else writer.push(sFalse);
}

namespace {

template<typename T>
void format(Writer& writer, const char* format, T value)
{
    auto& buffer = writer.buffer();

    size_t n = snprintf(buffer.data(), buffer.size(), format, value);
    if (n == buffer.size()) {
        writer.error("buffer too small to format value");
        return;
    }

    writer.push(buffer.data(), n);
}

} // namespace anonymous

void formatInt(Writer& writer, int64_t value)
{
    format(writer, "%ld", value);
}

void formatFloat(Writer& writer, double value)
{
    format(writer, "%1.12g", value);
}

size_t escapeUnicode(Writer& writer, const std::string& value, size_t i)
{
    size_t bytes = clz(~value[i]);
    if (bytes > 4 || bytes < 2) writer.error("invalid UTF-8 header");
    if (i + bytes > value.size()) writer.error("invalid UTF-8 encoding");

    size_t leftover = 8 - (bytes + 1);
    uint32_t code = value[i] & ((1 << leftover) - 1);

    i++;
    for (size_t j = 0; writer && j < (bytes - 1); ++j, ++i) {
        if ((value[i] & 0xC0) != 0x80) writer.error("invalid UTF-8 encoding");
        code = (code << 6) | (value[i] & 0x3F);
    }

    format(writer, "\\u%04x", code);

    return i - 1;
}

size_t readUnicode(Writer& writer, const std::string& value, size_t i)
{
    size_t bytes = clz(~value[i]);
    if (bytes > 4 || bytes < 2) writer.error("invalid UTF-8 header");
    if (i + bytes > value.size()) writer.error("invalid UTF-8 encoding");

    size_t leftover = 8 - (bytes + 1);
    uint32_t code = value[i] & ((1 << leftover) - 1);
    writer.push(value[i]);

    i++;
    for (size_t j = 0; writer && j < (bytes - 1); ++j, ++i) {
        if ((value[i] & 0xC0) != 0x80) writer.error("invalid UTF-8 encoding");
        code = (code << 6) | (value[i] & 0x3F);
        writer.push(value[i]);
    }

    switch (bytes) {
    case 4: if (code <= 0xFFFF) writer.error("invalid UTF-8 encoding"); break;
    case 3: if (code <= 0x7FF) writer.error("invalid UTF-8 encoding"); break;
    case 2: if (code <= 0x7F) writer.error("invalid UTF-8 encoding"); break;
    }

    return i - 1;
}

void formatString(Writer& writer, const std::string& value)
{
    writer.push('"');

    for (size_t i = 0; i < value.size(); ++i) {
        char c = value[i];

        if (c & 0x80) {
            if (writer.escapeUnicode()) {
                i = escapeUnicode(writer, value, i);
                continue;
            }
            else if (writer.validateUnicode()) {
                i = readUnicode(writer, value, i);
                continue;
            }
        }

        switch (c) {
        case '"': writer.push("\\\"", 2); break;
        case '/': writer.push("\\/", 2); break;
        case '\\': writer.push("\\\\", 2); break;

        case '\b': writer.push("\\b", 2); break;
        case '\f': writer.push("\\f", 2); break;
        case '\n': writer.push("\\n", 2); break;
        case '\r': writer.push("\\r", 2); break;
        case '\t': writer.push("\\t", 2); break;
        default: writer.push(c);
        }
    }

    writer.push('"');
}


} // namespace json
} // namespace reflect
