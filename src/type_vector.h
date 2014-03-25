/* type_vector.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Meta programing vector of types.

   Well, it's official. I've now gone over to the dark side.
   ALL HAIL TEMPLATES!
*/

#pragma once

namespace reflect {


/******************************************************************************/
/* TYPE VECTOR                                                                */
/******************************************************************************/

template<typename... T> struct TypeVector {};


/******************************************************************************/
/* REPEAT TYPE                                                                */
/******************************************************************************/

namespace details {

template<typename T, size_t N, typename... Pack>
struct RepeatType : RepeatType<T, N-1, T, Pack...> {};

template<typename T, typename... Pack>
struct RepeatType<T, 0, Pack...>
{
    typedef TypeVector<Pack...> type;
};

} // namespace details

template<typename T, size_t N>
struct RepeatType
{
    typedef typename details::RepeatType<T, N>::type type;
};


} // reflect
