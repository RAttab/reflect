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
    if (it != traits_.end()) {
        // const_cast is required due to a rather complicated issue where we
        // can't cast const Value. Complicated issue that needs some serious
        // attention.
        return retCast<Ret>(const_cast<Value&>(it->second));
    }

    reflectError("trait <%s> doesn't exist", trait);
}

} // namespace reflect
