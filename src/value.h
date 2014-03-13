/* value.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value front-end.
*/

#pragma once

namespace reflect {


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


    void* value() { return value_; }
    Reflection* reflection() { return reflection_; }

    template<typename T>
    T cast()
    {
        typedef typename std::decay<T>::type CleanT;
        assert(reflection_->isConvertibleTo<CleanT>());
        return *reinterpret_cast<CleanT*>(value);
    }

    template<typename T>
    void set(T&& newValue)
    {
        reflection_->set(value_, std::forward<T>(newValue));
    }

    template<typename T>
    void set(const std::string& field, T&& newValue)
    {
        reflection_->set(value_, field, std::foward<T>(newValue));
    }


    Value get()
    {
        return reflection_->get(value_);
    }

    template<typename T>
    T get()
    {
        return reflection_->get<T>(value_);
    }

    Value get(const std::string& field)
    {
        return reflection_->get(value_, field);
    }

    template<typename T>
    T get(const std::string& field)
    {
        return reflection_->get<T>(value_, field);
    }

private:
    void* value_;
    Reflection* reflection_;
};


} // reflect
