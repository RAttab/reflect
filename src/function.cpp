/* function.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Function reflection implementation.
*/

#include "reflect.h"

#include <sstream>

namespace reflect {


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

Match
Function::
testReturn(const Argument& value, const Argument& target) const
{
    // If our caller doesn't want a return value then we can just discard it.
    if (value.type() == type<void>()) return Match::Exact;

    if (target.type() == type<void>()) {
        if (value.type() != type<Value>()) return Match::None;
        if (value.refType() == RefType::LValue) return Match::None;
        return Match::Exact;
    }

    if (target.isConvertibleTo(value) == Match::None) return Match::None;

    // While this is valid C++, it doesn't quite work through the reflection
    // since the compiler doesn't know enough to extend the lifespan of an
    // r-value reference when binding to a const l-value referencce.
    if (target.isTemporary() && value.isConstRef()) return Match::None;

    return Match::Exact;
}

Match
Function::
testArguments(
        const std::vector<Argument>& value,
        const std::vector<Argument>& target) const
{
    if (value.size() != target.size()) return Match::None;

    Match match = Match::Exact;
    for (size_t i = 0; i < target.size(); ++i) {
        match = combine(match, value[i].isConvertibleTo(target[i]));
        if (match == Match::None) return Match::None;
    }

    return match;
}

Match
Function::
test(const Function& other) const
{
    return combine(
            testReturn(other.ret, ret),
            testArguments(other.args, args));
}

Match
Function::
test(const Argument& ret, const std::vector<Argument>& args) const
{
    return combine(
            testReturn(ret, this->ret),
            testArguments(args, this->args));
}


bool
Function::
isGetter() const
{
    return !ret.isVoid()
        && args.size() == 1
        && args[0].isConstRef();
}

const Type*
Function::
getterType() const
{
    if (!isGetter()) {
        reflectError("function <%s, %s> is not a getter",
                name_, signature(*this));
    }

    return ret.type();
}

bool
Function::
isSetter() const
{
    return ret.isVoid()
        && args.size() == 2
        && args[0].isLValueRef();
}

const Type*
Function::
setterType() const
{
    if (!isGetter()) {
        reflectError("function <%s, %s> is not a setter",
                name_, signature(*this));
    }

    return args[1].type();
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

} // reflect
