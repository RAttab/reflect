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
Type(std::string id) :
    id_(std::move(id)), parent_(nullptr), pointee_(nullptr)
{}

bool
Type::
isChildOf(const Type* other) const
{
    if (this == other) return true;

    if (isPointer() && other->isPointer()) {
        if (pointer() != other->pointer()) return false;
        return pointee()->isChildOf(other->pointee());
    }

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
addFunction(const std::string& name, Function&& fn)
{
    fns_[name].add(std::move(fn));
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
hasFunction(const std::string& fn) const
{
    if (fns_.count(fn)) return true;
    return parent_ ? parent_->hasFunction(fn) : false;
}

Overloads&
Type::
function(const std::string& fn)
{
    auto it = fns_.find(fn);
    if (it != fns_.end()) return it->second;

    if (!parent_)
        reflectError("<%s> doesn't have a function <%s>", id_, fn);

    return parent_->function(fn);
}

const Overloads&
Type::
function(const std::string& fn) const
{
    return const_cast<Type*>(this)->function(fn);
}

void
Type::
addField(const std::string& name, Field&& field)
{
    auto ret = fields_.emplace(name, std::move(field));
    if (!ret.second) {
        reflectError("unable to add field <%s>, already exists as <%s>",
                field.print(), ret.first->second.print());
    }
}

void
Type::
fields(std::vector<std::string>& result) const
{
    result.reserve(result.size() + fields_.size());

    for (const auto& field : fields_)
        result.push_back(field.first);

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
    if (fields_.count(field)) return true;
    return parent_ ? parent_->hasField(field) : false;
}

Field&
Type::
field(const std::string& field)
{
    auto it = fields_.find(field);
    if (it != fields_.end()) return it->second;

    if (!parent_)
        reflectError("<%s> doesn't have a field <%s>", id_, field);

    return parent_->field(field);
}

const Field&
Type::
field(const std::string& field) const
{
    return const_cast<Type*>(this)->field(field);
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

namespace  {

std::vector<const Field*>
sortedFields(const std::unordered_map<std::string, Field>& fields)
{
    std::vector<const Field*> result;
    result.reserve(fields.size());

    for (const auto& field : fields)
        result.push_back(&field.second);

    std::sort(result.begin(), result.end(), [](const Field* lhs, const Field* rhs) {
                return lhs->offset() < rhs->offset();
            });

    return result;
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

    ss << pad0 << "type " << id_ << "\n";
    ss << pad0 << "{\n";

    if (parent_) ss << parent_->print(indent) << "\n";

    if (!traits().empty())
        ss << pad1 << Traits::print() << "\n";

    for (const auto& field : sortedFields(fields_))
        ss << pad1 << field->print() << "\n";

    ss << pad1 << "\n";

    for (const auto& fn : fns_) {
        ss << pad1 << fn.first << ":\n";
        ss << fn.second.print(indent + PadInc);
    }

    ss << pad0 << "}\n";

    return ss.str();
}

} // reflect
