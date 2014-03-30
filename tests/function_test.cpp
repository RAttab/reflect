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


BOOST_AUTO_TEST_CASE(void_test)
{
    using reflect::reflect;

    Function fn("foo", [] {});

    BOOST_CHECK_EQUAL(fn.returnType().type(), reflect<void>());
    BOOST_CHECK_EQUAL(fn.arguments(), 0);

    // void
    BOOST_CHECK( fn.test<void(void)>());

    // copy
    BOOST_CHECK(!fn.test<void(int)>());
    BOOST_CHECK(!fn.test<int(void)>());

    // l-ref
    BOOST_CHECK(!fn.test<void(int&)>());
    BOOST_CHECK(!fn.test<int&(void)>());

    // const l-ref
    BOOST_CHECK(!fn.test<void(const int&)>());
    BOOST_CHECK(!fn.test<const int&(void)>());

    // r-ref
    BOOST_CHECK(!fn.test<void(int&&)>());
}

BOOST_AUTO_TEST_CASE(void_call)
{
    Function fn("foo", [] {});

    // void
    fn.call<void>();

    // copy
    BOOST_CHECK_THROW(fn.call<void>(10), ReflectError);
    BOOST_CHECK_THROW(fn.call<int>(), ReflectError);

    // l-ref
    int i = 0; Value lValue(i);
    BOOST_CHECK_THROW(fn.call<void>(i), ReflectError);
    BOOST_CHECK_THROW(fn.call<void>(lValue), ReflectError);
    BOOST_CHECK_THROW(fn.call<int&>(), ReflectError);

    // const l-ref
    const int& c = i; Value constLValue(c);
    BOOST_CHECK_THROW(fn.call<void>(c), ReflectError);
    BOOST_CHECK_THROW(fn.call<void>(constLValue), ReflectError);
    BOOST_CHECK_THROW(fn.call<const int&>(), ReflectError);

    // r-ref
    int r = i;
    BOOST_CHECK_THROW(fn.call<void>(std::move(r)), ReflectError);
    BOOST_CHECK_THROW(fn.call<void>(Value(std::move(r))), ReflectError);
}


BOOST_AUTO_TEST_CASE(copy_test)
{
    Function fn("foo", [] (int i) -> int { return i; });

    BOOST_CHECK(!fn.returnType().isConst());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Copy);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Copy);

    // void
    BOOST_CHECK( fn.test<void(int)>());
    BOOST_CHECK(!fn.test<int()>());

    // copy
    BOOST_CHECK( fn.test<int(int)>());

    // l-ref
    BOOST_CHECK( fn.test<int(int&)>());
    BOOST_CHECK(!fn.test<int&(int)>());

    // const l-ref
    BOOST_CHECK( fn.test<int(const int&)>());
    BOOST_CHECK(!fn.test<const int&(int)>());

    // r-ref
    BOOST_CHECK( fn.test<int(int&&)>());
}

BOOST_AUTO_TEST_CASE(copy_call)
{
    auto foo = [] (int i) -> int { return i + 1; };
    Function fn("foo", foo);


    // void
    fn.call<void>(10);
    BOOST_CHECK_THROW(fn.call<int>(), ReflectError);
    BOOST_CHECK_THROW(fn.call<int>(Value()), ReflectError);

    // copy
    BOOST_CHECK_EQUAL(fn.call<int>(10), foo(10));

    // l-ref
    int i = 10; Value lValue(i);
    BOOST_CHECK_EQUAL(fn.call<int>(i), foo(i));
    BOOST_CHECK_EQUAL(fn.call<int>(lValue), foo(i));
    BOOST_CHECK_THROW(fn.call<int&>(10), ReflectError);

    // const l-ref
    const auto& c = i; Value constLValue(c);
    BOOST_CHECK_EQUAL(fn.call<int>(c), foo(c));
    BOOST_CHECK_EQUAL(fn.call<int>(constLValue), foo(c));
    BOOST_CHECK_THROW(fn.call<const int&>(10), ReflectError);

    // r-ref
    int r = i;
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

    // void
    BOOST_CHECK( fn.test<void(int&)>());
    BOOST_CHECK(!fn.test<int&()>());

    // copy
    BOOST_CHECK(!fn.test<int&(int)>());
    BOOST_CHECK( fn.test<int(int&)>());

    // l-ref
    BOOST_CHECK( fn.test<int&(int&)>());

    // const l-ref
    BOOST_CHECK(!fn.test<int&(const int&)>());
    BOOST_CHECK( fn.test<const int&(int&)>());

    // r-ref
    BOOST_CHECK(!fn.test<int&(int&&)>());
}

