/* type.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type implementation.
*/

#include "reflect.h"

namespace reflect {


/******************************************************************************/
/* TYPE                                                                       */
/******************************************************************************/

bool
Type::
isConvertibleTo(Type* other)
{
    return this == other
        || (parent_ && parent_->isConvertibleTo(other));
}


} // reflect
