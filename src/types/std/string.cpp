/* string.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   String reflection
*/

#include "string.h"

#include "dsl/basics.h"
#include "dsl/plumbing.h"

/******************************************************************************/
/* STRING                                                                     */
/******************************************************************************/

reflectTypeImpl(std::string)
{
    reflectPlumbing();

    reflectTypeTrait(string);
}
