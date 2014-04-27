/* config.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection-based configuration utility.
*/

#pragma once

#include "reflect.h"
#include "path.h"

namespace reflect {

/******************************************************************************/
/* CONFIG                                                                     */
/******************************************************************************/

struct Config
{
    explicit Config(std::string trait = "config");

    bool count(const std::string& key) const { return keys.count(key); }
    Value operator[] (const std::string& key) const { return keys[key]; }

    void add(const std::string& key, Value value);
    void addLink(const Path& link, const Path& target);

    void load(std::istream& config);
    void load(const std::string& config);

    std::string save();
    void save(std::ostream& config);

private:
    std::string trait;
    std::unordered_map<std::string, Value> keys;

    struct LinkNode
    {
        ~LinkNode();

        LinkNode* get(const Path& path, size_t index = 0);
        std::vector<Path> subtree() const;

        void add(const Path& path, const std::string& link);
        void erase(const Path& path, size_t index = 0);

    private:
        void subtree(std::vector<Path>& result, Path prefix) const;

        std::list<std::string> links;
        std::unordered_map<std::string, LinkNode*> children;

    } root;
};

} // reflect
