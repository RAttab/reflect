/* value_function_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for ValueFunction.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

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
    auto valueFn = makeValueFunction(&foo);

    unsigned value = 0;

    Value ret = valueFn(Value(value), Value(10));
    BOOST_CHECK(ret.isVoid());
    BOOST_CHECK_EQUAL(value, 10);

    valueFn(Value(value), Value(10));
    BOOST_CHECK_EQUAL(value, 20);
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

    auto valueFn = makeValueFunction(lambda);

    Value ret = valueFn(Value(10u), Value(10u));
    BOOST_CHECK_EQUAL(value, 110);
    BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 110);
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
    auto valueFn = makeValueFunction(Functor());

    {
        Value ret = valueFn(Value(10u));
        BOOST_CHECK_EQUAL(ret.refType(), RefType::LValue);
        BOOST_CHECK(ret.castable<unsigned>());
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 10);
    }

    {
        Value ret = valueFn(Value(10u));
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 20);
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

namespace reflect {

template<>
struct Reflect<Foo>
{
    static constexpr const char* id = "Foo";
    static Type* create() { return new Type(id); }
};

} // namespace reflect

BOOST_AUTO_TEST_CASE(memberFn)
{
    Foo foo;
    auto valueFn = makeValueFunction(&Foo::bar);

    {
        Value ret = valueFn(Value(foo), Value(10u));
        BOOST_CHECK_EQUAL(ret.refType(), RefType::LValue);
        BOOST_CHECK(ret.castable<unsigned>());
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 10);
        BOOST_CHECK_EQUAL(foo.value, 10);
    }

    {
        Value ret = valueFn(Value(foo), Value(10u));
        BOOST_CHECK_EQUAL(ret.cast<unsigned>(), 20);
        BOOST_CHECK_EQUAL(foo.value, 20);
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
    auto valueFn = makeValueFunction(fn);

    {
        unsigned i = 0;
        Value ret = valueFn(Value(i), Value(10u));
        BOOST_CHECK(ret.isConst());
        BOOST_CHECK_EQUAL(ret.refType(), RefType::LValue);

        BOOST_CHECK_EQUAL(i, 10);
        BOOST_CHECK_EQUAL(&ret.cast<const unsigned>(), &i);
    }
}
