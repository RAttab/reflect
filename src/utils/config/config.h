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
    bool count(const Path& path) const;
    Value operator[] (const Path& path) const;

    template<typename T>
    const T& get(const Path& path)
    {
        return operator[path].get<T>();
    }

    void set(const Path& path, Value value);
    void link(const Path& link, const Path& target);

    void load(std::istream& config);
    void load(const std::string& config);

    std::string save(const std::string& trait = "config");
    void save(std::ostream& config, const std::string& trait = "config");

private:
    void propagate(const Path& path);
    void relink(const Path& link, const Path& target);

    std::unordered_map<std::string, Value> keys;
    Node links;
};


} // namespace config
} // namespace reflect
