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
    const Type* t = type<void>();

    BOOST_CHECK(!t->parent());
    BOOST_CHECK(t->isParentOf<void>());
    BOOST_CHECK(t->isChildOf<void>());

    BOOST_CHECK_EQUAL(t->fields().size(), 0u);

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

BOOST_AUTO_TEST_CASE(function_)
{
    const Type* tInt = type<int>();
    BOOST_CHECK( tInt->hasFunction("int"));
    BOOST_CHECK( tInt->hasFunction("operator="));
    BOOST_CHECK(!tInt->hasFunction("bob"));

    const Type* tObject = type<test::Object>();
    BOOST_CHECK( tObject->hasFunction("test::Object"));
    BOOST_CHECK(!tObject->hasFunction("Object"));
    BOOST_CHECK( tObject->hasFunction("value"));
    BOOST_CHECK( tObject->hasFunction("operator="));
    BOOST_CHECK( tObject->hasFunction("operator+="));
    BOOST_CHECK( tObject->hasFunction("operator++"));
    BOOST_CHECK(!tObject->hasFunction("bob"));

    const Type* tParent = type<test::Parent>();
    BOOST_CHECK( tParent->hasFunction("test::Parent"));
    BOOST_CHECK(!tParent->hasFunction("test::Child"));
    BOOST_CHECK( tParent->hasFunction("value"));
    BOOST_CHECK(!tParent->hasFunction("childValue"));
    BOOST_CHECK( tParent->hasFunction("shadowed"));
    BOOST_CHECK( tParent->hasFunction("normalVirtual"));
    BOOST_CHECK( tParent->hasFunction("pureVirtual"));

    const Type* tChild = type<test::Child>();
    BOOST_CHECK( tChild->hasFunction("test::Child"));
    BOOST_CHECK( tChild->hasFunction("test::Parent"));
    BOOST_CHECK( tChild->hasFunction("value"));
    BOOST_CHECK( tChild->hasFunction("childValue"));
    BOOST_CHECK( tChild->hasFunction("shadowed"));
    BOOST_CHECK( tChild->hasFunction("normalVirtual"));
    BOOST_CHECK( tChild->hasFunction("pureVirtual"));

    const Type* tConvertible = type<test::Convertible>();
    BOOST_CHECK( tConvertible->hasFunction("operator int()"));
    BOOST_CHECK( tConvertible->hasFunction("operator test::Parent()"));
}

BOOST_AUTO_TEST_CASE(field)
{
    const Type* tInt = type<int>();
    BOOST_CHECK(!tInt->hasField("int"));
    BOOST_CHECK(!tInt->hasField("operator="));
    BOOST_CHECK(!tInt->hasField("bob"));

    const Type* tObject = type<test::Object>();
    BOOST_CHECK(!tObject->hasField("test::Object"));
    BOOST_CHECK(!tObject->hasField("Object"));
    BOOST_CHECK( tObject->hasField("value"));
    BOOST_CHECK(!tObject->hasField("operator="));
    BOOST_CHECK(!tObject->hasField("operator+="));
    BOOST_CHECK(!tObject->hasField("operator++"));
    BOOST_CHECK(!tObject->hasField("bob"));

    const Type* tParent = type<test::Parent>();
    BOOST_CHECK(!tParent->hasField("test::Parent"));
    BOOST_CHECK(!tParent->hasField("test::Child"));
    BOOST_CHECK( tParent->hasField("value"));
    BOOST_CHECK(!tParent->hasField("childValue"));
    BOOST_CHECK( tParent->hasField("shadowed"));
    BOOST_CHECK(!tParent->hasField("normalVirtual"));
    BOOST_CHECK(!tParent->hasField("pureVirtual"));

    const Type* tChild = type<test::Child>();
    BOOST_CHECK(!tChild->hasField("test::Child"));
    BOOST_CHECK(!tChild->hasField("test::Parent"));
    BOOST_CHECK( tChild->hasField("value"));
    BOOST_CHECK( tChild->hasField("childValue"));
    BOOST_CHECK( tChild->hasField("shadowed"));
    BOOST_CHECK(!tChild->hasField("normalVirtual"));
    BOOST_CHECK(!tChild->hasField("pureVirtual"));

    const Type* tConvertible = type<test::Convertible>();
    BOOST_CHECK(!tConvertible->hasField("operator int()"));
    BOOST_CHECK(!tConvertible->hasField("operator test::Parent()"));
}

