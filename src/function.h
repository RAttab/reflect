/* function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* ARGUMENT                                                                   */
/******************************************************************************/

struct Argument
{
    Type* type() const { return type_; }
    RefType refType() const { return refType_; }
    bool isConst() const { return isConst_; }
    bool isVoid() const;

    template<typename T>
    static Argument make();

    std::string print() const;

private:
    Type* type_;
    RefType refType_;
    bool isConst_;
};


/******************************************************************************/
/* REFLECT FUNCTION                                                           */
/******************************************************************************/

template<typename Fn> Argument reflectReturn();
template<typename Fn> std::vector<Argument> reflectArguments();


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

struct Function
{
    template<typename Fn>
    Function(std::string name, Fn fn);

    const Argument& returnType() const { return ret; }

    size_t arguments() const { return args.size(); }
    const Argument& argument(size_t index) const { return args[index]; }

    template<typename Fn>
    bool test() const;

    bool test(const Function& other) const;

    template<typename Ret, typename... Args>
    Ret call(Args&&... args);

private:

    typedef std::function<void()> VoidFn;

    template<typename Ret, typename... Args>
    void initFn(std::function<Ret(Args...)> fn);

    bool test(const Argument& value, const Argument& target) const;
    bool testReturn(const Argument& value, const Argument& target) const;
    bool testArguments(
            const std::vector<Argument>& value,
            const std::vector<Argument>& target) const;

    VoidFn fn;
    std::string name;

    Argument ret;
    std::vector<Argument> args;
};


/******************************************************************************/
/* SIGNATURE                                                                  */
/******************************************************************************/

std::string signature(const Function& fn);
std::string signature(const Argument& ret, const std::vector<Argument>& args);

template<typename Fn>
std::string signature()
{
    return signature(reflectReturn<Fn>(), reflectArguments<Fn>());
}


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
