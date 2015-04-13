/* traits.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   JSON Traits
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
    std::string alias;
    bool skip;

    std::string parser;
    std::string printer;
};

Traits skip();
Traits alias(std::string alias);

} // namespace json 
} // namespace reflect

reflectTypeDecl(reflect::json::Traits);
