/* config.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection-based configuration implementation.
*/

#include "config.h"
#include "json/token.h"

namespace reflect {

/******************************************************************************/
/* LINK NODE                                                                  */
/******************************************************************************/

LinkNode::
~LinkNode()
{
    for (auto child : children) delete child;
}

LinkNode*
Config::LinkNode::
get(const Path& path, size_t index)
{
    if (index == path.size()) return this;

    auto it = children.find(path[index]);
    if (it == end())
        it = children.insert(path[index], new LinkNode).first;

    return it->second->get(key, path, index + 1);
}

void
Config::LinkNode::
subtree(std::vector<Path>& result, Path prefix) const
{
    for (auto& link : links) result.emplace_back(prefix, link);
    for (auto child : children)
        child.second->subtree(result, Path(prefix, child.first));
}

std::vector<Path>
Config::LinkNode::
subtree() const
{
    std::vector<Path> result;
    subtree(result, Path());
    return result;
}


void
Config::LinkNode::
add(const Path& path, const std::string& link)
{
    get(path)->links.push_back(key);
}

void
Config::LinkNode::
erase(const Path& path, size_t index)
{
    if (index == path.size() - 1) {
        children.erase(path.back());
        return;
    }

    auto it = children.find(path[index]);
    if (it == children.end()) return;
    it->second->erase(path, index + 1);

    if (links.empty() && it->second->children.empty())
        children.erase(path[index]);
}


/******************************************************************************/
/* CONFIG                                                                     */
/******************************************************************************/

Config::
Config(std::string trait) : trait(std::move(trait))
{}

void
Config::
add(const std::string& key, Value value)
{
    auto keyIt = keys.find(key);
    if (keyIt == keys.end()) keys[key] = value;
    else keyIt->second.assign(value);

    auto linkIt = links.find(key);
    if (linkIt == links.end()) return;

    for (auto link : linkIt->second) add(link, value);
    links.erase(linkIt);
}

void
Config::
addLink(const Path& link, const Path& target)
{


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
