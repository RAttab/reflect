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


/******************************************************************************/
/* USE EXCEPTIONS                                                             */
/******************************************************************************/

#ifndef REFLECT_USE_EXCEPTIONS
#   define REFLECT_USE_EXCEPTIONS 0
#endif

enum { UseExceptions = REFLECT_USE_EXCEPTIONS };


/******************************************************************************/
/* PRINT TYPE                                                                 */
/******************************************************************************/

template<typename T>
struct PrintType
{
    typedef typename T::_print type;
};

// Debugging tool which causes a compile error containing the string
// representation of the type T as seen by the compiler.
template<typename T>
void printType()
{
    typedef typename PrintType<T>::type type;

    // To make the compiler shut-up about unused type definition. Very ironic...
    type blah;
    (void) blah;
}


/******************************************************************************/
/* REFLECT ERROR                                                              */
/******************************************************************************/

struct ReflectError : public std::logic_error
{
    ReflectError(std::string what) : std::logic_error(std::move(what)) {}
};


/******************************************************************************/
/* ERROR FORMAT                                                               */
/******************************************************************************/

template<typename T>
auto errorConvert(T&& value) -> decltype(std::forward<T>(value))
{
    return std::forward<T>(value);
}

inline const char* errorConvert(const char* value) { return value; }
inline const char* errorConvert(std::string& value) { return value.c_str(); }
inline const char* errorConvert(std::string&& value) { return value.c_str(); }
inline const char* errorConvert(const std::string& value) { return value.c_str(); }

std::string verrorFormat(const char* fmt, ...);

template<typename... Args>
std::string errorFormat(const char* fmt, Args&&... args)
{
    return verrorFormat(fmt, errorConvert(std::forward<Args>(args))...);
}


/******************************************************************************/
/* ERROR                                                                      */
/******************************************************************************/

void verror(bool except, const char* file, int line, std::string msg);

template<typename... Args>
void error(const char* file, int line, const char* fmt, Args&&... args)
{
    std::string msg = errorFormat(fmt, std::forward<Args>(args)...);
    verror(UseExceptions, file, line, std::move(msg));
}

#define reflectError(...)                        \
    do {                                         \
        error(__FILE__, __LINE__, __VA_ARGS__);  \
        reflectUnreachable();                    \
    } while(false)

} // reflect
