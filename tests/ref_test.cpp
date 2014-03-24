/* rnd_tests.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Random tests
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "utils.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;

template<typename T>
constexpr RefType proxyRefType(T&& t)
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

    BOOST_CHECK_EQUAL(refType(value), RefType::LValue);
    BOOST_CHECK_EQUAL(refType(lValue), RefType::LValue);
    BOOST_CHECK_EQUAL(refType(constLValue), RefType::LValue);
    BOOST_CHECK_EQUAL(refType(std::move(value)), RefType::RValue);

    BOOST_CHECK_EQUAL(proxyRefType(value), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(lValue), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(constLValue), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(std::move(value)), RefType::RValue);

    BOOST_CHECK_EQUAL(refType(fValue()), RefType::RValue);
    BOOST_CHECK_EQUAL(refType(fLValue()), RefType::LValue);
    BOOST_CHECK_EQUAL(refType(fConstLValue()), RefType::LValue);

    BOOST_CHECK_EQUAL(proxyRefType(fValue()), RefType::RValue);
    BOOST_CHECK_EQUAL(proxyRefType(fLValue()), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(fConstLValue()), RefType::LValue);
}
