/* cperf_reflect_args_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 23 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Argument reflect compilation performance test.

   Note that if we repeat the same function N times in Target then the compile
   times don't increase because we're essentially repeating the same template
   over and over.

 */

#include "cperf.h"

#include <iostream>


/******************************************************************************/
/* THING                                                                     */
/******************************************************************************/

#define reflectThing(n)                                         \
    struct Thing ## n                                           \
    {                                                           \
        void f0(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) {}      \
        void f1(A9, A0, A1, A2, A3, A4, A5, A6, A7, A8) {}      \
        void f2(A8, A9, A0, A1, A2, A3, A4, A5, A6, A7) {}      \
        void f3(A7, A8, A9, A0, A1, A2, A3, A4, A5, A6) {}      \
        void f4(A6, A7, A8, A9, A0, A1, A2, A3, A4, A5) {}      \
        void f5(A5, A6, A7, A8, A9, A0, A1, A2, A3, A4) {}      \
        void f6(A4, A5, A6, A7, A8, A9, A0, A1, A2, A3) {}      \
        void f7(A3, A4, A5, A6, A7, A8, A9, A0, A1, A2) {}      \
        void f8(A2, A3, A4, A5, A6, A7, A8, A9, A0, A1) {}      \
        void f9(A1, A2, A3, A4, A5, A6, A7, A8, A9, A0) {}      \
    };                                                          \
                                                                \
    reflectClass(Thing ## n)                                    \
    {                                                           \
        reflectFn(f0);                                          \
        reflectFn(f1);                                          \
        reflectFn(f2);                                          \
        reflectFn(f3);                                          \
        reflectFn(f4);                                          \
        reflectFn(f5);                                          \
        reflectFn(f6);                                          \
        reflectFn(f7);                                          \
        reflectFn(f8);                                          \
        reflectFn(f9);                                          \
    }

// Add/remove thing classes to adjust compile time.
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
