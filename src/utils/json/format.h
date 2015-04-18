/* format.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {

/******************************************************************************/
/* FORMAT                                                                     */
/******************************************************************************/

void formatNull(Writer& writer);
void formatBool(Writer& writer, bool value);
void formatInt(Writer& writer, int64_t value);
void formatFloat(Writer& writer, double value);
void formatString(Writer& writer, const std::string& value);

} // namespace json
} // namespace reflect
