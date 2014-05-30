/* json_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Json tests.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "types/primitives.h"
#include "types/std/string.h"
#include "types/std/vector.h"
#include "types/std/map.h"
#include "dsl/plumbing.h"
#include "dsl/type.h"
#include "dsl/field.h"
#include "utils/json/parser.h"
#include "utils/json/printer.h"

#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace std;
using namespace reflect;

/******************************************************************************/
/* BLEH                                                                       */
/******************************************************************************/

struct Bleh
{
    long i;
    bool b;

    Bleh() : i(0), b(false) {}
    Bleh(long i, bool b) : i(i), b(b) {}
};

reflectType(Bleh)
{
    reflectPlumbing();
    reflectField(i);
    reflectField(b);
}


/******************************************************************************/
/* BLAH                                                                       */
/******************************************************************************/

struct Blah
{
    std::string str;
    std::vector<Bleh> vec;
    std::map<std::string, Bleh> map;
};

reflectType(Blah)
{
    reflectPlumbing();
    reflectField(str);
    reflectField(vec);
    reflectField(map);
}


/******************************************************************************/
/* PARSING                                                                    */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(parsing)
{
    std::ifstream ifs("tests/data/basics.json");
    BOOST_CHECK(ifs);

    auto blah = json::parse<Blah>(ifs);

    auto checkBleh = [] (const Bleh& bleh, long i, bool b) {
        BOOST_CHECK_EQUAL(bleh.i, i);
        BOOST_CHECK_EQUAL(bleh.b, b);
    };

    BOOST_CHECK_EQUAL(blah.str, "I like candy");

    BOOST_CHECK_EQUAL(blah.vec.size(), 3u);
    checkBleh(blah.vec[0], 10, false);
    checkBleh(blah.vec[1], 0, true);
    checkBleh(blah.vec[2], 30, false);

    BOOST_CHECK_EQUAL(blah.map.size(), 2u);
    checkBleh(blah.map["foo"], 20, true);
    checkBleh(blah.map["bar"], 0, true);

    json::print(blah, std::cerr, true);
}
