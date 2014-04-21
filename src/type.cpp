/* type.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type implementation.
*/

#include "reflect.h"

#include <algorithm>
#include <sstream>

namespace reflect {

/******************************************************************************/
/* TYPE                                                                       */
/******************************************************************************/

Type::
Type(std::string id, const Type* parent) :
    id_(std::move(id)), parent_(parent), pointee_(nullptr)
{}

void
Type::
addTrait(std::string trait)
{
    traits_.emplace(std::move(trait));
}

bool
Type::
is(const std::string& trait) const
{
    return traits_.count(trait);
}

std::vector<std::string>
Type::
traits() const
{
    return { traits_.begin(), traits_.end() };
}

void
Type::
addFunctionTrait(const std::string& fn, std::string trait)
{
    fnTraits_[fn].emplace(std::move(trait));
}

bool
Type::
functionIs(const std::string& fn, const std::string& trait) const
{
    auto it = fnTraits_.find(fn);
    if (it != fnTraits_.end())
        return it->second.count(trait);

    return parent_ ? parent_->functionIs(fn, trait) : false;
}

std::vector<std::string>
Type::
functionTraits(const std::string& fn) const
{
    auto it = fnTraits_.find(fn);
    if (it != fnTraits_.end())
        return { it->second.begin(), it->second.end() };

    return parent_ ? parent_->functionTraits(fn) : std::vector<std::string>();
}


bool
Type::
isChildOf(const Type* other) const
{
    if (this == other) return true;

    if (isPointer() && other->isPointer())
        return pointee()->isChildOf(other->pointee());

    return parent_ && parent_->isChildOf(other);
}

bool
Type::
isParentOf(const Type* other) const
{
    return other->isChildOf(this);
}

bool
Type::
hasConverter(const Type* other) const
{
    return hasFunction("operator " + other->id() + "()");
}

const Function&
Type::
converter(const Type* other) const
{
    auto& fns = function("operator " + other->id() + "()");

    if (fns.size() > 1) {
        reflectError("<%s> has too many converters for <%s>",
                id_, other->id());
    }

    return fns[0];
}

bool
Type::
isCopiable() const
{
    if (!hasFunction(id_)) return false;

    auto& fns = function(id_);
    return fns.test(
            Argument(this, RefType::Copy, false),
            { Argument(this, RefType::Copy, false) });
}

bool
Type::
isMovable() const
{
    if (!hasFunction(id_)) return false;

    auto& fns = function(id_);
    return fns.test(
            Argument(this, RefType::Copy, false),
            { Argument(this, RefType::RValue, false) });
}


void
Type::
functions(std::vector<std::string>& result, const std::string& trait) const
{
    result.reserve(result.size() + fns_.size());

    for (const auto& f : fns_) {
        if (!trait.empty() && !functionIs(f.first, trait)) continue;
        result.push_back(f.first);
    }

    if (parent_) parent_->functions(result, trait);
}

std::vector<std::string>
Type::
functions(const std::string& trait) const
{
    std::vector<std::string> result;
    functions(result, trait);

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

bool
Type::
hasFunction(const std::string& fn) const
{
    if (fns_.find(fn) != fns_.end()) return true;
    return parent_ ? parent_->hasFunction(fn) : false;
}

const Overloads&
Type::
function(const std::string& fn) const
{
    auto it = fns_.find(fn);
    if (it != fns_.end()) return it->second;

    if (!parent_)
        reflectError("<%s> doesn't have a function <%s>", id_, fn);

    return parent_->function(fn);
}

std::vector<std::string>
Type::
fields() const
{
    return functions("field");
}

bool
Type::
hasField(const std::string& field) const
{
    return functionIs(field, "field");
}

const Overloads&
Type::
field(const std::string& field) const
{
    if (!hasField(field))
        reflectError("<%s> doesn't have a field <%s>", id_, field);

    return function(field);
}

const Type*
Type::
fieldType(const std::string& field) const
{
    return this->field(field).fieldType();
}


bool
Type::
isPointer() const
{
    return is("pointer");
}

std::string
Type::
pointer() const
{
    if (!isPointer()) reflectError("<%s> is not a pointer", id());
    return pointer_;
}

const Type*
Type::
pointee() const
{
    if (!isPointer()) reflectError("<%s> is not a pointer", id());
    return pointee_;
}

void
Type::
setPointer(std::string pointer, const Type* pointee)
{
    if (isPointer()) reflectError("<%s> is already a pointer", id());

    addTrait("pointer");
    pointer_ = std::move(pointer);
    pointee_ = pointee;
}

void
Type::
add(const std::string& name, Function fn)
{
    bool isField = fn.isGetter() || fn.isSetter();
    fns_[name].add(std::move(fn));

    // let's add to fields if it passes the constructor and operator filter.

    if (!isField) return;
    if (name == id_) return;

    static const std::string op = "operator";
    size_t pos = name.find(op);

    if (pos == 0 && name.size() > op.size()) {
        char c = name[op.size()];
        if (c != '_' && !std::isalpha(c)) return;
    }

    addFunctionTrait(name, "field");
}


namespace {

void printTraits(
        std::stringstream& ss, const std::unordered_set<std::string>& traits)
{
    ss<< "traits: [ ";
    for (auto& trait : traits) ss << trait << " ";
    ss << "]\n";
}

} // namespace anonymous

std::string
Type::
print(size_t indent) const
{
    enum { PadInc = 4 };

    std::stringstream ss;
    std::string pad0(indent, ' ');

    indent += PadInc;
    std::string pad1(indent, ' ');
    std::string pad2(indent + PadInc, ' ');

    ss << pad0 << "struct " << id_ << "\n";
    ss << pad0 << "{\n";

    if (parent_) ss << parent_->print(indent) << "\n";

    if (!traits_.empty()) {
        ss << pad1; printTraits(ss, traits_);
    }

    for (auto& fn : fns_) {
        ss << pad1 << fn.first << ":\n";

        auto it = fnTraits_.find(fn.first);
        if (it != fnTraits_.end()) {
            ss << pad2; printTraits(ss, it->second);
        }

        ss << fn.second.print(indent + PadInc);
    }

    ss << pad0 << "}\n";

    return ss.str();
}

} // reflect
