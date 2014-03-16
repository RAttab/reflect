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
/* REFLECT                                                                    */
/******************************************************************************/

template<typename T> struct Reflect;


/******************************************************************************/
/* REFLECTION REGISTRY                                                        */
/******************************************************************************/

struct ReflectionRegistry
{
    template<typename T>
    static Reflection* get()
    {
        const auto& id = Reflect<T>::id;

        Reflection* reflect = get();
        if (!reflect) add(id, reflect = Reflect<T>::create());

        return reflect;
    }

    static Reflection* get(const std::string& id);
    static void add(std::string id, Reflection* reflection);
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
