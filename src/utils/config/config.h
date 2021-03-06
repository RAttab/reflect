/* config.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection-based configuration utility.
*/

#include "includes.h"
#pragma once

namespace reflect {
namespace config {

/******************************************************************************/
/* CONFIG                                                                     */
/******************************************************************************/

struct Config
{
    bool count(const Path& path) const;

    template<typename T>
    T get(const Path& path) const;
    Value operator[] (const Path& path) const;

    std::vector<std::string> keys() const;

    void set(const Path& path, Value value);
    void link(const Path& link, const Path& target);

private:
    void propagate(const Path& path, Value value);
    void relink(const Path& link, const Path& target);

    Node links;
    std::unordered_map<std::string, Value> keys_;
};


} // namespace config
} // namespace reflect
