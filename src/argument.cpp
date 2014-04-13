/* argument.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Implementation of argument thingy.
*/

#include "reflect.h"

#include <sstream>

namespace reflect {

/******************************************************************************/
/* ARGUMENT                                                                   */
/******************************************************************************/

Argument::
Argument() : type_(reflect::type<void>()) {}

Argument::
Argument(const Type* type, RefType refType, bool isConst) :
    type_(type), refType_(refType), isConst_(isConst)
{}

bool
Argument::
isVoid() const
{
    return type_ == reflect::type<void>();
}

bool
Argument::
isConstRef() const
{
    return isConst() && refType_ == RefType::LValue;
}

bool
Argument::
isTemporary() const
{
    return refType_ == RefType::RValue
        || refType_ == RefType::Copy;
}

bool
Argument::
isConvertibleTo(const Argument& target) const
{
    static Type* valueType = reflect::type<Value>();

    if ((type() == valueType) ^ (target.type() == valueType))
        return true;

    if (type()->hasConverter(target.type())) {
        if (target.refType() == RefType::LValue && !target.isConst())
            return false;
        return true;
    }

    if (target.refType() != RefType::Copy) {
        if (!testConstConversion(isConst(), target.isConst()))
            return false;
    }

    if (target.refType() == RefType::LValue) {
        if (target.isConst()) {}
        else if (isConst()) return false;
        else if (refType() != RefType::LValue) return false;
    }

    else if (target.refType() == RefType::RValue) {
        if (refType() != RefType::RValue) return false;
    }

    return target.type()->isParentOf(type());
}


std::string
Argument::
print() const
{
    std::stringstream ss;

    if (isConst_) ss << "const ";

    ss << type_->id();

    if (refType_ == RefType::LValue) ss << "&";
    if (refType_ == RefType::RValue) ss << "&&";

    return ss.str();
}

std::string printArgument(Type* type, RefType refType, bool isConst)
{
    return Argument(type, refType, isConst).print();
}

} // reflect
