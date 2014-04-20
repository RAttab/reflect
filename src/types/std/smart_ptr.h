/* smart_ptr.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   std::shared_ptr reflection.
*/

#pragma once

#include "reflect.h"
#include "vector.h"
#include "types/reflect/type.h"
#include "reflect/basics.h"
#include "reflect/plumbing.h"
#include "reflect/operators.h"

#include <memory>

namespace reflect {

/******************************************************************************/
/* REFLECT SMART PTR                                                          */
/******************************************************************************/

template<typename T_, typename InnerT>
void reflectSmartPtr(Type* type_)
{
    reflectPlumbing();

    reflectTrait(pointer);
    reflectOp(operator*, Indirection);
    reflectOp(operator->, MemberAccess);
    reflectCustom("pointee") () -> const Type* { return type<InnerT>(); };

    reflectFn(get);
    reflectFn(reset);
    reflectOpCast(bool);
    reflectOp(operator==, EqComp);
}


/******************************************************************************/
/* REFLECT SHARED PTR                                                         */
/******************************************************************************/

template<typename T>
struct Reflect< std::shared_ptr<T> >
{
    typedef std::shared_ptr<T> T_;
    static std::string id() { return "std::shared_ptr<" + typeId<T>() + ">"; }

    reflectTemplateLoader()

    static void reflect(Type* type_)
    {
        reflectSmartPtr<T_, T>(type_);
    }
};


/******************************************************************************/
/* REFLECT UNIQUE PTR                                                         */
/******************************************************************************/

template<typename T>
struct Reflect< std::unique_ptr<T> >
{
    typedef std::unique_ptr<T> T_;
    static std::string id() { return "std::unique_ptr<" + typeId<T>() + ">"; }

    reflectTemplateLoader()

    static void reflect(Type* type_)
    {
        reflectSmartPtr<T_, T>(type_);
    }
};

} // reflect
