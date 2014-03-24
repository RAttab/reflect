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
/* REF TYPE                                                                   */
/******************************************************************************/

enum class RefType
{
    LValue,
    RValue,
};

template<typename T>
constexpr RefType refType(T&&)
{
    return std::is_lvalue_reference<T>::value ?
        RefType::LValue : RefType::RValue;
}


} // reflect
