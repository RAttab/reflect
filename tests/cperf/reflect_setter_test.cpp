/* reflect_setter_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Setter reflection compilation performancce test.
*/

#include "cperf.h"

#include <iostream>


/******************************************************************************/
/* THING                                                                      */
/******************************************************************************/

#define reflectThing(n)                         \
    struct Thing ## n                           \
    {                                           \
        void f0(A0) {}                          \
        void f1(A1) {}                          \
        void f2(A2) {}                          \
        void f3(A3) {}                          \
        void f4(A4) {}                          \
        void f5(A5) {}                          \
        void f6(A6) {}                          \
        void f7(A7) {}                          \
        void f8(A8) {}                          \
        void f9(A9) {}                          \
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
    std::cerr << reflect::scope()->print() << std::endl;
    std::cerr << reflect::type<Thing0>()->print() << std::endl;
}
