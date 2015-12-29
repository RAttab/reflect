/* argument.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Implementation of argument thingy.
*/

#include "reflect.h"

#include <sstream>

namespace reflect {

/******************************************************************************/
/* MATCH                                                                      */
/******************************************************************************/

Match combine(Match a, Match b)
{
    if (a == Match::None || b == Match::None) return Match::None;
    if (a == Match::Partial || b == Match::Partial) return Match::Partial;
    return Match::Exact;
}

std::ostream& operator<<(std::ostream& stream, Match match)
{
    switch(match)
    {
    case Match::None:    stream << "None"; break;
    case Match::Partial: stream << "Partial"; break;
    case Match::Exact:   stream << "Exact"; break;
    default: reflectError("unknown match value");
    };

    return stream;
}


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
isLValueRef() const
{
    return !isConst()
        && refType_ == RefType::LValue;
}


bool
Argument::
operator==(const Argument& other) const
{
    return type_ == other.type_
        && refType_ == other.refType_
        && isConst_ == other.isConst_;
}

Match
Argument::
isConvertibleTo(const Argument& target) const
{
    static const Type* valueType = reflect::type<Value>();

    if (*this == target) return Match::Exact;

    if ((type() == valueType) ^ (target.type() == valueType))
        return Match::Exact;

    bool isExact = false;

    if (type()->hasConverter(target.type())) {
        if (target.refType() != RefType::LValue) return Match::Partial;
        return Match::None;
    }

    if (target.refType() != RefType::Copy) {
        if (!testConstConversion(isConst(), target.isConst()))
            return Match::None;
    }
    else isExact = refType() == RefType::RValue;

    if (target.refType() == RefType::LValue) {
        if (target.isConst()) {}
        else if (isConst()) return Match::None;
        else if (refType() != RefType::LValue) return Match::None;
    }

    else if (target.refType() == RefType::RValue) {
        if (refType() != RefType::RValue) return Match::None;
    }

    if (!target.type()->isParentOf(type())) return Match::None;

    if (isExact) return Match::Exact;
    if (isConst() == target.isConst() && refType() == target.refType())
        return Match::Exact;

    return Match::Partial;
}


std::string
Argument::
print() const
{
    std::stringstream ss;

    ss << type_->id();

    if (isConst_) ss << " const";
    if (refType_ == RefType::LValue) ss << "&";
    if (refType_ == RefType::RValue) ss << "&&";

    return ss.str();
}

std::string printArgument(const Type* type, RefType refType, bool isConst)
{
    return Argument(type, refType, isConst).print();
}

} // reflect
