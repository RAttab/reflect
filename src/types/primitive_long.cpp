/* primitive_long.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Long reflection
*/

#include "primitives.h"
#include "reflect/plumbing.h"

/******************************************************************************/
/* REFLECT LONG                                                               */
/******************************************************************************/

reflectClassImpl(long int)
{
    reflectPlumbing();
}

reflectClassImpl(unsigned long int)
{
    reflectPlumbing();
}
