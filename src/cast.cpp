/* cast.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Dec 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"

namespace reflect {

/******************************************************************************/
/* CAST                                                                       */
/******************************************************************************/

namespace {

Value lref(Value &value, const Argument& target)
{
    if (target.type()->isParentOf(value.type())) {
        if (target.isConst()) return value;
        if (!value.isConst() && value.refType() == RefType::LValue)
            return value;
    }

    // While this is allowed in C++, we can't return a reference to a value that
    // we're constructing here because that would lead to a dangling reference.
    //
    // No idea how to solve this issue unfortunately so it's disabled for now.
    if (false && target.isConst()) {
        if (value.type()->hasConverter(target.type()))
            return value.convert<Value>(target.type());
    }

    reflectError("<%s> is not castable to <%s>",
            value.argument().print(), target.print());
}

Value rref(Value& value, const Argument& target)
{
    if (target.type()->isMovable()) {

        if (value.type()->isChildOf(target.type()))
            return value;

        if (value.type()->hasConverter(target.type()))
            return value.convert<Value>(target.type());
    }

    reflectError("<%s> is not movable to <%s>",
            value.argument().print(), target.print());
}

Value copy(Value& value, const Argument& target)
{
    if (target.type()->isCopiable()) {

        if (value.type()->isChildOf(target.type()))
            return target.type()->construct(value);

        if (value.type()->hasConverter(target.type()))
            return value.convert<Value>(target.type());
    }

    reflectError("<%s> is not copiable to <%s>",
            value.argument().print(), target.print());
}

} // namespace anonymous


// isCastable is currently more strict then cast is.
//
// This is due to Argument::isConvertibleTo being designed to test function
// signature and cast working on Value object which don't support the full-range
// of ref types. Because of this we need to support LRef to RRef casts because
// Value(123) will result in an LRef which we can't properly convert to an RRef
// in the casting code.
//
// Short version, a bit of a clusterfuck. Will need to think up a better
// solution.

bool isCastable(const Value& value, const Argument& target)
{
    return value.argument().isConvertibleTo(target) != Match::None;
}

Value cast(Value& value, const Argument& target)
{
    switch (target.refType()) {
    case RefType::LValue: return lref(value, target);
    case RefType::RValue: return rref(value, target);
    case RefType::Copy:   return copy(value, target);
    default: reflectUnreachable();
    }
}


} // namespace reflect
