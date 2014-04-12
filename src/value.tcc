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
Value(T&& value)
{
    Value::fill(*this, std::forward<T>(value));
}


// compile-tile optimization: Allows Value construction in heavily used
// templates.
template<typename T>
void 
Value::
fill(Value& obj, T&& value)
{
    obj.arg = Argument::make(std::forward<T>(value));
    obj.value_ = (void*)&value; // cast-away constness

    if (obj.refType() != RefType::RValue) return;

    typedef typename std::decay<T>::type CleanT;
    reflectStaticAssert(
            std::is_copy_constructible<CleanT>::value ||
            std::is_move_constructible<CleanT>::value);

    obj.storage.reset(obj.value_ = new CleanT(std::forward<T>(value)));

    // We now own the value so we're now l-ref-ing our internal storage.
    obj.arg = Argument(obj.arg.type(), RefType::LValue, false);
}


template<typename T>
const T&
Value::
get() const
{
    if (!type()->isConvertibleTo<T>()) {
        reflectError("<%s> is not convertible to <%s>",
                type()->id(), reflect::type<T>()->id());
    }

    return *static_cast<T*>(value_);
}


template<typename T>
bool
Value::
castable() const
{
    typedef typename CleanRef<T>::type Target;
    return !isVoid() && arg.isConvertibleTo<Target>();
}

template<typename T>
auto
Value::
cast() const -> typename CleanRef<T>::type
{
    if (!castable<T>()) {
        reflectError("<%s> is not castable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;
    return *static_cast<CleanT*>(value_);
}


template<typename T>
bool
Value::
copiable() const
{
    typedef typename CleanValue<T>::type Target;
    return !isVoid()
        && type()->isCopiable()
        && arg.isConvertibleTo<Target>();
}

template<typename T>
auto
Value::
copy() const -> typename CleanValue<T>::type
{
    if (!copiable<T>()) {
        reflectError("<%s> is not copiable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;
    return *static_cast<CleanT*>(value_);
}


template<typename T>
bool
Value::
movable() const
{
    typedef typename CleanValue<T>::type Target;
    return !isVoid()
        && type()->isMovable()
        && !isConst()
        && arg.isConvertibleTo<Target>();
}

template<typename T>
auto
Value::
move() -> typename CleanValue<T>::type
{
    if (!movable<T>()) {
        reflectError("<%s> is not movable to <%s>",
                arg.print(), printArgument<T>());
    }

    typedef typename std::decay<T>::type CleanT;

    CleanT value = std::move(*static_cast<CleanT*>(value_));
    *this = Value();
    return value;
}


template<typename Ret, typename... Args>
Ret
Value::
call(const char* fn, Args&&... args)
{
    auto& field = type()->field(fn);
    return field.call<Ret>(*this, std::forward<Args>(args)...);
}

template<typename Ret>
Ret
Value::
get(const char* field)
{
    return call<Ret>(field);
}

template<typename Arg>
void
Value::
set(const char* field, Arg&& arg)
{
    call<void>(field, std::forward<Arg>(arg));
}


} // reflect
