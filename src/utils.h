/* utils.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Random reflection utilities.
*/

#pragma once

namespace reflect {


/******************************************************************************/
/* STATIC ASSERT                                                              */
/******************************************************************************/

#define reflectStaticAssert(x) static_assert(x, #x)


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

template<typename Val>
struct Cast<Val, Value>
{
    static Value cast(Val value)
    {
        return Value(value);
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

template<typename Target, typename V>
Target cast(V&& value)
{
    return Cast<V, Target>::cast(std::forward<V>(value));
}

} // reflect
