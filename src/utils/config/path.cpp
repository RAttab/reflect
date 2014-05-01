/* path.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Path implementation.
*/

#include "includes.h"
#include "types/std/string.h"
#include "types/std/vector.h"

#include <algorithm>

namespace reflect {
namespace config {

/******************************************************************************/
/* PATH                                                                       */
/******************************************************************************/

void
Path::
parse(const std::string& path, char sep)
{
    size_t i = 0;
    while (i < path.size()) {

        size_t j = path.find(sep, i);

        if (j == std::string::npos) j = path.size();
        if (i == j) reflectError("empty path component <%s>", path);

        items.emplace_back(path.substr(i, j - i));
        i = j + 1;
    }
}

std::string
Path::
toString(char sep) const
{
    std::string path;

    for (size_t i = 0; i < items.size(); ++i) {
        if (i) path += sep;
        path += items[i];
    }

    return path;
}


Path::
Path(const std::string& path, char sep)
{
    parse(path, sep);
}

Path::
Path(const char* path, char sep)
{
    parse(path, sep);
}

Path::
Path(const Path& prefix, const std::string& path, char sep) :
    items(prefix.items)
{
    parse(path, sep);
}

Path::
Path(const Path& prefix, size_t index) :
    items(prefix.items)
{
    items.emplace_back(std::to_string(index));
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

bool
Path::
operator<(const Path& other) const
{
    for (size_t i = 0; i < std::min(size(), other.size()); ++i) {
        int res = items[i].compare(other[i]);
        if (!res) continue;

        return res < 0;
    }

    return size() < other.size();
}


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/
// We use recursive pathing to ensure that rvalue returns remain valid during
// the duration of the access.

bool has(Value value, const Path& path, size_t index)
{
    if (index == path.size()) return true;

    if (value.type()->isPointer()) {
        if (value.type()->pointee()->isPointer()) return false;
        return has(*value, path, index);
    }

    if (value.is("list")) {
        size_t i = path.index(index);

        if (!path.isIndex(index)) return false;
        if (i >= value.call<size_t>("size")) return false;

        return has(value[i], path, index + 1);
    }

    if (value.is("map")) {
        if (value.type()->call<const Type*>("keyType") != type<std::string>())
            return false;

        if (!value.call<size_t>("count", path[index]))
            return false;

        return has(value[path[index]], path, index + 1);
    }

    if (!value.type()->hasField(path[index])) return false;

    return has(value.get<Value>(path[index]), path, index + 1);
}


Value get(Value value, const Path& path, size_t index)
{
    if (index == path.size()) return value;

    if (value.type()->isPointer()) {
        if (value.type()->pointee()->isPointer()) {
            reflectError("can't path down a double indirection <%s>",
                    value.typeId());
        }
        return get(*value, path, index);
    }

    if (value.is("list"))
        return get(value[path.index(index)], path, index + 1);

    if (value.is("map"))
        return get(value[path[index]], path, index + 1);

    return get(value.get<Value>(path[index]), path, index + 1);
}


} // namespace config
} // namespace reflect
