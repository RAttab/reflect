/* value.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value front-end.
*/

#pragma once

namespace reflect {


/******************************************************************************/
/* REF TYPE                                                                   */
/******************************************************************************/

enum class RefType
{
    LValue,
    RValue,
};

template<typename T>
RefType refType(T&&)
{
    return std::is_lvalue_reference<T>::value ?
        RefType::LValue : RefType::RValue;
}


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
    T* cast() const
    {
        typedef typename std::decay<T>::type CleanT;
        assert(reflection_->isConvertibleTo<CleanT>());
        return reinterpret_cast<CleanT*>(value);
    }

    bool isVoid() const
    {
        return reflection_ == ReflectionRegistry::get<void>();
    }

private:
    void* value_;
    Reflection* reflection_;
    std::shared_ptr<void> storage;
};

} // reflect
