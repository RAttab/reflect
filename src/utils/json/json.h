/* json.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#pragma once

#include <istream>
#include <ostream>
#include <sstream>

#include "reflect.h"

namespace reflect {
namespace json {

struct Reader;

} // namespace json
} // namespace reflect

#include "error.h"
#include "token.h"
#include "reader.h"
#include "writer.h"
#include "traits.h"
#include "parser.h"
#include "format.h"
#include "printer.h"

#include "reader.tcc"
#include "writer.tcc"
#include "parser.tcc"
#include "printer.tcc"
