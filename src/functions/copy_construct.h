/* copy_construct.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* COPY CONSTRUCT                                                             */
/******************************************************************************/

template<typename T>
struct CopyConstruct : public Callable
{
    Signature signature() const
    {
        return Signature::make(
                Argument(type<T>(), RefType::RValue, false),
                { Argument(type<T>(), RefType::LValue, true) });
    }

    Value call(const Arguments& args) const
    {
        return Value( T(cast<const T&>(args[0])) );
    }
};


/******************************************************************************/
/* REFLECT COPY CONSTRUCT                                                     */
/******************************************************************************/

namespace details {

template<typename T>
struct HasCopyConstruct
{
    static constexpr bool value =
        std::is_copy_constructible<T>::value &&
        std::is_move_constructible<T>::value;
};

template<typename T,
    class = typename std::enable_if<HasCopyConstruct<T>::value>::type>
void reflectCopyConstruct(Type* type)
{
    type->addCallable(type->id(), new CopyConstruct<T>);
}

template<typename>
void reflectCopyConstruct(...) {}

} // namespace details


template<typename T>
void reflectCopyConstruct(Type* type)
{
    details::reflectCopyConstruct<T>(type);
}

} // namespace reflect
