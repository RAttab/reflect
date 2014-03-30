/* value_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for the Value function.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


BOOST_AUTO_TEST_CASE(void_)
{
    Value val;
    BOOST_CHECK(val.isVoid());
    BOOST_CHECK_EQUAL(Value().type(), val.type());
}

BOOST_AUTO_TEST_CASE(lValue)
{
    unsigned u = 10;
    Value lValue(u);

    BOOST_CHECK(!lValue.isConst());
    BOOST_CHECK_EQUAL(lValue.refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(lValue.get<unsigned>(), u);

    // copy
    BOOST_CHECK(lValue.copiable<unsigned>());
    BOOST_CHECK_EQUAL(lValue.copy<unsigned>(), u);

    // l-ref
    BOOST_CHECK(lValue.castable<unsigned>());
    {
        auto& value = lValue.cast<unsigned>();
        BOOST_CHECK_EQUAL(&value, &u);
    }

    // const l-ref
    BOOST_CHECK(lValue.castable<const unsigned>());
    {
        const auto& value = lValue.cast<const unsigned>();
        BOOST_CHECK_EQUAL(&value, &u);
    }

    // r-ref
    BOOST_CHECK(lValue.movable<unsigned>());
    {
        auto value = lValue.move<unsigned>();
        BOOST_CHECK_EQUAL(value, 10);
        BOOST_CHECK(lValue.isVoid());

        // \todo primitives don't get wiped. Should use another type;
        // BOOST_CHECK_NE(u, 10);
    }
}

BOOST_AUTO_TEST_CASE(constLValue)
{
    const unsigned u = 10;
    Value lValue(u);

    BOOST_CHECK(lValue.isConst());
    BOOST_CHECK_EQUAL(lValue.refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(lValue.get<unsigned>(), u);

    // copy
    BOOST_CHECK(lValue.copiable<unsigned>());
    BOOST_CHECK_EQUAL(lValue.copy<unsigned>(), u);

    // l-ref
    BOOST_CHECK(!lValue.castable<unsigned>());
    BOOST_CHECK_THROW(lValue.cast<unsigned>(), ReflectError);

    // const l-ref
    BOOST_CHECK(lValue.castable<const unsigned>());
    {
        const auto& value = lValue.cast<const unsigned>();
        BOOST_CHECK_EQUAL(&value, &u);
    }

    // r-ref
    BOOST_CHECK(!lValue.movable<unsigned>());
    BOOST_CHECK_THROW(lValue.move<unsigned>(), ReflectError);
}

BOOST_AUTO_TEST_CASE(rValue)
{
    unsigned u = 10;
    Value rValue(std::move(u));
    u = 20; // Just to make sure we don't have a ref to u.

    BOOST_CHECK(!rValue.isConst());
    BOOST_CHECK_EQUAL(rValue.refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(rValue.get<unsigned>(), 10);

    // copy
    BOOST_CHECK(rValue.copiable<unsigned>());
    BOOST_CHECK_EQUAL(rValue.copy<unsigned>(), 10);

    // l-ref
    BOOST_CHECK(rValue.castable<unsigned>());
    BOOST_CHECK_EQUAL(rValue.cast<unsigned>(), 10);

    // const l-ref
    BOOST_CHECK( rValue.castable<const unsigned>());
    {
        const auto& value = rValue.cast<const unsigned>();
        BOOST_CHECK_NE(&value, &u);
        BOOST_CHECK_EQUAL(value, 10);
    }

    // r-ref
    BOOST_CHECK(rValue.movable<unsigned>());
    BOOST_CHECK_EQUAL(rValue.move<unsigned>(), 10);
    BOOST_CHECK(rValue.isVoid());
}

BOOST_AUTO_TEST_CASE(constRValue)
{
    const unsigned i = 0;

    Value rValue(std::move(i));
    BOOST_CHECK(!rValue.isConst());
    BOOST_CHECK_EQUAL(rValue.refType(), RefType::LValue);
}
