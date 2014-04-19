/* primitive_bool.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Bool reflection
*/

#include "primitives.h"
#include "reflect/plumbing.h"

/******************************************************************************/
/* REFLECT BOOL                                                               */
/******************************************************************************/

reflectClassImpl(bool)
{
    reflectPlumbing();
}
