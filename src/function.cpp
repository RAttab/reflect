/* function.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection implementation.
*/

#include "reflect.h"
#include "types/void.h"
#include "types/value.h"

namespace reflect {


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

bool
Function::
test(Reflection* value, Reflection* target) const
{
    static Reflection* valueType = reflect<Value>();
    return value == valueType
        || target == valueType
        || value->isConvertibleTo(target);
}

bool
Function::
test(const Function& other) const
{
    if (args.size() != other.args.size()) return false;

    for (size_t i = 0; i < args.size(); ++i) {
        if (!other.args[i]->isConvertibleTo(args[i])) return false;
    }

    return true;
}

bool
Function::
isGetter() const
{
    return ret != reflect<void>() && args.size() == 1;
}

bool
Function::
isSetter() const
{
    return ret == reflect<void>() && args.size() == 2;
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

bool
Functions::
test(Function fn)
{
    for (const auto& other : overloads) {
        if (fn.test(other)) return true;
    }
    return false;
}

void
Functions::
add(Function fn)
{
    assert(!test(fn) && "ambiguous overload");
    overloads.push_back(fn);
}


} // reflect
