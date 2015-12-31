/* function_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 28 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for function.h
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "tests.h"
#include "reflect.h"
#include "test_types.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace reflect;


/******************************************************************************/
/* BASICS                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(basics)
{
    auto foo = [] (unsigned i, int& j, const int&, int&&) -> unsigned {
        return i + 1 * j;
    };
    Function fn("foo", foo);

    BOOST_CHECK_EQUAL(fn.returnType().type(), type<unsigned>());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Copy);

    BOOST_CHECK_EQUAL(fn.arguments(), 4u);

    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.argument(0).type(), type<unsigned>());
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Copy);

    BOOST_CHECK(!fn.argument(1).isConst());
    BOOST_CHECK_EQUAL(fn.argument(1).type(), type<int>());
    BOOST_CHECK_EQUAL(fn.argument(1).refType(), RefType::LValue);

    BOOST_CHECK(fn.argument(2).isConst());
    BOOST_CHECK_EQUAL(fn.argument(2).type(), type<int>());
    BOOST_CHECK_EQUAL(fn.argument(2).refType(), RefType::LValue);

    BOOST_CHECK(!fn.argument(3).isConst());
    BOOST_CHECK_EQUAL(fn.argument(3).type(), type<int>());
    BOOST_CHECK_EQUAL(fn.argument(3).refType(), RefType::RValue);


    BOOST_CHECK_EQUAL(fn.test(fn), Match::Exact);

    int i = 2;
    const unsigned expected = foo(1, i, 3, std::move(i));

    fn.call<void>(1u, i, 3, std::move(i));

    BOOST_CHECK_EQUAL(fn.call<unsigned>(1u, i, 3, std::move(i)), expected);

    Value ret = fn.call<Value>(1u, i, 3, std::move(i));
    BOOST_CHECK_EQUAL(ret.get<unsigned>(), expected);
}


/******************************************************************************/
/* VOID                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(void_test)
{
    Function fn("foo", [] {});

    BOOST_CHECK_EQUAL(fn.returnType().type(), type<void>());
    BOOST_CHECK_EQUAL(fn.arguments(), 0u);

    // void
    BOOST_CHECK_EQUAL(fn.test<void(void)>(), Match::Exact);

    // copy
    BOOST_CHECK_EQUAL(fn.test<void(int)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<void(Value)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<int(void)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<Value(void)>(), Match::Exact);

    // l-ref
    BOOST_CHECK_EQUAL(fn.test<void(int&)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<void(Value&)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<int&(void)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<Value&(void)>(), Match::None);

    // const l-ref
    BOOST_CHECK_EQUAL(fn.test<void(const int&)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<void(const Value&)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<const int&(void)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<const Value&(void)>(), Match::None);

    // r-ref
    BOOST_CHECK_EQUAL(fn.test<void(int&&)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<void(Value&&)>(), Match::None);
}

BOOST_AUTO_TEST_CASE(void_call)
{
    Function fn("foo", [] {});

    // void
    fn.call<void>();

    // copy
    CHECK_ERROR(fn.call<void>(10));
    CHECK_ERROR(fn.call<int>());
    BOOST_CHECK(fn.call<Value>().isVoid());

    // l-ref
    int i = 0; Value lValue(i);
    CHECK_ERROR(fn.call<void>(i));
    CHECK_ERROR(fn.call<void>(lValue));
    CHECK_ERROR(fn.call<int&>());
    CHECK_ERROR(fn.call<Value&>());

    // const l-ref
    const int& c = i; Value constLValue(c);
    CHECK_ERROR(fn.call<void>(c));
    CHECK_ERROR(fn.call<void>(constLValue));
    CHECK_ERROR(fn.call<const int&>());
    CHECK_ERROR(fn.call<const Value&>());

    // r-ref
    int r = i;
    CHECK_ERROR(fn.call<void>(std::move(r)));
    CHECK_ERROR(fn.call<void>(Value(r).rvalue()));
}


/******************************************************************************/
/* COPY                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(copy_test)
{
    Function fn("foo", [] (int i) -> int { return i; });

    BOOST_CHECK(!fn.returnType().isConst());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::Copy);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::Copy);

    // void
    BOOST_CHECK_EQUAL(fn.test<void(int)>(), Match::Exact);
    BOOST_CHECK_EQUAL(fn.test<int()>(), Match::None);

    // copy
    BOOST_CHECK_EQUAL(fn.test<int(int)>(), Match::Exact);

    // l-ref
    BOOST_CHECK_EQUAL(fn.test<int(int&)>(), Match::Partial);
    BOOST_CHECK_EQUAL(fn.test<int&(int)>(), Match::None);

    // const l-ref
    BOOST_CHECK_EQUAL(fn.test<int(const int&)>(), Match::Partial);
    BOOST_CHECK_EQUAL(fn.test<const int&(int)>(), Match::None);

    // r-ref
    BOOST_CHECK_EQUAL(fn.test<int(int&&)>(), Match::Exact);
}

BOOST_AUTO_TEST_CASE(copy_call)
{
    auto foo = [] (int i) -> int { return i + 1; };
    Function fn("foo", foo);


    // void
    fn.call<void>(10);
    CHECK_ERROR(fn.call<int>());
    CHECK_ERROR(fn.call<int>(Value()));

    // copy
    BOOST_CHECK_EQUAL(fn.call<int>(10), foo(10));

    // l-ref
    int i = 10; Value lValue(i);
    BOOST_CHECK_EQUAL(fn.call<int>(i), foo(i));
    BOOST_CHECK_EQUAL(fn.call<int>(lValue), foo(i));
    CHECK_ERROR(fn.call<int&>(10));

    // const l-ref
    const auto& c = i; Value constLValue(c);
    BOOST_CHECK_EQUAL(fn.call<int>(c), foo(c));
    BOOST_CHECK_EQUAL(fn.call<int>(constLValue), foo(c));
    CHECK_ERROR(fn.call<const int&>(10));

    // r-ref
    int r = i;
    BOOST_CHECK_EQUAL(fn.call<int>(std::move(r)), foo(10));
    BOOST_CHECK_EQUAL(fn.call<int>(Value(r).rvalue()), foo(10));
}


/******************************************************************************/
/* LVALUE                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(lValue_test)
{
    Function fn("foo", [] (int& i) -> int& { return i; });

    BOOST_CHECK(!fn.returnType().isConst());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::LValue);

    // void
    BOOST_CHECK_EQUAL(fn.test<void(int&)>(), Match::Exact);
    BOOST_CHECK_EQUAL(fn.test<int&()>(), Match::None);

    // copy
    BOOST_CHECK_EQUAL(fn.test<int&(int)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<int(int&)>(), Match::Exact);

    // l-ref
    BOOST_CHECK_EQUAL(fn.test<int&(int&)>(), Match::Exact);

    // const l-ref
    BOOST_CHECK_EQUAL(fn.test<int&(const int&)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<const int&(int&)>(), Match::Exact);

    // r-ref
    BOOST_CHECK_EQUAL(fn.test<int&(int&&)>(), Match::None);
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
    CHECK_ERROR(fn.call<int>());
    CHECK_ERROR(fn.call<int>(Value()));

    // copy
    CHECK_ERROR(fn.call<int&>(10));
    BOOST_CHECK_EQUAL(fn.call<int>(i), foo(i));
    BOOST_CHECK_EQUAL(fn.call<int>(lValue), foo(i));

    // l-ref
    BOOST_CHECK_EQUAL(&fn.call<int&>(i), &i);
    BOOST_CHECK_EQUAL(&fn.call<int&>(lValue), &i);

    // const l-ref
    const auto& c = i; Value constLValue(c);
    CHECK_ERROR(fn.call<int&>(c));
    CHECK_ERROR(fn.call<int&>(constLValue));
    BOOST_CHECK_EQUAL(&fn.call<const int&>(i), &c);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(lValue), &c);

    // r-ref
    {
        int r = i;
        CHECK_ERROR(fn.call<int&>(std::move(r)));
    }
    {
        int r = i;
        CHECK_ERROR(fn.call<int&>(Value(r).rvalue()));
    }
}


/******************************************************************************/
/* CONST LVALUE                                                               */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(constLValue_test)
{
    Function fn("foo", [] (const int& i) -> const int& { return i; });

    BOOST_CHECK(fn.returnType().isConst());
    BOOST_CHECK(fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.returnType().refType(), RefType::LValue);
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::LValue);

    // void
    BOOST_CHECK_EQUAL(fn.test<void(const int&)>(), Match::Exact);
    BOOST_CHECK_EQUAL(fn.test<const int&()>(), Match::None);

    // copy
    BOOST_CHECK_EQUAL(fn.test<const int&(int)>(), Match::Partial);
    BOOST_CHECK_EQUAL(fn.test<int(const int&)>(), Match::Exact);

    // l-ref
    BOOST_CHECK_EQUAL(fn.test<int&(const int&)>(), Match::None);
    BOOST_CHECK_EQUAL(fn.test<const int&(int&)>(), Match::Partial);

    // const l-ref
    BOOST_CHECK_EQUAL(fn.test<const int&(const int&)>(), Match::Exact);

    // r-ref
    BOOST_CHECK_EQUAL(fn.test<const int&(int&&)>(), Match::Partial);
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
    CHECK_ERROR(fn.call<const int&>());
    CHECK_ERROR(fn.call<const int&>(Value()));

    // copy
    fn.call<const int&>(i);
    fn.call<const int&>(i);
    BOOST_CHECK_EQUAL(fn.call<int>(c), foo(c));
    BOOST_CHECK_EQUAL(fn.call<int>(constLValue), foo(c));

    // l-ref
    Value lValue(i);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(i), &i);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(lValue), &i);
    CHECK_ERROR(fn.call<int&>(c));
    CHECK_ERROR(fn.call<int&>(constLValue));

    // const l-ref
    BOOST_CHECK_EQUAL(&fn.call<const int&>(c), &c);
    BOOST_CHECK_EQUAL(&fn.call<const int&>(constLValue), &c);

    // r-ref
    int r = i;
    // We return a value to a temporary here so it's not safe to check it.
    fn.call<const int&>(std::move(r));
    fn.call<const int&>(Value(r).rvalue());
}


