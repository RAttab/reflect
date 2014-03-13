/* utils.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Random reflection utilities.
*/

#pragma once

namespace reflect {



/******************************************************************************/
/* CAST                                                                       */
/******************************************************************************/

template<typename Val, typename Target>
struct Cast
{
    static Target cast(Val value)
    {
        return value;
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

template<typename Target, typename V>
Target cast(V&& value)
{
    return Cast<V, Target>::cast(std::forward<V>(value));
}

/******************************************************************************/
/* GET REFLECTION                                                             */
/******************************************************************************/

template<typename T>
struct GetReflection
{
    Reflection* get() const { ReflectionRegistry::get<T>(); }
};

template<>
struct GetReflection<Value>
{
    Reflection* get() const { return value.reflection(); }
};

} // reflect
