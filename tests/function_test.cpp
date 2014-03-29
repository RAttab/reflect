/* function_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for function.h
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


BOOST_AUTO_TEST_CASE(basics)
{
    using reflect::reflect;

    auto foo = [] (unsigned i, int& j, const int&, int&&) -> unsigned {
        return i + 1 * j;
    };
    Function fn("foo", foo);

    BOOST_CHECK_EQUAL(fn.returnType().type(), reflect<unsigned>());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Value);

    BOOST_CHECK_EQUAL(fn.arguments(), 4);

    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.argument(0).type(), reflect<unsigned>());
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Value);

    BOOST_CHECK(!fn.argument(1).isConst());
    BOOST_CHECK_EQUAL(fn.argument(1).type(), reflect<int>());
    BOOST_CHECK_EQUAL(fn.argument(1).refType(), RefType::LValue);

    BOOST_CHECK(fn.argument(2).isConst());
    BOOST_CHECK_EQUAL(fn.argument(2).type(), reflect<int>());
    BOOST_CHECK_EQUAL(fn.argument(2).refType(), RefType::LValue);

    BOOST_CHECK(!fn.argument(3).isConst());
    BOOST_CHECK_EQUAL(fn.argument(3).type(), reflect<int>());
    BOOST_CHECK_EQUAL(fn.argument(3).refType(), RefType::RValue);


    BOOST_CHECK(fn.test(fn));

    int i = 2;
    const unsigned expected = foo(1, i, 3, std::move(i));

    fn.call<void>(1u, i, 3, std::move(i));

    BOOST_CHECK_EQUAL(fn.call<unsigned>(1u, i, 3, std::move(i)), expected);

    Value ret = fn.call<Value>(1u, i, 3, std::move(i));
    BOOST_CHECK_EQUAL(ret.cast<unsigned>(), expected);

}

BOOST_AUTO_TEST_CASE(voids)
{
    using reflect::reflect;

    auto foo = [] {};
    Function fn("foo", foo);

    BOOST_CHECK_EQUAL(fn.returnType().type(), reflect<void>());
    BOOST_CHECK_EQUAL(fn.arguments(), 0);

    BOOST_CHECK(fn.test(fn));
    fn.call<void>();
}

BOOST_AUTO_TEST_CASE(value_test)
{
    Function fn("foo", [] (int i) -> int { return i; });

    BOOST_CHECK(!fn.returnType().isConst());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Value);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Value);

    BOOST_CHECK( fn.test<void(int)>());

    BOOST_CHECK( fn.test<int(int)>());

    BOOST_CHECK( fn.test<int(int&)>());
    BOOST_CHECK(!fn.test<int&(int)>());
    BOOST_CHECK( fn.test<int(const int&)>());
    BOOST_CHECK( fn.test<const int&(int)>());

    BOOST_CHECK( fn.test<int(int&&)>());
}

BOOST_AUTO_TEST_CASE(value_call)
{
    auto foo = [] (int i) -> int { return i + 1; };
    Function fn("foo", foo);

    BOOST_CHECK_EQUAL(fn.call<int>(10), foo(10));

    Value ret = fn.call<Value>(Value(10));
    BOOST_CHECK_EQUAL(ret.cast<int>(), foo(10));

    int i = 10;
    BOOST_CHECK_EQUAL(fn.call<int>(Value(i)), foo(i));

    const int c = 10;
    BOOST_CHECK_EQUAL(fn.call<int>(Value(c)), foo(c));
}

BOOST_AUTO_TEST_CASE(lValue_test)
{
    Function fn("foo", [] (int& i) -> int& { return i; });

    BOOST_CHECK(!fn.returnType().isConst());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::LValue);

    BOOST_CHECK( fn.test<void(int&)>());

    BOOST_CHECK(!fn.test<int&(int)>());
    BOOST_CHECK( fn.test<int(int&)>());

    BOOST_CHECK( fn.test<int&(int&)>());
    BOOST_CHECK(!fn.test<int&(const int&)>());
    BOOST_CHECK( fn.test<const int&(int&)>());

    BOOST_CHECK(!fn.test<int&(int&&)>());
}

BOOST_AUTO_TEST_CASE(lValue_call)
{
    auto foo = [] (int& i) -> int& {
        i += 1;
        return i;
    };
    Function fn("foo", foo);

}

BOOST_AUTO_TEST_CASE(constLValue_test)
{
    Function fn("foo", [] (const int& i) -> const int& { return i; });

    BOOST_CHECK(fn.returnType().isConst());
    BOOST_CHECK(fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::LValue);

    BOOST_CHECK( fn.test<void(const int&)>());

    BOOST_CHECK( fn.test<const int&(int)>());
    BOOST_CHECK( fn.test<int(const int&)>());

    BOOST_CHECK(!fn.test<int&(const int&)>());
    BOOST_CHECK( fn.test<const int&(int&)>());
    BOOST_CHECK( fn.test<const int&(const int&)>());

    BOOST_CHECK( fn.test<const int&(int&&)>());
}

BOOST_AUTO_TEST_CASE(constLValue_call)
{
    auto foo = [] (const int& i) -> const int& { return i; };
    Function fn("foo", foo);

}


BOOST_AUTO_TEST_CASE(rValue_test)
{
    Function fn("foo", [] (int&&) {});

    BOOST_CHECK(fn.returnType().isVoid());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::RValue);


    BOOST_CHECK( fn.test<void(int)>());
    BOOST_CHECK(!fn.test<void(int&)>());
    BOOST_CHECK( fn.test<void(int&&)>());
    BOOST_CHECK(!fn.test<void(const int&)>());
}

BOOST_AUTO_TEST_CASE(rValue_call)
{
    auto foo = [] (int&& i) { return i + 1; };
    Function fn("foo", foo);

}
