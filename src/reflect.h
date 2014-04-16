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

#include <iostream> // debug only

#include "utils.h"
#include "ref_type.h"
#include "type_vector.h"
#include "function_traits.h"
#include "type_name.h"
#include "make_function.h"

namespace reflect {

struct Namespace;
struct Type;
struct Function;
struct Functions;
struct Value;

} // namespace reflect

#include "registry.h"
#include "argument.h"
#include "value.h"
#include "cast.h"
#include "value_function.h"
#include "function.h"
#include "type.h"
#include "namespace.h"

#include "argument.tcc"
#include "value.tcc"
#include "function.tcc"
#include "type.tcc"

#include "reflect/class.h"

#include "types/primitives.h"
#include "types/value.h"
