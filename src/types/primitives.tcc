/* primitives.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection utilities for primitives.
*/

#pragma once

#include "reflect.h"
#include "dsl/basics.h"
#include "dsl/plumbing.h"

#include <limits>

namespace reflect {

/******************************************************************************/
/* REFLECT LIMIT                                                              */
/******************************************************************************/

#define reflectLimit(name) \
    type_->addTrait(#name, std::numeric_limits<T_>::name())


/******************************************************************************/
/* REFLECT NUMBER                                                             */
/******************************************************************************/

template<typename T_>
void reflectNumberImpl(Type* type_)
{
    reflectPlumbing();
    reflectAlloc();

    reflectLimit(min);
    reflectLimit(max);

    reflectTypeTrait(primitive);

    reflectCustom(operator+) (const T_& obj, T_ value) {
        return obj + value;
    };

    if (std::is_same<T_, bool>::value)
        reflectTypeTrait(bool);

    else if (std::numeric_limits<T_>::is_integer) {
        reflectTypeTrait(integer);

        if (std::numeric_limits<T_>::is_signed)
            reflectTypeTrait(signed);
        else reflectTypeTrait(unsigned);
    }

    else reflectTypeTrait(float);
}

#define reflectNumber(num) \
    reflectTypeImpl(num) { reflectNumberImpl<T_>(type_); }

} // reflect
