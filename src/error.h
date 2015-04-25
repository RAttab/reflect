/* error.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Error definition for Reflect.
*/

#include "reflect.h"
#pragma once

namespace reflect {

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

struct Error : public std::logic_error
{
    Error() : std::logic_error(""), active(false) {}
    Error(std::string what) : std::logic_error(std::move(what)), active(true) {}

    operator bool() const { return active; }

private:
    bool active;
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

#define reflectError(...)                                       \
    do {                                                        \
        ::reflect::error(__FILE__, __LINE__, __VA_ARGS__);      \
        reflectUnreachable();                                   \
    } while(false)


} // namespace reflect
