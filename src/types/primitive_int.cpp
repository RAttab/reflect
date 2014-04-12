/* primitive_int.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Int reflection
*/

#include "primitives.h"
#include "reflect/constructor.h"

/******************************************************************************/
/* REFLECT INT                                                                */
/******************************************************************************/

reflectClassImpl(int)
{
    reflectConsBasics();
}

reflectClassImpl(unsigned int)
{
    reflectConsBasics();
}
