/* default_construct.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* DEFAULT CONSTRUCT                                                          */
/******************************************************************************/

template<typename T>
struct DefaultConstruct : public Callable
{
    Signature signature() const
    {
        return Signature::make(Argument(type<T>(), RefType::RValue, false), {});
    }

    Value call(const Arguments&) const
    {
        return Value( T() );
    }
};


/******************************************************************************/
/* REFLECT DEFAULT CONSTRUCT                                                  */
/******************************************************************************/

namespace details {

template<typename T>
struct HasDefaultConstruct
{
    static constexpr bool value =
        std::is_default_constructible<T>::value &&
        std::is_move_constructible<T>::value;
};

template<typename T, class = 
    typename std::enable_if<HasDefaultConstruct<T>::value>::type>
void reflectDefaultConstruct(Type* type)
{
    type->addCallable(type->id(), new DefaultConstruct<T>);
}

template<typename>
void reflectDefaultConstruct(...) {}
 
} // namespace details 


template<typename T>
void reflectDefaultConstruct(Type* type)
{
    details::reflectDefaultConstruct<T>(type);
}

} // namespace reflect
