/* vector.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   vector reflection.
*/

#pragma once

#include "reflect.h"
#include "reflect/basics.h"
#include "reflect/function.h"
#include "reflect/operators.h"

#include <vector>

namespace reflect {

/******************************************************************************/
/* REFLECT VECTOR                                                             */
/******************************************************************************/

template<typename T>
struct Reflect< std::vector<T> >
{
    typedef std::vector<T> T_;
    static std::string id() { return "std::vector<" + typeId<T>() + ">"; }

    reflectTemplateLoader()

    static void reflect(Type* type_)
    {
        reflectPlumbing();

        reflectTrait(list);
        reflectCustom(valueType) { return type<T>(); };

        reflectFn(size);
        reflectFnTyped(push_back, void (T_::*) (const T&));
        reflectFnTyped(push_back, void (T_::*) (T&&));

        reflectCustom(operator[]) (const T_& value, size_t i) -> const T& {
            return value[i];
        };
        reflectCustom(operator[]) (T_& value, size_t i) -> T& {
            return value[i];
        };

    }
};

} // namespace reflect
