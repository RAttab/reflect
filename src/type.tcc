/* type.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Template implementation of Type.
*/

#include "reflect.h"
#pragma  once

namespace reflect {

/******************************************************************************/
/* TYPE                                                                       */
/******************************************************************************/

template<typename... Args>
Value
Type::
construct(Args&&... args)
{
    return call<Value>(id().c_str(), std::forward<Args>(args)...);
}

template<typename Ret, typename... Args>
Ret
Type::
call(const char* fn, Args&&... args)
{
    return field(fn).call<Ret>(std::forward<Args>(args)...);
}


template<typename Fn>
void
Type::
add(const char* name, Fn fn)
{
    add(name, Function(name, std::move(fn)));
}

} // namespace reflect
