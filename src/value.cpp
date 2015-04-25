/* value.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value implementation.
*/

#include "reflect.h"

namespace reflect {


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

Value::
Value() : value_(nullptr) {}

// This is required to avoid trigerring the templated constructor for Value when
// trying to copy non-const Values. This is common in data-structures like
// vectors where entries would get infinitely wrapped in layers of Values
// everytime a resize takes place.
Value::
Value(Value& other) :
    arg(other.arg),
    value_(other.value_),
    storage(other.storage)
{}

Value::
Value(const Value& other) :
    arg(other.arg),
    value_(other.value_),
    storage(other.storage)
{}

Value&
Value::
operator=(const Value& other)
{
    if (this == &other) return *this;

    arg = other.arg;
    value_ = other.value_;
    storage = other.storage;

    return *this;
}

Value::
Value(Value&& other) :
    arg(std::move(other.arg)),
    value_(std::move(other.value_)),
    storage(std::move(other.storage))
{}

Value&
Value::
operator=(Value&& other)
{
    if (this == &other) return *this;

    arg = std::move(other.arg);
    value_ = std::move(other.value_);
    storage = std::move(other.storage);

    return *this;
}

const std::string&
Value::
typeId() const
{
    return type()->id();
}

bool
Value::
is(const std::string& trait) const
{
    return type()->is(trait);
}


Value
Value::
copy() const
{
    if (!type()->isCopiable())
        reflectError("<%s> is not copiable", type()->id());

    return type()->construct(*this);
}

Value
Value::
move()
{
    if (!type()->isMovable())
        reflectError("<%s> is not movable", type()->id());

    Value arg = rvalue();
    *this = Value();
    return arg.type()->construct(arg);
}

Value
Value::
rvalue() const
{
    Value result(*this);
    result.arg = Argument(type(), RefType::RValue, isConst());
    return result;
}

bool
Value::
operator!() const
{
    if (type()->hasFunction("operator!"))
        return call<bool>("operator!");
    return !((bool) *this);
}

Value::
operator bool() const
{
    return call<bool>("operator bool()");
}


} // reflect
