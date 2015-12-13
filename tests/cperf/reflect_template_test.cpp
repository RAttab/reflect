/* reflect_template_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Dec 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "cperf.h"
#include "dsl/field.h"

#include <iostream>


/******************************************************************************/
/* TEMPLATE                                                                   */
/******************************************************************************/

template<typename T>
struct Template
{
    void f0(A0) {}
    void f1(A1) {}
    void f2(A2) {}
    void f3(A3) {}
    void f4(A4) {}
    void f5(A5) {}
    void f6(A6) {}
    void f7(A7) {}
    void f8(A8) {}
    void f9(A9) {}
};

reflectTemplate(Template, T)
{
    reflectPlumbing();
    reflectFn(f0);
    reflectFn(f1);
    reflectFn(f2);
    reflectFn(f3);
    reflectFn(f4);
    reflectFn(f5);
    reflectFn(f6);
    reflectFn(f7);
    reflectFn(f8);
    reflectFn(f9);
}


/******************************************************************************/
/* THING                                                                      */
/******************************************************************************/

#define reflectThing(n)                         \
    struct Thing ## n                           \
    {                                           \
        Template<A0> t0;                        \
        Template<A1> t1;                        \
        Template<A2> t2;                        \
        Template<A3> t3;                        \
        Template<A4> t4;                        \
        Template<A5> t5;                        \
        Template<A6> t6;                        \
        Template<A7> t7;                        \
        Template<A8> t8;                        \
        Template<A9> t9;                        \
    };                                          \
                                                \
    reflectType(Thing ## n)                     \
    {                                           \
        reflectField(t0);                       \
        reflectField(t1);                       \
        reflectField(t2);                       \
        reflectField(t3);                       \
        reflectField(t4);                       \
        reflectField(t5);                       \
        reflectField(t6);                       \
        reflectField(t7);                       \
        reflectField(t8);                       \
        reflectField(t9);                       \
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

