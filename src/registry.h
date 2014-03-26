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

template<typename T, typename Enable = void>
struct Reflect;


/******************************************************************************/
/* REFLECTION REGISTRY                                                        */
/******************************************************************************/

struct Registry
{
    template<typename T>
    static Reflection* get()
    {
        typedef typename std::decay<T>::type CleanT;

        const auto& id = Reflect<CleanT>::id;

        Reflection* reflect = get(id);
        if (!reflect) add(id, reflect = Reflect<CleanT>::create());

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
    return Registry::get<T>();
}

inline Reflection* reflect(const std::string& id)
{
    return Registry::get(id);
}


} // reflect
