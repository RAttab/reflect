/* namespace_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 16 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for namespace reflection.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "reflect/namespace.h"
#include "reflect/class.h"
#include "reflect/plumbing.h"

#include <boost/test/unit_test.hpp>

using namespace reflect;

/******************************************************************************/
/* FOO                                                                        */
/******************************************************************************/

namespace foo {

int fooFn(int i) { return i + 1; }
struct Foo {};

}

reflectNamespace(foo)
{
    reflectGlobalFn(foo::fooFn);
}

reflectClass(foo::Foo)
{
    reflectPlumbing();
}


/******************************************************************************/
/* BAR                                                                        */
/******************************************************************************/

namespace foo { namespace bar {

struct Bar {};
int barFn(int i) { return i + 1; }

}}

reflectNamespace(foo::bar)
{
    reflectGlobalFn(foo::bar::barFn);
}

reflectClass(foo::bar::Bar)
{
    reflectPlumbing();
}


/******************************************************************************/
/* BAZ                                                                        */
/******************************************************************************/

namespace foo { namespace baz {

int i = 0;
int bazFn(int i ) { return i * 2; }

}}

reflectNamespace(foo::baz)
{
    reflectGlobalVar(foo::baz::i);
    reflectGlobalFn(foo::baz::bazFn);
}


/******************************************************************************/
/* TEST                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(basics)
{
    Namespace* nFoo = namespace_("foo");

    std::cerr << nFoo->print() << std::endl;

    BOOST_CHECK(nFoo->hasType("Foo"));
    BOOST_CHECK(nFoo->type("Foo")->hasField("foo::Foo"));

    BOOST_CHECK(nFoo->type("bar::Bar")->hasField("foo::bar::Bar"));
    BOOST_CHECK(nFoo->hasType("bar::Bar"));

    BOOST_CHECK_EQUAL(nFoo->call<int>("fooFn", 10), foo::fooFn(10));
    BOOST_CHECK_EQUAL(nFoo->call<int>("bar::barFn", 10), foo::bar::barFn(10));

    nFoo->call<void>("baz::i", 10);
    BOOST_CHECK_EQUAL(foo::baz::i, 10);
    BOOST_CHECK_EQUAL(nFoo->call<int>("baz::i"), foo::baz::i);
}
