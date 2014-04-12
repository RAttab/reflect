/* primitive_char.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Char reflection
*/

#include "primitives.h"
#include "reflect/constructor.h"

/******************************************************************************/
/* REFLECT CHAR                                                               */
/******************************************************************************/

reflectClassImpl(signed char)
{
    reflectConsBasics();
}

reflectClassImpl(unsigned char)
{
    reflectConsBasics();
}
