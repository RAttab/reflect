/* function.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   template implementation for Function Functions
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT FUNCTION                                                           */
/******************************************************************************/

template<typename Fn>
Argument reflectReturn()
{
    typedef typename GetReturnValue<Fn>::type Ret;
    return Argument::make<Ret>();
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
void reflectArguments(std::vector<Argument>&, TypeVector<>) {}

template<typename Arg, typename... Rest>
void reflectArguments(std::vector<Argument>& args, TypeVector<Arg, Rest...>)
{
    args.push_back(Argument::make<Arg>());
    reflectArguments(args, TypeVector<Rest...>());
}


template<typename Fn>
std::vector<Argument> reflectArguments()
{
    typedef typename GetArguments<Fn>::type Args;

    std::vector<Argument> args;
    reflectArguments(args, Args());
    return args;
}

void reflectArguments(std::vector<Argument>&) {}

template<typename... Rest>
void reflectArguments(
        std::vector<Argument>& args, Value& value, Rest&&... rest)
{
    args.emplace_back(value.argument());
    reflectArguments(args, std::forward<Rest>(rest)...);
}

template<typename... Rest>
void reflectArguments(
        std::vector<Argument>& args, Value&& value, Rest&&... rest)
{
    args.emplace_back(value.argument());
    reflectArguments(args, std::forward<Rest>(rest)...);
}

template<typename Arg, typename... Rest>
void reflectArguments(std::vector<Argument>& args, Arg&& arg, Rest&&... rest)
{
    args.emplace_back(Argument::make(std::forward<Arg>(arg)));
    reflectArguments(args, std::forward<Rest>(rest)...);
}

template<typename... Args>
std::vector<Argument> reflectArguments(Args&&... args)
{
    std::vector<Argument> result;
    reflectArguments(result, std::forward<Args>(args)...);
    return result;
}


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

    ret = reflectReturn<Ret(Args...)>();
    args = reflectArguments<Ret(Args...)>();
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
    auto otherArgs = reflectArguments(std::forward<Args>(args)...);

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

    typedef typename MakeStdValueFunction<Args...>::type Fn;
    const auto& typedFn = *reinterpret_cast<Fn*>(&fn);

    Value ret = typedFn(cast<Value>(std::forward<Args>(args))...);
    return retCast<Ret>(ret);
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

template<typename Fn>
bool
Functions::
test()
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
    for (const auto& fn : overloads) {
        if (!fn.test<Ret(Args...)>()) continue;

        return fn.call<Ret>(std::forward<Args>(args)...);
    }

    reflectError("No overloads available for <%s>", signature<Ret(Args...)>());
}

} // reflect
