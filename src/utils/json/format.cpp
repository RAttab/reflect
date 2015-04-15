/* format.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Formatter implementation.
*/

namespace reflect {
namespace json {

/******************************************************************************/
/* FORMATTER                                                                  */
/******************************************************************************/

void formatNull(Writer& writer)
{
    static const std::string sNull = "null";
    writer.write(sNull);
}

void formatBool(Writer& writer, bool value)
{
    static const std::string sTrue = "true";
    static const std::string sFalse = "false";

    if (value) writer.write(sTrue);
    else writer.write(sFalse);
}

namespace {

template<typename T>
void format(Writer& writer, const char* format, T value)
{
    auto& buffer = writer.buffer();

    size_t n = snprintf(buffer.c_str(), buffer.size(), format, value);
    if (n == buffer.size()) {
        writer.error("buffer too small to format value");
        return;
    }

    buffer.push(buffer.c_str(), n);
}

} // namespace anonymous

void formatInt(Writer& writer, int64_t value)
{
    format(writer, "%ld", value);
}

void formatFloat(Writer& writer, double value)
{
    format(writer, "%g", value);
}

size_t escapeUnicode(Writer& writer, const std::string& value, size_t i)
{
    uint32_t code = 0;

    size_t bytes = clz(~value[i]);
    if (bytes > 6) reader.error("invalid UTF-8 header");
    if (i + bytes >= value.size()) reader.error("invalid UTF-8 encoding");

    uint8_t leftover = 8 - (bytes + 1);
    uint32_t code = value[i] & ((1 << leftover) - 1);

    i++;
    for (; writer && i < (bytes - 1); ++i) {
        if ((value[i] & 0xC0) != 0x80) reader.error("invalid UTF-8 encoding");
        code = (code << 6) | (value[i] & 0x3F);
    }

    format(writer, "\\u%4x", code);
}

void formatString(Writer& writer, const std::string& value)
{
    writer.push('"');

    for (size_t i = 1; i < value.size(); ++i) {
        char c = value[i];
        
        if ((c & 0x80) && writer.escapeUnicode()) {
            i = escapeUnicode(writer, value, i);
            continue;
        }

        switch (c) {
        case '"': writer.push("\\\"", 2);
        case '/': writer.push("\\/", 2);
        case '\\': writer.push("\\\\", 2);

        case '\b': writer.push("\\b", 2);
        case '\f': writer.push("\\f", 2);
        case '\n': writer.push("\\n", 2);
        case '\r': writer.push("\\r", 2);
        case '\t': writer.push("\\t", 2);
        default: writer.push(c);
        }
    }

    writer.push('"');
}


} // namespace json 
} // namespace reflect
