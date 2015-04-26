/* plumbing_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"

/******************************************************************************/
/* REFLECT THING                                                              */
/******************************************************************************/

#define reflectThingImpl(name)                          \
    struct name                                         \
    {                                                   \
        name() {}                                       \
        name(const name&) {}                            \
        name(name&&) {}                                 \
        name& operator=(const name&) { return *this; }  \
        name& operator=(name&&) { return *this; }       \
    };                                                  \
    reflectType(name) { reflectPlumbing(); }

#define reflectThing1()                         \
    reflectThingImpl(reflectUniqueName(Thing))


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

reflectThing10()

struct Target {};
reflectType(Target) {}


int main(int, char**)
{
    const reflect::Type* tTarget = reflect::type<Thing_0>();
    std::cerr << tTarget->print() << std::endl;
}
