/* copy_alloc.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* COPY ALLOC                                                                 */
/******************************************************************************/

template<typename T>
struct CopyAlloc : public Callable
{
    Signature signature() const
    {
        return Signature::make(
                Argument(type<T*>(), RefType::Copy, false),
                { Argument(type<T>(), RefType::LValue, true) });
    }

    Value call(const Arguments& args) const
    {
        return new T(cast<const T&>(args[0]));
    }
};


/******************************************************************************/
/* REFLECT COPY ALLOC                                                         */
/******************************************************************************/

namespace details {

template<typename T>
struct HasCopyAlloc
{
    static constexpr bool value =
        !std::is_pointer<T>::value &&
        std::is_copy_constructible<T>::value;
};

template<typename T,
    class = typename std::enable_if<HasCopyAlloc<T>::value>::type>
void reflectCopyAlloc(Type* type)
{
    type->addCallable("new", new CopyAlloc<T>);
}

template<typename>
void reflectCopyAlloc(Type*) {}

} // namespace details


template<typename T>
void reflectCopyAlloc(Type* type)
{
    details::reflectCopyAlloc<T>(type);
}

} // namespace reflect
