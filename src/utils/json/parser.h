/* parser.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Parser interface.
*/

#pragma once

namespace reflect {
namespace json {


/******************************************************************************/
/* PARSER FN                                                                  */
/******************************************************************************/

template<typename T> void parseBool(Reader& reader, T& value);
template<typename T> void parseInt(Reader& reader, T& value);
template<typename T> void parseFloat(Reader& reader, T& value);
template<typename T> void parseString(Reader& reader, T& value);
template<typename Fn> void parseObject(Reader& reader, const Fn& fn);
template<typename Fn> void parseArray(Reader& reader, const Fn& fn);

void parse(Reader& reader, Value& value);

} // namespace json
} // namespace reflect
