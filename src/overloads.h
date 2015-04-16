/* overloads.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Overloads class thingy.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* OVERLOADS                                                                  */
/******************************************************************************/

struct Overloads : public Traits
{
    // For debugging purposes only.
    std::string name() const;

    size_t size() const { return overloads.size(); }
    Function& operator[] (size_t i) { return overloads[i]; }
    const Function& operator[] (size_t i) const { return overloads[i]; }

    void add(Function fn);

    template<typename Fn>
    bool test() const;
    bool test(const Function& fn) const;
    bool test(const Argument& ret, const std::vector<Argument>& args) const;

    template<typename Fn>
    const Function& get() const;
    const Function& get(const Argument& ret, const std::vector<Argument>& args) const;

    template<typename Ret, typename... Args>
    Ret call(Args&&... args) const;

    std::string print(size_t indent = 0) const;

private:
    std::vector<Function> overloads;
};

} // reflect
