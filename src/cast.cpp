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

Value move(Value& value, const Argument& target)
{
    if (target.type()->isMovable()) {

        if (value.type()->isChildOf(target.type()))
                return std::move(value);

        if (value.type()->hasConverter(target.type()))
            return type()->converter(target.type()).call<Value>(value);
    }

    reflectError("<%s> is not movable to <%s>",
            value.argument().print(), target.print());
}

Value copy(Value& value, const Argument& target)
{
    if (target.type()->isCopiable()) {

        if (value.type()->isChildOf(target.type()))
            return target.type()->construct<Value>(value);

        if (value.type()->hasConverter(target.type()))
            return type()->converter(target.type()).call<Value>(value);
    }

    reflectError("<%s> is not copiable to <%s>",
            value.argument().print(), target.print());
}

} // namespace anonymous

bool isCastable(const Value& value, const Argument& target)
{
    return value.argument().isConvertibleTo(target);
}

Value cast(Value& value, const Argument& target)
{
    if (!isCastable(value, target)) {
        reflectError("<%s> is not castable to <%s>",
                value.argument().print(), target.print());
    }

    switch (target.refType()) {
    case RefType::LValue: return value;
    case RefType::RValue: return move(value, target);
    case RefType::Copy:   return copy(value, target);
    default: reflectUnreachable();
    }
}


} // namespace reflect
