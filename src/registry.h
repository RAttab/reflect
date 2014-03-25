/* registry.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection registry.
*/

#include "reflect.h"
#pragma once

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

        Reflection* reflect = get(id);
        if (!reflect) add(id, reflect = Reflect<T>::create());

        return reflect;
    }

    static Reflection* get(const std::string& id);
    static void add(std::string id, Reflection* reflection);
};


/******************************************************************************/
/* REFLECT                                                                    */
/******************************************************************************/

template<typename T>
Reflection* reflect()
{
    return ReflectionRegistry::get<T>();
}

inline Reflection* reflect(const std::string& id)
{
    return ReflectionRegistry::get(id);
}


} // reflect
