/* printer.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

namespace reflect {
namespace json {


/******************************************************************************/
/* GENERIC PRINTER                                                            */
/******************************************************************************/

template<typename Keys, typename Fn>
void printObject(Writer& writer, const Keys& keys, const Fn& fn)
{
    writer.push('{');

    if (keys.size() > 1) {
        writer.indent();
        writer.newline();
    }
    else writer.space();

    bool first = true;
    for (const auto& key : keys) {
        if (!writer) return;

        if (!first) {
            writer.push(',');
            writer.newline();
        }
        first = false;

        printString(writer, key);
        writer.push(':');
        writer.space();
        fn(key);
    }

    if (keys.size() > 1) {
        writer.unindent();
        writer.newline();
    }
    else writer.space();

    writer.push('}');
}

template<typename Fn>
void printArray(Writer& writer, size_t n, const Fn& fn)
{
    writer.push('[');

    if (n > 1) {
        writer.indent();
        writer.newline();
    }
    else writer.space();

    bool first = true;
    for (size_t i = 0; i < n; ++i) {
        if (!writer) return;

        if (!first) {
            writer.push(',');
            writer.newline();
        }
        first = false;

        fn(i);
    }

    if (n > 1) {
        writer.unindent();
        writer.newline();
    }
    else writer.space();

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
