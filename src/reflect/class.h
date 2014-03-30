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
        static constexpr const char* id = reflectStringify(_type_);     \
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
        Loader()                                                \
        {                                                       \
            typedef Reflect<_type_> type;                       \
            Registry::add(type::id, [] (Type* type_) {          \
                        type::reflect(type_);                   \
                    });                                         \
        }                                                       \
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

} // reflect
