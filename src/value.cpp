/* value.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value implementation. Mostly exists to break some header dependencies.
*/

#include "reflect.h"
#include "types/void.h"

namespace reflect {


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

Value::
Value() : value_(nullptr) {}

Value
Value::
copy() const
{
    if (!type()->isCopiable())
        reflectError("<%s> is not copiable", type()->id());

    reflectError("Value::copy() is not implemented yet.");
    return Value();
}

Value
Value::
move()
{
    if (!type()->isMovable())
        reflectError("<%s> is not movable", type()->id());

    reflectError("Value::move() is not implemented yet.");
    return Value();
}


} // reflect
