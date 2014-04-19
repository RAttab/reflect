/* clean_type.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Remove const and references from a given type.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* CLEAN TYPE                                                                 */
/******************************************************************************/

template<typename T>
struct CleanType
{
    typedef typename std::decay<T>::type type;
};

template<typename T>
struct CleanType<T*>
{
    typedef typename std::decay<T>::type CleanT;
    typedef typename CleanType<CleanT>::type* type;
};

} // reflect
