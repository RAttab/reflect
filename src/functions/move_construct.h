/* move_construct.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style moveright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* MOVE CONSTRUCT                                                             */
/******************************************************************************/

template<typename T>
struct MoveConstruct : public Callable
{
    Signature signature() const
    {
        return Signature::make(
                Argument(type<T>(), RefType::RValue, false),
                { Argument(type<T>(), RefType::RValue, false) });
    }

    Value call(const Arguments& args) const
    {
        return Value( T(cast<T&&>(args[0])) );
    }
};


/******************************************************************************/
/* REFLECT MOVE CONSTRUCT                                                     */
/******************************************************************************/

namespace details {

template<typename T>
struct HasMoveConstruct
{
    static constexpr bool value =
        std::is_move_constructible<T>::value;
};

template<typename T,
    class = typename std::enable_if<HasMoveConstruct<T>::value>::type>
void reflectMoveConstruct(Type* type)
{
    type->addCallable(type->id(), new MoveConstruct<T>);
}

template<typename>
void reflectMoveConstruct(...) {}

} // namespace details


template<typename T>
void reflectMoveConstruct(Type* type)
{
    details::reflectMoveConstruct<T>(type);
}

} // namespace reflect
