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

void printNull(Writer& writer)
{
    formatNull(writer);
}

template<typename T>
void printBool(Writer& writer, const T& value)
{
    formatBool(writer, cast<bool>(value));
}

template<typename T>
void printInt(Writer& writer, const T& value)
{
    formatInt(writer, cast<int32_t>(value));
}

template<typename T>
void printFloat(Writer& writer, const T& value)
{
    formatFloat(writer, cast<double>(value));
}

template<typename T>
void printString(Writer& writer, const T& value)
{
    formatFloat(writer, cast<std::string>(value));
}

template<typename T>
void printObject(Writer& writer, const T& keys, const Fn& fn)
{
    writer.put('{');
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
    writer.put('}');
}

template<typename T, typename Fn>
void printArray(Writer& writer, size_t n, const Fn& fn)
{
    writer.put('[');
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
    writer.put(']');
}


/******************************************************************************/
/* VALUE PRINTER                                                              */
/******************************************************************************/

template<typename T>
Error print(Writer& writer, const T& value)
{
    Value value = cast<Value>(value);
    print(reader, value);
    return reader.error();
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
