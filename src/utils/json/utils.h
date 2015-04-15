/* utils.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Misc utilities.
*/

#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* BIT OPS                                                                    */
/******************************************************************************/

template<typename T>
size_t clz(T value) { return __builtin_clz(value); }

} // namespace json
} // namespace reflect
