/* function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT FN                                                                 */
/******************************************************************************/

#define reflectFn(fn) \
    type_->add(#fn, reflect::Function(#fn, &T_::fn))

#define reflectFnTyped(fn, signature) \
    type_->add(#fn, reflect::Function(#fn, (signature) &T_::fn))

#define reflectLambda(name, fn) \
    type_->add(name, reflect::Function(name, std::move(fn)))


/******************************************************************************/
/* REFLECT CUSTOM                                                             */
/******************************************************************************/

struct AddLambdaToType
{
    AddLambdaToType(Type* type, const char* name) :
        type(type), name(name)
    {}

    template<typename Fn>
    void operator+= (Fn fn)
    {
        type->add(name, reflect::Function(name, std::move(fn)));
    }

private:
    Type* type;
    const char* name;
};

#define reflectCustom(name)                             \
    reflect::AddLambdaToType(type_, #name) += []


} // reflect
