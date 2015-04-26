/* move_assign.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style moveright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* MOVE ASSIGN                                                                */
/******************************************************************************/

template<typename T>
struct MoveAssign : public Callable
{
    Signature signature() const
    {
        return Signature::make(
                Argument(type<T>(), RefType::LValue, false),
                { 
                    Argument(type<T>(), RefType::LValue, false),
                    Argument(type<T>(), RefType::RValue, false),
                });
    }

    Value call(const Arguments& args) const
    {
        return retCast<Value>(cast<T&>(args[0]) = cast<T&&>(args[1]));
    }
};


/******************************************************************************/
/* REFLECT MOVE ASSIGN                                                        */
/******************************************************************************/

namespace details {

template<typename T, 
    class = typename std::enable_if<std::is_move_assignable<T>::value>::type>
void reflectMoveAssign(Type* type)
{
    type->addCallable("operator=", new MoveAssign<T>);
}

template<typename>
void reflectMoveAssign(...) {}
 
} // namespace details 


template<typename T>
void reflectMoveAssign(Type* type)
{
    details::reflectMoveAssign<T>(type);
}

} // namespace reflect
