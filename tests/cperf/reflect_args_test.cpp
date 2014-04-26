/* cperf_reflect_args_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 23 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Argument reflect compilation performance test.

   Note that if we repeat the same function N times in Target then the compile
   times don't increase because we're essentially repeating the same template
   over and over.

 */

#include "reflect.h"
#include "reflect/class.h"

#include <iostream>


/******************************************************************************/
/* ARGS                                                                       */
/******************************************************************************/

#define reflectArg(n)                       \
    struct A ## n {};                       \
    reflectClass(A ## n) {}

reflectArg(0)
reflectArg(1)
reflectArg(2)
reflectArg(3)
reflectArg(4)
reflectArg(5)
reflectArg(6)
reflectArg(7)
reflectArg(8)
reflectArg(9)


/******************************************************************************/
/* TARGET                                                                     */
/******************************************************************************/

#define reflectTarget(n)                                        \
    struct Target ## n                                          \
    {                                                           \
        void test0(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) {}   \
        void test1(A9, A0, A1, A2, A3, A4, A5, A6, A7, A8) {}   \
        void test2(A8, A9, A0, A1, A2, A3, A4, A5, A6, A7) {}   \
        void test3(A7, A8, A9, A0, A1, A2, A3, A4, A5, A6) {}   \
        void test4(A6, A7, A8, A9, A0, A1, A2, A3, A4, A5) {}   \
        void test5(A5, A6, A7, A8, A9, A0, A1, A2, A3, A4) {}   \
        void test6(A4, A5, A6, A7, A8, A9, A0, A1, A2, A3) {}   \
        void test7(A3, A4, A5, A6, A7, A8, A9, A0, A1, A2) {}   \
        void test8(A2, A3, A4, A5, A6, A7, A8, A9, A0, A1) {}   \
        void test9(A1, A2, A3, A4, A5, A6, A7, A8, A9, A0) {}   \
    };                                                          \
                                                                \
    reflectClass(Target ## n) {                                 \
        reflectFn(test0);                                       \
        reflectFn(test1);                                       \
        reflectFn(test2);                                       \
        reflectFn(test3);                                       \
        reflectFn(test4);                                       \
        reflectFn(test5);                                       \
        reflectFn(test6);                                       \
        reflectFn(test7);                                       \
        reflectFn(test8);                                       \
        reflectFn(test9);                                       \
    }

// Add/remove target classes to adjust compile time.
reflectTarget(0)
reflectTarget(1)
reflectTarget(2)
reflectTarget(3)
reflectTarget(4)
// reflectTarget(5)
// reflectTarget(6)
// reflectTarget(7)
// reflectTarget(8)
// reflectTarget(9)


/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

int main(int, char**)
{
    std::cerr << reflect::namespace_()->print() << std::endl;
    std::cerr << reflect::type<Target0>()->print() << std::endl;
}
