/* type_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for types.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "test_types.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;

BOOST_AUTO_TEST_CASE(void_)
{
    Type* t = type<void>();

    BOOST_CHECK(!t->parent());
    BOOST_CHECK(t->isParentOf<void>());
    BOOST_CHECK(t->isChildOf<void>());

    BOOST_CHECK_EQUAL(t->fields().size(), 0);

    BOOST_CHECK(!t->isCopiable());
    BOOST_CHECK(!t->isMovable());
}

BOOST_AUTO_TEST_CASE(copiable)
{
    Type* t = type<test::Object>();
    std::cerr << t->print() << std::endl;

    BOOST_CHECK(!t->parent());
    BOOST_CHECK(t->isParentOf<test::Object>());

    BOOST_CHECK(t->isCopiable());
    BOOST_CHECK(t->isMovable());
}
