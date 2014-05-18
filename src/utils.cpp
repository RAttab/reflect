/* utils.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Utils implementations
*/

#include "reflect.h"

#include <cstdio>
#include <cstdarg>

namespace reflect {

/******************************************************************************/
/* ERROR FORMAT                                                               */
/******************************************************************************/

std::string verrorFormat(const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);

    char buf[1024];
    int n = std::vsnprintf(buf, sizeof(buf) - 1, fmt, args);

    return std::string(buf, n);
}


/******************************************************************************/
/* ERROR                                                                      */
/******************************************************************************/

void verror(bool except, const char* file, int line, std::string msg)
{
    msg = std::string(file) + ":" + std::to_string(line) + ": " + msg;

    if (except) throw ReflectError(msg);

    printf("%s\n", msg.c_str());
    abort();
}


} // reflect
