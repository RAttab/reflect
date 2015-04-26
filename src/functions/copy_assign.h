/* copy_assign.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* COPY ASSIGN                                                                */
/******************************************************************************/

template<typename T>
struct CopyAssign : public Callable
{
    Signature signature() const
    {
        return Signature::make(
                Argument(type<T>(), RefType::LValue, false),
                { 
                    Argument(type<T>(), RefType::LValue, false),
                    Argument(type<T>(), RefType::LValue, true),
                });
    }

    Value call(const Arguments& args) const
    {
        return retCast<Value>(cast<T&>(args[0]) = cast<const T&>(args[1]));
    }
};


/******************************************************************************/
/* REFLECT COPY ASSIGN                                                        */
/******************************************************************************/

namespace details {

template<typename T, 
    class = typename std::enable_if<std::is_copy_assignable<T>::value>::type>
void reflectCopyAssign(Type* type)
{
    type->addCallable("operator=", new CopyAssign<T>);
}

template<typename>
void reflectCopyAssign(...) {}
 
} // namespace details 


template<typename T>
void reflectCopyAssign(Type* type)
{
    details::reflectCopyAssign<T>(type);
}

} // namespace reflect
