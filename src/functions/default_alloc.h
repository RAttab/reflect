/* default_alloc.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* DEFAULT ALLOC                                                              */
/******************************************************************************/

template<typename T>
struct DefaultAlloc : public Callable
{
    Signature signature() const
    {
        return Signature::make(Argument(type<T*>(), RefType::Copy, false), {});
    }

    Value call(const Arguments& args) const
    {
        return new T;
    }
};


/******************************************************************************/
/* REFLECT DEFAULT ALLOC                                                      */
/******************************************************************************/

namespace details {

template<typename T>
struct HasDefaultAlloc
{
    static constexpr bool value =
        !std::is_pointer<T>::value &&
        std::is_default_constructible<T>::value;
};

template<typename T,
    class = typename std::enable_if<HasDefaultAlloc<T>::value>::type>
void reflectDefaultAlloc(Type* type)
{
    type->addCallable("new", new DefaultAlloc<T>);
}

template<typename>
void reflectDefaultAlloc(Type*) {}
 
} // namespace details 


template<typename T>
void reflectDefaultAlloc(Type* type)
{
    details::reflectDefaultAlloc<T>(type);
}

} // namespace reflect
