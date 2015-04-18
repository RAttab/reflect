/* utils.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* BIT OPS                                                                    */
/******************************************************************************/

size_t clz(char value) { return __builtin_clz(uint32_t(value) << 24); }

} // namespace json
} // namespace reflect
