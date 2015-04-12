/* reflect.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Main include header for reflect.

   This exists because the order in which the headers are included is hard to
   get right and since all the core headers are always required there's not much
   point trying to deal with includes on a file per file basis.

   Also, I'm lazy.

*/

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <cstddef>

#include <iostream> // used only in print functions.

#include "utils.h"
#include "clean_type.h"
#include "ref_type.h"
#include "type_vector.h"
#include "function_type.h"

namespace reflect {

struct Type;
struct Scope;
struct Value;
struct Field;
struct Function;
struct Overloads;

} // namespace reflect

#include "registry.h"
#include "argument.h"
#include "value.h"
#include "traits.h"
#include "cast.h"
#include "value_function.h"
#include "field.h"
#include "function.h"
#include "overloads.h"
#include "type.h"
#include "scope.h"

#include "traits.tcc"
#include "argument.tcc"
#include "value.tcc"
#include "field.tcc"
#include "function.tcc"
#include "overloads.tcc"
#include "type.tcc"
#include "scope.tcc"

#include "dsl/type.h"

#include "types/primitives.h"
#include "types/reflect/value.h"
