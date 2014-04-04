/* number.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Number reflection.
*/

#include "reflect.h"
#pragma once

/******************************************************************************/
/* REFLECT NUMBER                                                             */
/******************************************************************************/

reflectClassDecl(char)
reflectClassDecl(signed char)
reflectClassDecl(unsigned char)
reflectClassDecl(char16_t)
reflectClassDecl(char32_t)
reflectClassDecl(wchar_t)

reflectClassDecl(short)
reflectClassDecl(unsigned short)

reflectClassDecl(int)
reflectClassDecl(unsigned int)

reflectClassDecl(long)
reflectClassDecl(unsigned long)

reflectClassDecl(long long)
reflectClassDecl(unsigned long long)

reflectClassDecl(float)
reflectClassDecl(double)
reflectClassDecl(long double)
