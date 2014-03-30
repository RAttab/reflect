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
/* UNIQUE NAME                                                                */
/******************************************************************************/

#define reflectUniqueNameImpl2(prefix, b)    \
    prefix ## _ ## b

#define reflectUniqueNameImpl(prefix, b)     \
    reflectUniqueNameImpl2(prefix, b)

#define reflectUniqueName(prefix)               \
    reflectUniqueNameImpl(prefix, __COUNTER__)


/******************************************************************************/
/* UNUSED                                                                     */
/******************************************************************************/

#define reflectUnused __attribute__((unused))


/******************************************************************************/
/* USE EXCEPTIONS                                                             */
/******************************************************************************/

#ifndef REFLECT_USE_EXCEPTIONS
#   define REFLECT_USE_EXCEPTIONS 0
#endif

enum { UseExceptions = REFLECT_USE_EXCEPTIONS };


/******************************************************************************/
/* REFLECT ERROR                                                              */
/******************************************************************************/

struct ReflectError : public std::logic_error
{
    ReflectError(std::string what) : std::logic_error(std::move(what)) {}
};


/******************************************************************************/
/* ERROR                                                                      */
/******************************************************************************/

template<typename T>
auto errorConvert(T&& value) -> decltype(std::forward<T>(value))
{
    return std::forward<T>(value);
}

const char* errorConvert(const char* value) { return value; }
const char* errorConvert(std::string& value) { return value.c_str(); }
const char* errorConvert(std::string&& value) { return value.c_str(); }
const char* errorConvert(const std::string& value) { return value.c_str(); }


// \todo Makes gcc extremely unhappy for whatever reason.
#define REFLECT_PRINTF_ATTR \
    __attribute__((format (printf, 3, 4)))


void verror(bool except, const char* file, int line, const char* fmt, ...);

template<typename... Args>
void error(const char* file, int line, const char* fmt, Args&&... args)
{
    verror(UseExceptions, file, line, fmt,
            errorConvert(std::forward<Args>(args))...);
}


#define reflectError(...)                        \
    error(__FILE__, __LINE__, __VA_ARGS__)

} // reflect
