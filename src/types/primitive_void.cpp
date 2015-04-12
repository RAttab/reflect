/* primitive_void.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Void reflection
*/

#include "primitives.h"
#include "dsl/basics.h"

/******************************************************************************/
/* REFLECT VOID                                                               */
/******************************************************************************/

reflectTypeImpl(void)
{
    reflectTypeTrait(primitive);
    reflectTypeTrait(void);
}