/******************************************************************************/
/* RVALUE                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(rValue_test)
{
    Function fn("foo", [] (int&&) {});

    BOOST_CHECK(fn.returnType().isVoid());
    BOOST_CHECK(!fn.argument(0).isConst());
    BOOST_CHECK_EQUAL(fn.argument(0).refType(), RefType::RValue);

    // void
    BOOST_CHECK_EQUAL(fn.test<void()>(), Match::None);

    // copy
    BOOST_CHECK_EQUAL(fn.test<void(int)>(), Match::None);

    // l-ref
    BOOST_CHECK_EQUAL(fn.test<void(int&)>(), Match::None);

    // const l-ref
    BOOST_CHECK_EQUAL(fn.test<void(const int&)>(), Match::None);

    // r-ref
    BOOST_CHECK_EQUAL(fn.test<void(int&&)>(), Match::Exact);
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
    CHECK_ERROR(fn.call<const int&>());
    CHECK_ERROR(fn.call<const int&>(Value()));

    // copy
    int v = 10;
    CHECK_ERROR(fn.call<int>(v));
    BOOST_CHECK_EQUAL(fn.call<int>(10), foo(10));

    // l-ref
    int i = 10; Value lValue(i);
    CHECK_ERROR(fn.call<int>(i));
    CHECK_ERROR(fn.call<int>(lValue));
    {
        int r = i;
        CHECK_ERROR(fn.call<int&>(std::move(r)));
    }
    {
        int r = i;
        CHECK_ERROR(fn.call<int&>(Value(r).rvalue()));
    }

    // const l-ref
    const auto& c = i; Value constLValue(c);
    CHECK_ERROR(fn.call<int>(c));
    CHECK_ERROR(fn.call<int>(constLValue));
    {
        int r = i;
        CHECK_ERROR(fn.call<const int&>(std::move(r)));
    }
    {
        int r = i;
        CHECK_ERROR(fn.call<const int&>(Value(r).rvalue()));
    }

    // r-ref
    {
        int r = i;
        BOOST_CHECK_EQUAL(fn.call<int>(std::move(r)), foo(10));
    }
    {
        int r = i;
        fn.call<int>(Value(r).rvalue());
    }
}


/******************************************************************************/
/* CHILD PARENT                                                               */
/******************************************************************************/

