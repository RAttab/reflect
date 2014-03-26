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

    return !isVoid()
        && reflection_->isConvertibleTo<CleanT>()
        && castable<T>(std::is_rvalue_reference<T>());
}

template<typename T>
bool
Value::
castable(std::false_type) const
{
    return true;
}

template<typename T>
bool
Value::
castable(std::true_type) const
{
    return refType() == RefType::RValue && storage.unique();
}

template<typename T>
T
Value::
cast()
{
    assert(castable<T>());

    typedef typename std::decay<T>::type CleanT;

    CleanT* ptr = static_cast<CleanT*>(value_);
    return cast<T>(ptr, std::is_rvalue_reference<T>());
}

/** Return by lvalue-ref or copy. */
template<typename T, typename U>
T
Value::
cast(U* value, std::false_type)
{
    return *value;
}

/** Return by rvalue-ref so gut our object in the process. */
template<typename T, typename U>
T
Value::
cast(U* value, std::true_type)
{
    auto toReturn = std::move(*value);
    *this = Value();
    return std::move(toReturn);
}

} // reflect
