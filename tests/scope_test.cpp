/* scope_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 16 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for scope reflection.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "reflect/scope.h"
#include "reflect/type.h"
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

reflectScope(foo)
{
    reflectGlobalFn(foo::fooFn);
}

reflectType(foo::Foo)
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

reflectScope(foo::bar)
{
    reflectGlobalFn(foo::bar::barFn);
}

reflectType(foo::bar::Bar)
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

reflectScope(foo::baz)
{
    reflectGlobalVar(foo::baz::i);
    reflectGlobalFn(foo::baz::bazFn);
}


/******************************************************************************/
/* TEST                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(head)
{
    auto check = [] (
            std::pair<std::string, std::string> r,
            std::string head, std::string rest)
        {
            BOOST_CHECK_EQUAL(r.first, head);
            BOOST_CHECK_EQUAL(r.second, rest);
        };

    check(Scope::head(""),        "",  "");
    check(Scope::head("::"),      "",  "");
    check(Scope::head("a"),       "a", "");
    check(Scope::head("a::b"),    "a", "b");
    check(Scope::head("a::b::c"), "a", "b::c");
    check(Scope::head("::b::c"),  "",  "b::c");

    check(Scope::head("a<b>"),       "a<b>",    "");
    check(Scope::head("a<b::c>"),    "a<b::c>", "");
    check(Scope::head("a<b>::c"),    "a<b>",    "c");
    check(Scope::head("a<b::c>::d"), "a<b::c>", "d");
    check(Scope::head("a<b::c<d::f>::g>::h"), "a<b::c<d::f>::g>", "h");
}

BOOST_AUTO_TEST_CASE(tail)
{
    auto check = [] (
            std::pair<std::string, std::string> r,
            std::string tail, std::string rest)
        {
            BOOST_CHECK_EQUAL(r.first, tail);
            BOOST_CHECK_EQUAL(r.second, rest);
        };

    check(Scope::tail(""),        "",  "");
    check(Scope::tail("::"),      "",  "");
    check(Scope::tail("a"),       "a", "");
    check(Scope::tail("a::b"),    "b", "a");
    check(Scope::tail("a::b::c"), "c", "a::b");
    check(Scope::tail("b::c::"),  "",  "b::c");

    check(Scope::tail("a<b>"),       "a<b>",    "");
    check(Scope::tail("a<b::c>"),    "a<b::c>", "");
    check(Scope::tail("c::a<b>"),    "a<b>",    "c");
    check(Scope::tail("d::a<b::c>"), "a<b::c>", "d");
    check(Scope::tail("h::a<b::c<d::f>::g>"), "a<b::c<d::f>::g>", "h");
}

BOOST_AUTO_TEST_CASE(basics)
{
    Scope* nFoo = scope("foo");

    std::cerr << nFoo->print() << std::endl;

    BOOST_CHECK(nFoo->hasType("Foo"));
    BOOST_CHECK(nFoo->type("Foo")->hasFunction("foo::Foo"));

    BOOST_CHECK(nFoo->type("bar::Bar")->hasFunction("foo::bar::Bar"));
    BOOST_CHECK(nFoo->hasType("bar::Bar"));

    BOOST_CHECK_EQUAL(nFoo->call<int>("fooFn", 10), foo::fooFn(10));
    BOOST_CHECK_EQUAL(nFoo->call<int>("bar::barFn", 10), foo::bar::barFn(10));

    nFoo->call<void>("baz::i", 10);
    BOOST_CHECK_EQUAL(foo::baz::i, 10);
    BOOST_CHECK_EQUAL(nFoo->call<int>("baz::i"), foo::baz::i);
}
