/* function.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection implementation.
*/

#include "reflect.h"
#include "types/void.h"
#include "types/value.h"

#include <sstream>

namespace reflect {


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

bool
Function::
testReturn(const Argument& value, const Argument& target) const
{
    // If our caller doesn't want a return value then we can just discard it.
    if (value.type() == type<void>()) return true;

    if (!target.isConvertibleTo(value)) return false;

    // While this is valid C++, it doesn't quite work through the reflection
    // since the compiler doesn't know enough to extend the lifespan of an
    // r-value reference when binding to a const l-value referencce.
    if (target.isTemporary() && value.isConstRef()) return false;

    return true;
}

bool
Function::
testArguments(
        const std::vector<Argument>& value,
        const std::vector<Argument>& target) const
{
    if (value.size() != target.size()) return false;

    for (size_t i = 0; i < target.size(); ++i) {
        if (!value[i].isConvertibleTo(target[i])) return false;
    }

    return true;
}

bool
Function::
test(const Function& other) const
{
    return testReturn(other.ret, ret)
        && testArguments(other.args, args);
}


/******************************************************************************/
/* SIGNATURE                                                                  */
/******************************************************************************/

std::string
signature(const Function& fn)
{
    std::vector<Argument> args;
    for (size_t i = 0; i < fn.arguments(); ++i)
        args.push_back(fn.argument(i));

    return signature(fn.returnType(), args);
}

std::string
signature(const Argument& ret, const std::vector<Argument>& args)
{
    std::stringstream ss;

    ss << ret.print() << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        ss << (i ? ", " : "") << args[i].print();
    }
    ss << ")";

    return ss.str();
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

bool
Functions::
test(Function fn)
{
    for (const auto& other : overloads) {
        if (fn.test(other)) return true;
    }
    return false;
}

void
Functions::
add(Function fn)
{
    if (test(fn)) reflectError("<%s> is ambiguous", signature(fn));
    overloads.push_back(fn);
}


} // reflect
