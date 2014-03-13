/* function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection.
*/

#pragma once

#include "utils.h"

#include <function>

namespace reflect {


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

struct Function
{
    template<typename Ret, typename... Args>
    Function(std::function<Ret(Args...)> fn) :
        fn(*reinterpret_cast<std::function<void()>*>(&fn)),
        ret(GetReflection<Ret>::get())
    {
        reflectArgs<Args...>();
    }

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

    template<typename... Args>
    void call(Args&&... args)
    {
        assert(test<void, Args...>());

        typedef std::function<void(Args...)> Fn;
        *reinterpret_cast<Fn*>(&fn)(std::forward<Args>(args)...);
    }

    template<typename Ret, typename... Args>
    Ret call(Args&&... args)
    {
        assert(test<Ret, Args...>());

        typedef std::function<Ret(Args...)> Fn;
        return *reinterpret_cast<Fn*>(&fn)(std::forward<Args>(args)...);
    }

    Reflection* returnType() const { return ret; }
    size_t size() const { return args.size(); }
    Reflection* operator[] (size_t i) const { return args[i]; }

    bool isGetter() const
    {
        return ret != GetReflection<void>::get() && args.empty();
    }

    bool isSetter() const
    {
        return ret == GetReflection<void>::get() && args.size() == 1;
    }

private:

    template<typename Arg>
    void reflectArgs()
    {
        args.push_back(GetReflection<Arg>::get());
    }

    template<typename Arg, typename... Rest>
    void reflectArgs()
    {
        reflectArgs<Arg>();
        reflectArgs<Rest...>();
    }

    bool test(Reflection* value, Reflection* target) const
    {
        return value->isConveritbleTo(target);
    }

    template<size_t Index, typename Arg>
    void testArgs() const
    {
        return test(GetReflection<Arg>::get(), args[Index]);
    }

    template<size_t Index, typename Arg, typename... Rest>
    void testArgs() const
    {
        if (!testArgs<Index, Arg>()) return false;
        return testArgs<Index + 1, Rest...>();
    }


    std::unique_ptr< std::function<void()> > fn;

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


    template<typename... Args>
    void call(Args&&... args)
    {
        for (const auto& fn : overloads) {
            if (!fn.test<Ret, Args...>()) continue;

            fn.call(std::forward<Args>(args)...);
            return;
        }

        assert(false && "no oveloads available");
    }

    template<typename Ret, typename... Args>
    Ret call(Args&&... args)
    {
        for (const auto& fn : overloads) {
            if (!fn.test<Ret, Args...>()) continue;

            return fn.call(std::forward<Args>(args)...);
        }

        assert(false && "No oveloads available");
    }

private:
    std::vector<Function> overloads;
};

} // reflect
