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
/* SIZEOF                                                                     */
/******************************************************************************/

template<typename T>
void reflectSizeof(Type* type)
{
    type->addTrait("sizeof", sizeof(T));
}

#define reflectSizeof() \
    reflect::reflectSizeof<T_>(type_)


/******************************************************************************/
/* CONS DEFAULT                                                               */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<
        std::is_default_constructible<T>::value &&
        std::is_move_constructible<T>::value>::type>
void reflectDefaultConstructor(Type* type)
{
    type->addFunction(type->id(), [] { return T(); });
}

template<typename>
void reflectDefaultConstructor(...) {}

#define reflectDefaultCons() \
    reflect::reflectDefaultConstructor<T_>(type_)


/******************************************************************************/
/* ALLOC DEFAULT                                                              */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<
        std::is_default_constructible<T>::value>::type>
void reflectAllocDefaultConstructor(Type* type)
{
    type->addFunction("new", [] () -> T* { return new T(); });
}

template<typename>
void reflectAllocDefaultConstructor(...) {}

#define reflectAllocDefaultCons() \
    reflect::reflectAllocDefaultConstructor<T_>(type_)


/******************************************************************************/
/* CONS COPY                                                                  */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<
        std::is_copy_constructible<T>::value &&
        std::is_move_constructible<T>::value>::type>
void reflectCopyConstructor(Type* type)
{
    type->addFunction(type->id(), [] (const T& other) {
                return T(other);
            });
}

template<typename>
void reflectCopyConstructor(...) {}

#define reflectCopyCons() \
    reflect::reflectCopyConstructor<T_>(type_)


/******************************************************************************/
/* ALLOC COPY                                                                 */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_copy_constructible<T>::value>::type>
void reflectAllocCopyConstructor(Type* type)
{
    type->addFunction("new", [] (const T& other) -> T* {
                return new T(other);
            });
}

template<typename>
void reflectAllocCopyConstructor(...) {}


#define reflectAllocCopyCons() \
    reflect::reflectAllocCopyConstructor<T_>(type_)


/******************************************************************************/
/* CONS MOVE                                                                  */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_move_constructible<T>::value>::type>
void reflectMoveConstructor(Type* type)
{
    type->addFunction(type->id(), [] (T&& other) {
                return T(std::move(other));
            });
}

template<typename>
void reflectMoveConstructor(...) {}


#define reflectMoveCons() \
    reflect::reflectMoveConstructor<T_>(type_)


/******************************************************************************/
/* ALLOC MOVE                                                                 */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_move_constructible<T>::value>::type>
void reflectAllocMoveConstructor(Type* type)
{
    type->addFunction("new", [] (T&& other) -> T* {
                return new T(std::move(other));
            });
}

template<typename>
void reflectAllocMoveConstructor(...) {}


#define reflectAllocMoveCons() \
    reflect::reflectAllocMoveConstructor<T_>(type_)


/******************************************************************************/
/* OP ASSIGN COPY                                                             */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_copy_assignable<T>::value>::type>
void reflectCopyAssignOperator(Type* type)
{
    type->addFunction("operator=", [] (T& obj, const T& other) -> T& {
                return obj = other;
            });
}

template<typename>
void reflectCopyAssignOperator(...) {}

#define reflectOpCopyAssign() \
    reflect::reflectCopyAssignOperator<T_>(type_)


/******************************************************************************/
/* OP ASSIGN MOVE                                                             */
/******************************************************************************/

template<typename T,
    class = typename std::enable_if<std::is_move_assignable<T>::value>::type>
void reflectMoveAssignOperator(Type* type)
{
    type->addFunction("operator=", [] (T& obj, T&& other) -> T& {
                return obj = std::move(other);
            });
}

template<typename>
void reflectMoveAssignOperator(...) {}

#define reflectOpMoveAssign() \
    reflect::reflectMoveAssignOperator<T_>(type_)


/******************************************************************************/
/* PLUMBING                                                                   */
/******************************************************************************/

#define reflectPlumbing()                               \
    do {                                                \
        reflectSizeof();                                \
        reflectDefaultCons();                           \
        reflectCopyCons();                              \
        reflectOpCopyAssign();                          \
        reflectMoveCons();                              \
        reflectOpMoveAssign();                          \
    } while(false)


#define reflectAllocPlumbing()                          \
    do {                                                \
        reflectAllocDefaultCons();                      \
        reflectAllocCopyCons();                         \
        reflectAllocMoveCons();                         \
    } while(false)


} // reflect
