/* registry.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection registry.
*/

#pragma once

namespace reflect {

struct Reflection;

/******************************************************************************/
/* REFLECTION REGISTRY                                                        */
/******************************************************************************/

template<typename T> struct ReflectionId;

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

} // reflect
