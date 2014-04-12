/* primitive_short.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Short reflection
*/

#include "primitives.h"
#include "reflect/constructor.h"

/******************************************************************************/
/* REFLECT SHORT                                                              */
/******************************************************************************/

reflectClassImpl(short int)
{
    reflectConsBasics();
}

reflectClassImpl(unsigned short int)
{
    reflectConsBasics();
}
