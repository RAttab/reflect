/* value.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value implementation. Mostly exists to break some header dependencies.
*/

#include "reflect.h"
#include "types/void.h"

namespace reflect {


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

Value::
Value() : value_(nullptr) {}

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


Value
Value::
copy() const
{
    if (!type()->isCopiable())
        reflectError("<%s> is not copiable", type()->id());

    reflectError("Value::copy() is not implemented yet.");
    return Value();
}

Value
Value::
move()
{
    if (!type()->isMovable())
        reflectError("<%s> is not movable", type()->id());

    reflectError("Value::move() is not implemented yet.");
    return Value();
}


} // reflect
