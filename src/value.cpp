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
Value() :
    value_(nullptr), type_(reflect<void>())
{}

bool
Value::
isVoid() const
{
    return type_ == reflect<void>();
}


} // reflect
