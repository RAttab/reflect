/* value_function_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for ValueFunction.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


/******************************************************************************/
/* FUNCTION                                                                   */
/******************************************************************************/

void foo(unsigned& value, int other)
{
    value += other;
}

BOOST_AUTO_TEST_CASE(fn)
{
    auto valueFn = makeValueFunctionSafe(&foo);

    unsigned value = 0;

    Value ret = (*valueFn)(Value(value), Value(10));
    BOOST_CHECK(ret.isVoid());
    BOOST_CHECK_EQUAL(value, 10u);

    (*valueFn)(Value(value), Value(10));
    BOOST_CHECK_EQUAL(value, 20u);
}


/******************************************************************************/
/* LAMBDA                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(lambda)
{
    unsigned value = 0;
    auto lambda = [&](unsigned x, unsigned&& z) {
        return value = x * x + std::move(z);
    };

    auto valueFn = makeValueFunctionSafe(lambda);

    Value ret = (*valueFn)(Value(10u), Value(10u));
    BOOST_CHECK_EQUAL(value, 110u);
    BOOST_CHECK_EQUAL(ret.get<unsigned>(), 110u);
}


/******************************************************************************/
/* FUNCTOR                                                                    */
/******************************************************************************/

struct Functor
{
    Functor() : value(0) {}

    unsigned& operator() (unsigned i)
    {
        value += i;
        return value;
    }

private:
    unsigned value;
};


BOOST_AUTO_TEST_CASE(functor)
{
    auto valueFn = makeValueFunctionSafe(Functor());

    {
        Value ret = (*valueFn)(Value(10u));
        BOOST_CHECK_EQUAL(ret.refType(), RefType::LValue);
        BOOST_CHECK(ret.isCastable<unsigned>());
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 10u);
    }

    {
        Value ret = (*valueFn)(Value(10u));
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 20u);
    }
}


/******************************************************************************/
/* MEMBER FUNCTION                                                            */
/******************************************************************************/

struct Foo
{
    Foo() : value(0) {}

    unsigned& bar(unsigned i)
    {
        value += i;
        return value;
    }

    unsigned value;
};

reflectType(Foo) {}

BOOST_AUTO_TEST_CASE(memberFn)
{
    Foo foo;
    auto valueFn = makeValueFunctionSafe(&Foo::bar);

    {
        Value ret = (*valueFn)(Value(foo), Value(10u));
        BOOST_CHECK_EQUAL(ret.refType(), RefType::LValue);
        BOOST_CHECK(ret.isCastable<unsigned>());
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 10u);
        BOOST_CHECK_EQUAL(foo.value, 10u);
    }

    {
        Value ret = (*valueFn)(Value(foo), Value(10u));
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 20u);
        BOOST_CHECK_EQUAL(foo.value, 20u);
    }
}


/******************************************************************************/
/* CONSTNESS                                                                  */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(constness)
{
    auto fn = [] (unsigned& i, const unsigned& j) -> const unsigned& {
        i += j;
        return i;
    };
    auto valueFn = makeValueFunctionSafe(fn);

    {
        unsigned i = 0;
        Value ret = (*valueFn)(Value(i), Value(10u));
        BOOST_CHECK(ret.isConst());
        BOOST_CHECK_EQUAL(ret.refType(), RefType::LValue);

        BOOST_CHECK_EQUAL(i, 10u);
        BOOST_CHECK_EQUAL(&ret.cast<const unsigned>(), &i);
    }
}
