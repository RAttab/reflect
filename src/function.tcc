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

// \todo I doubt that the reinterpret cast is safe here...
template<typename Ret, typename... Args>
Function::
Function(std::string name, std::function<Ret(Args...)> fn) :
    name(std::move(name)),
    fn(*reinterpret_cast<VoidFn*>(&makeValueFunction(fn))),
    ret(reflect<Ret>())
{
    args.reserve(sizeof...(Args));
    reflectArgs<Args...>();
}

template<typename Ret, typename... Args>
bool
Function::
test() const
{
    return
        test(reflect<Ret>(), ret) &&
        testArgs<0, Args...>();
}

template<typename Ret, typename... Args>
Ret
Function::
call(Args&&... args)
{
    bool signatureMatch = test<Ret, Args...>();
    assert(signatureMatch);

    typedef typename MakeStdValueFunction<Args...>::type Fn;
    const auto& typedFn = *reinterpret_cast<Fn*>(&fn);

    Value ret = typedFn(cast<Value>(std::forward<Args>(args))...);
    return retCast<Ret>(ret);
}

template<typename Arg, typename... Rest>
void
Function::
reflectArgs()
{
    args.push_back(reflect<Arg>());
    reflectArgs<Rest...>();
}

template<size_t Index, typename Arg, typename... Rest>
void
Function::
testArgs() const
{
    if (!test(reflect<Arg>(), args[Index])) return false;
    return testArgs<Index + 1, Rest...>();
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
