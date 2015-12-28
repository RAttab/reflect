/* reflect_plumbing_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Dec 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "cperf.h"
#include "dsl/plumbing.h"

#include <iostream>


/******************************************************************************/
/* THING                                                                      */
/******************************************************************************/

#define reflectThing(name)                      \
    struct name {};                             \
    reflectType(name) { reflectPlumbing(); }

#define reflectThing1()                         \
    reflectThing(reflectUniqueName(Thing))

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


/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

reflectThing(Thing)
reflectThing10()
reflectThing10()
reflectThing10()

int main(int, char**)
{
    const reflect::Type* tThing = reflect::type<Thing>();
    std::cerr << tThing->print() << std::endl;
}