// The test cases for this are less complicated so stuff everything in one test.
BOOST_AUTO_TEST_CASE(childParent_test)
{
    using test::Parent;
    using test::Child;

    Child child(10, 0);

    auto doCopy = [] (Parent p) { return Child(p.value + 1, 0); };
    Function copyFn("doCopy", doCopy);

    auto doLRef = [&] (Parent&) -> Child& { return child; };
    Function lrefFn("doLRef", doLRef);

    auto doConstLRef = [&] (const Parent&) -> const Child& { return child; };
    Function clrefFn("doConstLRef", doConstLRef);

    auto doRRef = [] (Parent&& p) { return p.value.value + 1; };
    Function rrefFn("doRRef", doRRef);

    BOOST_CHECK_EQUAL(copyFn.test<void(Child)>(),           Match::Exact);
    BOOST_CHECK_EQUAL(copyFn.test<void(Child&)>(),          Match::Partial);
    BOOST_CHECK_EQUAL(copyFn.test<void(const Child&)>(),    Match::Partial);
    BOOST_CHECK_EQUAL(copyFn.test<void(Child&&)>(),         Match::Exact);
    BOOST_CHECK_EQUAL(copyFn.test<Parent(Parent)>(),        Match::Exact);
    BOOST_CHECK_EQUAL(copyFn.test<Parent&(Parent)>(),       Match::None);
    BOOST_CHECK_EQUAL(copyFn.test<const Parent&(Parent)>(), Match::None);

    BOOST_CHECK_EQUAL(lrefFn.test<void(Child)>(),            Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<void(Child&)>(),           Match::Exact);
    BOOST_CHECK_EQUAL(lrefFn.test<void(const Child&)>(),     Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<void(Child&&)>(),          Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<Parent(Parent&)>(),        Match::Exact);
    BOOST_CHECK_EQUAL(lrefFn.test<Parent&(Parent&)>(),       Match::Exact);
    BOOST_CHECK_EQUAL(lrefFn.test<const Parent&(Parent&)>(), Match::Exact);

    BOOST_CHECK_EQUAL(clrefFn.test<void(Child)>(),            Match::Partial);
    BOOST_CHECK_EQUAL(clrefFn.test<void(Child&)>(),           Match::Partial);
    BOOST_CHECK_EQUAL(clrefFn.test<void(const Child&)>(),     Match::Exact);
    BOOST_CHECK_EQUAL(clrefFn.test<void(Child&&)>(),          Match::Partial);
    BOOST_CHECK_EQUAL(clrefFn.test<Parent(const Parent&)>(),  Match::Exact);
    BOOST_CHECK_EQUAL(clrefFn.test<Parent&(const Parent&)>(), Match::None);
    BOOST_CHECK_EQUAL(clrefFn.test<const Parent&(const Parent&)>(), Match::Exact);

    BOOST_CHECK_EQUAL(rrefFn.test<void(Child)>(),        Match::None);
    BOOST_CHECK_EQUAL(rrefFn.test<void(Child&)>(),       Match::None);
    BOOST_CHECK_EQUAL(rrefFn.test<void(const Child&)>(), Match::None);
    BOOST_CHECK_EQUAL(rrefFn.test<void(Child&&)>(),      Match::Exact);
}


