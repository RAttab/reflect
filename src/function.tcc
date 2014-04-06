/* function.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   template implementation for Function Functions
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT RETURN                                                             */
/******************************************************************************/

template<typename Fn>
Argument reflectReturn()
{
    typedef typename FunctionType<Fn>::Return Ret;
    return Argument::make<Ret>();
}


template<typename Fn>
void reflectReturn(Argument& ret)
{
    typedef typename FunctionType<Fn>::Return Ret;
    Argument::fill<Ret>(ret);
}


/******************************************************************************/
/* REFLECT ARGUMENTS                                                          */
/******************************************************************************/

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
inline void reflectArguments(std::vector<Argument>&, TypeVector<>) {}

template<typename Arg, typename... Rest>
void reflectArguments(std::vector<Argument>& args, TypeVector<Arg, Rest...>)
{
    args.push_back(Argument::make<Arg>());
    reflectArguments(args, TypeVector<Rest...>());
}

// Compile-time optimization: Constructing the vector in the template is
// extremely expensive when called from the Function construcctor.
template<typename Fn>
void reflectArguments(std::vector<Argument>& args)
{
    typedef typename FunctionType<Fn>::Arguments Args;
    reflectArguments(args, Args());
}

template<typename Fn>
std::vector<Argument> reflectArguments()
{
    std::vector<Argument> args;
    reflectArguments<Fn>(args);
    return args;
}


/******************************************************************************/
/* REFLECT PARAMS                                                             */
/******************************************************************************/

inline void reflectParams(std::vector<Argument>&) {}

template<typename... Rest>
void reflectParams(
        std::vector<Argument>& args, Value& value, Rest&&... rest)
{
    args.emplace_back(value.argument());
    reflectParams(args, std::forward<Rest>(rest)...);
}

template<typename... Rest>
void reflectParams(
        std::vector<Argument>& args, Value&& value, Rest&&... rest)
{
    args.emplace_back(value.argument());
    reflectParams(args, std::forward<Rest>(rest)...);
}

template<typename Arg, typename... Rest>
void reflectParams(std::vector<Argument>& args, Arg&& arg, Rest&&... rest)
{
    args.emplace_back(Argument::make(std::forward<Arg>(arg)));
    reflectParams(args, std::forward<Rest>(rest)...);
}

template<typename... Args>
std::vector<Argument> reflectParams(Args&&... args)
{
    std::vector<Argument> result;
    reflectParams(result, std::forward<Args>(args)...);
    return result;
}


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

template<typename Fn>
Function::
Function(const char* name, Fn rawFn)
    // fn(makeValueFunction(std::move(rawFn)))
{
    (void) rawFn;

    init(name);
    reflectReturn<Fn>(ret);
    reflectArguments<Fn>(args);
}


template<typename Fn>
bool
Function::
test() const
{
    auto otherRet = reflectReturn<Fn>();
    auto otherArgs = reflectArguments<Fn>();

    return testReturn(otherRet, ret)
        && testArguments(otherArgs, args);
}

template<typename Ret, typename... Args>
bool
Function::
testParams(Args&&... args) const
{
    auto otherRet = reflectReturn<Ret(Args...)>();
    auto otherArgs = reflectParams(std::forward<Args>(args)...);

    return testReturn(otherRet, ret)
        && testArguments(otherArgs, this->args);
}

template<typename Ret, typename... Args>
Ret
Function::
call(Args&&... args)
{
    if (!testParams<Ret>(std::forward<Args>(args)...)) {
        reflectError("<%s> is not convertible to <%s>",
                signature<Ret(Args...)>(), signature(*this));
    }

    typedef ValueFunction<sizeof...(Args)> Fn;
    Fn& typedFn = *static_cast<Fn*>(fn);

    Value ret = typedFn(cast<Value>(std::forward<Args>(args))...);
    return retCast<Ret>(ret);
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

template<typename Fn>
bool
Functions::
test() const
{
    for (const auto& fn : overloads) {
        if (fn.test<Fn>()) return true;
    }
    return false;

}

template<typename Ret, typename... Args>
Ret
Functions::
call(Args&&... args)
{
    for (auto& fn : overloads) {
        if (!fn.test<Ret(Args...)>()) continue;

        return fn.call<Ret>(std::forward<Args>(args)...);
    }

    reflectError("no overloads available for <%s>", signature<Ret(Args...)>());
}

} // reflect
