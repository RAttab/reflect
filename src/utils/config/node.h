/* node.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tri on path nodes.
*/

#include "includes.h"
#pragma once

namespace reflect {
namespace config {

/******************************************************************************/
/* NODE                                                                       */
/******************************************************************************/

struct Node
{
    ~Node();

    bool has(const Path& path, size_t index = 0) const;
    Node* get(const Path& path, size_t index = 0);

    typedef std::pair<Path, Path> LinkPair;
    std::vector<LinkPair> subtree() const;

    void add(const Path& link, const Path& path);
    void erase(const Path& path, size_t index = 0);

private:
    void subtree(std::vector<LinkPair>& result, Path prefix) const;

    std::set<Path> links;
    std::unordered_map<std::string, Node*> children;
};

} // namespace config
} // namespace reflect
