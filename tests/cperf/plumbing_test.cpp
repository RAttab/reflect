/* plumbing_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Dec 2015
   FreeBSD-style copyright and disclaimer apply

   To get the true cost of the plumbing call gotta make a diff with and without
   the body of main. Required to remove the overhead of reflecting the
   class. Usually the overhead is about 10-11 sec.

   Note that we reflect alloc because plain reflect plumbing reflects sizeof
   which requires linkage with reflect_primitives.
*/

#include "cperf.h"
#include "dsl/plumbing.h"

#include <iostream>


/******************************************************************************/
/* THING                                                                      */
/******************************************************************************/

#define reflectThing(num)                                       \
    struct Thing ## num {};                                     \
    reflectType(Thing ## num) { reflectAllocPlumbing(); }       \


#define construct(num)                                                  \
    reflect::alloc<reflect::Value>(reflect::type<Thing ## num>(), Thing ## num{})


/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

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
reflectThing(10)
reflectThing(11)
reflectThing(12)
reflectThing(13)
reflectThing(14)
reflectThing(15)
reflectThing(16)
reflectThing(17)
reflectThing(18)
reflectThing(19)

int main(int, const char**)
{
    construct(0);
    construct(1);
    construct(2);
    construct(3);
    construct(4);
    construct(5);
    construct(6);
    construct(7);
    construct(8);
    construct(9);
    construct(10);
    construct(11);
    construct(12);
    construct(13);
    construct(14);
    construct(15);
    construct(16);
    construct(17);
    construct(18);
    construct(19);
}
