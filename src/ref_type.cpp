/* ref_type.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   implementation of RefType.
*/

#include "reflect.h"

#include <ostream>

namespace reflect {

/******************************************************************************/
/* REF TYPE                                                                   */
/******************************************************************************/

std::ostream& operator<<(std::ostream& stream, RefType type)
{
    switch(type)
    {
    case RefType::LValue: stream << "LValue"; break;
    case RefType::RValue: stream << "RValue"; break;
    case RefType::Copy:   stream << "Copy";   break;
    default: reflectError("Unknown RefType");
    };

    return stream;
}

} // reflect
