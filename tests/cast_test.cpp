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


/******************************************************************************/
/* COPY                                                                       */
/******************************************************************************/

template<typename T>
Value lref(T&& val) { return Value(std::forward<T>(val)); }

template<typename T>
Value rref(T&& val) { return Value(std::forward<T>(val)).rvalue(); }

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
        Child child(70);
        auto result = cast<Parent>(lref(child));
        BOOST_CHECK_EQUAL(result.value, 70);
        BOOST_CHECK_NE(&result, &child);
    }

    BOOST_CHECK_EQUAL(cast<int>(rref(Convertible(80))), 80);
    BOOST_CHECK_EQUAL(cast<int>(lref(Convertible(90))), 90);

    {
        const Convertible cconv(100);
        BOOST_CHECK_EQUAL(cast<int>(lref(cconv)), 100);
    }
}
