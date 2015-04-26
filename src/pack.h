/* pack.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* PACK                                                                       */
/******************************************************************************/

namespace details {

void pack(std::vector<Value>&) {}

template<typename Front, typename... Rest>
void pack(std::vector<Value>& result, Front&& value, Rest&&... rest)
{
    result.emplace_back(std::forward<Front>(value));
    pack(result, rest...);
}

} // namespace details

template<typename... Args>
std::vector<Value> pack(Args&&... args)
{
    std::vector<Value> result;
    result.reserve(sizeof...(args));

    pack(result, cast<Value>(std::forward<Args>(args))...);

    return result;
}

} // namespace reflect
