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

template<typename T>
Field
Field::
make(std::string name, size_t offset)
{
    return Field(std::move(name), Argument::make<T>(), offset);
}


} // namespace reflect
