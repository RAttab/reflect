/* error.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* ERROR                                                                      */
/******************************************************************************/

struct Error : public reflect::Error
{
    Error() : reflect::Error() {}
    Error(std::string msg) : reflect::Error(std::move(msg)) {}
};

} // namespace json
} // namespace reflect
