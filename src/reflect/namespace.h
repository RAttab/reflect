/* namespace.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection for a namespace.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* NAMESPACE                                                                  */
/******************************************************************************/

#define reflectNamespaceImpl(_ns_,_class_)                              \
    namespace { struct _class_ {}; }                                    \
    namespace reflect {                                                 \
    template<>                                                          \
    struct ReflectNamespace<_class_>                                    \
    {                                                                   \
        ReflectNamespace()                                              \
        {                                                               \
            reflect(::reflect::Registry::globalNamespace()->sub(#_ns_)); \
        };                                                              \
        static void reflect(Namespace* ns);                             \
    };                                                                  \
    namespace { ReflectNamespace<_class_> reflectConcat(_class_,loader); } \
    }                                                                   \
                                                                        \
    void                                                                \
    reflect::ReflectNamespace<_class_>::                                \
    reflect(reflectUnused Namespace* ns_)

#define reflectNamespace(_ns_) \
    reflectNamespaceImpl(_ns_, reflectUniqueName(ReflectNamespace))


/******************************************************************************/
/* REFLECT GLOBAL FN                                                          */
/******************************************************************************/

template<typename Fn>
void reflectGlobalFunction(Namespace*, const std::string& name, Fn fn)
{
    Registry::globalNamespace()->addFunction(name, std::move(fn));
}

#define reflectGlobalFn(fn) \
    ::reflect::reflectGlobalFunction(ns_, #fn, &fn)

#define reflectGlobalFnTyped(fn,signature) \
    ::reflect::reflectGlobalFunction<signature>(ns_, #fn, &fn)


/******************************************************************************/
/* REFLECT GLOBAL VAR                                                         */
/******************************************************************************/

template<typename T>
void reflectGlobalVar(Namespace*, const std::string& name, T* var)
{
    Namespace* gns = Registry::globalNamespace();
    gns->addFunction(name, [=] { return *var; });
    gns->addFunction(name, [=] (const T& other) { *var = other; });
}

template<typename T>
void reflectGlobalVar(Namespace*, const std::string& name, const T* var)
{
    Namespace* gns = Registry::globalNamespace();
    gns->addFunction(name, [=] { return *var; });
}

#define reflectGlobalVar(var) \
    reflectGlobalVar(ns_, #var, &var)

} // reflect
