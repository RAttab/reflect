/* json.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json config serialization.
*/

#include "includes.h"
#pragma once

namespace reflect {
namespace config {

/******************************************************************************/
/* LOAD                                                                       */
/******************************************************************************/

void loadJson(Config& config, std::istream& json);
void loadJson(Config& config, const std::string& json);


/******************************************************************************/
/* SAVE                                                                       */
/******************************************************************************/

std::string saveJson(
        const Config& config,
        const std::string& trait = "config");

void saveJson(
        const Config& config,
        std::ostream& json,
        const std::string& trait = "config");

} // namespace config
} // namespace reflect
