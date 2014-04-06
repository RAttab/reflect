/* function_type.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 04 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Function type introspection.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* FUNCTION TYPES                                                             */
/******************************************************************************/

struct CallFunction {};
struct GlobalFunction {};
struct MemberFunction {};
struct FunctorFunction {};


/******************************************************************************/
/* FUNCTOR                                                                    */
/******************************************************************************/

template<typename Fn> struct FunctorType;

template<typename Obj, typename Ret, typename... Args>
struct FunctorType<Ret(Obj::*)(Args...)>
{
    typedef FunctorFunction type;

    typedef Ret Return;
    typedef TypeVector<Args...> Arguments;
    static constexpr size_t ArgCount = sizeof...(Args);

    typedef Ret (Fn)(Args...);
};

template<typename Obj, typename Ret, typename... Args>
struct FunctorType<Ret(Obj::*)(Args...) const>
{
    typedef FunctorFunction type;

    typedef Ret Return;
    typedef TypeVector<Args...> Arguments;
    static constexpr size_t ArgCount = sizeof...(Args);

    typedef Ret (Fn)(Args...);
};


/******************************************************************************/
/* FUNCTION TYPE                                                              */
/******************************************************************************/

// By default we assume a functor so extract operator() and call FunctorType
template<typename Fn>
struct FunctionType : public FunctorType<decltype(&Fn::operator())>
{};


/******************************************************************************/
/* CALL FUNCTION                                                              */
/******************************************************************************/

template<typename Ret, typename... Args>
struct FunctionType<Ret(Args...)>
{
    typedef CallFunction type;

    typedef Ret Return;
    typedef TypeVector<Args...> Arguments;
    static constexpr size_t ArgCount = sizeof...(Args);
};


/******************************************************************************/
/* GLOBAL FUNCTION                                                            */
/******************************************************************************/

template<typename Ret, typename... Args>
struct FunctionType<Ret(*)(Args...)>
{
    typedef GlobalFunction type;

    typedef Ret Return;
    typedef TypeVector<Args...> Arguments;
    static constexpr size_t ArgCount = sizeof...(Args);

    typedef Ret (Fn)(Args...);
};


/******************************************************************************/
/* MEMBER FUNCTION                                                            */
/******************************************************************************/

template<typename Obj, typename Ret, typename... Args>
struct FunctionType<Ret(Obj::*)(Args...)>
{
    typedef MemberFunction type;

    typedef Ret Return;
    typedef TypeVector<Obj&, Args...> Arguments;
    static constexpr size_t ArgCount = sizeof...(Args) + 1;

    typedef Ret (Fn)(Obj&, Args...);
};

template<typename Obj, typename Ret, typename... Args>
struct FunctionType<Ret(Obj::*)(Args...) const>
{
    typedef MemberFunction type;

    typedef Ret Return;
    typedef TypeVector<const Obj&, Args...> Arguments;
    static constexpr size_t ArgCount = sizeof...(Args) + 1;

    typedef Ret (Fn)(const Obj&, Args...);
};

} // reflect
