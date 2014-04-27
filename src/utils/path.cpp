/* path.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Path implementation.
*/

#include "path.h"
#include "types/std/string.h"
#include "types/std/vector.h"

#include <algorithm>

namespace reflect {

/******************************************************************************/
/* PATH                                                                       */
/******************************************************************************/

Path::
Path(const std::string& path, char sep)
{
    size_t i = 0;
    while (i < path.size()) {

        size_t j = path.find(sep, i);

        if (j == std::string::npos) j = path.size();
        if (i == j) reflectError("empty path component <%s>", path);

        items.emplace_back(path.substr(i, j));
        i = j + 1;
    }
}

bool
Path::
isIndex(size_t index) const
{
    const auto& item = items[index];

    for (char c : item) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

size_t
Path::
index(size_t index) const
{
    const auto& item = items[index];

    for (char c : item) {
        if (!std::isdigit(c))
            reflectError("component at <%s> is not an index <%s>", index, item);
    }

    return std::stoull(item);
}

Path
Path::
popFront() const
{
    if (items.empty()) reflectError("pop on an empty path");

    Path result;
    result.items.insert(result.items.begin(), items.begin() + 1, items.end());
    return std::move(result);
}

Path
Path::
popBack() const
{
    if (items.empty()) reflectError("pop on an empty path");

    Path result;
    result.items.insert(result.items.begin(), items.begin(), items.end() - 1);
    return std::move(result);
}


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/
// We use recursive pathing to ensure that rvalue returns remain valid during
// the duration of the access.

bool has(Value value, const Path& path)
{
    if (!path) return true;

    if (value.type()->isPointer()) {
        if (value.type()->pointee()->isPointer()) return false;
        return has(*value, path);
    }

    if (value.is("list")) {
        if (!path.isIndex(0)) return false;
        return has(value[path.index(0)], path.popFront());
    }

    if (value.is("map")) {
        if (value.call<const Type*>("keyType") != type<std::string>())
            return false;

        if (!value.call<bool>("count", path.front()))
            return  false;

        return has(value[path.front()], path.popFront());
    }

    return has(value.get<Value>(path.front()), path.popFront());
}

Value get(Value value, const Path& path)
{
    if (value.type()->isPointer()) {
        if (value.type()->pointee()->isPointer()) {
            reflectError("can't path down a double indirection <%s>",
                    value.typeId());
        }
        return get(*value, path);
    }

    if (value.is("list"))
        return get(value[path.index(0)], path.popFront());

    if (value.is("map"))
        return get(value[path.front()], path.popFront());

    return get(value.get<Value>(path.front()), path.popFront());
}

} // reflect
