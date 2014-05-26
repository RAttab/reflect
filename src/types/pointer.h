/* pointer.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection for pointers.
*/

#pragma once

#include "reflect.h"
#include "types/reflect/type.h"
#include "dsl/basics.h"
#include "dsl/template.h"
#include "dsl/plumbing.h"
#include "dsl/function.h"

namespace reflect {

/******************************************************************************/
/* REFLECT POINTERS                                                           */
/******************************************************************************/

template<typename T>
struct Reflect<T*>
{
    typedef T* T_;
    static std::string id() { return typeId<T>() + "*"; }

    reflectTemplateLoader()

    static void reflect(Type* type_)
    {
        reflectPlumbing();

        reflectTrait(primitive);

        reflectPointer(*, T);
        reflectCustom(operator*) (T* & value) -> T& {
            return *value;
        };
        reflectCustom(operator*) (T* const& value) -> const T& {
            return *value;
        };
    }
};

} // namespace reflect
