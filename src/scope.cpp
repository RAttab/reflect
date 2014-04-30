/* scope.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Scope implementation.
*/

#include "reflect.h"

#include <sstream>

namespace reflect {

/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

std::pair<std::string, std::string>
Scope::
head(const std::string& name)
{
    size_t i;
    size_t next = name.size();
    size_t nesting = 0;

    for (i = 0; i < name.size(); ++i) {

        switch (name[i]) {
        case '<': nesting++; break;
        case '>': nesting--; break;
        case ':':
            if (nesting) continue;
            if (name.size() <= i + 1 || name[i + 1] != ':')
                reflectError("unmatched <%c> in <%s>", ':', name);

            next = i + 2;
            goto done;
        }
    }

  done:
    if (nesting) reflectError("unmatched <%c> in <%s>", '<', name);
    return std::make_pair(name.substr(0, i), name.substr(next));
}

std::pair<std::string, std::string>
Scope::
tail(const std::string& name)
{
    size_t i;
    size_t nesting = 0;
    size_t next = 0;

    for (i = name.size(); i > 0;) {
        i--;

        switch (name[i]) {
        case '<': nesting--; break;
        case '>': nesting++; break;
        case ':':
            if (nesting) continue;
            if (!i || name[i - 1] != ':')
                reflectError("unmatched <%c> in <%s>", ':', name);

            i++;
            next = i - 2;
            goto done;
        }
    }

  done:
    if (nesting) reflectError("unmatched <%c> in <%s>", '<', name);
    return std::make_pair(name.substr(i), name.substr(0, next));
}

std::string
Scope::
join(const std::string& head, const std::string& tail)
{
    return head.empty() ? tail : (head + "::" + tail);
}


/******************************************************************************/
/* BASICS                                                                     */
/******************************************************************************/

Scope::
Scope() : parent_(nullptr)
{}

Scope::
Scope(const std::string& name, Scope* parent) :
    name_(name), parent_(parent)
{}

std::string
Scope::
id() const
{
    if (!parent_) return "";
    return join(parent_->id(), name_);
}

std::string
Scope::
print(int indent) const
{
    enum { PadInc = 4 };

    std::stringstream ss;
    std::string pad0(indent, ' ');

    indent += PadInc;
    std::string pad1(indent, ' ');

    ss << pad0 << "scope " << name_ << "\n";
    ss << pad0 << "{\n";

    for (auto& fn : functions_) {
        ss << pad1 << fn.first << ":\n";
        ss << fn.second.print(indent + PadInc);
    }

    if (!functions_.empty() && (!types_.empty() || !scopes_.empty()))
        ss << pad0 << "\n";

    for (auto& type : types_)
        ss << pad1 << "type " << type.first << ";\n";

    if (!types_.empty() && !scopes_.empty()) ss << pad0 << "\n";

    for (auto& scope : scopes_)
        ss << scope.second->print(indent) << "\n";

    ss << pad0 << "}\n";

    return ss.str();
}


/******************************************************************************/
/* SCOPES                                                                     */
/******************************************************************************/

std::vector<std::string>
Scope::
scopes() const
{
    std::vector<std::string> result;
    result.reserve(scopes_.size());

    for(auto& scope : scopes_)
        result.push_back(scope.first);

    return result;
}

bool
Scope::
hasScope(const std::string& name) const
{
    auto split = head(name);

    auto it = scopes_.find(split.first);
    if (it == scopes_.end()) return false;

    return !split.second.empty() ? it->second->hasScope(split.second) : true;
}

Scope*
Scope::
scope(const std::string& name) const
{
    auto split = head(name);

    auto it = scopes_.find(split.first);
    if (it == scopes_.end())
        reflectError("<%s> doesn't have scope <%s>", name_, name);

    return !split.second.empty() ? it->second->scope(split.second) : it->second;
}

Scope*
Scope::
scope(const std::string& name)
{
    auto split = head(name);

    auto it = scopes_.find(split.first);
    if (it == scopes_.end()) {
        std::unique_ptr<Scope> scope(new Scope(split.first, this));
        it = scopes_.emplace(split.first, scope.release()).first;
    }

    return !split.second.empty() ? it->second->scope(split.second) : it->second;
}


/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/

std::vector<std::string>
Scope::
types(bool includeScopes) const
{
    std::vector<std::string> result;
    result.reserve(types_.size());

    for(auto& type : types_)
        result.push_back(type.first);

    if (!includeScopes) return result;

    for (auto& scope : scopes_) {
        auto scopeTypes = scope.second->types(true);
        result.reserve(result.size() + scopeTypes.size());

        for (auto& type : scopeTypes)
            result.emplace_back(join(scope.first, type));
    }

    return result;
}

bool
Scope::
hasType(const std::string& name)
{
    auto split = head(name);

    auto it = types_.find(split.first);
    if (it == types_.end()) {
        if (split.second.empty()) return false;

        auto scopeIt = scopes_.find(split.first);
        if (scopeIt == scopes_.end()) return false;

        return scopeIt->second->hasType(split.second);
    }

    if (!split.second.empty())
        reflectError("Type doesn't support inner classes yet");

    return true;
}

const Type*
Scope::
type(const std::string& name)
{
    auto split = head(name);

    auto it = types_.find(split.first);
    if (it == types_.end()) {

        auto scopeIt = scopes_.find(split.first);
        if (split.second.empty() || scopeIt == scopes_.end())
            reflectError("unknown type <%s::%s>", id(), split.first);

        return scopeIt->second->type(split.second);
    }

    // lazy load the type.
    if (!it->second)
        it->second = reflect::type(join(id(), split.first));

    if (!split.second.empty())
        reflectError("Type doesn't support inner classes yet");

    return it->second;
}


void
Scope::
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

    scope(split.first)->addType(split.second);
}


/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

std::vector<std::string>
Scope::
functions(bool includeScopes) const
{
    std::vector<std::string> result;
    result.reserve(types_.size());

    for(auto& function : functions_)
        result.push_back(function.first);

    if (!includeScopes) return result;

    for (auto& scope : scopes_) {
        auto scopeFunctions = scope.second->functions(true);
        result.reserve(result.size() + scopeFunctions.size());

        for (auto& fn : scopeFunctions)
            result.emplace_back(join(scope.first, fn));
    }

    return result;
}

bool
Scope::
hasFunction(const std::string& name) const
{
    auto split = tail(name);

    if (!split.second.empty())
        return scope(split.second)->hasFunction(split.first);

    auto it = functions_.find(split.first);
    return it != functions_.end();
}

const Overloads&
Scope::
function(const std::string& name) const
{
    auto split = tail(name);

    if (!split.second.empty())
        return scope(split.second)->function(split.first);

    auto it = functions_.find(split.first);
    if (it == functions_.end())
        reflectError("<%s> has no function <%s>", id(), split.first);

    return it->second;
}

void
Scope::
addFunction(const std::string& name, Function fn)
{
    auto split = tail(name);

    if (!split.second.empty())
        return scope(split.second)->addFunction(split.first, std::move(fn));

    functions_[split.first].add(std::move(fn));
}


} // reflect
