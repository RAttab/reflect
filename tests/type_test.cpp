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

BOOST_AUTO_TEST_CASE(print)
{
    std::cerr << type<int>()->print() << std::endl;
    std::cerr << type<test::Object>()->print() << std::endl;
    std::cerr << type<test::NotCopiable>()->print() << std::endl;
    std::cerr << type<test::NotMovable>()->print() << std::endl;
    std::cerr << type<test::NotConstructible>()->print() << std::endl;
    std::cerr << type<test::Child>()->print() << std::endl;
    std::cerr << type<test::Convertible>()->print() << std::endl;
}

BOOST_AUTO_TEST_CASE(field)
{
    Type* tInt = type<int>();
    BOOST_CHECK( tInt->hasField("int"));
    BOOST_CHECK( tInt->hasField("operator="));
    BOOST_CHECK(!tInt->hasField("bob"));

    Type* tObject = type<test::Object>();
    BOOST_CHECK( tObject->hasField("test::Object"));
    BOOST_CHECK(!tObject->hasField("Object"));
    BOOST_CHECK( tObject->hasField("value"));
    BOOST_CHECK( tObject->hasField("operator="));
    BOOST_CHECK( tObject->hasField("operator+="));
    BOOST_CHECK( tObject->hasField("operator++"));
    BOOST_CHECK(!tObject->hasField("bob"));

    Type* tParent = type<test::Parent>();
    BOOST_CHECK( tParent->hasField("test::Parent"));
    BOOST_CHECK(!tParent->hasField("test::Child"));
    BOOST_CHECK( tParent->hasField("value"));
    BOOST_CHECK(!tParent->hasField("childValue"));
    BOOST_CHECK( tParent->hasField("shadowed"));
    BOOST_CHECK( tParent->hasField("normalVirtual"));
    BOOST_CHECK( tParent->hasField("pureVirtual"));

    Type* tChild = type<test::Child>();
    BOOST_CHECK( tChild->hasField("test::Child"));
    BOOST_CHECK( tChild->hasField("test::Parent")); // \todo Do we want this?
    BOOST_CHECK( tChild->hasField("value"));
    BOOST_CHECK( tChild->hasField("childValue"));
    BOOST_CHECK( tChild->hasField("shadowed"));
    BOOST_CHECK( tChild->hasField("normalVirtual"));
    BOOST_CHECK( tChild->hasField("pureVirtual"));

    Type* tConvertible = type<test::Convertible>();
    BOOST_CHECK( tConvertible->hasField("operator int()"));
    BOOST_CHECK( tConvertible->hasField("operator test::Parent()"));
}

BOOST_AUTO_TEST_CASE(moveCopy)
{
    Type* tInt = type<int>();
    BOOST_CHECK(tInt->isCopiable());
    BOOST_CHECK(tInt->isMovable());

    Type* tObject = type<test::Object>();
    BOOST_CHECK(tObject->isCopiable());
    BOOST_CHECK(tObject->isMovable());

    Type* tNotCopiable = type<test::NotCopiable>();
    BOOST_CHECK(!tNotCopiable->isCopiable());
    // BOOST_CHECK( tNotCopiable->isMovable()); // \todo

    Type* tNotMovable = type<test::NotMovable>();
    BOOST_CHECK(!tNotMovable->isCopiable());
    BOOST_CHECK(!tNotMovable->isMovable());

    Type* tNotConstructible = type<test::NotConstructible>();
    BOOST_CHECK(!tNotConstructible->isCopiable());
    // BOOST_CHECK( tNotConstructible->isMovable()); // \todo
}

BOOST_AUTO_TEST_CASE(parentChild)
{
    Type* tInt = type<int>();
    BOOST_CHECK( tInt->isParentOf<int>());
    BOOST_CHECK(!tInt->isParentOf<unsigned>());
    BOOST_CHECK(!tInt->isParentOf<test::Object>());

    Type* tParent = type<test::Parent>();
    BOOST_CHECK( tParent->isParentOf<test::Parent>());
    BOOST_CHECK( tParent->isParentOf<test::Child>());
    BOOST_CHECK(!tParent->isParentOf<test::Object>());
    BOOST_CHECK( tParent->isChildOf<test::Parent>());
    BOOST_CHECK(!tParent->isChildOf<test::Child>());
    BOOST_CHECK(!tParent->isChildOf<test::Object>());

    Type* tChild = type<test::Child>();
    BOOST_CHECK(!tChild->isParentOf<test::Parent>());
    BOOST_CHECK( tChild->isParentOf<test::Child>());
    BOOST_CHECK(!tChild->isParentOf<test::Object>());
    BOOST_CHECK( tChild->isChildOf<test::Parent>());
    BOOST_CHECK( tChild->isChildOf<test::Child>());
    BOOST_CHECK(!tChild->isChildOf<test::Object>());
}

BOOST_AUTO_TEST_CASE(converter)
{
    Type* tParent = type<test::Parent>();
    BOOST_CHECK(!tParent->hasConverter<int>());
    BOOST_CHECK(!tParent->hasConverter<test::Parent>());
    BOOST_CHECK(!tParent->hasConverter<test::Convertible>());

    Type* tConvertible = type<test::Convertible>();
    BOOST_CHECK( tConvertible->hasConverter<int>());
    BOOST_CHECK( tConvertible->hasConverter<test::Parent>());
    BOOST_CHECK(!tConvertible->hasConverter<test::Convertible>());
}
