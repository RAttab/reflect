/* value_function_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for ValueFunction.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;

struct Foo
{
    Foo() : value(0) {}

    unsigned operator() (unsigned i)
    {
        return value += i;
    }

    unsigned value;
};


BOOST_AUTO_TEST_CASE(basics)
{
    Foo foo;
    std::function<unsigned(unsigned)> fn(foo);

    auto valueFn = makeValueFunction(fn);

    {
        Value ret = valueFn(Value(unsigned(10)));
        BOOST_CHECK(ret.castable<unsigned>());
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 10);
    }

    {
        Value ret = valueFn(Value(unsigned(10)));
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 20);
    }
}
