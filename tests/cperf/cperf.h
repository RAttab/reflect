/* cperf.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Compilation performance tests utiltiies
*/

#pragma once

#include "reflect.h"
#include "reflect/type.h"

/******************************************************************************/
/* ARGS                                                                       */
/******************************************************************************/

#define reflectArg(n)                       \
    struct A ## n {};                       \
    reflectType(A ## n) {}

reflectArg(0)
reflectArg(1)
reflectArg(2)
reflectArg(3)
reflectArg(4)
reflectArg(5)
reflectArg(6)
reflectArg(7)
reflectArg(8)
reflectArg(9)
