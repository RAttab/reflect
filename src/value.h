/* value.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value front-end.
*/

#include "reflect.h"
#pragma once

namespace reflect {

struct Type;

/******************************************************************************/
/* CLEAN REF                                                                  */
/******************************************************************************/

template<typename T>
struct CleanRef
{
    typedef typename std::remove_reference<T>::type CleanT;
    typedef typename std::add_lvalue_reference<CleanT>::type type;
};


/******************************************************************************/
/* CLEAN VALUE                                                                */
/******************************************************************************/

template<typename T>
struct CleanValue
{
    typedef typename std::remove_reference<T>::type type;
};


/******************************************************************************/
/* VALUE OP                                                                   */
/******************************************************************************/
// All operators return Values because there's no clean way to provide a
// template parameter for the return value when using the operator.

#define reflectValueOpUnary(op)                 \
    Value op() const                            \
    {                                           \
        return call<Value>(#op);                \
    }

#define reflectValueOpBinary(op)                                \
    template<typename Arg>                                      \
    Value op(Arg&& arg) const                                   \
    {                                                           \
        return call<Value>(#op, std::forward<Arg>(arg));        \
    }

#define reflectValueOpBool(op)                          \
    template<typename Arg>                              \
    bool op(Arg&& arg) const                            \
    {                                                   \
        return call<bool>(#op, std::forward<Arg>(arg)); \
    }

#define reflectValueOpNary(op)                                  \
    template<typename... Args>                                  \
    Value op(Args&&... args) const                              \
    {                                                           \
        return call<Value>(#op, std::forward<Args>(args)...);   \
    }


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

struct Value
{
    Value();

    template<typename T>
    explicit Value(T&& value);

    Value(Value& other);
    Value(const Value& other);
    Value& operator=(const Value& other);

    Value(Value&& other);
    Value& operator=(Value&& other);

    void* value() const { return value_; }
    const Type* type() const { return arg.type(); }
    const std::string& typeId() const;
    RefType refType() const { return arg.refType(); }
    bool isConst() const { return arg.isConst(); }
    bool isVoid() const { return arg.isVoid(); }
    bool isStored() const { return !!storage; }

    const Argument& argument() const { return arg; }

    bool is(const std::string& trait) const;

    // Get a reference to the value without any type checks.
    template<typename T> const T& get() const;

    template<typename T> auto cast() const -> typename CleanRef<T>::type;
    template<typename T> bool isCastable() const;

    template<typename T> auto copy() const -> typename CleanValue<T>::type;
    template<typename T> bool isCopiable() const;

    template<typename T> auto move() -> typename CleanValue<T>::type;
    template<typename T> bool isMovable() const;

    Value rvalue() const;
    Value copy() const;
    Value move();

    template<typename Ret, typename... Args>
    Ret call(const std::string& fn, Args&&... args) const;

    template<typename Ret = Value>
    Ret field(const std::string& field) const;

    // operator= for the contained value.
    template<typename Arg>
    void assign(Arg&& arg) const;

    reflectValueOpBinary(operator+=)
    reflectValueOpBinary(operator-=)
    reflectValueOpBinary(operator*=)
    reflectValueOpBinary(operator/=)
    reflectValueOpBinary(operator%=)
    reflectValueOpBinary(operator&=)
    reflectValueOpBinary(operator|=)
    reflectValueOpBinary(operator^=)
    reflectValueOpBinary(operator<<=)
    reflectValueOpBinary(operator>>=)

    reflectValueOpUnary (operator++)
    reflectValueOpBinary(operator++)
    reflectValueOpUnary (operator--)
    reflectValueOpBinary(operator--)

    reflectValueOpBinary(operator+)
    reflectValueOpBinary(operator-)
    reflectValueOpBinary(operator*)
    reflectValueOpBinary(operator/)
    reflectValueOpBinary(operator%)
    reflectValueOpUnary (operator~)
    reflectValueOpBinary(operator&)
    reflectValueOpBinary(operator|)
    reflectValueOpBinary(operator^)
    reflectValueOpBinary(operator<<)
    reflectValueOpBinary(operator>>)

    bool operator!() const;
    reflectValueOpBool(operator&&)
    reflectValueOpBool(operator||)

    reflectValueOpBool(operator==)
    reflectValueOpBool(operator!=)
    reflectValueOpBool(operator<)
    reflectValueOpBool(operator>)
    reflectValueOpBool(operator<=)
    reflectValueOpBool(operator>=)

    reflectValueOpNary  (operator())
    reflectValueOpBinary(operator[])
    reflectValueOpUnary (operator*)

    explicit operator bool() const;

private:

    template<typename T>
    T convert() const;

    Argument arg;
    void* value_;
    std::shared_ptr<void> storage;
};


} // reflect
