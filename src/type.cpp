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

bool
Type::
isConvertibleTo(Type* other)
{
    return this == other
        || (parent_ && parent_->isConvertibleTo(other));
}

void
Type::
fields(std::vector<std::string>& result) const
{
    result.reserve(result.size() + fields_.size());
    for (const auto& f : fields_)
        result.push_back(f.first);

    if (parent_) parent_->fields(result);
}

std::vector<std::string>
Type::
fields() const
{
    std::vector<std::string> result;
    fields(result);

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()));

    return result;
}

bool
Type::
hasField(const std::string& field) const
{
    if (fields_.find(field) != fields_.end()) return true;
    return parent_ ? parent_->hasField(field) : false;
}

const Functions&
Type::
field(const std::string& field) const
{
    auto it = fields_.find(field);
    if (it != fields_.end()) return it->second;

    if (!parent_)
        reflectError("<%s> doesn't have a field <%s>", id_, field);

    return parent_->field(field);
}


std::string
Type::
print(size_t indent) const
{
    enum { PadInc = 4 };

    std::stringstream ss;
    std::string pad0(indent, ' ');

    indent += PadInc;
    std::string pad1(indent, ' ');

    ss << pad0 << "struct " << id_ << "\n";
    ss << pad0 << "{\n";

    if (parent_) ss << parent_->print(indent) << "\n";

    for (const auto& field : fields_) {
        ss << pad1 << field.first << ":\n";
        ss << field.second.print(indent + PadInc);
    }

    ss << pad0 << "}\n";

    return ss.str();
}

} // reflect
