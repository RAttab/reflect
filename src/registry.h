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

template<typename T, typename Enable = void> struct Reflect;
template<typename T, typename Enable = void> struct Loader;


/******************************************************************************/
/* REGISTRY                                                                   */
/******************************************************************************/

struct Registry
{
    template<typename T>
    static Type* get()
    {
        typedef typename std::decay<T>::type CleanT;
        return get(Reflect<CleanT>::id);
    }

    static Type* get(const char* id);
    static Type* get(const std::string& id);
    static void add(const std::string& id, std::function<void(Type*)> loader);

    template<typename T>
    static void alias(const std::string& alias)
    {
        typedef typename std::decay<T>::type CleanT;

        Registry::alias(Reflect<CleanT>::id, alias);
    }
    static void alias(const std::string& id, const std::string& alias);

private:
    static void add(const std::string& id, Type* type);
    static Type* load(const std::string& id);
};


/******************************************************************************/
/* TYPE                                                                       */
/******************************************************************************/

template<typename T>
Type* type()
{
    return Registry::get<T>();
}

inline Type* type(const std::string& id)
{
    return Registry::get(id);
}


} // reflect
