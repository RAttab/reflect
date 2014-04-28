/* config.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection-based configuration implementation.
*/

#include "config.h"
#include "json/token.h"

namespace reflect {


/******************************************************************************/
/* CONFIG                                                                     */
/******************************************************************************/

Config::
Config(std::string trait) : trait(std::move(trait))
{}

void
Config::
propagate(const Path& path, Value value)
{
    auto sublinks = links.get(path)->subtree();

    for (auto& sub : sublinks)
        set(sub.second, value);

    links.erase(path);
}

void
Config::
set(const Path& path, Value value)
{
    auto it = keys.find(path.front());

    if (it != keys.end())
        set(it->second, path.popFront(), value);

    else {
        if (path.size() > 1) reflectError("unknown path <%s>", path.tostring());
        it->second = value;
    }

    propagate(path, value);
}

void
Config::
relink(const Path& link, const Path& target)
{
    if (!links.has(link)) return;

    auto sublinks = links.get(link)->subtree();

    Node* node = links.get(target);
    for (auto& sub : sublinks)
        node->add(sub.second, sub.first);

    links.erase(link);
}

void
Config::
link(const Path& link, const Path& target)
{
    auto it = keys.find(target.front());
    if (it == keys.end()) {
        links.add(link, target);
        relink(link, target);
        return;
    }

    Value value = it->second;
    if (target.size() > 1)
        value = get(value, target.popFront());
    set(link, value);
}

void
Config::
load(std::istream& config)
{

}

void
Config::
load(const std::string& config)
{
    std::stringstream ss(config);
    load(ss);
}

void
Config::
save(std::ostream& config)
{
}

std::string
Config::
save()
{
    std::stringstream ss(config);
    save(ss);
}

} // reflect