BOOST_AUTO_TEST_CASE(fieldType)
{
    const Type* tInt = type<int>();

    const Type* tObject = type<test::Object>();
    BOOST_CHECK_EQUAL(tObject->fieldType("value"), tInt);

    const Type* tParent = type<test::Parent>();
    BOOST_CHECK_EQUAL(tParent->fieldType("value"), tObject);
    BOOST_CHECK_EQUAL(tParent->fieldType("shadowed"), tInt);

    const Type* tChild = type<test::Child>();
    BOOST_CHECK_EQUAL(tChild->fieldType("value"), tObject);
    BOOST_CHECK_EQUAL(tChild->fieldType("childValue"), tObject);
    BOOST_CHECK_EQUAL(tChild->fieldType("shadowed"), type<bool>());
}

BOOST_AUTO_TEST_CASE(moveCopy)
{
    const Type* tInt = type<int>();
    BOOST_CHECK(tInt->isCopiable());
    BOOST_CHECK(tInt->isMovable());

    const Type* tObject = type<test::Object>();
    BOOST_CHECK(tObject->isCopiable());
    BOOST_CHECK(tObject->isMovable());

    const Type* tNotCopiable = type<test::NotCopiable>();
    BOOST_CHECK(!tNotCopiable->isCopiable());
    BOOST_CHECK( tNotCopiable->isMovable());

    const Type* tNotMovable = type<test::NotMovable>();
    BOOST_CHECK(!tNotMovable->isCopiable());
    BOOST_CHECK(!tNotMovable->isMovable());

    const Type* tNotConstructible = type<test::NotConstructible>();
    BOOST_CHECK(!tNotConstructible->isCopiable());
    BOOST_CHECK(!tNotConstructible->isMovable());
}

BOOST_AUTO_TEST_CASE(parentChild)
{
    const Type* tInt = type<int>();
    BOOST_CHECK( tInt->isParentOf<int>());
    BOOST_CHECK(!tInt->isParentOf<unsigned>());
    BOOST_CHECK(!tInt->isParentOf<test::Object>());

    const Type* tInterface = type<test::Interface>();
    BOOST_CHECK( tInterface->isParentOf<test::Interface>());
    BOOST_CHECK( tInterface->isParentOf<test::Parent>());
    BOOST_CHECK( tInterface->isParentOf<test::Child>());
    BOOST_CHECK(!tInterface->isParentOf<test::Object>());
    BOOST_CHECK( tInterface->isChildOf<test::Interface>());
    BOOST_CHECK(!tInterface->isChildOf<test::Parent>());
    BOOST_CHECK(!tInterface->isChildOf<test::Child>());
    BOOST_CHECK(!tInterface->isChildOf<test::Object>());

    const Type* tParent = type<test::Parent>();
    BOOST_CHECK(!tParent->isParentOf<test::Interface>());
    BOOST_CHECK( tParent->isParentOf<test::Parent>());
    BOOST_CHECK( tParent->isParentOf<test::Child>());
    BOOST_CHECK(!tParent->isParentOf<test::Object>());
    BOOST_CHECK( tParent->isChildOf<test::Interface>());
    BOOST_CHECK( tParent->isChildOf<test::Parent>());
    BOOST_CHECK(!tParent->isChildOf<test::Child>());
    BOOST_CHECK(!tParent->isChildOf<test::Object>());

    const Type* tChild = type<test::Child>();
    BOOST_CHECK(!tChild->isParentOf<test::Interface>());
    BOOST_CHECK(!tChild->isParentOf<test::Parent>());
    BOOST_CHECK( tChild->isParentOf<test::Child>());
    BOOST_CHECK(!tChild->isParentOf<test::Object>());
    BOOST_CHECK( tChild->isChildOf<test::Interface>());
    BOOST_CHECK( tChild->isChildOf<test::Parent>());
    BOOST_CHECK( tChild->isChildOf<test::Child>());
    BOOST_CHECK(!tChild->isChildOf<test::Object>());
}

BOOST_AUTO_TEST_CASE(converter)
{
    const Type* tParent = type<test::Parent>();
    BOOST_CHECK(!tParent->hasConverter<int>());
    BOOST_CHECK(!tParent->hasConverter<test::Parent>());
    BOOST_CHECK(!tParent->hasConverter<test::Convertible>());

    const Type* tConvertible = type<test::Convertible>();
    BOOST_CHECK( tConvertible->hasConverter<int>());
    BOOST_CHECK( tConvertible->hasConverter<test::Parent>());
    BOOST_CHECK(!tConvertible->hasConverter<test::Convertible>());
}
