/* function_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for function.h
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

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
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Copy);

    BOOST_CHECK_EQUAL(fn.arguments(), 4);

    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.argument(0).type(), reflect<unsigned>());
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Copy);

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
    BOOST_CHECK_EQUAL(ret.get<unsigned>(), expected);
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

BOOST_AUTO_TEST_CASE(copy_test)
{
    Function fn("foo", [] (int i) -> int { return i; });

    BOOST_CHECK(!fn.returnType().isConst());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Copy);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Copy);

    BOOST_CHECK( fn.test<void(int)>());

    BOOST_CHECK( fn.test<int(int)>());

    BOOST_CHECK( fn.test<int(int&)>());
    BOOST_CHECK(!fn.test<int&(int)>());
    BOOST_CHECK( fn.test<int(const int&)>());
    BOOST_CHECK( fn.test<const int&(int)>());

    BOOST_CHECK( fn.test<int(int&&)>());
}

BOOST_AUTO_TEST_CASE(copy_call)
{
    auto foo = [] (int i) -> int { return i + 1; };
    Function fn("foo", foo);

    int i = 10;        Value lValue(i);
    const auto& c = i; Value constLValue(c);
    int r = i;         Value rValue(std::move(r));

    // copy
    BOOST_CHECK_EQUAL(fn.call<int>(10), foo(10));

    // l-ref
    BOOST_CHECK_EQUAL(fn.call<int>(i), foo(i));
    BOOST_CHECK_EQUAL(fn.call<int>(lValue), foo(i));
    BOOST_CHECK_THROW(fn.call<int&>(10), ReflectError);

    // const l-ref
    BOOST_CHECK_EQUAL(fn.call<int>(c), foo(c));
    BOOST_CHECK_EQUAL(fn.call<int>(constLValue), foo(c));
    fn.call<const int&>(10); // Valid but returns garbage.

    // r-ref
    BOOST_CHECK_EQUAL(fn.call<int>(std::move(r)), foo(10));
    BOOST_CHECK_EQUAL(fn.call<int>(Value(std::move(r))), foo(10));
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

    int i = 10;        Value lValue(i);
    const auto& c = i; Value constLValue(c);
    int r = i;

    // copy
    BOOST_CHECK_THROW(fn.call<int&>(10), ReflectError);
    BOOST_CHECK_EQUAL(fn.call<int>(i), foo(i));
    BOOST_CHECK_EQUAL(fn.call<int>(lValue), foo(i));

    // l-ref
    BOOST_CHECK_EQUAL(&fn.call<int&>(i), &i);
    BOOST_CHECK_EQUAL(&fn.call<int&>(lValue), &i);

    // const l-ref
    BOOST_CHECK_THROW(fn.call<int&>(c), ReflectError);
    BOOST_CHECK_THROW(fn.call<int&>(constLValue), ReflectError);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(i), &c);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(lValue), &c);

    // r-ref
    BOOST_CHECK_THROW(fn.call<int&>(std::move(r)), ReflectError);
    BOOST_CHECK_THROW(fn.call<int&>(Value(std::move(r))), ReflectError);
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

    int i = 10;        Value lValue(i);
    const auto& c = i; Value constLValue(c);
    int r = i;

    // copy
    BOOST_CHECK_EQUAL(fn.call<int>(c), foo(c));
    BOOST_CHECK_EQUAL(fn.call<int>(constLValue), foo(c));

    // l-ref
    BOOST_CHECK_EQUAL(&fn.call<const int&>(i), &i);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(lValue), &i);
    BOOST_CHECK_THROW(fn.call<int&>(c), ReflectError);
    BOOST_CHECK_THROW(fn.call<int&>(constLValue), ReflectError);

    // const l-ref
    BOOST_CHECK_EQUAL(&fn.call<const int&>(c), &c);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(constLValue), &c);

    // r-ref
    // We return a value to a temporary here so it's not safe to check it.
    fn.call<const int&>(std::move(r));
    fn.call<const int&>(Value(std::move(r)));
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

    int i = 10;        Value lValue(i);
    const auto& c = i; Value constLValue(c);

    // copy
    BOOST_CHECK_EQUAL(fn.call<int>(10), foo(10));

    // l-ref
    BOOST_CHECK_THROW(fn.call<int>(i), ReflectError);
    BOOST_CHECK_EQUAL(fn.call<int>(lValue), foo(10)); // \todo Fix this edge case.
    {
        int r = i;
        BOOST_CHECK_THROW(fn.call<int&>(std::move(r)), ReflectError);
    }
    {
        int r = i;
        BOOST_CHECK_THROW(fn.call<int&>(Value(std::move(r))), ReflectError);
    }

    // const l-ref
    BOOST_CHECK_THROW(fn.call<int>(c), ReflectError);
    BOOST_CHECK_THROW(fn.call<int>(constLValue), ReflectError);
    {
        int r = i;
        fn.call<const int&>(std::move(r)); // Valid but returns garbage
    }
    {
        int r = i; ;
        fn.call<const int&>(Value(std::move(r))), foo(10); // Valid but returns garbage.
    }

    // r-ref
    {
        int r = i;
        BOOST_CHECK_EQUAL(fn.call<int>(std::move(r)), foo(10));
    }
    {
        int r = i;
        BOOST_CHECK_EQUAL(fn.call<int>(Value(std::move(r))), foo(10));
    }
}
