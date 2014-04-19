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
template<typename T, typename Enable = void> struct ReflectNamespace;
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

        Reflect<CleanT>::loader();
        return get(Reflect<CleanT>::id());
    }

    static Type* get(const std::string& id);

    template<typename T>
    static void add()
    {
        typedef typename std::decay<T>::type CleanT;

        auto loader = [] (Type* type) { Reflect<CleanT>::reflect(type); };
        add(Reflect<CleanT>::id(), std::move(loader));
    }
    static void add(const std::string& id, std::function<void(Type*)> loader);

    template<typename T>
    static void alias(const std::string& alias)
    {
        typedef typename std::decay<T>::type CleanT;

        Registry::alias(Reflect<CleanT>::id(), alias);
    }
    static void alias(const std::string& id, const std::string& alias);

    static Namespace* globalNamespace();

private:
    static void add(const std::string& id, Type* type);
    static Type* load(const std::string& id);
};


/******************************************************************************/
/* TYPE ID                                                                    */
/******************************************************************************/

template<typename T>
std::string typeId()
{
    typedef typename std::decay<T>::type CleanT;
    return Reflect<CleanT>::id();
}

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


/******************************************************************************/
/* NAMESPACE                                                                  */
/******************************************************************************/

Namespace* namespace_(const std::string& name = "");

} // reflect
