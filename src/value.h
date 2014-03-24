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
    None,
    LValue,
    RValue,
};

template<typename T>
RefType refType()
{
    if (std::is_lvalue_reference<T>::value) return RefType::LValue;
    if (std::is_rvalue_reference<T>::value) return RefType::RValue;
    return RefType::None;
}


/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

struct Value
{
    template<typename T>
    explicit Value(T& value) :
        value_(&value), reflection_(ReflectionRegistry::get<T>())
    {}

    Value(void* value, Reflection* reflection) :
        value_(value), reflection_(reflection)
    {}

    void* value() const { return value_; }
    Reflection* reflection() const { return reflection_; }

    template<typename T>
    T* cast() const
    {
        typedef typename std::decay<T>::type CleanT;
        assert(reflection_->isConvertibleTo<CleanT>());
        return reinterpret_cast<CleanT*>(value);
    }

protected:
    void* value_;
    Reflection* reflection_;
};


/******************************************************************************/
/* RETURN VALUE                                                               */
/******************************************************************************/

struct ReturnValue : public Value
{
    ReturnValue() : Value(nullptr, ReflectionRegistry::get<void>()) {}

    template<typename T, typename Y>
    ReturnValue(Y&& value) :
        Value(value), refType_(refType<T>())
    {
        if (refType_ == RefType::LValue) return;
        storage_.reset(value_ = new Y(std::move(value)));
    }

    RefType refType() const { return refType_; }

private:
    RefType refType_;
    std::shared_ptr<void> storage_;
};

} // reflect
