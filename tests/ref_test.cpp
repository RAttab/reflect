/* rnd_tests.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Random tests
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"

#include <boost/test/unit_test.hpp>

using namespace reflect;

template<typename T>
constexpr RefType proxyRefType(T&& t)
{
    return makeRefType(std::forward<T>(t));
}

static size_t staticValue = 0;

size_t fValue() { return 0; }
size_t& fLValue() { return staticValue; }
const size_t& fConstLValue() { return staticValue; }

BOOST_AUTO_TEST_CASE(value)
{
    size_t value;
    auto& lValue = value;
    const auto& constLValue = value;

    BOOST_CHECK_EQUAL(makeRefType(value), RefType::LValue);
    BOOST_CHECK_EQUAL(makeRefType(lValue), RefType::LValue);
    BOOST_CHECK_EQUAL(makeRefType(constLValue), RefType::LValue);
    BOOST_CHECK_EQUAL(makeRefType(std::move(value)), RefType::RValue);

    BOOST_CHECK_EQUAL(proxyRefType(value), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(lValue), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(constLValue), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(std::move(value)), RefType::RValue);

    BOOST_CHECK_EQUAL(makeRefType(fValue()), RefType::RValue);
    BOOST_CHECK_EQUAL(makeRefType(fLValue()), RefType::LValue);
    BOOST_CHECK_EQUAL(makeRefType(fConstLValue()), RefType::LValue);

    BOOST_CHECK_EQUAL(proxyRefType(fValue()), RefType::RValue);
    BOOST_CHECK_EQUAL(proxyRefType(fLValue()), RefType::LValue);
    BOOST_CHECK_EQUAL(proxyRefType(fConstLValue()), RefType::LValue);
}

BOOST_AUTO_TEST_CASE(types)
{
    BOOST_CHECK_EQUAL(makeRefType<unsigned>(), RefType::Copy);
    BOOST_CHECK_EQUAL(makeRefType<unsigned&>(), RefType::LValue);
    BOOST_CHECK_EQUAL(makeRefType<unsigned&&>(), RefType::RValue);
    BOOST_CHECK_EQUAL(makeRefType<const unsigned&>(), RefType::LValue);
}

BOOST_AUTO_TEST_CASE(constness)
{
    unsigned i = 0;
    unsigned& ref = i;
    unsigned&& rref = std::move(i);
    const unsigned c = 0;;
    const unsigned& cref = c;

    BOOST_CHECK(!isConst(i));
    BOOST_CHECK(!isConst(ref));
    BOOST_CHECK(!isConst(rref));
    BOOST_CHECK( isConst(c));
    BOOST_CHECK( isConst(cref));

    BOOST_CHECK(!isConst<unsigned>());
    BOOST_CHECK(!isConst<unsigned&>());
    BOOST_CHECK(!isConst<unsigned&&>());
    BOOST_CHECK( isConst<const unsigned>());
    BOOST_CHECK( isConst<const unsigned&>());

    // \todo We don't respect pointer const-ness
    BOOST_CHECK(!isConst<unsigned      *>());
    BOOST_CHECK(!isConst<unsigned const*>());
    BOOST_CHECK(!isConst<unsigned const* const*>());
    BOOST_CHECK(!isConst<unsigned const*      *>());
    BOOST_CHECK(!isConst<unsigned      * const*>());
    BOOST_CHECK(!isConst<unsigned      *      &>());
    BOOST_CHECK(!isConst<unsigned const*      &>());
    BOOST_CHECK( isConst<unsigned      * const&>());
    BOOST_CHECK( isConst<unsigned const* const&>());
}

