/* value_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for the Value function.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "test_types.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


/******************************************************************************/
/* VOID                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(void_)
{
    Value val;
    BOOST_CHECK(val.isVoid());
    BOOST_CHECK_EQUAL(Value().type(), val.type());
}


/******************************************************************************/
/* CONSTRUCT                                                                  */
/******************************************************************************/

// Make sure that Value compiles regardless of whether the copy/move
// constructors are there or not. We should instead break at runtime IF they are
// used.
BOOST_AUTO_TEST_CASE(construct)
{
    BOOST_TEST_CHECKPOINT("construct-int");
    Value(0);

    BOOST_TEST_CHECKPOINT("construct-ncp");
    test::NotCopiable ncp;
    Value(std::move(ncp));

    BOOST_TEST_CHECKPOINT("construct-nmv");
    test::NotMovable nmv;
    Value(std::move(nmv));

    BOOST_TEST_CHECKPOINT("construct-ncs");
    std::unique_ptr<test::NotConstructible> ncs(test::NotConstructible::make());
    BOOST_CHECK_THROW(Value(std::move(*ncs)), Error);

    BOOST_TEST_CHECKPOINT("construct-done");
}


/******************************************************************************/
/* DESTRUCT                                                                   */
/******************************************************************************/

struct Destruct
{
    ~Destruct() { count++; }
    static void reset() { count = 0; }
    static size_t count;
};
size_t Destruct::count = 0;

reflectType(Destruct) { reflectPlumbing(); }

BOOST_AUTO_TEST_CASE(destruct)
{
    auto check = [] (std::string name, size_t count) {
        std::cerr << name << ": " << Destruct::count << std::endl;
        BOOST_CHECK_EQUAL(Destruct::count, count);
        Destruct::reset();
    };

    Destruct d;
    Destruct::reset();

    {
        Value obj{d};
        (void) obj;
    }
    check("lref", 0);

    {
        Value obj(std::move(d));
        (void) obj;
    }
    check("rref", 1);

    {
        Value obj(std::move(d));
        Value valueCopy(obj);
        (void) valueCopy;
    }
    check("value-copy", 1);

    // The reflected copy constructor is a lambda that returns a rvalue which is
    // moved into the Value object but also destroyed. hence the 3 instead of
    // the expected two.
    {
        Value obj(std::move(d));
        Value objCopy = obj.copy();
        (void) objCopy;
    }
    check("obj-copy", 2 + 1);

    {
        Value obj(std::move(d));
        Value valueMove = std::move(obj);
        (void) valueMove;
    }
    check("value-move", 1);

    // Calling move creates lots of temporaries that are moved around quite a
    // bit. I hand checked these to make sure that there's only call to the
    // destructor that originates from a shared_ptr as we expect.
    {
        Value obj(std::move(d));
        Value objMove = obj.move();
        (void) objMove;
    }
    check("obj-move", 2 + 3);
}


