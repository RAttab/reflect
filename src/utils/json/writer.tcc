/* writer.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* WRITER                                                                     */
/******************************************************************************/

template<typename... Args>
void
Writer::
error(const char* fmt, Args&&... args)
{
    error_ = Error(reflect::errorFormat(fmt, std::forward<Args>(args)...));
}


} // namespace json
} // namespace reflect
