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

#include <boost/test/unit_test.hpp>

using namespace reflect;

/******************************************************************************/
/* FOO                                                                        */
/******************************************************************************/

namespace foo {

void fooFn() {}
struct Foo {};

}

reflectNamespace(foo)
{
    reflectGlobalFn(foo::fooFn);
}
reflectClass(foo::Foo) {}


/******************************************************************************/
/* BAR                                                                        */
/******************************************************************************/

namespace foo {namespace bar {

struct Bar {};
int barFn(int i) { return i + 1; }

}}

reflectNamespace(foo::bar)
{
    reflectGlobalFn(foo::bar::barFn);
}
reflectClass(foo::bar::Bar) {}


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
    std::cerr << namespace_("foo")->print() << std::endl;
}
