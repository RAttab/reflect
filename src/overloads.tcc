/* overloads.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Overloads template implementation.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* OVERLOADS                                                                  */
/******************************************************************************/

template<typename Fn>
bool
Overloads::
test() const
{
    for (const auto& fn : overloads) {
        if (fn.test<Fn>() != Match::None) return true;
    }
    return false;

}

template<typename Fn>
const Function&
Overloads::
get() const
{
    for (const auto& fn : overloads) {
        if (fn.test<Fn>() != Match::None) return fn;
    }

    reflectError("no overload <%s> available for function <%s>",
            signature(
                    Argument::make<Ret>(),
                    reflectArguments(std::forward<Args>(args)...)),
            name());
}

template<typename Ret, typename... Args>
Ret
Overloads::
call(Args&&... args) const
{
    const Function* bestFn = nullptr;
    bool ambiguous = false;

    for (const auto& fn : overloads) {

        Match match = fn.testParams<Ret>(std::forward<Args>(args)...);
        if (match == Match::None) continue;

        if (bestFn && match == Match::Partial) {
            ambiguous = true;
            continue;
        }

        bestFn = &fn;

        if (match == Match::Exact) {
            ambiguous = false;
            break;
        }
    }

    if (!bestFn) {
        reflectError("no overload <%s> available for function <%s>",
                signature(
                        Argument::make<Ret>(),
                        reflectArguments(std::forward<Args>(args)...)),
                name());
    }

    if (ambiguous) {
        reflectError("ambiguous function call <%s> for function <%s>",
                signature(
                        Argument::make<Ret>(),
                        reflectArguments(std::forward<Args>(args)...)),
                name());
    }

    return bestFn->call<Ret>(std::forward<Args>(args)...);
}

} // reflect
