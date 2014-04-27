/* primitives.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Include headers for all primitive types.
*/

#pragma once

#include "reflect.h"
#include "reflect/type.h"
#include "pointer.h"

/******************************************************************************/
/* REFLECT PRIMITIVES                                                         */
/******************************************************************************/

reflectTypeDecl(void)

reflectTypeDecl(bool)

reflectTypeDecl(char)
reflectTypeDecl(signed char)
reflectTypeDecl(unsigned char)

reflectTypeDecl(short int)
reflectTypeDecl(unsigned short int)

reflectTypeDecl(int)
reflectTypeDecl(unsigned int)

reflectTypeDecl(long int)
reflectTypeDecl(unsigned long int)

reflectTypeDecl(long long int)
reflectTypeDecl(unsigned long long int)

reflectTypeDecl(float)
reflectTypeDecl(double)
reflectTypeDecl(long double)
