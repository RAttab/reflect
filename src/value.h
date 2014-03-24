/* value.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value front-end.

   \todo Need to support const-ness somehow.
*/

#pragma once

namespace reflect {


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

struct Value
{
    Value() :
        value_(nullptr), reflection_(ReflectionRegistry::get<void>())
    {}

    Value(void* value, Reflection* reflection) :
        value_(value), reflection_(reflection)
    {}

    template<typename T>
    explicit Value(T&& value) :
        value_(&value), reflection_(ReflectionRegistry::get<T>())
    {
        if (refType(std::forward<T>(value)) == RefType::RValue)
            storage.reset(value_ = new T(std::move(value)));
    }

    void* value() const { return value_; }
    Reflection* reflection() const { return reflection_; }
    RefType refType() const
    {
        return storage ? RefType::RValue : RefType::LValue;
    }

    template<typename T>
    T cast()
    {
        assert(!isVoid());

        typedef typename std::decay<T>::type CleanT;
        assert(reflection_->isConvertibleTo<CleanT>());

        return cast<T>(reinterpret_cast<CleanT*>(value),
                std::is_lvalue_reference<T>());
    }

    bool isVoid() const
    {
        return reflection_ == ReflectionRegistry::get<void>();
    }

private:

    /** Return by lvalue-ref or copy. */
    template<typename T, typename U>
    T cast(U* value, std::false_type)
    {
        return *value;
    }

    /** Return by rvalue-ref so gut our object in the process. */
    template<typename T, typename U>
    T cast(U* value, std::true_type)
    {
        assert(refType() == RValue);
        assert(storage.unique());

        auto toReturn = std::move(*value);
        *this = Value();

        return std::move(toReturn);
    }

    void* value_;
    Reflection* reflection_;
    std::shared_ptr<void> storage;
};

} // reflect
