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


void foo(unsigned& value, int other)
{
    value += other;
}

BOOST_AUTO_TEST_CASE(fn)
{
    std::function<void(unsigned&, int)> fn(&foo);
    auto valueFn = makeValueFunction(fn);

    unsigned value = 0;

    Value ret = valueFn(Value(value), Value(int(10)));
    BOOST_CHECK(ret.isVoid());
    BOOST_CHECK_EQUAL(value, 10);

    valueFn(Value(value), Value(int(10)));
    BOOST_CHECK_EQUAL(value, 20);
}


struct Functor
{
    Functor() : value(0) {}

    unsigned& operator() (unsigned i)
    {
        value += i;
        return value;
    }

    unsigned value;
};


BOOST_AUTO_TEST_CASE(functor)
{
    Functor foo;
    std::function<unsigned&(unsigned)> fn(foo);

    auto valueFn = makeValueFunction(fn);

    {
        Value ret = valueFn(Value(unsigned(10)));
        BOOST_CHECK_EQUAL(ret.refType(), RefType::LValue);
        BOOST_CHECK(ret.castable<unsigned>());
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 10);
    }

    {
        Value ret = valueFn(Value(unsigned(10)));
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 20);
    }
}


BOOST_AUTO_TEST_CASE(lambda)
{
    unsigned value = 0;
    auto lambda = [&](unsigned x, unsigned&& z) {
        return value = x * x + std::move(z);
    };

    std::function<unsigned(unsigned, unsigned&&)> fn(lambda);
    auto valueFn = makeValueFunction(fn);

    Value ret = valueFn(Value(unsigned(10)), Value(unsigned(10)));
    BOOST_CHECK_EQUAL(value, 110);
    BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 110);
}
