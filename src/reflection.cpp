/* reflection.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection implementation.
*/

#include "reflect.h"

namespace reflect {


/******************************************************************************/
/* REFLECTION                                                                 */
/******************************************************************************/

bool
Reflection::
isConvertibleTo(Reflection* other)
{
    if (this == other) return true;
    if (!parent) return false;
    return parent->isConvertibleTo(other);
}


} // reflect
