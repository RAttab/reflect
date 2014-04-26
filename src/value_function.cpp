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

void* allocValueFunction(size_t size)
{
    return std::malloc(size);
}

void freeValueFunction(void* fn)
{
    if (!fn) return;

    typedef ValueFunction<0> Fn;
    static_cast<Fn*>(fn)->free();

    free(fn);
}

void
ValueFunctionDestructor::
operator() (void* ptr) const
{
    freeValueFunction(ptr);
}

} // reflect