BOOST_AUTO_TEST_CASE(childParent_call)
{
    using test::Parent;
    using test::Child;
    using test::Object;

    Child child(10, 0);

    auto doCopy = [] (Parent p) { return Child(p.value + 1, 0); };
    Function copyFn("doCopy", doCopy);

    auto doLRef = [&] (Parent&) -> Child& { return child; };
    Function lrefFn("doLRef", doLRef);

    auto doConstLRef = [&] (const Parent&) -> const Child& { return child; };
    Function clrefFn("doConstLRef", doConstLRef);

    auto doRRef = [] (Parent&& p) { return p.value + 1; };
    Function rrefFn("doRRef", doRRef);

    Parent copy(10, 0);
    Parent& lref = copy;
    const Parent& clref = copy;
    (void) copy; (void) lref; (void) clref;

    Child childCopy(10, 0);
    auto& childLRef = childCopy;
    const auto& childConstLRef = childCopy;
    (void) childCopy; (void) childLRef; (void) childConstLRef;

    BOOST_CHECK_EQUAL(copyFn.call<Child>(childLRef), doCopy(childLRef));
    BOOST_CHECK_EQUAL(copyFn.call<Child>(childConstLRef), doCopy(childConstLRef));
    BOOST_CHECK_EQUAL(copyFn.call<Child>(Child(10, 0)), doCopy(Child(10, 0)));
    BOOST_CHECK_EQUAL(copyFn.call<Parent>(copy), (Parent) doCopy(copy));
    CHECK_ERROR(copyFn.call<Parent&>(copy));
    CHECK_ERROR(copyFn.call<const Parent&>(copy));

    BOOST_CHECK_EQUAL(lrefFn.call<Child&>(childLRef), doLRef(lref));
    CHECK_ERROR(lrefFn.call<Child&>(childConstLRef));
    CHECK_ERROR(lrefFn.call<Child&>(Child(10, 0)));
    BOOST_CHECK_EQUAL(lrefFn.call<Parent>(lref), (Parent) doLRef(lref));
    BOOST_CHECK_EQUAL(lrefFn.call<Parent&>(lref), (Parent&) doLRef(lref));
    BOOST_CHECK_EQUAL(lrefFn.call<const Parent&>(lref), (const Parent&) doLRef(lref));

    BOOST_CHECK_EQUAL(clrefFn.call<const Child&>(childLRef), doConstLRef(childLRef));
    BOOST_CHECK_EQUAL(clrefFn.call<const Child&>(childConstLRef), doConstLRef(childConstLRef));
    BOOST_CHECK_EQUAL(clrefFn.call<const Child&>(Child(10, 0)), doConstLRef(Child(10, 0)));
    BOOST_CHECK_EQUAL(clrefFn.call<Parent>(clref), (Parent) doConstLRef(clref));
    CHECK_ERROR(clrefFn.call<Parent&>(clref));
    BOOST_CHECK_EQUAL(clrefFn.call<const Parent&>(clref), (const Parent&) doConstLRef(clref));

    CHECK_ERROR(rrefFn.call<Object>(childLRef));
    CHECK_ERROR(rrefFn.call<Object>(childConstLRef));
    BOOST_CHECK_EQUAL(rrefFn.call<Object>(Child(10, 0)), doRRef(Child(10, 0)));
}


