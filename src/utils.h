/* utils.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Random reflection utilities.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* STATIC ASSERT                                                              */
/******************************************************************************/

#define reflectStaticAssert(x) static_assert(x, #x)


/******************************************************************************/
/* STRINGIFY                                                                  */
/******************************************************************************/

#define reflectStringifyImpl(s) #s
#define reflectStringify(s) reflectStringifyImpl(s)


/******************************************************************************/
/* CONCAT                                                                     */
/******************************************************************************/

#define reflectConcatImpl(a,b) a ## b
#define reflectConcat(a,b) reflectConcatImpl(a,b)


/******************************************************************************/
/* UNIQUE NAME                                                                */
/******************************************************************************/

#define reflectUniqueNameImpl2(prefix, b)    \
    prefix ## _ ## b

#define reflectUniqueNameImpl(prefix, b)     \
    reflectUniqueNameImpl2(prefix, b)

#define reflectUniqueName(prefix)               \
    reflectUniqueNameImpl(prefix, __COUNTER__)


/******************************************************************************/
/* COMPILER                                                                   */
/******************************************************************************/

#define reflectUnused __attribute__((unused))
#define reflectUnreachable() __builtin_unreachable()


} // reflect
