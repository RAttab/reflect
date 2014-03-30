/* int.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Integer reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* REFLECT INTEGRAL                                                           */
/******************************************************************************/

template<typename T>
struct Reflect<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
    static constexpr const char* id = TypeName<T>::name;

    static Type* create() { return new Type(id); }
    static void reflect(Type*) {}
};


/******************************************************************************/
/* TYPE NAMES                                                                 */
/******************************************************************************/

REFLECT_TYPE_NAME(         char);
REFLECT_TYPE_NAME(unsigned char);
REFLECT_TYPE_NAME(         short);
REFLECT_TYPE_NAME(unsigned short);
REFLECT_TYPE_NAME(         int);
REFLECT_TYPE_NAME(unsigned int);
REFLECT_TYPE_NAME(         long);
REFLECT_TYPE_NAME(unsigned long);
REFLECT_TYPE_NAME(         long long);
REFLECT_TYPE_NAME(unsigned long long);

} // reflect
