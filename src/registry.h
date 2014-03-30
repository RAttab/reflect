/* registry.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type registry.
*/

#include "reflect.h"
#pragma once

namespace reflect {

struct Type;


/******************************************************************************/
/* REFLECT                                                                    */
/******************************************************************************/

template<typename T, typename Enable = void>
struct Reflect;


/******************************************************************************/
/* REGISTRY                                                                   */
/******************************************************************************/

struct Registry
{
    template<typename T>
    static Type* get()
    {
        typedef typename std::decay<T>::type CleanT;

        const auto& id = Reflect<CleanT>::id;

        Type* type = get(id);
        if (!type) {
            add(id, type = Reflect<CleanT>::create());
            Reflect<CleanT>::reflect(type);
        }

        return type;
    }

    static Type* get(const std::string& id);
    static void add(std::string id, Type* type);

    template<typename T>
    static void alias(std::string alias)
    {
        add(std::move(alias), get<T>());
    }
};


/******************************************************************************/
/* REFLECT                                                                    */
/******************************************************************************/

template<typename T>
Type* reflect()
{
    return Registry::get<T>();
}

inline Type* reflect(const std::string& id)
{
    return Registry::get(id);
}


} // reflect
