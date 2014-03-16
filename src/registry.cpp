/* registry.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 16 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Static global state for the reflection registry.
*/

#include "registry.h"

namespace reflect {

namespace {

struct Registry
{
    std::mutex lock;
    std::unordered_map<std::string, Reflection*> idMap;
} registry;

} // namespace anonymous


/******************************************************************************/
/* REFLECTION REGISTRY                                                        */
/******************************************************************************/

Reflection*
ReflectionRegistry::
get(const std::string& id)
{
    std::lock_guard<std::mutex>(registry.lock);

    auto it = registry.idMap.find(id);
    if (it == registry.idMap.end()) return nullptr;

    return it->second;
}

void
ReflectionRegistry::
add(std::string id, Reflection* reflection)
{
    std::lock_guard<std::mutex>(registry.lock);

    registry.idMap.emplace(std::move(id), reflection);
}

} // reflect
