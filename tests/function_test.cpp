/* function_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for function.h
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


BOOST_AUTO_TEST_CASE(basics)
{
    using reflect::reflect;

    auto foo = [] (unsigned i, int j) -> unsigned {
        return i + 1 * j;
    };
    Function fn("foo", foo);

    BOOST_CHECK_EQUAL(fn.returnType().type(), reflect<unsigned>());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Value);

    BOOST_CHECK_EQUAL(fn.arguments(), 2);
    BOOST_CHECK_EQUAL(fn.argument(0).type(), reflect<unsigned>());
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Value);
    BOOST_CHECK_EQUAL(fn.argument(1).type(), reflect<int>());
    BOOST_CHECK_EQUAL(fn.argument(1).refType(), RefType::Value);

    BOOST_CHECK( fn.test(fn));
    BOOST_CHECK( fn.test<unsigned(unsigned, int)>());
    BOOST_CHECK( fn.test<void    (unsigned, int)>());
    BOOST_CHECK(!fn.test<unsigned(int, unsigned)>());

    BOOST_CHECK_EQUAL(fn.call<unsigned>(1u, 2), foo(1, 2));
    fn.call<void>(1u, 2);
}

BOOST_AUTO_TEST_CASE(voids)
{
    using reflect::reflect;

    auto foo = [] {};
    Function fn("foo", foo);

    BOOST_CHECK_EQUAL(fn.returnType().type(), reflect<void>());
    BOOST_CHECK_EQUAL(fn.arguments(), 0);

    BOOST_CHECK(fn.test(fn));
    fn.call<void>();
}

BOOST_AUTO_TEST_CASE(lValue)
{
    using reflect::reflect;

    auto foo = [] (unsigned& i, unsigned j) -> unsigned& {
        i += j;
        return i;
    };
    Function fn("foo", foo);

    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::LValue);

    BOOST_CHECK( fn.test(fn));
    BOOST_CHECK( fn.test<unsigned&(unsigned&,  unsigned )>());
    BOOST_CHECK(!fn.test<unsigned&(unsigned,   unsigned )>());
    BOOST_CHECK(!fn.test<unsigned (unsigned&,  unsigned )>());
    BOOST_CHECK(!fn.test<unsigned&(unsigned&,  unsigned&)>());
    BOOST_CHECK(!fn.test<unsigned&(unsigned&&, unsigned )>());

    unsigned value = 0;
    {
        auto& ret = fn.call<unsigned&>(value, 10u);
        BOOST_CHECK_EQUAL(value, 10);
        BOOST_CHECK_EQUAL(&ret, &value);
    }

    {
        unsigned arg = 10;
        cerr << endl;
        Value ret = fn.call<Value>(Value(value), arg);
        BOOST_CHECK_EQUAL(value, 20);
        BOOST_CHECK_EQUAL(&ret.cast<unsigned>(), &value);
    }

    {
        fn.call<void>(value, Value(10u));
        BOOST_CHECK_EQUAL(value, 30);
    }
}


BOOST_AUTO_TEST_CASE(rValue)
{

}
