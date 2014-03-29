/* value.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Template implementation for Value.
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

template<typename T>
Value::
Value(T&& value) :
    arg(Argument::make(std::forward<T>(value))),
    value_((void*)&value) // cast-away any const
{
    if (refType() != RefType::RValue) return;

    typedef typename std::decay<T>::type CleanT;
    storage.reset(value_ = new CleanT(std::move(value)));
}

template<typename T>
bool
Value::
castable() const
{
    return !isVoid() && arg.isConvertibleTo<T>();
}

template<typename T>
auto
Value::
cast() const -> typename CleanRef<T>::type
{
    if (!castable<T>()) {
        reflectError("<%s> is not castable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;
    return *static_cast<CleanT*>(value_);
}

template<typename T>
bool
Value::
copiable() const
{
    return type()->isCopiable() && castable<T>();
}

template<typename T>
auto
Value::
copy() const -> typename CleanValue<T>::type
{
    if (!copiable<T>()) {
        reflectError("<%s> is not copiable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;
    return *static_cast<CleanT*>(value_);
}

template<typename T>
bool
Value::
movable() const
{
    return type()->isMovable()
        && castable<T>()
        && (!storage || storage.unique());
}

template<typename T>
auto
Value::
move() -> typename CleanValue<T>::type
{
    if (!castable<T>()) {
        reflectError("<%s> is not movable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;

    CleanT value = std::move(*static_cast<CleanT*>(value_));
    *this = Value();
    return value;
}


} // reflect
