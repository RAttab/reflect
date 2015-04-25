/* traits.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {


/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

struct Traits
{
    Traits();

    bool skip;
    bool skipEmpty;
    std::string alias;
    std::string parser;
    std::string printer;

    Traits operator| (const Traits& other) const;
};

Traits skip();
Traits skipEmpty();
Traits alias(std::string alias);
Traits custom(std::string parser, std::string printer);

} // namespace json
} // namespace reflect

reflectTypeDecl(reflect::json::Traits)
