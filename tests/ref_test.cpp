/* rnd_tests.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Random tests
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

using namespace std;


enum RefType
{
    LValue,
    RValue,
};

template<typename T>
RefType refType(T&&)
{
    return std::is_lvalue_reference<T>::value ? LValue : RValue;
}

template<typename T>
RefType proxyRefType(T&& t)
{
    return refType(std::forward<T>(t));
}

static size_t staticValue = 0;

size_t fValue() { return 0; }
size_t& fLValue() { return staticValue; }
const size_t& fConstLValue() { return staticValue; }

BOOST_AUTO_TEST_CASE(blah)
{
    size_t value;
    auto& lValue = value;
    const auto& constLValue = value;

    BOOST_CHECK_EQUAL(refType(value), LValue);
    BOOST_CHECK_EQUAL(refType(lValue), LValue);
    BOOST_CHECK_EQUAL(refType(constLValue), LValue);
    BOOST_CHECK_EQUAL(refType(std::move(value)), RValue);

    BOOST_CHECK_EQUAL(proxyRefType(value), LValue);
    BOOST_CHECK_EQUAL(proxyRefType(lValue), LValue);
    BOOST_CHECK_EQUAL(proxyRefType(constLValue), LValue);
    BOOST_CHECK_EQUAL(proxyRefType(std::move(value)), RValue);

    BOOST_CHECK_EQUAL(refType(fValue()), RValue);
    BOOST_CHECK_EQUAL(refType(fLValue()), LValue);
    BOOST_CHECK_EQUAL(refType(fConstLValue()), LValue);

    BOOST_CHECK_EQUAL(proxyRefType(fValue()), RValue);
    BOOST_CHECK_EQUAL(proxyRefType(fLValue()), LValue);
    BOOST_CHECK_EQUAL(proxyRefType(fConstLValue()), LValue);
}
