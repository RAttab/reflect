/* pointer.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection for pointers.
*/

#pragma once

#include "reflect.h"

namespace reflect {

/******************************************************************************/
/* REFLECT POINTERS                                                           */
/******************************************************************************/

template<typename T>
struct Reflect<T*>
{
    typedef T* T_;
    static std::string id() { return Reflect<T>::id() + "*"; }
    reflectTemplateLoader()

    static void reflect(Type* type_)
    {
        (void) type_; // \todo Implement pointer reflection.
    }
};

} // namespace reflect
