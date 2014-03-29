/* ref_type.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reference type introspection and enum.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REF TYPE                                                                   */
/******************************************************************************/

enum class RefType
{
    Copy,
    LValue,
    RValue,
};

std::ostream& operator<<(std::ostream& stream, RefType type);


/******************************************************************************/
/* MAKE REF TYPE                                                              */
/******************************************************************************/

template<typename T>
RefType makeRefType()
{
    if (std::is_rvalue_reference<T>::value) return RefType::RValue;
    if (std::is_lvalue_reference<T>::value) return RefType::LValue;
    return RefType::Copy;
}

template<typename T>
RefType makeRefType(T&&)
{
    if (std::is_lvalue_reference<T>::value) return RefType::LValue;
    return RefType::RValue;
}


/******************************************************************************/
/* IS CONST                                                                   */
/******************************************************************************/

template<typename T>
bool isConst()
{
    typedef typename std::remove_reference<T>::type CleanT;
    return std::is_const<CleanT>::value;
}

template<typename T>
bool isConst(T&&)
{
    typedef typename std::remove_reference<T>::type CleanT;
    return std::is_const<CleanT>::value;
}

inline bool testConstConversion(bool fromIsConst, bool toIsConst)
{
    return fromIsConst == toIsConst || toIsConst;
}

} // reflect
