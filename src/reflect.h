/* reflect.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Main include header for reflect.

   This exists because the order in which the headers are included is rather
   important and it's just easier to deal with it all in a single place.

   Also, I'm lazy.

*/

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <cassert>

#include <iostream> // debug only

#include "utils.h"
#include "type_vector.h"
#include "function_traits.h"
#include "type_name.h"
#include "make_function.h"

#include "registry.h"
#include "value.h"
#include "cast.h"
#include "value_function.h"
#include "function.h"
#include "reflection.h"
// #include "macros.h"

#include "value.tcc"
#include "function.tcc"

#include "types/value.h"