/******************************************************************************/
/* CONVERTERS                                                                 */
/******************************************************************************/

// The test cases for this are less complicated so stuff everything in one test.
//
// Currently converters into references aren't supported because it would result
// on a dangling reference.
BOOST_AUTO_TEST_CASE(converters_test)
{
    typedef test::Convertible Conv;

    Conv conv(113);

    auto doCopy = [] (int i) { return Conv(i + 1); };
    Function copyFn("doCopy", doCopy);

    auto doLRef = [&] (int&) -> Conv& { return conv; };
    Function lrefFn("doLRef", doLRef);

    auto doConstLRef = [&] (const int&) -> const Conv& { return conv; };
    Function clrefFn("doConstLRef", doConstLRef);

    auto doRRef = [] (int&& i) { return i + 1; };
    Function rrefFn("doRRef", doRRef);

    BOOST_CHECK_EQUAL(copyFn.test<void(Conv)>(),        Match::Partial);
    BOOST_CHECK_EQUAL(copyFn.test<void(Conv&)>(),       Match::Partial);
    BOOST_CHECK_EQUAL(copyFn.test<void(const Conv&)>(), Match::Partial);
    BOOST_CHECK_EQUAL(copyFn.test<void(Conv&&)>(),      Match::Partial);
    BOOST_CHECK_EQUAL(copyFn.test<int(int)>(),          Match::Exact);
    BOOST_CHECK_EQUAL(copyFn.test<int&(int)>(),         Match::None);
    BOOST_CHECK_EQUAL(copyFn.test<const int&(int)>(),   Match::None);

    BOOST_CHECK_EQUAL(lrefFn.test<void(Conv)>(),        Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<void(Conv&)>(),       Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<void(const Conv&)>(), Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<void(Conv&&)>(),      Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<int(int&)>(),         Match::Exact);
    BOOST_CHECK_EQUAL(lrefFn.test<int&(int&)>(),        Match::None);
    BOOST_CHECK_EQUAL(lrefFn.test<const int&(int&)>(),  Match::None);

    BOOST_CHECK_EQUAL(clrefFn.test<void(Conv)>(),             Match::None);
    BOOST_CHECK_EQUAL(clrefFn.test<void(Conv&)>(),            Match::None);
    BOOST_CHECK_EQUAL(clrefFn.test<void(const Conv&)>(),      Match::None);
    BOOST_CHECK_EQUAL(clrefFn.test<void(Conv&&)>(),           Match::None);
    BOOST_CHECK_EQUAL(clrefFn.test<int(const int&)>(),        Match::Exact);
    BOOST_CHECK_EQUAL(clrefFn.test<int&(const int&)>(),       Match::None);
    BOOST_CHECK_EQUAL(clrefFn.test<const int&(const int&)>(), Match::None);

    BOOST_CHECK_EQUAL(rrefFn.test<void(Conv)>(),        Match::Partial);
    BOOST_CHECK_EQUAL(rrefFn.test<void(Conv&)>(),       Match::Partial);
    BOOST_CHECK_EQUAL(rrefFn.test<void(const Conv&)>(), Match::Partial);
    BOOST_CHECK_EQUAL(rrefFn.test<void(Conv&&)>(),      Match::Partial);
}

