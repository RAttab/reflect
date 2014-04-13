/* cperf_type_lookup_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Used to tests the complexity of type lookups within the compiler.

   In other words, if we have thousands of reflected types in our project,
   what's the overhead of looking up one of those types. gcc 4.7 seems to have a
   complexity of O(1) as was expected.

*/

#include "reflect.h"

#include <boost/test/unit_test.hpp>

using namespace reflect;


/******************************************************************************/
/* REFLECT THING                                                              */
/******************************************************************************/

#define reflectThing1Impl(name)                 \
    struct name {};                             \
    reflectClassDecl(name)


#define reflectThing1()                         \
    reflectThing1Impl(reflectUniqueName(Thing))

#define reflectThing10()                        \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()                             \
    reflectThing1()

#define reflectThing100()                       \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()                            \
    reflectThing10()

#define reflectThing1000()                      \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()                           \
    reflectThing100()


/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

reflectThing1000()

struct Target {};
reflectClass(Target) {}


int main(int, char**)
{
    const Type* tTarget = type<Target>();
    std::cerr << tTarget->print() << std::endl;
}
