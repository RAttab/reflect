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

template<typename Fn>
void reflectFunction(Type* type, std::string name, Fn fn)
{
    type->add(std::move(name), std::move(fn));
}

#define reflectFn(fn)                           \
    reflectFunction(type_, #fn, &T_::fn)

#define reflectFnTyped(fn, signature)           \
    reflectFunction<signature>(type_, #fn, &T_::fn)


/******************************************************************************/
/* REFLECT CUSTOM                                                             */
/******************************************************************************/

struct AddLambdaToType
{
    AddLambdaToType(Type* type, std::string name) :
        type(type), name(std::move(name))
    {}

    template<typename Fn>
    void operator+= (Fn fn)
    {
        type->add(std::move(name), std::move(fn));
    }

private:
    Type* type;
    std::string name;
};

AddLambdaToType reflectLambda(Type* type, std::string name)
{
    return AddLambdaToType(type, std::move(name));
}

#define reflectCustom(name)                     \
    reflectLambda(type_, #name) += []


} // reflect
