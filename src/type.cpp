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

bool
Type::
fieldIs(const std::string& field, const std::string& trait) const
{
    return functionIs(field, trait);
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

std::vector<std::string>
Type::
fieldTraits(const std::string& field) const
{
    return functionTraits(field);
}


bool
Type::
isChildOf(const Type* other) const
{
    return this == other
        || (parent_ && parent_->isChildOf(other));
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
    return hasField("operator " + other->id() + "()");
}

const Function&
Type::
converter(const Type* other) const
{
    auto& fns = field("operator " + other->id() + "()");

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
    if (!hasField(id_)) return false;

    auto& fns = field(id_);
    return fns.test(
            Argument(this, RefType::Copy, false),
            { Argument(this, RefType::Copy, false) });
}

bool
Type::
isMovable() const
{
    if (!hasField(id_)) return false;

    auto& fns = field(id_);
    return fns.test(
            Argument(this, RefType::Copy, false),
            { Argument(this, RefType::RValue, false) });
}


void
Type::
functions(std::vector<std::string>& result) const
{
    result.reserve(result.size() + fns_.size());
    for (const auto& f : fns_)
        result.push_back(f.first);

    if (parent_) parent_->functions(result);
}

std::vector<std::string>
Type::
functions() const
{
    std::vector<std::string> result;
    functions(result);

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

bool
Type::
hasFunction(const std::string& function) const
{
    if (fns_.find(function) != fns_.end()) return true;
    return parent_ ? parent_->hasFunction(function) : false;
}

const Overloads&
Type::
function(const std::string& function) const
{
    auto it = fns_.find(function);
    if (it != fns_.end()) return it->second;

    if (!parent_)
        reflectError("<%s> doesn't have a function <%s>", id_, function);

    return parent_->function(function);
}


void
Type::
fields(std::vector<std::string>& result) const
{
    result.reserve(result.size() + fns_.size());
    for (const auto& f : fns_) {
        if (!f.second.isField()) continue;
        result.push_back(f.first);
    }

    if (parent_) parent_->fields(result);
}

std::vector<std::string>
Type::
fields() const
{
    std::vector<std::string> result;
    fields(result);

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

bool
Type::
hasField(const std::string& field) const
{
    auto it = fns_.find(field);
    if (it != fns_.end() && it->second.isField()) return true;
    return parent_ ? parent_->hasField(field) : false;
}

const Overloads&
Type::
field(const std::string& field) const
{
    auto it = fns_.find(field);
    if (it != fns_.end() && it->second.isField()) return it->second;

    if (!parent_)
        reflectError("<%s> doesn't have a field <%s>", id_, field);

    return parent_->field(field);
}

const Type*
Type::
fieldType(const std::string& field) const
{
    auto& f = this->field(field);
    return f.fieldType();
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

    for (auto& field : fns_) {
        ss << pad1 << field.first << ":\n";

        auto it = fnTraits_.find(field.first);
        if (it != fnTraits_.end()) {
            ss << pad2; printTraits(ss, it->second);
        }

        ss << field.second.print(indent + PadInc);
    }

    ss << pad0 << "}\n";

    return ss.str();
}

} // reflect
