/* reader.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Reader template implementation.
*/

#include "json"
#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* READER                                                                     */
/******************************************************************************/

template<typename... Args>
void 
Reader::
error(const char* fmt, Args&&... args)
{
    std::stringstream ss;
    ss << reader.line() << ":" << reader.pos() << ": ";
    ss<< reflect::errorFormat(fmt, std::forward<Args>(args)...);
    error_ = Error(ss.str());
}

} // namespace json
} // namespace reflect
