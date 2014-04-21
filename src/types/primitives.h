/* primitives.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Include headers for all primitive types.
*/

#pragma once

#include "reflect.h"
#include "reflect/class.h"
#include "pointer.h"

/******************************************************************************/
/* REFLECT PRIMITIVES                                                         */
/******************************************************************************/

reflectClassDecl(void)

reflectClassDecl(bool)

reflectClassDecl(char)
reflectClassDecl(signed char)
reflectClassDecl(unsigned char)

reflectClassDecl(short int)
reflectClassDecl(unsigned short int)

reflectClassDecl(int)
reflectClassDecl(unsigned int)

reflectClassDecl(long int)
reflectClassDecl(unsigned long int)

reflectClassDecl(long long int)
reflectClassDecl(unsigned long long int)

reflectClassDecl(float)
reflectClassDecl(double)
reflectClassDecl(long double)
