/* class.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Class reflection
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT CLASS                                                              */
/******************************************************************************/

#define reflectClassDecl(_type_)                                        \
    namespace reflect {                                                 \
    template<>                                                          \
    struct Reflect<_type_>                                              \
    {                                                                   \
        typedef _type_ T_;                                              \
        static std::string id() { return reflectStringify(_type_); }    \
        static void loader() {}                                         \
                                                                        \
        static void reflect(Type*);                                     \
    };                                                                  \
    } // namespace reflect


// Note that we need the template specialization because we if we use
// reflectUniqueName to get a unique typename then we have no way to recover the
// type to name our constructor.
#define reflectClassLoader(_type_)                              \
    namespace reflect {                                         \
    template<>                                                  \
    struct Loader<_type_>                                       \
    {                                                           \
        Loader() { Registry::add<_type_>(); }                   \
    };                                                          \
    namespace { Loader<_type_> reflectUniqueName(loader); }     \
    } // namespace reflect


#define reflectClassImpl(_type_)                        \
    reflectClassLoader(_type_)                          \
                                                        \
    void                                                \
    reflect::Reflect<_type_>::                          \
    reflect(reflectUnused Type* type_)

#define reflectClass(_type_)                  \
    reflectClassDecl(_type_)                  \
    reflectClassImpl(_type_)


/******************************************************************************/
/* REFLECT TEMPLATE                                                           */
/******************************************************************************/

#define reflectTemplateLoader()                 \
    static void loader()                        \
    {                                           \
        static bool loaded = false;             \
        if (loaded) return;                     \
        loaded = true;                          \
        Registry::add<T_>();                    \
    }

} // reflect
