/* template.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 02 May 2014
   FreeBSD-style copyright and disclaimer apply

   Template reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT TEMPLATE LOADER                                                    */
/******************************************************************************/

#define reflectTemplateLoader()                 \
    static void loader()                        \
    {                                           \
        static bool loaded = false;             \
        if (loaded) return;                     \
        loaded = true;                          \
        Registry::add<T_>();                    \
    }


/******************************************************************************/
/* REFLECT TEMPLATE                                                           */
/******************************************************************************/

#define reflectTemplateDecl(temp, type)                                 \
    namespace reflect {                                                 \
    template<typename type>                                             \
    struct Reflect< temp<type> >                                        \
    {                                                                   \
        typedef temp<type> T_;                                          \
        static std::string id() { return #temp "<" + typeId<type>() + ">"; } \
                                                                        \
        static void reflect(Type* type_);                               \
        reflectTemplateLoader()                                         \
    };                                                                  \
    } // namespace reflect

#define reflectTemplateImpl(temp, type)                                 \
    template<typename type>                                             \
    void                                                                \
    ::reflect::Reflect< temp<type> >::                                  \
    reflect(reflectUnused Type* type_)

#define reflectTemplate(temp, type)                \
    reflectTemplateDecl(temp, type)                \
    reflectTemplateImpl(temp, type)


} // namespace reflect
