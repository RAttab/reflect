/* blah_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Experimental tests
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
/* BAR                                                                        */
/******************************************************************************/

namespace test {

struct Bar
{
    Bar() : bar(0) {}

    int bar;
};

} // namespace test

reflectType(test::Bar)
{
    reflectField(bar);
}


/******************************************************************************/
/* FOO                                                                        */
/******************************************************************************/

namespace test {

struct Foo : public Bar
{
    Foo() : field(0), constField(0), value(0) {}
    Foo(int i) : field(i), constField(i), value(i) {}

    int field;
    const int constField;

    void void_() {}

    const int& getter() const { return value; }
    void setter(int i) { value = i; }

    void copy(int i) { value = i; }
    int copy() const { return value; }

    void lValue(int& i) { value = i; }
    int& lValue() { return value; }

    void constLValue(const int& i) { value = i; }
    const int& constLValue() const { return value; }

    void rValue(int&& i) { value = std::move(i); }
    int rValue() { return std::move(value); }

    void fn(int a, int b, int c) { value += a * b + c; }
    static int staticFn(int a, int b) { return a * b ;}
    static int staticFn(int a, int b, int c) { return a * b + c;}

    Foo& operator+=(const Foo& other)
    {
        field += other.field;
        value += other.value;
        return *this;
    }

    Foo operator+(const Foo& other) const
    {
        Foo result(*this);
        result += other;
        return result;
    }

    int value;
};

} // namespace test


reflectType(test::Foo)
{
    reflectParent(test::Bar);

    reflectCons(int);
    reflectPlumbing();

    reflectOpAssign();
    reflectOpArithmetic();

    reflectField(void_);
    reflectField(field);
    reflectField(constField);

    reflectField(getter);
    reflectField(setter);

    reflectField(copy);
    reflectField(lValue);
    reflectField(constLValue);
    reflectField(rValue);

    reflectFn(fn);
    reflectFnTyped(staticFn, int(int, int));
    reflectFnTyped(staticFn, int(int, int, int));

    reflectCustom(custom) (test::Foo& obj, int a, int b) {
        obj.setter(a + b);
    };
}


/******************************************************************************/
/* TESTS                                                                      */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(basics)
{
    const Type* typeBar = type("test::Bar");
    const Type* typeFoo = type("test::Foo");

    BOOST_CHECK(typeBar);
    BOOST_CHECK(typeFoo);

    std::cerr << scope()->print() << std::endl;
    std::cerr << typeBar->print() << std::endl;
    std::cerr << typeFoo->print() << std::endl;

    BOOST_CHECK_EQUAL(typeFoo->parent(), typeBar);
    BOOST_CHECK( typeFoo->isChildOf(typeBar));
    BOOST_CHECK(!typeBar->isChildOf(typeFoo));

    BOOST_CHECK( typeBar->hasField("bar"));
    BOOST_CHECK(!typeBar->hasField("field"));

    BOOST_CHECK( typeFoo->hasField("bar"));
    BOOST_CHECK(!typeFoo->hasField("baz"));
    BOOST_CHECK(!typeFoo->hasField("void_"));
    BOOST_CHECK( typeFoo->hasField("field"));
    BOOST_CHECK( typeFoo->hasField("getter"));
    BOOST_CHECK( typeFoo->hasField("rValue"));
    BOOST_CHECK( typeFoo->hasFunction("fn"));
    BOOST_CHECK( typeFoo->hasFunction("custom"));

    Value vFoo = typeFoo->construct();
    const auto& foo = vFoo.get<test::Foo>();
    const auto& bar = vFoo.get<test::Bar>();

    BOOST_CHECK_EQUAL(
            typeFoo->call<int>("staticFn", 1, 2),
            test::Foo::staticFn(1,2));

    vFoo.call<void>("bar", 1);
    BOOST_CHECK_EQUAL(foo.bar, 1);
    BOOST_CHECK_EQUAL(bar.bar, 1);
    BOOST_CHECK_EQUAL(vFoo.call<int>("bar"), foo.bar);

    vFoo.call<void>("setter", 1);
    BOOST_CHECK_EQUAL(foo.value, 1);
    BOOST_CHECK_EQUAL(vFoo.call<int>("getter"), foo.value);

    Value a = vFoo.call<Value>("copy");

    vFoo.call<void>("custom", a, 2);
    BOOST_CHECK_EQUAL(foo.value, a.get<int>() + 2);

    Value result = typeFoo->construct(100) + typeFoo->construct(20);
    BOOST_CHECK_EQUAL(result.get<int>("field"), 120);
}
