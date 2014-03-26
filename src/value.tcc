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
    value_(&value), reflection_(reflect<T>())
{
    if (reflect::refType(std::forward<T>(value)) != RefType::RValue)
        return;

    typedef typename std::decay<T>::type CleanT;
    storage.reset(value_ = new CleanT(std::move(value)));
}

template<typename T>
bool
Value::
castable() const
{
    typedef typename std::decay<T>::type CleanT;
    return !isVoid() && reflection_->isConvertibleTo<CleanT>();
}

template<typename T>
T&
Value::
cast() const
{
    assert(castable<T>());
    return *static_cast<T*>(value_);
}

template<typename T>
T
Value::
move()
{
    assert(movable<T>());

    T value = std::move(*static_cast<T*>(value_));
    *this = Value();
    return value;
}

template<typename T>
bool
Value::
movable() const
{
    return castable<T>() && (!storage || storage.unique());
}


} // reflect
