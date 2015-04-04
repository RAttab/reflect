/* field.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 04 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Field template implementation.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* FIELD                                                                      */
/******************************************************************************/

template<typename T, typename M>
Field::
Field(std::string name, M T::* field) :
    name_(std::move(name)),
    arg(Argument<M>::make()),
    offset_(reinterpret_cast<size_t>(field))
{
    if (arg.refType() != RefType::Copy) {
        reflectError("field <%s> of type <%s> uses unsupported references",
                name, arg.print());
    }
}


} // namespace reflect
