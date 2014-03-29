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
/* ERROR                                                                      */
/******************************************************************************/

void verror(bool except, const char* file, int line, const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);

    char buf[1024];
    int n = std::vsnprintf(buf, sizeof(buf) - 1, fmt, args);

    std::string msg =
        std::string(file) + ":" +
        std::to_string(line) + ": " +
        std::string(buf, n);

    if (except) throw ReflectError(msg);

    printf("%s\n", msg.c_str());
    abort();
}


} // reflect
