/* cast.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Casting utility.
*/

#pragma once


#include "value.h"


namespace reflect {

/******************************************************************************/
/* CAST                                                                       */
/******************************************************************************/

template<typename T, typename Target>
struct Cast
{
    template<typename T>
    static Target cast(T&& value)
    {
        return std::forward<T>(value);
    }
};

template<typename Target>
struct Cast<Value, Target>
{
    static Target cast(Value value)
    {
        return value->cast<Target>();
    }
};

template<typename T>
struct Cast<T, Value>
{
    template<typename T>
    static Value cast(T&& value)
    {
        return Value(std::forward<T>(value));
    }
};

template<>
struct Cast<Value, Value>
{
    static Value cast(Value value)
    {
        return value;
    }
};

template<typename Target, typename T>
Target cast(T&& value)
{
    return Cast<T, Target>::cast(std::forward<T>(value));
}


} // reflect
