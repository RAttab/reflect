/* primitives.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Implementation of the various primitives reflection.
*/

#include "primitives.h"
#include "dsl/basics.h"

#include <cstdint>

namespace {

/******************************************************************************/
/* PRIMITIVES ALIASES                                                         */
/******************************************************************************/

struct PrimitiveAliases
{
    PrimitiveAliases()
    {
        reflectAlias(short);
        reflectAlias(unsigned short);

        reflectAlias(long);
        reflectAlias(unsigned long);

        reflectAlias(long long);
        reflectAlias(unsigned long long);

        reflectAlias( size_t);
        reflectAlias(ssize_t);

        reflectAlias( intmax_t);
        reflectAlias(uintmax_t);
        reflectAlias( intptr_t);
        reflectAlias(uintptr_t);

        reflectAlias( int8_t);
        reflectAlias(uint8_t);
        reflectAlias( int16_t);
        reflectAlias(uint16_t);
        reflectAlias( int32_t);
        reflectAlias(uint32_t);
        reflectAlias( int64_t);
        reflectAlias(uint64_t);

        reflectAlias( int_fast8_t);
        reflectAlias(uint_fast8_t);
        reflectAlias( int_fast16_t);
        reflectAlias(uint_fast16_t);
        reflectAlias( int_fast32_t);
        reflectAlias(uint_fast32_t);
        reflectAlias( int_fast64_t);
        reflectAlias(uint_fast64_t);

        reflectAlias( int_least8_t);
        reflectAlias(uint_least8_t);
        reflectAlias( int_least16_t);
        reflectAlias(uint_least16_t);
        reflectAlias( int_least32_t);
        reflectAlias(uint_least32_t);
        reflectAlias( int_least64_t);
        reflectAlias(uint_least64_t);

    };
} primitiveAliasesLoader;

} // namespace anonymous
