/* primitive_void.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Void reflection
*/

#include "primitives.h"
#include "reflect/basics.h"

/******************************************************************************/
/* REFLECT VOID                                                               */
/******************************************************************************/

reflectClassImpl(void)
{
    reflectTrait(primitive);
    reflectTrait(void);
}
