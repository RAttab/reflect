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

template<typename T>
void
Type::
addField(const std::string& name, size_t offset)
{
    addField(name, Field::make<T>(name, offset));
}

template<typename T>
void
Type::
addConcept()
{
    addConcept(T::id(), new T);
}

template<typename T>
const T&
Type::
concept() const
{
    return static_cast<const T&>(concept(T::id()));
}

template<typename... Args>
Value
Type::
construct(Args&&... args) const
{
    return call<Value>(id(), std::forward<Args>(args)...);
}

template<typename Ret, typename... Args>
Ret
Type::
call(const std::string& fn, Args&&... args) const
{
    return function(fn).call<Ret>(std::forward<Args>(args)...);
}

} // namespace reflect
