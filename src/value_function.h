/* value_function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Functor used to proxy all function calls.
*/

#pragma once

#include "type_vector.h"
#include "utils.h"
#include "value.h"

#include <function>

namespace reflect {


/******************************************************************************/
/* VALUE FUNCTION                                                             */
/******************************************************************************/

template<typename Ret, typename Args, typename Values>
struct ValueFunction;

template<typename... Args, typename... Values>
struct ValueFunction<void, TypeVector<Args...>, TypeVector<Values...> >
{
    typedef std::function<Ret(Args...)> Fn;

    ValueFunction(Fn fn) : fn(std::move(fn)) {}

    void operator() (Values... values) const
    {
        fn(cast<std::decay<Args>::type>(values)...);
    }

private:
    Fn fn;
};

template<typename Ret, typename... Args, typename... Values>
struct ValueFunction<Ret, TypeVector<Args...>, TypeVector<Values...> >
{
    typedef std::function<Ret(Args...)> Fn;

    ValueFunction(Fn fn) : fn(std::move(fn)) {}

    Value operator() (Values... values) const
    {
        return Value(fn(cast<std::decay<Args>::type>(values)...));
    }

private:
    Fn fn;
};


/******************************************************************************/
/* WRAP FUNCTION                                                              */
/******************************************************************************/

template<typename Ret, typename... Args>
struct MakeValueFunctionType
{
    typedef typename RepeatType<Value, sizeof...(Args)>::type Values;
    typedef typename ValueFunction<Ret, TypeVector<Args...>, Values> type;
};

template<typename Ret, typename... Args>
auto wrapFunction(std::function<Ret(Args...)> fn) ->
    MakeValueFunctionType<Ret, Args...>::type
{
    return MakeValueFunctionType<Ret, Args...>::type(std::move(fn));
}

} // reflect
