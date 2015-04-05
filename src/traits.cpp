/* traits.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 04 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Implementation of the traits class.
*/

#include "reflect.h"

namespace reflect {

/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

void
Traits::
addTrait(const std::string& trait, Value value)
{
    if (traits_.count(trait))
        reflectError("trait <%s> already exists", trait);

    traits_[trait] = std::move(value);
}

std::vector<std::string>
Traits::
traits() const
{
    std::vector<std::string> result;
    result.reserve(traits_.size());

    for (const auto& trait : traits_)
        result.push_back(trait.first);

    return result;
}

bool
Traits::
is(const std::string& trait) const
{
    return traits_.count(trait);
}

std::string
Traits::
print() const
{
    std::stringstream ss;

    ss << "[ ";
    for (const auto& trait : traits_)
        ss << trait.first << " ";
    ss << "]";

    return ss.str();
}


} // namespace reflect
