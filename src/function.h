/* function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

struct Function
{
    typedef std::function<void()> VoidFn;

    // \todo I doubt that the reinterpret cast is safe here...
    template<typename Ret, typename... Args>
    Function(std::string name, std::function<Ret(Args...)> fn) :
        name(std::move(name)),
        fn(*reinterpret_cast<VoidFn*>(&makeValueFunction(fn))),
        ret(reflect<Ret>())
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
            test(reflect<Ret>(), ret) &&
            testArgs<0, Args...>();
    }

    bool test(const Function& other) const;

    template<typename Ret, typename... Args>
    Ret call(Args&&... args)
    {
        bool signatureMatch = test<Ret, Args...>();
        assert(signatureMatch);

        typedef typename MakeStdFunction<Args...>::type Fn;
        const auto& typedFn = *reinterpret_cast<Fn*>(&fn);

        Value ret = typedFn(cast<Value>(std::forward<Args>(args))...);
        return retCast<Ret>(ret);
    }


    bool isGetter() const;
    bool isSetter() const;

private:

    void reflectArgs() {}

    template<typename Arg, typename... Rest>
    void reflectArgs()
    {
        args.push_back(reflect<Arg>());
        reflectArgs<Rest...>();
    }


    bool test(Reflection* value, Reflection* target) const;

    template<size_t Index> void testArgs() const {}

    template<size_t Index, typename Arg, typename... Rest>
    void testArgs() const
    {
        if (!test(reflect<Arg>(), args[Index])) return false;
        return testArgs<Index + 1, Rest...>();
    }

    std::string name;
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
    Function& operator[] (size_t i) { return overloads[i]; }
    const Function& operator[] (size_t i) const { return overloads[i]; }

    void add(Function fn);

    bool test(Function fn);

    template<typename Ret, typename... Args>
    bool test()
    {
        for (const auto& fn : overloads) {
            if (fn.test<Ret, Args...>()) return true;
        }
        return false;

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
