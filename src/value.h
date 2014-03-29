/* value.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Value front-end.

   \todo Need to support const-ness somehow.
*/

#include "reflect.h"
#pragma once

namespace reflect {

struct Type;

/******************************************************************************/
/* VALUE                                                                      */
/******************************************************************************/

struct Value
{
    Value();

    template<typename T>
    explicit Value(T&& value);

    void* value() const { return value_; }
    Type* type() const { return arg.type(); }
    RefType refType() const { return arg.refType(); }
    bool isConst() const { return arg.isConst(); }
    bool isVoid() const { return arg.isVoid(); }

    template<typename T> T& cast() const;
    template<typename T> bool castable() const;

    template<typename T> T move();
    template<typename T> bool movable() const;


private:

    Argument arg;
    void* value_;
    std::shared_ptr<void> storage;
};


} // reflect
