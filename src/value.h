/* value.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value front-end.

   \todo Need to support const-ness somehow.
*/

#include "reflect.h"
#pragma once

namespace reflect {

struct Reflection;

/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

struct Value
{
    Value();
    Value(void* value, Reflection* reflection);

    template<typename T>
    explicit Value(T&& value);

    void* value() const { return value_; }
    Reflection* reflection() const { return reflection_; }
    RefType refType() const
    {
        return storage ? RefType::RValue : RefType::LValue;
    }

    template<typename T> T cast();
    template<typename T> bool castable() const;

    bool isVoid() const;

private:

    template<typename T, typename U> T cast(U* value, std::false_type);
    template<typename T, typename U> T cast(U* value, std::true_type);

    template<typename T> bool castable(std::false_type) const;
    template<typename T> bool castable(std::true_type) const;


    void* value_;
    Reflection* reflection_;
    std::shared_ptr<void> storage;
};


} // reflect
