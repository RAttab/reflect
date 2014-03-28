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
/* ARGUMENT                                                                   */
/******************************************************************************/

std::string
Argument::
print() const
{
    return type_->id() + (refType_ == RefType::LValue ? "&" : "&&");
}


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

bool
Function::
test(const Argument& value, const Argument& target) const
{
    static Reflection* valueType = reflect<Value>();

    if (value.type() == valueType || target.type() == valueType)
        return true;

    if (target.refType() == RefType::LValue) {
        if (target.isConst()) {}
        else if (value.isConst()) return false;
        else if (value.refType() != RefType::LValue) return false;
    }

    else if (target.refType() == RefType::RValue) {
        if (value.refType() != RefType::RValue) return false;
    }

    return value.type()->isConvertibleTo(target.type());
}

bool
Function::
testReturn(const Argument& value, const Argument& target) const
{
    return value.type() == reflect<void>()
        || test(target, value);
}

bool
Function::
testArguments(
        const std::vector<Argument>& value,
        const std::vector<Argument>& target) const
{
    if (value.size() != target.size()) return false;

    for (size_t i = 0; i < target.size(); ++i) {
        if (!test(value[i], target[i])) return false;
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

    ss << ret.print() << " (";
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
    assert(!test(fn) && "ambiguous overload");
    overloads.push_back(fn);
}


} // reflect
