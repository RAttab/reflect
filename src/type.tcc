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

template<typename Fn>
void
Type::
addFunction(const std::string& name, Fn&& rawFn)
{
    addFunction(name, Function(name, std::move(rawFn)));
}

template<typename T, typename M>
void
Type::
addField(const std::string& name, M T::* rawField)
{
    addField(name, Field(name, rawField));
}

template<typename... Args>
Value
Type::
construct(Args&&... args) const
{
    return call<Value>(id(), std::forward<Args>(args)...);
}

template<typename... Args>
Value
Type::
alloc(Args&&... args) const
{
    return call<Value>("new", std::forward<Args>(args)...);
}

template<typename Ret, typename... Args>
Ret
Type::
call(const std::string& fn, Args&&... args) const
{
    return function(fn).call<Ret>(std::forward<Args>(args)...);
}

} // namespace reflect