BOOST_AUTO_TEST_CASE(converters_call)
{
    typedef test::Convertible Conv;

    Conv conv(113);

    auto doCopy = [] (int i) { return Conv(i + 1); };
    Function copyFn("doCopy", doCopy);

    auto doLRef = [&] (int&) -> Conv& { return conv; };
    Function lrefFn("doLRef", doLRef);

    auto doConstLRef = [&] (const int&) -> const Conv& { return conv; };
    Function clrefFn("doConstLRef", doConstLRef);

    auto doRRef = [] (int&& i) { return i + 1; };
    Function rrefFn("doRRef", doRRef);

    int copy = 10;
    int& lref = copy;
    const int& clref = copy;
    (void) copy; (void) lref; (void) clref;

    Conv convCopy(10);
    auto& convLRef = convCopy;
    const auto& convConstLRef = convCopy;
    (void) convCopy; (void) convLRef; (void) convConstLRef;

    BOOST_CHECK_EQUAL(copyFn.call<Conv>(convLRef), doCopy(convLRef));
    BOOST_CHECK_EQUAL(copyFn.call<Conv>(convConstLRef), doCopy(convConstLRef));
    BOOST_CHECK_EQUAL(copyFn.call<Conv>(Conv(10)), doCopy(Conv(10)));
    BOOST_CHECK_EQUAL(copyFn.call<int>(copy), (int) doCopy(copy));
    CHECK_ERROR(copyFn.call<int&>(copy));
    CHECK_ERROR(copyFn.call<const int&>(copy));

    CHECK_ERROR(lrefFn.call<Conv&>(convLRef));
    CHECK_ERROR(lrefFn.call<Conv&>(convConstLRef));
    CHECK_ERROR(lrefFn.call<Conv&>(Conv(10)));
    BOOST_CHECK_EQUAL(lrefFn.call<int>(lref), (int) doLRef(lref));
    CHECK_ERROR(lrefFn.call<int&>(lref));
    CHECK_ERROR(lrefFn.call<const int&>(lref));

    CHECK_ERROR(clrefFn.call<const Conv&>(convLRef));
    CHECK_ERROR(clrefFn.call<const Conv&>(convConstLRef));
    CHECK_ERROR(clrefFn.call<const Conv&>(Conv(10)));
    BOOST_CHECK_EQUAL(clrefFn.call<int>(clref), (int) doConstLRef(clref));
    CHECK_ERROR(clrefFn.call<int&>(clref));
    CHECK_ERROR(clrefFn.call<const int&>(clref));

    BOOST_CHECK_EQUAL(rrefFn.call<int>(convLRef), doRRef(convLRef));
    BOOST_CHECK_EQUAL(rrefFn.call<int>(convConstLRef), doRRef(convConstLRef));
    BOOST_CHECK_EQUAL(rrefFn.call<int>(Conv(10)), doRRef(Conv(10)));
}
