/* string.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 19 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   String reflection
*/

#include "string.h"

#include "reflect/basics.h"
#include "reflect/plumbing.h"

/******************************************************************************/
/* STRING                                                                     */
/******************************************************************************/

reflectClassImpl(std::string)
{
    reflectPlumbing();

    reflectTrait(string);
}
