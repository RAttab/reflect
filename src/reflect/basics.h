/* basics.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   basic member reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT PARENT                                                             */
/******************************************************************************/

template<typename T>
void reflectParent_(Type* type)
{
    type->parent(reflect::type<T>());
}

#define reflectParent(parent)                                           \
    do {                                                                \
        reflectStaticAssert((std::is_base_of<parent, T_>::value));      \
        reflect::reflectParent_<parent>(type_);                         \
    } while(false)


/******************************************************************************/
/* REFLECT ALIAS                                                              */
/******************************************************************************/

#define reflectAlias(type) \
    reflect::Registry::alias<type>(#type)

} // reflect
