/* printer.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   printer template implementation.
*/

namespace reflect {
namespace json {


/******************************************************************************/
/* GENERIC PRINTER                                                            */
/******************************************************************************/

void printNull(Writer& writer) { formatNull(writer); }
void printBool(Writer& writer, bool value) { formatBool(writer, value); }
void printInt(Writer& writer, int64_t value) { formatInt(writer, value); }
void printFloat(Writer& writer, double value) { formatFloat(writer, value); }
void printString(Writer& writer, const std::string& value)
{
    formatString(writer, value);
}

void printBool(Writer& writer, const Value& value)
{
    formatBool(writer, value.cast<bool>());
}

void printInt(Writer& writer, const Value& value)
{
    formatInt(writer, value.cast<int64_t>());
}

void printFloat(Writer& writer, const Value& value)
{
    formatFloat(writer, value.cast<double>());
}

void printString(Writer& writer, const Value& value)
{
    formatString(writer, value.cast<std::string>());
}

template<typename Keys, typename Fn>
void printObject(Writer& writer, const Keys& keys, const Fn& fn)
{
    writer.push('{');
    writer.indent();
    writer.newline();

    bool first = true;
    for (const auto& key : keys) {
        if (!writer) return;

        if (!first) {
            writer.seperator(',');
            writer.newline();
        }
        first = false;

        printString(writer, key);
        writer.seperator(':');
        fn(key);
    }

    writer.newline();
    writer.unindent();
    writer.push('}');
}

template<typename Fn>
void printArray(Writer& writer, size_t n, const Fn& fn)
{
    writer.push('[');
    writer.indent();
    writer.newline();

    bool first = true;
    for (size_t i = 0; i < n; ++i) {
        if (!writer) return;

        if (!first) {
            writer.seperator(',');
            writer.newline();
        }
        first = false;

        fn(i);
    }

    writer.newline();
    writer.unindent();
    writer.push(']');
}


/******************************************************************************/
/* VALUE PRINTER                                                              */
/******************************************************************************/

template<typename T>
Error print(Writer& writer, const T& value)
{
    Value v = cast<Value>(value);
    print(writer, v);
    return writer.error();
}

template<typename T>
Error print(std::ostream& stream, const T& value)
{
    Writer writer(stream);
    return parse(stream, value);
}

template<typename T>
std::pair<std::string, Error> print(const T& value)
{
    std::ostringstream stream;
    Error err = print(stream, value);
    return std::make_pair(stream.str(), err);
}

} // namespace json
} // namespace reflect
