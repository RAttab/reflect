/* printer.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json printer.
*/

#pragma once

#include "reflect.h"

namespace reflect {
namespace json {


/******************************************************************************/
/* PRINT                                                                      */
/******************************************************************************/

std::string print(Value& value, bool pretty = false);
void print(const Value& value, std::ostream& json, bool pretty = false);

template<typename T>
void print(const T& value, std::ostream& json, bool pretty = false)
{
    print(Value(value), json, pretty);
}

template<typename T>
std::string print(const T& value, bool pretty = false)
{
    return print(Value(value), pretty);
}


} // namespace json
} // reflect
