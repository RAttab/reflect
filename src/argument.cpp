/* argument.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Implementation of argument thingy.
*/

#include "reflect.h"
#include "types/void.h"

#include <sstream>

namespace reflect {

/******************************************************************************/
/* ARGUMENT                                                                   */
/******************************************************************************/

Argument::
Argument() : type_(reflect<void>()) {}

Argument::
Argument(Type* type, RefType refType, bool isConst) :
    type_(type), refType_(refType), isConst_(isConst)
{}

bool
Argument::
isVoid() const
{
    return type_ == reflect<void>();
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
