/* path.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflect path.
*/

#include "includes.h"
#pragma once

namespace reflect {
namespace config {
namespace details {

template<typename Arg>
void set(Value value, const Path& path, size_t index, Arg&& arg)
{
    if (value.type()->isPointer())
        details::set(*value, path, index, std::forward<Arg>(arg));

    else if (value.is("list")) {
        value.call<void>("resize", path.index(index) + 1);
        value[path.index(index)].assign(std::forward<Arg>(arg));
    }

    else if (value.is("map"))
        value[path[index]].assign(std::forward<Arg>(arg));

    else value.set(path[index], std::forward<Arg>(arg));
}

} // namespace details
} // namespace config
} // namespace reflect
