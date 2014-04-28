/* config.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection-based configuration utility.
*/

#pragma once

#include "node.h"
#include "path.h"
#include "reflect.h"

namespace reflect {
namespace config {

/******************************************************************************/
/* CONFIG                                                                     */
/******************************************************************************/

struct Config
{
    explicit Config(std::string trait = "config");

    bool count(const std::string& key) const { return keys.count(key); }
    Value operator[] (const std::string& key) const { return keys[key]; }

    void set(const Path& path, Value value);
    void link(const Path& link, const Path& target);

    void load(std::istream& config);
    void load(const std::string& config);

    std::string save();
    void save(std::ostream& config);

private:
    void propagate(const Path& path);
    void relink(const Path& link, const Path& target);

    std::string trait;
    std::unordered_map<std::string, Value> keys;
    Node links;
};


} // namespace config
} // reflect
