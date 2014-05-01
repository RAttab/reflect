/* path_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for pathing utility

   This test is a leaky boat but cleaning up isn't worh the effort.
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
    A(int i) : i(i), p(new int(i)) {}
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

    A& fr() const { return *p; }
    A* fp() const { return p; }
    void fp(A* a) { p = a; }

    B() : p(new A(400))
    {
        m["x"] = { 300 };
        v.emplace_back(100);
        v.emplace_back(200);
    }
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

BOOST_AUTO_TEST_CASE(get)
{
    Value vA = type<A>()->construct();
    const A& a = vA.get<A>();
    BOOST_CHECK_EQUAL(config::get(vA, "i").get<int>(), a.i);
    BOOST_CHECK_EQUAL(config::get(vA, "p").get<int*>(), a.p);


    Value vB = type<B>()->construct();
    B& b = vB.cast<B>();
    BOOST_CHECK_EQUAL( config::get(vB, "p").get<A*>(), b.p);
    BOOST_CHECK_EQUAL( config::get(vB, "p.i").get<int>(), b.p->i);
    BOOST_CHECK_EQUAL( config::get(vB, "p.p").get<int*>(), b.p->p);

    BOOST_CHECK_EQUAL( config::get(vB, "fp").get<A*>(), b.fp());
    BOOST_CHECK_EQUAL( config::get(vB, "fp.i").get<int>(), b.fp()->i);
    BOOST_CHECK_EQUAL(&config::get(vB, "fr").get<A>(), &b.fr());
    BOOST_CHECK_EQUAL( config::get(vB, "fr.i").get<int>(), b.fr().i);

    typedef std::vector<A> Vec;
    BOOST_CHECK_EQUAL(&config::get(vB, "v").get<Vec>(), &b.v);
    BOOST_CHECK_EQUAL(&config::get(vB, "v.0").get<A>(), &b.v[0]);
    BOOST_CHECK_EQUAL( config::get(vB, "v.0.i").get<int>(), b.v[0].i);
    BOOST_CHECK_EQUAL(&config::get(vB, "v.1").get<A>(), &b.v[1]);

    typedef std::map<std::string, A> Map;
    BOOST_CHECK_EQUAL(&config::get(vB, "m").get<Map>(), &b.m);
    BOOST_CHECK_EQUAL(&config::get(vB, "m.x").get<A>(), &b.m["x"]);
    BOOST_CHECK_EQUAL( config::get(vB, "m.x.i").get<int>(), b.m["x"].i);
}

BOOST_AUTO_TEST_CASE(set)
{
    Value vA = type<A>()->construct();
    const A& a = vA.get<A>();

    config::set(vA, "i", 13);
    BOOST_CHECK_EQUAL(a.i, 13);

    {
        int* p = new int;
        config::set(vA, "p", p);
        BOOST_CHECK_EQUAL(a.p, p);
    }


    Value vB = type<B>()->construct();
    B& b = vB.cast<B>();

    {
        A* p = new A;
        config::set(vB, "p", p);
        BOOST_CHECK_EQUAL(b.p, p);
    }

    config::set(vB, "p.i", 14);
    BOOST_CHECK_EQUAL(b.p->i, 14);

    {
        int* p = new int;
        config::set(vB, "p.p", p);
        BOOST_CHECK_EQUAL(b.p->p, p);
    }

    {
        A* p = new A;
        config::set(vB, "fp", p);
        BOOST_CHECK_EQUAL(b.fp(), p);
    }

    config::set(vB, "fp.i", 15);
    BOOST_CHECK_EQUAL(b.fp()->i, 15);

    {
        typedef std::vector<A> Vec;

        config::set(vB, "v.0", A(16));
        BOOST_CHECK_EQUAL(b.v[0].i, 16);

        config::set(vB, "v.0.i", 17);
        BOOST_CHECK_EQUAL(b.v[0].i, 17);

        config::set(vB, "v.1", A(18));
        BOOST_CHECK_EQUAL(b.v[1].i, 18);

        config::set(vB, "v.2", A(19));
        BOOST_CHECK_EQUAL(b.v.at(2).i, 19);

        config::set(vB, "v.3.i", 23);
        BOOST_CHECK_EQUAL(b.v.at(3).i, 23);
    }

    {
        typedef std::map<std::string, A> Map;

        config::set(vB, "m.x", A(21));
        BOOST_CHECK_EQUAL(b.m["x"].i, 21);

        config::set(vB, "m.x.i", 22);
        BOOST_CHECK_EQUAL(b.m["x"].i, 22);

        config::set(vB, "m.y", A(25));
        BOOST_CHECK_EQUAL(b.m["y"].i, 25);

        config::set(vB, "m.z.i", 26);
        BOOST_CHECK_EQUAL(b.m["z"].i, 26);
    }
}


