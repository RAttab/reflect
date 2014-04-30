/* node.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Implementation of the trie node for links
*/

#include "includes.h"

namespace reflect {
namespace config {

/******************************************************************************/
/* LINK NODE                                                                  */
/******************************************************************************/

Node::
~Node()
{
    for (auto child : children) delete child.second;
}

bool
Node::
has(const Path& path, size_t index) const
{
    if (index == path.size()) return true;

    auto it = children.find(path[index]);
    if (it == children.end()) return false;

    return it->second->has(path, index + 1);
}

Node*
Node::
get(const Path& path, size_t index)
{
    if (index == path.size()) return this;

    auto it = children.find(path[index]);
    if (it == children.end())
        it = children.emplace(path[index], new Node).first;

    return it->second->get(path, index + 1);
}

void
Node::
subtree(std::vector<LinkPair>& result, Path path) const
{
    for (auto& link : links) result.emplace_back(path, link);
    for (auto child : children)
        child.second->subtree(result, Path(path, child.first));
}

auto
Node::
subtree() const -> std::vector<LinkPair>
{
    std::vector<LinkPair> result;
    subtree(result, Path());
    return result;
}

void
Node::
add(const Path& link, const Path& path)
{
    get(path)->links.insert(link);
}

void
Node::
erase(const Path& path, size_t index)
{
    if (index == path.size()) {
        for (const auto& child : children) delete child.second;
        children.clear();
        links.clear();
        return;
    }

    auto it = children.find(path[index]);
    if (it == children.end()) return;

    Node* node = it->second;
    node->erase(path, index + 1);

    if (node->links.empty() && node->children.empty()) {
        delete it->second;
        children.erase(it);
    }
}

} // namespace config
} // namespace reflect
