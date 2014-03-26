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

    template<typename T> T& cast() const;
    template<typename T> bool castable() const;

    template<typename T> T move();
    template<typename T> bool movable() const;

    bool isVoid() const;

private:


    void* value_;
    Reflection* reflection_;
    std::shared_ptr<void> storage;
};


} // reflect
