/* move_alloc.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style moveright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* MOVE ALLOC                                                                 */
/******************************************************************************/

template<typename T>
struct MoveAlloc : public Callable
{
    Signature signature() const
    {
        return Signature::make(
                Argument(type<T*>(), RefType::Copy, false),
                { Argument(type<T>(), RefType::RValue, false) });
    }

    Value call(const Arguments& args) const
    {
        return new T(cast<T&&>(args[0]));
    }
};


/******************************************************************************/
/* REFLECT MOVE ALLOC                                                         */
/******************************************************************************/

namespace details {

template<typename T>
struct HasMoveAlloc
{
    static constexpr bool value =
        !std::is_pointer<T>::value &&
        std::is_move_constructible<T>::value;
};

template<typename T,
    class = typename std::enable_if<HasMoveAlloc<T>::value>::type>
void reflectMoveAlloc(Type* type)
{
    type->addCallable("new", new MoveAlloc<T>);
}

template<typename>
void reflectMoveAlloc(Type*) {}

} // namespace details


template<typename T>
void reflectMoveAlloc(Type* type)
{
    details::reflectMoveAlloc<T>(type);
}

} // namespace reflect
