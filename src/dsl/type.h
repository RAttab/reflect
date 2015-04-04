/* type.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type reflection
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

#define reflectTypeTrait(trait)                 \
    type_->addTrait(#trait)

#define reflectTypeValue(trait, value)          \
    type_->addTrait(#trait, value)


/******************************************************************************/
/* REFLECT CLASS                                                              */
/******************************************************************************/

#define reflectTypeDecl(_type_)                                         \
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
#define reflectTypeLoader(_type_)                               \
    namespace reflect {                                         \
    template<>                                                  \
    struct Loader<_type_>                                       \
    {                                                           \
        Loader() { Registry::add<_type_>(); }                   \
    };                                                          \
    namespace { Loader<_type_> reflectUniqueName(loader); }     \
    } // namespace reflect


#define reflectTypeImpl(_type_)                 \
    reflectTypeLoader(_type_)                   \
                                                \
    void                                        \
    reflect::Reflect<_type_>::                  \
    reflect(reflectUnused Type* type_)

#define reflectType(_type_)                     \
    reflectTypeDecl(_type_)                     \
    reflectTypeImpl(_type_)


} // reflect
