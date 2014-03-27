/* value_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for the Value function.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


BOOST_AUTO_TEST_CASE(void_)
{
    Value val;
    BOOST_CHECK(val.isVoid());
    BOOST_CHECK_EQUAL(Value().reflection(), val.reflection());
}

BOOST_AUTO_TEST_CASE(lValue)
{
    unsigned u = 10;
    Value lValue(u);
    BOOST_CHECK_EQUAL(lValue.refType(), RefType::LValue);

    BOOST_CHECK(lValue.movable<unsigned>());
    BOOST_CHECK(lValue.castable<unsigned>());

    {
        auto& value = lValue.cast<unsigned>();
        BOOST_CHECK_EQUAL(&value, &u);
    }

    {
        auto value = lValue.move<unsigned>();
        BOOST_CHECK_EQUAL(value, 10);
        BOOST_CHECK(lValue.isVoid());

        // \todo primitives don't get wiped. Should use another type;
        // BOOST_CHECK_NE(u, 10);
    }
}

BOOST_AUTO_TEST_CASE(rValue)
{
    unsigned u = 10;
    Value rValue(std::move(u));
    BOOST_CHECK_EQUAL(rValue.refType(), RefType::RValue);

    u = 20; // Just to make sure we don't have a ref to u.

    BOOST_CHECK(rValue.castable<unsigned>());
    BOOST_CHECK(rValue.movable<unsigned>());

    // Safety checks against moving out a shared storage.
    {
        Value other = rValue;
        BOOST_CHECK(!rValue.movable<unsigned>());
        BOOST_CHECK(!other.movable<unsigned>());
    }
    BOOST_CHECK(rValue.castable<unsigned>());
    BOOST_CHECK(rValue.movable<unsigned>());

    {
        auto& value = rValue.cast<unsigned>();
        BOOST_CHECK_NE(&value, &u);
        BOOST_CHECK_EQUAL(value, 10);
    }

    BOOST_CHECK_EQUAL(rValue.move<unsigned>(), 10);
    BOOST_CHECK(rValue.isVoid());
}

BOOST_AUTO_TEST_CASE(uncastable)
{
    Value value(unsigned(10));
    BOOST_CHECK( value.castable<unsigned>());
    BOOST_CHECK(!value.castable<int>());
    BOOST_CHECK(!value.castable<long>());
}
