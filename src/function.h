/* function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT FUNCTION                                                           */
/******************************************************************************/

template<typename Fn> Argument reflectReturn();
template<typename Fn> std::vector<Argument> reflectArguments();

template<typename... Args>
std::vector<Argument> reflectArguments(Args&&... args);

/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

struct Function
{
    template<typename Fn>
    Function(const std::string& name, Fn fn);

    const std::string& name() const { return name_; }

    const Argument& returnType() const { return ret; }

    size_t arguments() const { return args.size(); }
    const Argument& argument(size_t index) const { return args[index]; }

    template<typename Fn>
    Match test() const;
    Match test(const Function& other) const;
    Match test(const Argument& ret, const std::vector<Argument>& args) const;

    template<typename Ret, typename... Args>
    Match testParams(Args&&... args) const;

    template<typename Ret, typename... Args>
    Ret call(Args&&... args) const;

    bool isGetter() const;
    const Type* getterType() const;

    bool isSetter() const;
    const Type* setterType() const;

private:

    typedef std::function<void()> VoidFn;

    template<typename Ret, typename... Args>
    void initFn(std::function<Ret(Args...)>&& fn);

    Match test(const Argument& value, const Argument& target) const;
    Match testReturn(const Argument& value, const Argument& target) const;
    Match testArguments(
            const std::vector<Argument>& value,
            const std::vector<Argument>& target) const;

    VoidFn fn;
    std::string name_;

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


} // reflect
