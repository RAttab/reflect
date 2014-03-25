/* void.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection for void.

   \todo Rewrite using macros.
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* REFLECT VOID                                                               */
/******************************************************************************/

template<>
struct Reflect<void>
{
    static constexpr const char* id = "void";

    static Reflection* create() { return new Reflection(id); }
};


} // reflect
