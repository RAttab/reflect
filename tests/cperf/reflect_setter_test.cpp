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
        void f0(A0 a) {}                        \
        void f1(A1 a) {}                        \
        void f2(A2 a) {}                        \
        void f3(A3 a) {}                        \
        void f4(A4 a) {}                        \
        void f5(A5 a) {}                        \
        void f6(A6 a) {}                        \
        void f7(A7 a) {}                        \
        void f8(A8 a) {}                        \
        void f9(A9 a) {}                        \
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
// reflectThing(7)
// reflectThing(8)
// reflectThing(9)


/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

int main(int argc, char** argv)
{
    std::cerr << reflect::namespace_()->print() << std::endl;
    std::cerr << reflect::type<Thing0>()->print() << std::endl;
}
