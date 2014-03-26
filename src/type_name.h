/* type_name.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type traits to extract a compile time name.
*/

#pragma once

namespace reflect {

/******************************************************************************/
/* TYPE NAME                                                                  */
/******************************************************************************/

template<typename T> struct TypeName;

#define REFLECT_TYPE_NAME(type)                          \
    template<>                                           \
    struct TypeName<type>                                \
    {                                                    \
        static constexpr const char* name = #type;       \
    }


} // reflect
