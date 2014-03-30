/* value.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type for reflect::Value.

   \todo Convert to macro.
*/

#include "reflect.h"
#pragma once

namespace reflect {

template<>
struct Reflect<Value>
{
    typedef Value T_;
    static constexpr const char* id = "reflect::Value";
    static void reflect(Type*) {}
};

} // reflect
