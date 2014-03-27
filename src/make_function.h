/* make_function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Creates an std::function of the correct type out of any callable object.

   The fun part is that it does all the type deduction for you and can therefor
   be used like so:

       auto blah = makeFunction(foo);
       blah(1, 2);

   Pretty convenient and all it requires is a few sacritfic to the altar of the
   dark templating gods. I'm pretty sure it will also fail spectacularly in all
   kinds of mundane situations.

*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* IS FUNCTOR                                                                 */
/******************************************************************************/

template<typename T>
struct IsFunctor
{
    template<typename U>
    static std::true_type test(decltype(&U::operator())* = 0);

    template<typename>
    static std::false_type test(...);

    typedef decltype(test<T>(0)) type;
    static constexpr bool value = type::value;
};


/******************************************************************************/
/* FUNCTION TYPE                                                              */
/******************************************************************************/

template<typename Fn> struct FunctionType;

template<typename Ret, typename... Args>
struct FunctionType<Ret(*)(Args...)>
{
    typedef Ret (type)(Args...);
};

template<typename Obj, typename Ret, typename... Args>
struct FunctionType<Ret(Obj::*)(Args...)>
{
    typedef Ret (type)(Obj&, Args...);
};

template<typename Obj, typename Ret, typename... Args>
struct FunctionType<Ret(Obj::*)(Args...) const>
{
    typedef Ret (type)(Obj&, Args...);
};


/******************************************************************************/
/* FUNCTOR TYPE                                                               */
/******************************************************************************/

template<typename Fn> struct FunctorType;

template<typename Obj, typename Ret, typename... Args>
struct FunctorType<Ret(Obj::*)(Args...)>
{
    typedef Ret (type)(Args...);
};

template<typename Obj, typename Ret, typename... Args>
struct FunctorType<Ret(Obj::*)(Args...) const>
{
    typedef Ret (type)(Args...);
};


/******************************************************************************/
/* MAKE FUNCTION                                                              */
/******************************************************************************/

template<typename Fn,
    class = typename std::enable_if<
        !IsFunctor<typename std::decay<Fn>::type>::value>::type>
auto makeFunction(Fn&& fn) ->
    std::function<typename FunctionType<Fn>::type>
{
    typedef typename FunctionType<Fn>::type FnType;
    return std::function<FnType>(std::forward<Fn>(fn));
}

template<typename Obj, typename Fn>
auto makeFunctorFunction(Obj&& obj, Fn) ->
    std::function<typename FunctorType<Fn>::type>
{
    typedef typename FunctorType<Fn>::type FnType;
    return std::function<FnType>(std::forward<Obj>(obj));
}

// C++14 return type deduction would be handy right about now...
template<typename Fn,
    class = typename std::enable_if<
        IsFunctor<typename std::decay<Fn>::type>::value>::type>
auto makeFunction(Fn&& fn) ->
    decltype(makeFunctorFunction(
                    std::forward<Fn>(fn),
                    &std::decay<Fn>::type::operator()))
{
    return makeFunctorFunction(
            std::forward<Fn>(fn),
            &std::decay<Fn>::type::operator());
}


} // reflect
