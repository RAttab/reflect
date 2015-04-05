/* smart_ptr.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   std::shared_ptr reflection.
*/

#pragma once

#include "reflect.h"
#include "vector.h"
#include "types/reflect/type.h"
#include "dsl/basics.h"
#include "dsl/template.h"
#include "dsl/plumbing.h"
#include "dsl/operators.h"

#include <memory>

namespace reflect {

/******************************************************************************/
/* REFLECT SMART PTR                                                          */
/******************************************************************************/

/** Half the members of std::shared_ptr are defined under a super-class in gcc
    4.7 which means that infering the type of the this parameter will lead to
    tears. Instead we use custom reflection to get around that problem.

 */
template<typename T_, typename InnerT>
void reflectSmartPtr(Type* type_)
{
    reflectPlumbing();
    reflectCons(InnerT*);

    reflectTypeTrait(smartPtr);
    reflectCustom(get) (const T_& value) { return value.get(); };
    reflectCustom(operator bool()) (const T_& value) { return !!value; };
    reflectCustom(operator*) (const T_& value) -> InnerT& { return *value; };
    reflectCustom(operator==) (const T_& value, const T_& other) {
        return value == other;
    };
}

} // namespace reflect


/******************************************************************************/
/* REFLECT SHARED PTR                                                         */
/******************************************************************************/

reflectTemplate(std::shared_ptr, T)
{
    reflectPointer(std::shared_ptr, T);

    reflectSmartPtr<T_, T>(type_);
    reflectFnTyped(reset, void (T_::*) ());
    reflectFnTyped(reset, void (T_::*) (T*));
}


/******************************************************************************/
/* REFLECT UNIQUE PTR                                                         */
/******************************************************************************/

reflectTemplate(std::unique_ptr, T)
{
    reflectPointer(std::unique_ptr, T);

    reflectSmartPtr<T_, T>(type_);
    reflectFn(release);
    reflectFnTyped(reset, void (T_::*) (T*));
    reflectCustom(reset) (T_& value) { value.reset(); };
}
