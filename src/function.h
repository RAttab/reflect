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
    template<typename Fn>
    Function(std::string name, Fn fn);

    Reflection* returnType() const { return ret; }
    size_t size() const { return args.size(); }
    Reflection* operator[] (size_t i) const { return args[i]; }

    template<typename Fn>
    bool test() const;

    bool test(const Function& other) const;

    template<typename Ret, typename... Args>
    Ret call(Args&&... args);

    bool isGetter() const;
    bool isSetter() const;

private:

    typedef std::function<void()> VoidFn;

    template<typename Ret, typename... Args>
    void initFn(std::function<Ret(Args...)> fn);

    void reflectArgs(TypeVector<>) {}

    template<typename Arg, typename... Rest>
    void reflectArgs(TypeVector<Arg, Rest...>);

    bool test(Reflection* value, Reflection* target) const;

    template<typename Ret, typename... Args>
    bool test(TypeVector<Args...>) const;

    template<size_t Index> 
    bool testArgs() const;

    template<size_t Index, typename Arg, typename... Rest>
    bool testArgs() const;

    VoidFn fn;
    std::string name;

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

    template<typename Fn>
    bool test();

    template<typename Ret, typename... Args>
    Ret call(Args&&... args);

private:
    std::vector<Function> overloads;
};

} // reflect
