/* reflect_getter_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Getter reflection compilation performancce test.
*/

#include "cperf.h"

#include <iostream>


/******************************************************************************/
/* THING                                                                      */
/******************************************************************************/

#define reflectThing(n)                         \
    struct Thing ## n                           \
    {                                           \
        A0 f0() { return {}; }                  \
        A1 f1() { return {}; }                  \
        A2 f2() { return {}; }                  \
        A3 f3() { return {}; }                  \
        A4 f4() { return {}; }                  \
        A5 f5() { return {}; }                  \
        A6 f6() { return {}; }                  \
        A7 f7() { return {}; }                  \
        A8 f8() { return {}; }                  \
        A9 f9() { return {}; }                  \
    };                                          \
                                                \
    reflectClass(Thing ## n)                    \
    {                                           \
        reflectFn(f0);                          \
        reflectFn(f1);                          \
        reflectFn(f2);                          \
        reflectFn(f3);                          \
        reflectFn(f4);                          \
        reflectFn(f5);                          \
        reflectFn(f6);                          \
        reflectFn(f7);                          \
        reflectFn(f8);                          \
        reflectFn(f9);                          \
    }

reflectThing(0)
reflectThing(1)
reflectThing(2)
reflectThing(3)
reflectThing(4)
reflectThing(5)
reflectThing(6)
reflectThing(7)
reflectThing(8)
reflectThing(9)


/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

int main(int, char**)
{
    std::cerr << reflect::namespace_()->print() << std::endl;
    std::cerr << reflect::type<Thing0>()->print() << std::endl;
}
