/* registry.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection registry.
*/

#pragma once

#include "value.h"

namespace reflect {

struct Reflection;


/******************************************************************************/
/* REFLECTION ID                                                              */
/******************************************************************************/

template<typename T> struct ReflectionId;


/******************************************************************************/
/* REFLECTION REGISTRY                                                        */
/******************************************************************************/

struct ReflectionRegistry
{
    template<typename T>
    static Reflection* get()
    {
        return get(ReflectionId<T>::value);
    }
    static Reflection* get(const std::string& id);

    static void add(Reflection* reflection)
};


/******************************************************************************/
/* GET REFLECTION                                                             */
/******************************************************************************/

template<typename T>
struct GetReflection
{
    Reflection* get() const { ReflectionRegistry::get<T>(); }
};

template<>
struct GetReflection<Value>
{
    Reflection* get() const { return value.reflection(); }
};

} // reflect
