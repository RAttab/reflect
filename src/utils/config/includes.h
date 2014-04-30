/* includes.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   include header for the config utility.
*/

#pragma once

#include <set>
#include <vector>
#include <unordered_map>

namespace reflect {

struct Config;
struct Path;

} // namespace reflect

#include "reflect.h"

#include "path.h"
#include "node.h"
#include "config.h"
#include "json.h"

#include "config.tcc"
