/* traits.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 04 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Template implementation of Traits
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

template<typename T>
void
Traits::
addTrait(const std::string& trait, T&& value)
{
    addTrait(trait, Value(std::forward<T>(value)));
}

template<typename Ret>
Ret
Traits::
getValue(const std::string& trait) const
{
    auto it = traits_.find(trait);
    if (it != traits_.end())
        return retCast<Ret>(it->second);

    reflectError("trait <%s> doesn't exist", trait);
}

} // namespace reflect
