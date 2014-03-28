/* function.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   template implementation for Function Functions
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

template<typename Fn>
Function::
Function(std::string name, Fn fn) :
    name(std::move(name))
{
    initFn(makeFunction(std::move(fn)));
}

template<typename Ret, typename... Args>
void
Function::
initFn(std::function<Ret(Args...)> rawFn)
{
    auto typedFn = makeValueFunction(std::move(rawFn));

    // std::function is stored as a single pointer (memory allocation are used
    // to handle spill overs) which means it's "safe" to cast it to VoidFn and
    // back. The original type can always be recovered before we make the calll.
    fn = *reinterpret_cast<VoidFn*>(&typedFn);

    ret = reflect<Ret>();

    args.reserve(sizeof...(Args));
    reflectArgs(TypeVector<Args...>());
}

/** Note that we have to use TypeVector otherwise if we're dealing purely with
    types then we'd need to define the base case like so:

        template<> void reflectArgs() {}

    The compiler will interpret this as a template specialization for function
    which it will refuse to allow. If we omit then template<> then the compiler
    doesn't recognize it as a valid overload for a call to:

        reflectArgs<>();

    So we instead have to rely on type deduction using a phony TypeVector
    parameter which allows us to write the base case as:

        void reflectArgs(TypeVector<>) {}

    C++ uses dark corner case. It's super effective!
*/
template<typename Arg, typename... Rest>
void
Function::
reflectArgs(TypeVector<Arg, Rest...>)
{
    args.push_back(reflect<Arg>());
    reflectArgs(TypeVector<Rest...>());
}

template<typename Fn>
bool
Function::
test() const
{
    typedef typename GetReturnValue<Fn>::type Ret;
    typedef typename GetArguments<Fn>::type Args;

    return test<Ret>(Args());
}

template<typename Ret, typename... Args>
bool
Function::
test(TypeVector<Args...>) const
{
    return testArgs<0, Args...>()
        && (reflect<Ret>() == reflect<void>() || test(reflect<Ret>(), ret));
}

template<size_t Index>
bool
Function::
testArgs() const
{
    return true;
}

template<size_t Index, typename Arg, typename... Rest>
bool
Function::
testArgs() const
{
    if (!test(reflect<Arg>(), args[Index])) return false;
    return testArgs<Index + 1, Rest...>();
}


template<typename Ret, typename... Args>
Ret
Function::
call(Args&&... args)
{
    bool signatureMatch = test<Ret(Args...)>();
    assert(signatureMatch);

    typedef typename MakeStdValueFunction<Args...>::type Fn;
    const auto& typedFn = *reinterpret_cast<Fn*>(&fn);

    Value ret = typedFn(cast<Value>(std::forward<Args>(args))...);
    return retCast<Ret>(ret);
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

template<typename Ret, typename... Args>
bool
Functions::
test()
{
    for (const auto& fn : overloads) {
        if (fn.test<Ret, Args...>()) return true;
    }
    return false;

}

template<typename Ret, typename... Args>
Ret
Functions::
call(Args&&... args)
{
    for (const auto& fn : overloads) {
        if (!fn.test<Ret, Args...>()) continue;

        return fn.call<Ret>(std::forward<Args>(args)...);
    }

    assert(false && "No oveloads available");
}

} // reflect
