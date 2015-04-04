/* scope.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection for a scope.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

#define reflectScopeTrait(trait)                \
    scope_->addTrait(#trait)

#define reflectScopeValue(trait, value)         \
    scope_->addTrait(#trait, value)

/******************************************************************************/
/* SCOPE                                                                      */
/******************************************************************************/

#define reflectScopeImpl(sc, tag)                                       \
    namespace { struct tag {}; }                                        \
    namespace reflect {                                                 \
    template<>                                                          \
    struct ReflectScope<tag>                                            \
    {                                                                   \
        ReflectScope()                                                  \
        {                                                               \
            reflect(::reflect::Registry::globalScope()->scope(#sc));    \
        };                                                              \
        static void reflect(Scope* scope_);                             \
    };                                                                  \
    namespace { ReflectScope<tag> reflectConcat(tag, loader); }         \
    }                                                                   \
                                                                        \
    void                                                                \
    reflect::ReflectScope<tag>::                                        \
    reflect(reflectUnused Scope* scope_)

#define reflectScope(scope)                                     \
    reflectScopeImpl(scope, reflectUniqueName(ReflectScope))


/******************************************************************************/
/* REFLECT GLOBAL FN                                                          */
/******************************************************************************/

template<typename Fn>
void reflectGlobalFunction(Scope*, const std::string& name, Fn fn)
{
    Registry::globalScope()->addFunction(name, std::move(fn));
}

#define reflectGlobalFn(fn)                             \
    ::reflect::reflectGlobalFunction(scope_, #fn, &fn)

#define reflectGlobalFnTyped(fn,signature)                              \
    ::reflect::reflectGlobalFunction<signature>(scope_, #fn, &fn)


/******************************************************************************/
/* REFLECT GLOBAL VAR                                                         */
/******************************************************************************/

template<typename T>
void reflectGlobalVar(Scope*, const std::string& name, T* var)
{
    Scope* gScope = Registry::globalScope();
    gScope->add(name, [=] { return *var; });
    gScope->add(name, [=] (const T& other) { *var = other; });
}

template<typename T>
void reflectGlobalVar(Scope*, const std::string& name, const T* var)
{
    Scope* gScope = Registry::globalScope();
    gScope->add(name, [=] { return *var; });
}

#define reflectGlobalVar(var)                   \
    reflectGlobalVar(scope_, #var, &var)

} // reflect
