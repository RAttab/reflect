/* primitive_long_long.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Long Long reflection
*/

#include "primitives.h"
#include "reflect/constructor.h"

/******************************************************************************/
/* REFLECT LONG LONG                                                          */
/******************************************************************************/

reflectClassImpl(long long int)
{
    reflectConsBasics();
}

reflectClassImpl(unsigned long long int)
{
    reflectConsBasics();
}
