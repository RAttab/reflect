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

    template<typename Ret, typename... Args>
    Function(std::string name, std::function<Ret(Args...)> fn);

    Reflection* returnType() const { return ret; }
    size_t size() const { return args.size(); }
    Reflection* operator[] (size_t i) const { return args[i]; }

    template<typename Ret, typename... Args>
    bool test() const;

    bool test(const Function& other) const;

    template<typename Ret, typename... Args>
    Ret call(Args&&... args);

    bool isGetter() const;
    bool isSetter() const;

private:

    void reflectArgs() {}

    template<typename Arg, typename... Rest>
    void reflectArgs();

    bool test(Reflection* value, Reflection* target) const;

    template<size_t Index> 
    void testArgs() const {}

    template<size_t Index, typename Arg, typename... Rest>
    void testArgs() const;

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
    bool test();

    template<typename Ret, typename... Args>
    Ret call(Args&&... args);

private:
    std::vector<Function> overloads;
};

} // reflect
