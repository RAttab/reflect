/* blah_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Experimental tests
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


/******************************************************************************/
/* FOO                                                                        */
/******************************************************************************/

namespace test {

struct Foo
{
    Foo() : constField(0) {}

    int field;
    const int constField;

    void void_() {}

    const int& getter() const { return value; }
    void setter(int i) { value = i; }

    void copy(int i) { value = i; }
    int copy() const { return value; }

    void lValue(int& i) { value = i; }
    int& lValue() { return value; }

    void constLValue(const int& i) { value = i; }
    const int& constLValue() const { return value; }

    void rValue(int&& i) { value = std::move(i); }
    int rValue() { return std::move(value); }

    void function(int a, int b, int c) { value += a * b + c; };

private:
    int value;
};

} // namespace test


/******************************************************************************/
/* REFLECT FOO                                                                */
/******************************************************************************/

reflectClass(test::Foo)
{
    printf("\nfield(void)\n");        reflectField(void_);
    printf("\nfield(field)\n");       reflectField(field);
    printf("\nfield(constField)\n");  reflectField(constField);
    printf("\nfield(copy)\n");        reflectField(copy);
    printf("\nfield(lValue)\n");      reflectField(lValue);
    printf("\nfield(constLValue)\n"); reflectField(constLValue);
    printf("\nfield(rValue)\n");      reflectField(rValue);

    printf("\nfn(function)\n");       reflectFn(function);

    printf("\nlambda(custom)\n");
    reflectCustom(custom) (test::Foo& obj, int a, int b) {
        obj.setter(a + b);
    };
}

BOOST_AUTO_TEST_CASE(blah)
{
    Registry::get<test::Foo>();
}
