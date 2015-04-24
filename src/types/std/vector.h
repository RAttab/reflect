/* vector.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   vector reflection.
*/

#pragma once

#include "reflect.h"
#include "dsl/basics.h"
#include "dsl/template.h"
#include "dsl/function.h"
#include "dsl/operators.h"

#include <vector>

/******************************************************************************/
/* REFLECT VECTOR                                                             */
/******************************************************************************/

reflectTemplate(std::vector, ValueT)
{
    reflectPlumbing();

    reflectTypeTrait(list);
    reflectTypeValue(valueType, type<ValueT>());

    reflectFn(size);
    reflectFn(clear);
    reflectFnTyped(resize, void (T_::*) (size_t));
    reflectFnTyped(push_back, void (T_::*) (const ValueT&));
    reflectFnTyped(push_back, void (T_::*) (ValueT&&));

    reflectCustom(operator[]) (T_& value, size_t i) -> ValueT& {
        return value[i];
    };
    reflectCustom(at) (const T_& value, size_t i) -> const ValueT& {
        if (i >= value.size())
            reflectError("index <%lu> out-of-bound <%lu>", i, value.size());
        return value[i];
    };
}
