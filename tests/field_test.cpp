/* field_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 06 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Tests for field reflection
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "types/primitives.h"
#include "dsl/all.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


/******************************************************************************/
/* FOO                                                                        */
/******************************************************************************/

int global;

struct Foo
{
    Foo() : field(0), constField(123), ref(global), privateField(0) {}

    int field;
    const int constField;

    int& ref; // \todo Not sure how to support this yet.

    int getPrivateField() const { return privateField; }

private:
    friend reflect::Reflect<Foo>;
    int privateField;
};

reflectType(Foo)
{
    reflectField(field);
    reflectField(constField);
    reflectField(privateField);
}

BOOST_AUTO_TEST_CASE(foo)
{
    const Type* tFoo = type("Foo");
    std::cerr << tFoo->print() << std::endl;

    BOOST_CHECK( tFoo->hasField("field"));
    BOOST_CHECK( tFoo->hasField("constField"));
    BOOST_CHECK( tFoo->hasField("privateField"));
    BOOST_CHECK(!tFoo->hasField("bob"));
    BOOST_CHECK(!tFoo->field("field").argument().isConst());
    BOOST_CHECK( tFoo->field("constField").argument().isConst());
    BOOST_CHECK_EQUAL(tFoo->field("field").type(), type<int>());

    Foo foo;
    Value vFoo(foo);

    BOOST_CHECK_EQUAL(vFoo.field<int>("field"), foo.field);
    vFoo.field("field").assign(321);
    BOOST_CHECK_EQUAL(foo.field, 321);
    BOOST_CHECK_EQUAL(vFoo.field<int>("field"), foo.field);
    BOOST_CHECK_EQUAL(&vFoo.field<int&>("field"), &foo.field);
    BOOST_CHECK_EQUAL(&vFoo.field<const int&>("field"), &foo.field);

    BOOST_CHECK_THROW(vFoo.field("constField").assign(987), Error);
    BOOST_CHECK_EQUAL(vFoo.field<int>("constField"), foo.constField);
    BOOST_CHECK_THROW(vFoo.field<int&>("constField"), Error);
    BOOST_CHECK_EQUAL(&vFoo.field<const int&>("constField"), &foo.constField);

    vFoo.field("privateField").assign(654);
    BOOST_CHECK_EQUAL(foo.getPrivateField(), 654);
    BOOST_CHECK_EQUAL(vFoo.field<int>("privateField"), foo.getPrivateField());
}


/******************************************************************************/
/* BAR                                                                        */
/******************************************************************************/

struct Bar
{
    Bar() : shadowed(0) {}

    Foo object;
    const Foo constObject;

    int shadowed;
};

reflectType(Bar)
{
    reflectField(object);
    reflectField(constObject);
    reflectField(shadowed);
}

BOOST_AUTO_TEST_CASE(bar)
{
    const Type* tFoo = type("Foo");
    const Type* tBar = type("Bar");
    std::cerr << tBar->print() << std::endl;

    BOOST_CHECK( tBar->hasField("object"));
    BOOST_CHECK( tBar->hasField("constObject"));
    BOOST_CHECK( tBar->hasField("shadowed"));
    BOOST_CHECK(!tBar->field("object").argument().isConst());
    BOOST_CHECK( tBar->field("constObject").argument().isConst());
    BOOST_CHECK_EQUAL(tBar->field("object").type(), tFoo);

    Bar bar;
    Value vBar(bar);

    BOOST_CHECK_EQUAL(vBar.field("object").field<int>("field"), bar.object.field);
    vBar.field("object").field("field").assign(147);
    BOOST_CHECK_EQUAL(bar.object.field, 147);
    BOOST_CHECK_EQUAL(vBar.field<Foo&>("object").field, bar.object.field);
    BOOST_CHECK_EQUAL(vBar.field("object").field<int>("field"), bar.object.field);

    BOOST_CHECK_THROW(vBar.field("constObject").field("field").assign(258), Error);
    BOOST_CHECK_EQUAL(vBar.field<const Foo&>("constObject").field, bar.constObject.field);
    BOOST_CHECK_EQUAL(vBar.field("constObject").field<int>("field"), bar.constObject.field);
}

/******************************************************************************/
/* BAZ                                                                        */
/******************************************************************************/

struct Baz : public Bar
{
    Baz() : shadowed(0) {}

    int shadowed;
};

reflectType(Baz)
{
    reflectParent(Bar);
    reflectField(shadowed);
}

BOOST_AUTO_TEST_CASE(baz)
{
    const Type* tBar = type("Bar");
    const Type* tBaz = type("Baz");
    std::cerr << tBaz->print() << std::endl;

    BOOST_CHECK( tBaz->hasField("shadowed"));
    BOOST_CHECK( tBaz->hasField("object"));
    BOOST_CHECK( tBaz->hasField("constObject"));

    BOOST_CHECK_EQUAL(
            tBaz->field("object").offset(),
            tBar->field("object").offset());
    BOOST_CHECK_NE(
            tBaz->field("shadowed").offset(),
            tBar->field("shadowed").offset());

    Baz baz;
    Value vBaz(baz);

    Bar& bazParent = baz;
    Value vBazParent(bazParent);

    BOOST_CHECK_EQUAL(&vBaz.field<int&>("shadowed"), &baz.shadowed);
    BOOST_CHECK_EQUAL(&vBazParent.field<int&>("shadowed"), &bazParent.shadowed);
    BOOST_CHECK_NE(&vBaz.field<int&>("shadowed"), &vBazParent.field<int&>("shadowed"));

    vBaz.field("shadowed").assign(123);
    BOOST_CHECK_EQUAL(baz.shadowed, 123);
    BOOST_CHECK_NE(baz.shadowed, bazParent.shadowed);
    BOOST_CHECK_EQUAL(vBaz.field<int>("shadowed"), baz.shadowed);
    BOOST_CHECK_NE(vBazParent.field<int>("shadowed"), baz.shadowed);

    vBazParent.field("shadowed").assign(321);
    BOOST_CHECK_EQUAL(bazParent.shadowed, 321);
    BOOST_CHECK_NE(bazParent.shadowed, baz.shadowed);
    BOOST_CHECK_EQUAL(vBazParent.field<int>("shadowed"), bazParent.shadowed);
    BOOST_CHECK_NE(vBaz.field<int>("shadowed"), bazParent.shadowed);
}
