/* primitive_long.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Long reflection
*/

#include "primitives.h"
#include "reflect/constructor.h"

/******************************************************************************/
/* REFLECT LONG                                                               */
/******************************************************************************/

reflectClassImpl(long int)
{
    reflectConsBasics();
}

reflectClassImpl(unsigned long int)
{
    reflectConsBasics();
}
