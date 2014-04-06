/* value_function.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 06 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   ValueFunction implementation.
*/

#include "reflect.h"

namespace reflect {

/******************************************************************************/
/* VALUE FUNCTION                                                             */
/******************************************************************************/

void freeValueFunction(void* fn)
{
    typedef ValueFunction<0> Fn;
    static_cast<Fn*>(fn)->free();
}

} // reflect