/******************************************************************************/
/* LVALUE                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(lValue)
{
    unsigned u = 10;
    Value lValue(u);

    BOOST_CHECK(!lValue.isConst());
    BOOST_CHECK_EQUAL(lValue.refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(lValue.get<unsigned>(), u);

    // copy
    BOOST_CHECK(lValue.isCopiable<unsigned>());
    BOOST_CHECK_EQUAL(lValue.copy<unsigned>(), u);

    // l-ref
    BOOST_CHECK(lValue.isCastable<unsigned>());
    {
        auto& value = lValue.cast<unsigned>();
        BOOST_CHECK_EQUAL(&value, &u);
    }

    // const l-ref
    BOOST_CHECK(lValue.isCastable<const unsigned>());
    {
        const auto& value = lValue.cast<const unsigned>();
        BOOST_CHECK_EQUAL(&value, &u);
    }

    // r-ref
    BOOST_CHECK(lValue.isMovable<unsigned>());
    {
        auto value = lValue.move<unsigned>();
        BOOST_CHECK_EQUAL(value, 10u);
        BOOST_CHECK(lValue.isVoid());

        // primitives don't get wiped. Should use another type;
        // BOOST_CHECK_NE(u, 10);
    }
}


/******************************************************************************/
/* CONST LVALUE                                                               */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(constLValue)
{
    const unsigned u = 10;
    Value lValue(u);

    BOOST_CHECK(lValue.isConst());
    BOOST_CHECK_EQUAL(lValue.refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(lValue.get<unsigned>(), u);

    // copy
    BOOST_CHECK(lValue.isCopiable<unsigned>());
    BOOST_CHECK_EQUAL(lValue.copy<unsigned>(), u);

    // l-ref
    BOOST_CHECK(!lValue.isCastable<unsigned>());
    BOOST_CHECK_THROW(lValue.cast<unsigned>(), Error);

    // const l-ref
    BOOST_CHECK(lValue.isCastable<const unsigned>());
    {
        const auto& value = lValue.cast<const unsigned>();
        BOOST_CHECK_EQUAL(&value, &u);
    }

    // r-ref
    BOOST_CHECK(!lValue.isMovable<unsigned>());
    BOOST_CHECK_THROW(lValue.move<unsigned>(), Error);
}


/******************************************************************************/
/* RVALUE                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(rValue)
{
    unsigned u = 10;
    Value rValue(std::move(u));
    u = 20; // Just to make sure we don't have a ref to u.

    BOOST_CHECK(!rValue.isConst());
    BOOST_CHECK_EQUAL(rValue.refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(rValue.get<unsigned>(), 10u);

    // copy
    BOOST_CHECK(rValue.isCopiable<unsigned>());
    BOOST_CHECK_EQUAL(rValue.copy<unsigned>(), 10u);

    // l-ref
    BOOST_CHECK(rValue.isCastable<unsigned>());
    BOOST_CHECK_EQUAL(rValue.cast<unsigned>(), 10u);

    // const l-ref
    BOOST_CHECK( rValue.isCastable<const unsigned>());
    {
        const auto& value = rValue.cast<const unsigned>();
        BOOST_CHECK_NE(&value, &u);
        BOOST_CHECK_EQUAL(value, 10u);
    }

    // r-ref
    BOOST_CHECK(rValue.isMovable<unsigned>());
    BOOST_CHECK_EQUAL(rValue.move<unsigned>(), 10u);
    BOOST_CHECK(rValue.isVoid());
}


/******************************************************************************/
/* CONST RVALUE                                                               */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(constRValue)
{
    const unsigned i = 0;

    Value rValue(std::move(i));
    BOOST_CHECK(!rValue.isConst());
    BOOST_CHECK_EQUAL(rValue.refType(), RefType::LValue);
}


/******************************************************************************/
/* PARENT-CHILD                                                               */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(parent)
{
    test::Parent p(10, 20);
    Value value(p);

    BOOST_CHECK_EQUAL(&value.get<test::Interface>(), &p);
    BOOST_CHECK_EQUAL(&value.get<test::Parent>(), &p);
    BOOST_CHECK_THROW(value.get<test::Child>(), Error);

    BOOST_CHECK( value.isCastable<test::Interface>());
    BOOST_CHECK( value.isCastable<test::Parent>());
    BOOST_CHECK(!value.isCastable<test::Child>());
    BOOST_CHECK_EQUAL(&value.cast<test::Interface>(), &p);
    BOOST_CHECK_EQUAL(&value.cast<test::Parent>(), &p);
    BOOST_CHECK_THROW(value.cast<test::Child>(), Error);

    BOOST_CHECK(!value.isCopiable<test::Interface>());
    BOOST_CHECK( value.isCopiable<test::Parent>());
    BOOST_CHECK(!value.isCopiable<test::Child>());
    BOOST_CHECK_EQUAL(value.copy<test::Parent>(), p);
    BOOST_CHECK_THROW(value.copy<test::Child>(), Error);

    // This will correctly fail during compilation.
    // BOOST_CHECK_THROW(&value.copy<test::Interface>(), Error);


    BOOST_CHECK(!value.isMovable<test::Interface>());
    BOOST_CHECK( value.isMovable<test::Parent>());
    BOOST_CHECK(!value.isMovable<test::Child>());
    BOOST_CHECK_NO_THROW(value.move<test::Parent>());
    BOOST_CHECK_THROW(value.move<test::Child>(), Error);

    // This will correctly fail during compilation.
    // BOOST_CHECK_THROW(&value.move<test::Interface>(), Error);
}

BOOST_AUTO_TEST_CASE(child)
{
    test::Child c(10, 20);
    Value value(c);

    BOOST_CHECK_EQUAL(&value.get<test::Interface>(), &c);
    BOOST_CHECK_EQUAL(&value.get<test::Parent>(), &c);
    BOOST_CHECK_EQUAL(&value.get<test::Child>(), &c);

    BOOST_CHECK(value.isCastable<test::Interface>());
    BOOST_CHECK(value.isCastable<test::Parent>());
    BOOST_CHECK(value.isCastable<test::Child>());
    BOOST_CHECK_EQUAL(&value.cast<test::Interface>(), &c);
    BOOST_CHECK_EQUAL(&value.cast<test::Parent>(), &c);
    BOOST_CHECK_EQUAL(&value.cast<test::Child>(), &c);

    BOOST_CHECK(!value.isCopiable<test::Interface>());
    BOOST_CHECK( value.isCopiable<test::Parent>());
    BOOST_CHECK( value.isCopiable<test::Child>());
    BOOST_CHECK_EQUAL(value.copy<test::Parent>(), c);
    BOOST_CHECK_EQUAL(value.copy<test::Child>(), c);

    // This will correctly fail during compilation.
    // BOOST_CHECK_THROW(&value.copy<test::Interface>(), Error);

    BOOST_CHECK(!value.isMovable<test::Interface>());
    BOOST_CHECK( value.isMovable<test::Parent>());
    BOOST_CHECK( value.isMovable<test::Child>());
    BOOST_CHECK_NO_THROW(value.move<test::Parent>());
    value = Value(c); // previous test cleared it. Reload the value.
    BOOST_CHECK_NO_THROW(value.move<test::Child>());

    // This will correctly fail during compilation.
    // BOOST_CHECK_THROW(&value.move<test::Interface>(), Error);
}


/******************************************************************************/
/* CONVERTIBLE                                                                */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(convertible)
{
    test::Object o(10);
    test::Convertible c(o);
    Value value(c);

    BOOST_CHECK_EQUAL(&value.get<test::Convertible>(), &c);
    BOOST_CHECK_THROW(value.get<test::Parent>(), Error);

    BOOST_CHECK( value.isCastable<test::Convertible>());
    BOOST_CHECK(!value.isCastable<test::Parent>());
    BOOST_CHECK_EQUAL(&value.cast<test::Convertible>(), &c);
    BOOST_CHECK_THROW(value.cast<test::Parent>(), Error);

    BOOST_CHECK( value.isCopiable<test::Convertible>());
    BOOST_CHECK( value.isCopiable<test::Parent>());
    BOOST_CHECK_EQUAL(value.copy<test::Convertible>().value, o);
    BOOST_CHECK_EQUAL(value.copy<test::Parent>().value, o);

    BOOST_CHECK( value.isMovable<test::Convertible>());
    BOOST_CHECK( value.isMovable<test::Parent>());
    BOOST_CHECK_EQUAL(value.move<test::Convertible>().value, o);
    value = Value(test::Convertible(o)); // previous test cleared it.
    BOOST_CHECK_EQUAL(value.move<test::Parent>().value, o);
}


/******************************************************************************/
/* COMPILATION                                                                */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(copyMove)
{
    test::Object obj(10);
    Value vObj(obj);

    {
        Value copy = vObj.copy();
        BOOST_CHECK_EQUAL(copy.field<int>("value"), 10);
        BOOST_CHECK_EQUAL(vObj.field<int>("value"), 10);
        BOOST_CHECK_EQUAL(obj.value, 10);
    }

    {
        Value moved = vObj.move();
        BOOST_CHECK_EQUAL(moved.field<int>("value"), 10);
        BOOST_CHECK(vObj.isVoid());
        BOOST_CHECK_EQUAL(obj.value, 0);
    }
}
