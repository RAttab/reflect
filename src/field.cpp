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

std::string
Field::
print() const
{
    std::stringstream ss;

    ss << offset_ << ": " << type_->id() << " " << name_;
    if (!traits().empty()) ss << " " << Traits::print();

    return ss.str();
}

} // reflect
