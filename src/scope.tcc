/* scope.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 17 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Template implementation of Scope
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* NAMESPACE                                                                  */
/******************************************************************************/

template<typename Fn>
void
Scope::
addFunction(const std::string& name, Fn&& rawFn)
{
    addFunction(name, Function(tail(name).first, std::forward<Fn>(rawFn)));
}

template<typename Ret, typename... Args>
Ret
Scope::
call(const std::string& fn, Args&&... args) const
{
    return function(fn).call<Ret>(std::forward<Args>(args)...);
}

} // reflect
