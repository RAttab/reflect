/* namespace.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Namespace implementation.
*/

#include "reflect.h"

#include <sstream>

namespace reflect {

/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

std::pair<std::string, std::string>
Namespace::
head(const std::string& name)
{
    size_t pos = name.find("::");
    size_t next = pos + 2;

    if (pos == std::string::npos)
        pos = next = name.size();

    return std::make_pair(name.substr(0, pos), name.substr(next));
}

std::pair<std::string, std::string>
Namespace::
tail(const std::string& name)
{
    size_t pos = name.rfind("::");
    size_t next = pos;

    if (pos == std::string::npos)
        pos = next = 0;
    else pos += 2;

    return std::make_pair(name.substr(pos), name.substr(0, next));
}

std::string
Namespace::
join(const std::string& head, const std::string& tail)
{
    return head.empty() ? tail : (head + "::" + tail);
}


/******************************************************************************/
/* BASICS                                                                     */
/******************************************************************************/

Namespace::
Namespace() : parent_(nullptr)
{}

Namespace::
Namespace(const std::string& name, Namespace* parent) :
    name_(name), parent_(parent)
{}

std::string
Namespace::
id() const
{
    if (!parent_) return "";
    return join(parent_->id(), name_);
}

std::string
Namespace::
print(int indent) const
{
    enum { PadInc = 4 };

    std::stringstream ss;
    std::string pad0(indent, ' ');

    indent += PadInc;
    std::string pad1(indent, ' ');

    ss << pad0 << "namespace " << name_ << "\n";
    ss << pad0 << "{\n";

    for (auto& fn : functions_) {
        ss << pad1 << fn.first << ":\n";
        ss << fn.second.print(indent + PadInc);
    }

    if (!functions_.empty() && (!types_.empty() || !subs_.empty()))
        ss << pad0 << "\n";

    for (auto& type : types_)
        ss << pad1 << "struct " << type.first << ";\n";

    if (!types_.empty() && !subs_.empty()) ss << pad0 << "\n";

    for (auto& sub : subs_)
        ss << sub.second->print(indent) << "\n";

    ss << pad0 << "}\n";

    return ss.str();
}


/******************************************************************************/
/* SUBS                                                                       */
/******************************************************************************/

std::vector<std::string>
Namespace::
subs() const
{
    std::vector<std::string> result;
    result.reserve(subs_.size());

    for(auto& sub : subs_)
        result.push_back(sub.first);

    return result;
}

bool
Namespace::
hasSub(const std::string& name) const
{
    auto split = head(name);

    auto it = subs_.find(split.first);
    if (it == subs_.end()) return false;

    return !split.second.empty() ? it->second->hasSub(split.second) : true;
}

Namespace*
Namespace::
sub(const std::string& name) const
{
    auto split = head(name);

    auto it = subs_.find(split.first);
    if (it == subs_.end())
        reflectError("<%s> doesn't have sub <%s>", name_, name);

    return !split.second.empty() ? it->second->sub(split.second) : it->second;
}

Namespace*
Namespace::
sub(const std::string& name)
{
    auto split = head(name);

    auto it = subs_.find(split.first);
    if (it == subs_.end()) {
        std::unique_ptr<Namespace> sub(new Namespace(split.first, this));
        it = subs_.emplace(split.first, sub.release()).first;
    }

    return !split.second.empty() ? it->second->sub(split.second) : it->second;
}


/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/

std::vector<std::string>
Namespace::
types(bool includeSubs) const
{
    std::vector<std::string> result;
    result.reserve(types_.size());

    for(auto& type : types_)
        result.push_back(type.first);

    if (!includeSubs) return result;

    for (auto& sub : subs_) {
        auto subTypes = sub.second->types(true);
        result.reserve(result.size() + subTypes.size());

        for (auto& type : subTypes)
            result.emplace_back(join(sub.first, type));
    }

    return result;
}

bool
Namespace::
hasType(const std::string& name)
{
    auto split = head(name);

    auto it = types_.find(split.first);
    if (it == types_.end()) {
        if (split.second.empty()) return false;

        auto subIt = subs_.find(split.first);
        if (subIt == subs_.end()) return false;

        return subIt->second->hasType(split.second);
    }

    if (!split.second.empty())
        reflectError("Type doesn't support inner classes yet");

    return true;
}

const Type*
Namespace::
type(const std::string& name)
{
    auto split = head(name);

    auto it = types_.find(split.first);
    if (it == types_.end()) {

        auto subIt = subs_.find(split.first);
        if (split.second.empty() || subIt == subs_.end())
            reflectError("unknown type <%s::%s>", id(), split.first);

        return subIt->second->type(split.second);
    }

    // lazy load the type.
    if (!it->second)
        it->second = reflect::type(join(id(), split.first));

    if (!split.second.empty())
        reflectError("Type doesn't support inner classes yet");

    return it->second;
}


void
Namespace::
addType(const std::string& name)
{
    auto split = head(name);

    if (split.second.empty()) {
        // The type is added before it's loaded so we'll lazy loaded as needed.
        types_.emplace(split.first, nullptr);
        return;
    }

    auto typeIt = types_.find(split.first);
    if (typeIt != types_.end())
        reflectError("Type doesn't support inner classes yet");

    auto fnIt = functions_.find(split.first);
    if (fnIt != functions_.end())
        reflectError("<%s> conflicts with function in <%s>", name, id());

    sub(split.first)->addType(split.second);
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

std::vector<std::string>
Namespace::
functions(bool includeSubs) const
{
    std::vector<std::string> result;
    result.reserve(types_.size());

    for(auto& function : functions_)
        result.push_back(function.first);

    if (!includeSubs) return result;

    for (auto& sub : subs_) {
        auto subFunctions = sub.second->functions(true);
        result.reserve(result.size() + subFunctions.size());

        for (auto& function : subFunctions)
            result.emplace_back(join(sub.first, function));
    }

    return result;
}

bool
Namespace::
hasFunction(const std::string& name) const
{
    auto split = tail(name);

    if (!split.second.empty())
        return sub(split.second)->hasFunction(split.first);

    auto it = functions_.find(split.first);
    return it != functions_.end();
}

const Overloads&
Namespace::
function(const std::string& name) const
{
    auto split = tail(name);

    if (!split.second.empty())
        return sub(split.second)->function(split.first);

    auto it = functions_.find(split.first);
    if (it == functions_.end())
        reflectError("<%s> has no function <%s>", id(), split.first);

    return it->second;
}

void
Namespace::
addFunction(const std::string& name, Function fn)
{
    auto split = tail(name);

    if (!split.second.empty())
        return sub(split.second)->addFunction(split.first, std::move(fn));

    functions_[split.first].add(std::move(fn));
}


} // reflect
