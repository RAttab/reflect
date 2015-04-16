/* printer.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Printer interface.
*/

#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* GENERIC PRINTER                                                            */
/******************************************************************************/

inline void printNull(Writer& writer);
inline void printBool(Writer& writer, bool value);
inline void printInt(Writer& writer, int64_t value);
inline void printFloat(Writer& writer, double value);
inline void printString(Writer& writer, const std::string& value);

inline void printBool(Writer& writer, Value& value);
inline void printInt(Writer& writer, Value& value);
inline void printFloat(Writer& writer, Value& value);
inline void printString(Writer& writer, Value& value);

template<typename Keys, typename Fn>
void printObject(Writer& writer, const Keys& keys, const Fn& fn);

template<typename Fn>
void printArray(Writer& writer, size_t n, const Fn& value);


/******************************************************************************/
/* VALUE PRINTER                                                              */
/******************************************************************************/

void print(Writer& writer, Value& value);
template<typename T> void print(Writer& writer, const T& value);
template<typename T> Error print(std::ostream& stream, const T& value);
template<typename T> std::pair<std::string, Error> print(const T& value);

} // namespace json
} // namespace reflect
