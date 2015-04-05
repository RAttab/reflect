/* field.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 22 Mar 2015
   FreeBSD-style copyright and disclaimer apply

   Implementation of the field details.
*/

#include "reflect.h"

namespace reflect {

/******************************************************************************/
/* FIELD                                                                      */
/******************************************************************************/

Field::
Field(std::string name, Argument arg, size_t offset) :
    name_(std::move(name)), arg(std::move(arg)), offset_(offset)
{
    if (arg.refType() != RefType::Copy) {
        reflectError("field <%s> of type <%s> uses unsupported references",
                name, arg.print());
    }
}

std::string
Field::
print() const
{
    std::stringstream ss;

    ss << std::hex << "0x" << offset_ << std::dec << ": ";
    ss << arg.type()->id() << " " << name_;

    if (!traits().empty()) ss << " " << Traits::print();

    return ss.str();
}

} // reflect
