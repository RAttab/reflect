/* parser.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json parser
*/

#pragma once

#include "reflect.h"

namespace reflect {
namespace json {


/******************************************************************************/
/* PARSE INTO                                                                 */
/******************************************************************************/

void parseInto(Value& value, std::istream& json);
void parseInto(Value& value, const std::string& json);

template<typename T>
void parseInto(T& value, std::istream& json)
{
    parseInto(Value(value), json);
}

template<typename T>
void parseInto(T& value, const std::string& json)
{
    parseInto(Value(value), json);
}


/******************************************************************************/
/* PARSE                                                                      */
/******************************************************************************/

Value parse(const Type* type, std::istream& json);
Value parse(const Type* type, const std::string& json);

template<typename T>
Value parse(std::istream& json)
{
    return parse(type<T>(), json);
}

template<typename T>
Value parse(const std::string& json)
{
    return parse(type<T>(), json);
}


} // namespace json
} // reflect
