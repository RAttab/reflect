/* path_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for pathing utility
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "reflect/type.h"
#include "reflect/plumbing.h"
#include "reflect/field.h"
#include "types/std/map.h"
#include "types/std/vector.h"
#include "types/std/string.h"
#include "utils/config/includes.h"

#include <boost/test/unit_test.hpp>

using namespace reflect;


/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/

struct A
{
    int i;
    int* p;

    A() : i(20), p(new int(10)) {}
    ~A() { delete p; }
};

reflectType(A)
{
    reflectPlumbing();
    reflectField(i);
    reflectField(p);
}

struct B
{
    A* p;
    std::vector<A> v;
    std::map<std::string, A> m;
    A* fp() const { return p; }
    A& fr() const { return *p; }

    B() : p(new A) 
    { 
        m["x"] = {}; 
        v.emplace_back();
        v.emplace_back();
    }
    ~B() { delete p; }
};

reflectType(B)
{
    reflectPlumbing();
    reflectField(p);
    reflectField(m);
    reflectField(v);
    reflectField(fp);
    reflectField(fr);
}


/******************************************************************************/
/* TESTS                                                                      */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(parsing)
{
    BOOST_CHECK_EQUAL(config::Path("a").toString(), "a");
    BOOST_CHECK_EQUAL(config::Path("a.b.c").toString(), "a.b.c");
    BOOST_CHECK_EQUAL(config::Path("a.0").toString(), "a.0");
    BOOST_CHECK_EQUAL(config::Path("a.0.c").toString(), "a.0.c");
}

BOOST_AUTO_TEST_CASE(has)
{
    Value a = type<A>()->construct();
    BOOST_CHECK( config::has(a, "i"));
    BOOST_CHECK(!config::has(a, "i.j"));
    BOOST_CHECK(!config::has(a, "j"));
    BOOST_CHECK( config::has(a, "p"));
    BOOST_CHECK(!config::has(a, "p.i"));
    

    Value b = type<B>()->construct();
    BOOST_CHECK( config::has(b, "p"));
    BOOST_CHECK( config::has(b, "p.i"));
    BOOST_CHECK( config::has(b, "p.p"));
    BOOST_CHECK(!config::has(b, "p.d"));

    BOOST_CHECK( config::has(b, "fp"));
    BOOST_CHECK( config::has(b, "fp.i"));
    BOOST_CHECK(!config::has(b, "fp.j"));
    BOOST_CHECK( config::has(b, "fr"));
    BOOST_CHECK( config::has(b, "fr.i"));
    BOOST_CHECK(!config::has(b, "fr.j"));

    BOOST_CHECK( config::has(b, "v"));
    BOOST_CHECK( config::has(b, "v.0"));
    BOOST_CHECK( config::has(b, "v.0.i"));
    BOOST_CHECK(!config::has(b, "v.0.j"));
    BOOST_CHECK( config::has(b, "v.1"));
    BOOST_CHECK(!config::has(b, "v.2"));

    BOOST_CHECK( config::has(b, "m"));
    BOOST_CHECK( config::has(b, "m.x"));
    BOOST_CHECK( config::has(b, "m.x.i"));
    BOOST_CHECK(!config::has(b, "m.x.j"));
    BOOST_CHECK(!config::has(b, "m.y"));
    BOOST_CHECK(!config::has(b, "m.y.i"));

}
