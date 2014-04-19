/* plumbing.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   General class plumbing.

   Autoom
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* CONS DEFAULT                                                               */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<
        std::is_default_constructible<T>::value &&
        std::is_move_constructible<T>::value>::type>
void reflectConsDefault(Type* type)
{
    type->add(type->id(), [] { return T(); });
    type->add("new", [] { return new T; });
}


template<typename T,
    class = typename std::enable_if<
        std::is_default_constructible<T>::value &&
        !std::is_move_constructible<T>::value>::type>
void reflectConsDefault(Type* type, int=0)
{
    type->add("new", [] { return new T; });
}

template<typename>
void reflectConsDefault(...) {}


/******************************************************************************/
/* CONS COPY                                                                  */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<
        std::is_copy_constructible<T>::value &&
        std::is_move_constructible<T>::value>::type>
void reflectConsCopy(Type* type)
{
    type->add(type->id(), [] (const T& other) { return T(other); });
    type->add("new", [] (const T& other) { return new T(other); });
}


template<typename T,
    class = typename std::enable_if<
        std::is_copy_constructible<T>::value &&
        !std::is_move_constructible<T>::value>::type>
void reflectConsCopy(Type* type, int=0)
{
    type->add("new", [] (const T& other) { return new T(other); });
}

template<typename>
void reflectConsCopy(...) {}


/******************************************************************************/
/* CONS MOVE                                                                  */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_move_constructible<T>::value>::type>
void reflectConsMove(Type* type)
{
    type->add(type->id(), [] (T&& other) {
                return T(std::move(other));
            });
}

template<typename>
void reflectConsMove(...) {}


/******************************************************************************/
/* OP ASSIGN COPY                                                             */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_copy_assignable<T>::value>::type>
void reflectOpAssignCopy(Type* type)
{
    type->add("operator=", [] (T& obj, const T& other) -> T& {
                return obj = other;
            });
}

template<typename>
void reflectOpAssignCopy(...) {}


/******************************************************************************/
/* OP ASSIGN MOVE                                                             */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_move_assignable<T>::value>::type>
void reflectOpAssignMove(Type* type)
{
    type->add("operator=", [] (T& obj, T&& other) -> T& {
                return obj = std::move(other);
            });
}

template<typename>
void reflectOpAssignMove(...) {}


/******************************************************************************/
/* PLUMBING                                                                   */
/******************************************************************************/

#define reflectPlumbing()                               \
    do {                                                \
        reflect::reflectConsDefault<T_>(type_);         \
        reflect::reflectConsCopy<T_>(type_);            \
        reflect::reflectConsMove<T_>(type_);            \
        reflect::reflectOpAssignCopy<T_>(type_);        \
        reflect::reflectOpAssignMove<T_>(type_);        \
    } while(false)


} // reflect
