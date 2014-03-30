/* int.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Integer reflection.
*/

#include "reflect.h"
#pragma once

/******************************************************************************/
/* REFLECT INTS                                                               */
/******************************************************************************/

reflectClass(char) {}
reflectClass(unsigned char) {}

reflectClass(short) {}
reflectClass(unsigned short) {}

reflectClass(int) {}
reflectClass(unsigned int) {}

reflectClass(long) {}
reflectClass(unsigned long) {}

reflectClass(long long) {}
reflectClass(unsigned long long) {}
