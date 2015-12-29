/* cast_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Dec 2015
   FreeBSD-style copyright and disclaimer apply
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "reflect.h"
#include "signals.h"
#include "test_types.h"

#include <boost/test/unit_test.hpp>

using namespace test;
using namespace reflect;

template<typename T>
Value lref(T&& val) { return Value(std::forward<T>(val)); }

template<typename T>
Value clref(T&& val) { return Value(std::forward<T>(val)).toConst(); }

template<typename T>
Value rref(T&& val) { return Value(std::forward<T>(val)).rvalue(); }


/******************************************************************************/
/* COPY                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_copy)
{
    BOOST_CHECK_EQUAL(cast<int>(rref(10)), 10);

    {
        int val = 20;
        auto result = cast<int>(lref(val));
        BOOST_CHECK_EQUAL(result, val);
        BOOST_CHECK_NE(&result, &val);
    }

    {
        const int cval = 30;
        auto result = cast<int>(lref(cval));
        BOOST_CHECK_EQUAL(result, cval);
        BOOST_CHECK_NE(&result, &cval);
    }

    BOOST_CHECK_EQUAL(cast<Object>(rref(Object(40))), Object(40));

    {
        Object obj(50);
        auto result = cast<Object>(lref(obj));
        BOOST_CHECK_EQUAL(result, obj);
        BOOST_CHECK_NE(&result, &obj);
    }

    {
        const Object cobj(60);
        auto result = cast<Object>(lref(cobj));
        BOOST_CHECK_EQUAL(result, cobj);
        BOOST_CHECK_NE(&result, &cobj);
    }

    check_signal(cast<NotCopiable>(rref(NotCopiable())));

    {
        NotCopiable noCopy;
        check_signal(cast<NotCopiable>(lref(noCopy)));
    }

    BOOST_CHECK_EQUAL(cast<Parent>(rref(Child(70))), Parent(70));

    {
        Child child(70);
        auto result = cast<Parent>(lref(child));
        BOOST_CHECK_EQUAL(result.value, 70);
        BOOST_CHECK_NE(&result, &child);
    }

    {
        const Child cchild(90);
        auto result = cast<Parent>(lref(cchild));
        BOOST_CHECK_EQUAL(result.value, 90);
        BOOST_CHECK_NE(&result, &cchild);
    }

    BOOST_CHECK_EQUAL(cast<int>(rref(Convertible(90))), 90);
    BOOST_CHECK_EQUAL(cast<int>(lref(Convertible(100))), 100);

    {
        const Convertible cconv(110);
        BOOST_CHECK_EQUAL(cast<int>(lref(cconv)), 110);
    }
}


/******************************************************************************/
/* lref                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_lref)
{

    int iVal = 10;
    check_signal(cast<int&>(rref(int(10))));
    BOOST_CHECK_EQUAL(&cast<int&>(lref(iVal)), &iVal);
    check_signal(cast<int&>(clref(iVal)));

    Object objVal(50);
    check_signal(cast<Object&>(rref(Object(40))));
    BOOST_CHECK_EQUAL(&cast<Object&>(lref(objVal)), &objVal);
    check_signal(cast<Object&>(clref(objVal)));

    NotCopiable ncVal;
    check_signal(cast<NotCopiable&>(rref(NotCopiable())));
    BOOST_CHECK_EQUAL(&cast<NotCopiable&>(lref(ncVal)), &ncVal);

    Child childVal(70);
    check_signal(cast<Parent&>(rref(Child(70))));
    BOOST_CHECK_EQUAL(&cast<Parent&>(lref(childVal)), &childVal);
    check_signal(cast<Parent&>(clref(childVal)));

    check_signal(cast<int&>(rref(Convertible(90))));
    check_signal(cast<int&>(lref(Convertible(100))));
    check_signal(cast<int&>(clref(Convertible(110))));
}


/******************************************************************************/
/* clref                                                                      */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_clref)
{
    int iVal = 10;
    BOOST_CHECK_EQUAL(cast<const int&>(rref(iVal)), iVal);
    BOOST_CHECK_EQUAL(&cast<const int&>(lref(iVal)), &iVal);
    BOOST_CHECK_EQUAL(&cast<const int&>(clref(iVal)), &iVal);

    Object objVal(40);
    BOOST_CHECK_EQUAL(cast<const Object&>(rref(objVal)), Object(40));
    BOOST_CHECK_EQUAL(&cast<const Object&>(lref(objVal)), &objVal);
    BOOST_CHECK_EQUAL(&cast<const Object&>(clref(objVal)), &objVal);

    NotCopiable ncVal;
    cast<const NotCopiable&>(rref(NotCopiable()));
    BOOST_CHECK_EQUAL(&cast<const NotCopiable&>(lref(ncVal)), &ncVal);
    BOOST_CHECK_EQUAL(&cast<const NotCopiable&>(clref(ncVal)), &ncVal);

    Child childVal(70);
    BOOST_CHECK_EQUAL(cast<const Parent&>(rref(Child(70))), Parent(70));
    BOOST_CHECK_EQUAL(&cast<const Parent&>(lref(childVal)), &childVal);
    BOOST_CHECK_EQUAL(&cast<const Parent&>(clref(childVal)), &childVal);

    Convertible convVal(90);
    BOOST_CHECK_EQUAL(cast<const int&>(rref(Convertible(90))), 90);
    BOOST_CHECK_EQUAL(cast<const int&>(lref(Convertible(100))), 100);
    BOOST_CHECK_EQUAL(cast<const int&>(clref(Convertible(110))), 110);
}


/******************************************************************************/
/* rref                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_rref)
{
    int iVal = 10;
    BOOST_CHECK_EQUAL(cast<int&&>(rref(int(10))), 10);
    check_signal(cast<int&&>(lref(iVal)));
    check_signal(cast<int&&>(clref(iVal)));

    Object objVal(40);
    BOOST_CHECK_EQUAL(cast<Object&&>(rref(Object(40))), Object(40));
    check_signal(cast<Object&&>(lref(objVal)));
    check_signal(cast<Object&&>(clref(objVal)));

    NotCopiable ncVal;
    cast<NotCopiable&&>(rref(NotCopiable()));
    check_signal(cast<NotCopiable&&>(lref(ncVal)));
    check_signal(cast<NotCopiable&&>(clref(ncVal)));

    Child childVal(70);
    BOOST_CHECK_EQUAL(cast<Parent&&>(rref(Child(70))), Parent(70));
    check_signal(cast<Parent&&>(lref(childVal)));
    check_signal(cast<Parent&&>(clref(childVal)));

    Convertible convVal(90);
    BOOST_CHECK_EQUAL(cast<int&&>(rref(Convertible(90))), 90);
    BOOST_CHECK_EQUAL(cast<int&&>(lref(Convertible(100))), 100);
    BOOST_CHECK_EQUAL(cast<int&&>(clref(Convertible(110))), 110);
}
