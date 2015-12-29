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
    if (!target.isConst()) return value;
    if (target.type()->isParentOf(value.type())) return value;

    if (value.type()->hasConverter(target.type()))
        return value.convert<Value>(target.type());

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


bool isCastable(const Value& value, const Argument& target)
{
    return value.argument().isConvertibleTo(target) != Match::None;
}

Value cast(Value& value, const Argument& target)
{
    if (!isCastable(value, target)) {
        reflectError("<%s> is not castable to <%s>",
                value.argument().print(), target.print());
    }

    switch (target.refType()) {
    case RefType::LValue: return lref(value, target);
    case RefType::RValue: return rref(value, target);
    case RefType::Copy:   return copy(value, target);
    default: reflectUnreachable();
    }
}


} // namespace reflect
