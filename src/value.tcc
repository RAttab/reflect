/* value.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Template implementation for Value.
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

template<typename T>
Value::
Value(T&& value) :
    arg(Argument::make(std::forward<T>(value))),
    value_((void*)&value) // cast-away any const
{
    if (refType() != RefType::RValue) return;

    typedef typename std::decay<T>::type CleanT;
    reflectStaticAssert(
            std::is_copy_constructible<CleanT>::value ||
            std::is_move_constructible<CleanT>::value);

    storage.reset(value_ = new CleanT(std::forward<T>(value)));

    // We now own the value so we're now l-ref-ing our internal storage.
    arg = Argument(arg.type(), RefType::LValue, false);
}


template<typename T>
const T&
Value::
get() const
{
    if (!type()->isChildOf<T>()) {
        reflectError("<%s> is not a base of <%s>",
                type()->id(), reflect::type<T>()->id());
    }

    return *static_cast<T*>(value_);
}


template<typename T>
bool
Value::
isCastable() const
{
    typedef typename CleanRef<T>::type RefT;
    return arg.isConvertibleTo<RefT>();
}

template<typename T>
auto
Value::
cast() const -> typename CleanRef<T>::type
{
    if (!isCastable<T>()) {
        reflectError("<%s> is not castable to <%s>",
                arg.print(), printArgument<T>());
    }

    // no conversion can take place if we're returning a ref.

    typedef typename std::decay<T>::type CleanT;
    return *static_cast<CleanT*>(value_);
}


template<typename T>
T
Value::
convert() const
{
    reflectStaticAssert((std::is_same< T, typename std::decay<T>::type>::value));

    auto& converter = type()->converter<T>();
    return converter.call<T>(*this);
}


template<typename T>
bool
Value::
isCopiable() const
{
    Type* target = reflect::type<T>();
    return target->isCopiable()
        && (type()->isChildOf(target) || type()->hasConverter(target));
}

template<typename T>
auto
Value::
copy() const -> typename CleanValue<T>::type
{
    if (!isCopiable<T>()) {
        reflectError("<%s> is not copiable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;

    if (type()->isChildOf<T>())
        return *static_cast<CleanT*>(value_);

    return convert<CleanT>();
}


template<typename T>
bool
Value::
isMovable() const
{
    reflectStaticAssert(!std::is_lvalue_reference<T>::value);

    Type* target = reflect::type<T>();

    if (!target->isMovable()) return false;
    if (type()->isChildOf(target)) return !isConst();
    return type()->hasConverter(target);
}

template<typename T>
auto
Value::
move() -> typename CleanValue<T>::type
{
    if (!isMovable<T>()) {
        reflectError("<%s> is not movable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;

    CleanT value = type()->isChildOf<CleanT>() ?
        std::move(*static_cast<CleanT*>(value_)) :
        convert<CleanT>();

    *this = Value();
    return std::move(value);
}


template<typename Ret, typename... Args>
Ret
Value::
call(const std::string& fn, Args&&... args) const
{
    const auto& field = type()->field(fn);
    return field.call<Ret>(*this, std::forward<Args>(args)...);
}

template<typename Ret>
Ret
Value::
get(const std::string& field) const
{
    return call<Ret>(field);
}

template<typename Arg>
void
Value::
set(const std::string& field, Arg&& arg) const
{
    call<void>(field, std::forward<Arg>(arg));
}


} // reflect
