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
    return this == other
        || (parent_ && parent_->isConvertibleTo(other));
}


} // reflect
