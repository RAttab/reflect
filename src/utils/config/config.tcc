/* config.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection-based configuration utility template implemenataion.
*/

#pragma once

#include "config.h"

namespace reflect {

/******************************************************************************/
/* CONFIG                                                                     */
/******************************************************************************/

template<typename T>
T
Config::
get(const Path& path) const
{
    const Type* target = type<T>();
    Value value = operator[path].get<T>();

    if (!target.isPointer()) value = *value;
    return cast<T>(v);
}


} // namespace reflect
