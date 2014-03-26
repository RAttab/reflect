/* value.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Template implementation for Value.
*/

#pragma once

#include "value.h"
#include "reflection.h"

namespace reflect {


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

template<typename T>
Value::
Value(T&& value) :
    value_(&value), reflection_(reflect<T>())
{
    if (refType(std::forward<T>(value)) == RefType::RValue)
        storage.reset(value_ = new T(std::move(value)));
}


template<typename T>
T
Value::
cast()
{
    assert(!isVoid());

    typedef typename std::decay<T>::type CleanT;
    assert(reflection_->isConvertibleTo<CleanT>());

    return cast<T>(reinterpret_cast<CleanT*>(value),
            std::is_lvalue_reference<T>());
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
    assert(refType() == RefType::RValue);
    assert(storage.unique());

    auto toReturn = std::move(*value);
    *this = Value();

    return std::move(toReturn);
}

} // reflect
