/* function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection.
*/

#pragma once

#include "utils.h"
#include "value.h"
#include "cast.h"
#include "value_function.h"

#include <function>

namespace reflect {


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

struct Function
{
    typedef std::function<void()> VoidFn;

    // \todo I doubt that the reinterpret cast is safe here...
    template<typename Ret, typename... Args>
    Function(std::function<Ret(Args...)> fn) :
        fn(*reinterpret_cast<VoidFn*>(&wrapFunction(fn))),
        ret(GetReflection<Ret>::get())
    {
        args.reserve(sizeof...(Args));
        reflectArgs<Args...>();
    }


    Reflection* returnType() const { return ret; }
    size_t size() const { return args.size(); }
    Reflection* operator[] (size_t i) const { return args[i]; }

    template<typename Ret, typename... Args>
    bool test() const
    {
        return
            test(GetReflection<Ret>::get(), ret) &&
            testArgs<0, Args...>();
    }

    bool test(const Function& other) const
    {
        if (args.size() != other.args.size()) return false;

        for (size_t i = 0; i < args.size(); ++i) {
            if (!args[i].isConvertibleTo(other.args[i]) &&
                    !other.args[i].isConvertibleTo(args[i]))
                return false;
        }

        return true;
    }

    template<typename Ret, typename... Args>
    Ret call(Args&&... args)
    {
        assert(test<Ret, Args...>());

        typedef typename MakeStdFunction<Args...>::type Fn;
        const auto& typedFn = *reinterpret_cast<Fn*>(&fn);

        Value ret = typedFn(cast<Value>(std::forward<Args>(args))...);
        return retCast<Ret>(ret);
    }


    bool isGetter() const
    {
        return ret != GetReflection<void>::get() && args.size() == 1;
    }

    bool isSetter() const
    {
        return ret == GetReflection<void>::get() && args.size() == 2;
    }


private:

    template<> void reflectArgs() {}

    template<typename Arg, typename... Rest>
    void reflectArgs()
    {
        args.push_back(GetReflection<Arg>::get());
        reflectArgs<Rest...>();
    }

    bool test(Reflection* value, Reflection* target) const
    {
        return value->isConveritbleTo(target);
    }

    template<size_t Index> void testArgs() const {}

    template<size_t Index, typename Arg, typename... Rest>
    void testArgs() const
    {
        if (!test(GetReflection<Arg>::get(), args[Index])) return false;
        return testArgs<Index + 1, Rest...>();
    }

    VoidFn fn;
    Reflection* ret;
    std::vector<Reflection*> args;
};


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

struct Functions
{

    size_t size() const { return overloads.size(); }
    Function& operator[] (size_t i) const { return overloads[i]; }

    bool test(Function fn)
    {
        for (const auto& other : overloads) {
            if (fn.test(other)) return true;
        }
        return false;
    }

    template<typename Ret, typename... Args>
    bool test()
    {
        for (const auto& fn : overloads) {
            if (fn.test<Ret, Args...>()) return true;
        }
        return false;

    }

    void add(Function fn)
    {
        assert(!test(fn) && "ambiguous overload");
        overloads.push_back(fn);
    }

    template<typename Ret, typename... Args>
    Ret call(Args&&... args)
    {
        for (const auto& fn : overloads) {
            if (!fn.test<Ret, Args...>()) continue;

            return fn.call<Ret>(std::forward<Args>(args)...);
        }

        assert(false && "No oveloads available");
    }

private:
    std::vector<Function> overloads;
};

} // reflect
