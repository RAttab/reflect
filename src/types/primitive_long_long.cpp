/* primitive_long_long.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Long Long reflection
*/

#include "primitives.h"
#include "reflect/plumbing.h"

/******************************************************************************/
/* REFLECT LONG LONG                                                          */
/******************************************************************************/

reflectClassImpl(long long int)
{
    reflectPlumbing();
}

reflectClassImpl(unsigned long long int)
{
    reflectPlumbing();
}
