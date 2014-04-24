/* value_function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Functor used to proxy all function calls.
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* VALUE FUNCTION                                                             */
/******************************************************************************/

template<typename Ret, typename Args, typename Values>
struct ValueFunction;

template<typename... Args, typename... Values>
struct ValueFunction<void, TypeVector<Args...>, TypeVector<Values...> >
{
    typedef std::function<void(Args...)> Fn;

    ValueFunction(Fn&& fn) : fn(std::move(fn)) {}

    Value operator() (Values... values) const
    {
        fn(cast<Args>(values)...);
        return Value();
    }

private:
    Fn fn;
};

template<typename Ret, typename... Args, typename... Values>
struct ValueFunction<Ret, TypeVector<Args...>, TypeVector<Values...> >
{
    typedef std::function<Ret(Args...)> Fn;

    ValueFunction(Fn&& fn) : fn(std::move(fn)) {}

    Value operator() (Values... values) const
    {
        return Value(fn(cast<Args>(values)...));
    }

private:
    Fn fn;
};


/******************************************************************************/
/* WRAP FUNCTION                                                              */
/******************************************************************************/

template<typename Ret, typename... Args>
struct MakeValueFunction
{
    typedef typename RepeatType<Value, sizeof...(Args)>::type Values;
    typedef ValueFunction<Ret, TypeVector<Args...>, Values> type;
};

template<typename... Args>
struct MakeStdValueFunction
{
    template<typename Values> struct Expand;

    template<typename... Values>
    struct Expand< TypeVector<Values...> >
    {
        typedef typename std::function<Value(Values...)> type;
    };

    typedef typename RepeatType<Value, sizeof...(Args)>::type Values;
    typedef typename Expand<Values>::type type;
};


template<typename Ret, typename... Args>
auto makeValueFunction(std::function<Ret(Args...)>&& fn) ->
    typename MakeStdValueFunction<Args...>::type
{
    return typename MakeValueFunction<Ret, Args...>::type(std::move(fn));
}

} // reflect
