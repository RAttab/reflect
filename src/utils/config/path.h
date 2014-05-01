/* path.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflect path.
*/

#include "includes.h"
#pragma once

namespace reflect {
namespace config {

/******************************************************************************/
/* PATH                                                                       */
/******************************************************************************/

struct Path
{
    Path() {}
    Path(const char* path, char sep = '.');
    Path(const std::string& path, char sep = '.');
    Path(const Path& prefix, size_t index);
    Path(const Path& prefix, const std::string& path, char sep = '.');

    explicit operator bool() const { return items.empty(); }

    bool empty() const { return items.empty(); }
    size_t size() const { return items.size(); }
    const std::string& operator[] (size_t index) const { return items[index]; }

    bool isIndex(size_t index) const;
    size_t index(size_t index) const;

    const std::string& front() const { return items.front(); }
    Path popFront() const;

    const std::string& back() const { return items.back(); }
    Path popBack() const;

    bool operator<(const Path& other) const;

    std::string toString(char sep = '.') const;

private:
    void parse(const std::string& path, char sep);

    std::vector<std::string> items;
};


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

bool has(Value value, const Path& path, size_t index = 0);
Value get(Value value, const Path& path, size_t index = 0);

template<typename Ret>
Ret get(Value value, const Path& path)
{
    Value v = get(value, path.popBack());
    return v.get<Ret>(path.back());
}

namespace details{

template<typename Arg>
void set(Value value, const Path& path, size_t index, Arg&& arg);

} // namespace details

template<typename Arg>
void set(Value value, const Path& path, size_t index, Arg&& arg)
{
    Value v = get(value, path.popBack(), index);
    details::set(v, path, path.size() - 1, std::forward<Arg>(arg));
}

template<typename Arg>
void set(Value value, const Path& path, Arg&& arg)
{
    set(value, path, 0, std::forward<Arg>(arg));
}

template<typename Ret, typename... Args>
Ret call(Value value, const Path& path, Args&&... args)
{
    Value v = get(value, path.popBack());
    return v.call<Ret>(path.back(), std::forward<Args>(args)...);
}


} // namespace config
} // namespace reflect