BOOST_AUTO_TEST_CASE(lValue_call)
{
    auto foo = [] (int& i) -> int& {
        i += 1;
        return i;
    };
    Function fn("foo", foo);

    int i = 10; Value lValue(i);

    // void
    fn.call<void>(lValue);
    BOOST_CHECK_THROW(fn.call<int>(), ReflectError);
    BOOST_CHECK_THROW(fn.call<int>(Value()), ReflectError);

    // copy
    BOOST_CHECK_THROW(fn.call<int&>(10), ReflectError);
    BOOST_CHECK_EQUAL(fn.call<int>(i), foo(i));
    BOOST_CHECK_EQUAL(fn.call<int>(lValue), foo(i));

    // l-ref
    BOOST_CHECK_EQUAL(&fn.call<int&>(i), &i);
    BOOST_CHECK_EQUAL(&fn.call<int&>(lValue), &i);

    // const l-ref
    const auto& c = i; Value constLValue(c);
    BOOST_CHECK_THROW(fn.call<int&>(c), ReflectError);
    BOOST_CHECK_THROW(fn.call<int&>(constLValue), ReflectError);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(i), &c);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(lValue), &c);

    // r-ref
    int r = i;
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

    // void
    BOOST_CHECK( fn.test<void(const int&)>());
    BOOST_CHECK(!fn.test<const int&()>());

    // copy
    BOOST_CHECK( fn.test<const int&(int)>());
    BOOST_CHECK( fn.test<int(const int&)>());

    // l-ref
    BOOST_CHECK(!fn.test<int&(const int&)>());
    BOOST_CHECK( fn.test<const int&(int&)>());

    // const l-ref
    BOOST_CHECK( fn.test<const int&(const int&)>());

    // r-ref
    BOOST_CHECK( fn.test<const int&(int&&)>());
}

BOOST_AUTO_TEST_CASE(constLValue_call)
{
    auto foo = [] (const int& i) -> const int& { return i; };
    Function fn("foo", foo);

    int i = 10;
    const auto& c = i;
    Value constLValue(c);

    // void
    fn.call<void>(constLValue);
    BOOST_CHECK_THROW(fn.call<const int&>(), ReflectError);
    BOOST_CHECK_THROW(fn.call<const int&>(Value()), ReflectError);

    // copy
    BOOST_CHECK_EQUAL(fn.call<int>(c), foo(c));
    BOOST_CHECK_EQUAL(fn.call<int>(constLValue), foo(c));

    // l-ref
    Value lValue(i);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(i), &i);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(lValue), &i);
    BOOST_CHECK_THROW(fn.call<int&>(c), ReflectError);
    BOOST_CHECK_THROW(fn.call<int&>(constLValue), ReflectError);

    // const l-ref
    BOOST_CHECK_EQUAL(&fn.call<const int&>(c), &c);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(constLValue), &c);

    // r-ref
    int r = i;
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

    // void
    BOOST_CHECK(!fn.test<void()>());

    // copy
    BOOST_CHECK( fn.test<void(int)>());

    // l-ref
    BOOST_CHECK(!fn.test<void(int&)>());

    // const l-ref
    BOOST_CHECK(!fn.test<void(const int&)>());

    // r-ref
    BOOST_CHECK( fn.test<void(int&&)>());
}

BOOST_AUTO_TEST_CASE(rValue_call)
{
    auto foo = [] (int&& i) { return i + 1; };
    Function fn("foo", foo);


    // void
    {
        int r = 10;
        fn.call<void>(std::move(r));
    }
    BOOST_CHECK_THROW(fn.call<const int&>(), ReflectError);
    BOOST_CHECK_THROW(fn.call<const int&>(Value()), ReflectError);

    // copy
    BOOST_CHECK_EQUAL(fn.call<int>(10), foo(10));

    // l-ref
    int i = 10; Value lValue(i);
    BOOST_CHECK_THROW(fn.call<int>(i), ReflectError);
    BOOST_CHECK_THROW(fn.call<int>(lValue), ReflectError);
    {
        int r = i;
        BOOST_CHECK_THROW(fn.call<int&>(std::move(r)), ReflectError);
    }
    {
        int r = i;
        BOOST_CHECK_THROW(fn.call<int&>(Value(std::move(r))), ReflectError);
    }

    // const l-ref
    const auto& c = i; Value constLValue(c);
    BOOST_CHECK_THROW(fn.call<int>(c), ReflectError);
    BOOST_CHECK_THROW(fn.call<int>(constLValue), ReflectError);
    {
        int r = i;
        BOOST_CHECK_THROW(fn.call<const int&>(std::move(r)), ReflectError);
    }
    {
        int r = i;
        BOOST_CHECK_THROW(fn.call<const int&>(Value(std::move(r))), ReflectError);
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
