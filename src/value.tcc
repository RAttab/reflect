/* value.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Template implementation for Value.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

template<typename T>
struct IsMovable
{
    static constexpr bool value =
        !std::is_const<typename std::remove_reference<T>::type>::value &&
        std::is_move_constructible<typename std::decay<T>::type>::value;

    typedef std::integral_constant<bool, value> type;
};

template<typename T>
struct ValueDestructor
{
    void operator() (void* ptr) const
    {
        delete static_cast<T*>(ptr);
    }
};


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

template<typename T, typename Meh>
void* store(T&& value, std::true_type, Meh)
{
    typedef typename std::decay<T>::type CleanT;
    return new CleanT(std::move(value));
}

template<typename T>
void* store(T&& value, std::false_type, std::true_type)
{
    typedef typename std::decay<T>::type CleanT;
    return new CleanT(value);
}

template<typename T, typename... Rest>
void* store(Rest&&...)
{
    reflectError(
            "<%s> cannot be stored (no move/copy constructor)",
            printArgument<T>());
}

template<typename T>
Value::
Value(T&& value) :
    arg(Argument::make(std::forward<T>(value))),
    value_((void*)&value) // cast-away any const
{
    if (refType() != RefType::RValue) return;

    typedef typename std::decay<T>::type CleanT;

    value_ = store<T>(std::forward<T>(value),
            typename IsMovable<T>::type(),
            typename std::is_copy_constructible<CleanT>::type());

    storage.reset(value_, ValueDestructor<CleanT>());

    // We now own the value so we're now l-ref-ing our internal storage.
    arg = Argument(arg.type(), RefType::LValue, false);
}

template<typename T>
T&
Value::
as()
{
    if (!type()->isChildOf<T>()) {
        reflectError("<%s> is not a base of <%s>",
                type()->id(), reflect::type<T>()->id());
    }

    return *static_cast<T*>(value_);
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

    return *static_cast<const T*>(value_);
}


template<typename Ret, typename... Args>
Ret
Value::
call(const std::string& fn, Args&&... args) const
{
    const auto& f = type()->function(fn);
    return f.call<Ret>(*this, std::forward<Args>(args)...);
}

template<typename Ret>
Ret
Value::
field(const std::string& field) const
{
    const auto& f = type()->field(field);
    bool isConst = f.argument().isConst() || this->isConst();

    Value value;
    value.arg = Argument(f.type(), RefType::LValue, isConst);
    value.value_ = static_cast<uint8_t*>(value_) + f.offset();

    return retCast<Ret>(value);
}

template<typename Arg>
void
Value::
assign(Arg&& arg)
{
    reflect::assign(this->type(), *this, std::forward<Arg>(arg));
}


template<typename Ret>
Ret
Value::
convert(const Type* target) const
{
    return type()->converter(target).call<Ret>(*this);
}

} // reflect
