/* parser.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {


/******************************************************************************/
/* GENERIC PARSER                                                             */
/******************************************************************************/

inline bool parseBool(Reader& reader);
inline int64_t parseInt(Reader& reader);
inline double parseFloat(Reader& reader);
inline std::string parseString(Reader& reader);
template<typename Fn> void parseObject(Reader& reader, const Fn& fn);
template<typename Fn> void parseArray(Reader& reader, const Fn& fn);


/******************************************************************************/
/* VALUE PARSER                                                               */
/******************************************************************************/

void parse(Reader& reader, Value& value);
template<typename T> void parse(Reader& reader, T& value);
template<typename T> Error parse(std::istream& stream, T& value);
template<typename T> Error parse(const std::string& str, T& value);

} // namespace json
} // namespace reflect
