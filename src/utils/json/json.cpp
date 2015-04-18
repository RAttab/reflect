/* json.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "json.h"

#include "types/std/string.h"
#include "types/std/vector.h"
#include "types/std/map.h"
#include "types/reflect/type.h"
#include "types/reflect/value.h"
#include "types/primitives.h"
#include "dsl/all.h"
#include "utils.h"

#include <mutex>
#include <algorithm>

#include "reader.cpp"
#include "writer.cpp"
#include "token.cpp"
#include "traits.cpp"
#include "parser.cpp"
#include "format.cpp"
#include "printer.cpp"
