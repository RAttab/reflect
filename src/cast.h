/* cast.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Casting utility.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* TARGET REF                                                                 */
/******************************************************************************/

namespace details {

template<typename T>
struct TargetRef
{
    typedef typename std::remove_reference<T>::type CleanT;
    typedef typename std::add_lvalue_reference<CleanT>::type RefT;

    typedef typename std::conditional<
        std::is_lvalue_reference<T>::value, RefT, CleanT>::type
    type;
};

} // namespace details


/******************************************************************************/
/* CAST                                                                       */
/******************************************************************************/

bool isCastable(const Value& value, const Argument& target);
Value cast(Value& value, const Argument& target);

template<typename T>
Value cast(T&& value, const Argument& target)
{
    return cast(Value(std::forward<T>(value)), target);
}

template<typename T, typename Target>
struct Cast
{
    template<typename U> static bool isCastable(U&&) { return true; }

    template<typename U>
    static auto cast(U&& value) -> typename details::TargetRef<Target>::type
    {
        return std::forward<U>(value);
    }
};

template<typename Target>
struct Cast<Value, Target>
{
    typedef typename details::TargetRef<Target>::type TargetRef;
    typedef typename std::decay<Target>::type CleanTarget;

    static bool isCastable(const Value& value) {
        return reflect::isCastable(value, Argument::make<Target>());
    }

    static TargetRef cast(Value value)
    {
        Value result = reflect::cast(value, Argument::make<Target>());
        return ret(result, retLValue(), retRValue(), retCopy());
    }


private:

    // This giant mess is required to avoid emitting a copy-constructor or a
    // move-constructor in situations where we don't need em.

    typedef typename std::is_lvalue_reference<TargetRef>::type retLValue;

    template<typename T0, typename T1>
    static TargetRef ret(Value& value, std::true_type, T0, T1)
    {
        return value.as<CleanTarget>();
    }

    typedef typename std::conditional<
        std::is_rvalue_reference<Target>::value,
        typename std::is_move_constructible<CleanTarget>::type, std::false_type>::type
    retRValue;

    template<typename T0>
    static TargetRef ret(Value& value, std::false_type, std::true_type, T0)
    {
        return std::move(value.as<CleanTarget>());
    }

    typedef typename std::is_copy_constructible<CleanTarget>::type retCopy;

    static TargetRef ret(
            Value& value, std::false_type, std::false_type, std::true_type)
    {
        return value.get<CleanTarget>();
    }

    static TargetRef ret(
            Value& value, std::false_type, std::false_type, std::false_type)
    {
        reflectError("<%s> can't be moved or constructed",
                value.argument().print());
    }

};

template<typename T>
struct Cast<T, Value>
{
    template<typename U> static bool isCastable(U&&) { return true; }

    template<typename U>
    static Value cast(U&& value)
    {
        return Value(std::forward<U>(value));
    }
};

template<>
struct Cast<Value, Value>
{
    template<typename U> static bool isCastable(U&&) { return true; }

    static       Value& cast(      Value&  value) { return value; }
    static       Value  cast(      Value&& value) { return std::move(value); }
    static const Value& cast(const Value&  value) { return value; }
};

template<typename T>
struct Cast<T, T>
{
    template<typename U> static bool isCastable(U&&) { return true; }

    static       T& cast(      T&  value) { return value; }
    static       T  cast(      T&& value) { return std::move(value); }
    static const T& cast(const T&  value) { return value; }
};

template<typename Target, typename T>
bool isCastable(T&& value)
{
    typedef typename std::decay<T>::type CleanT;
    return Cast<CleanT, Target>::isCastable(std::forward<T>(value));
}

template<typename Target, typename T>
auto cast(T&& value) ->
    decltype(Cast<typename std::decay<T>::type, Target>::cast(std::forward<T>(value)))
{
    typedef typename std::decay<T>::type CleanT;
    return Cast<CleanT, Target>::cast(std::forward<T>(value));
}


/******************************************************************************/
/* RET CAST                                                                   */
/******************************************************************************/
/** Used when returning a Value object which may or may-not be void. Since this
    is valid c++ (as far as gcc is concerned anyways):

        void bar() {}
        void foo() { return bar(); }

    which means that if our cast function is able to return void values (doing
    nothing in the cast), then we can use it to avoid having to define two
    seperate call functions. Fun.
 */

template<typename Target, typename T>
Target retCast(T&&, std::true_type) {}

template<typename Target, typename T>
Target retCast(T&& value, std::false_type)
{
    return cast<Target>(std::forward<T>(value));
}

template<typename Target, typename T>
Target retCast(T&& value)
{
    return retCast<Target>(std::forward<T>(value), std::is_same<Target, void>());
}

} // reflect
