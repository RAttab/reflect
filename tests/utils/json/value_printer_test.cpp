/* value_printer_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "printer_utils.h"
#include "test_types.h"

#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace reflect;
using namespace reflect::json;


/******************************************************************************/
/* TEST BASICS                                                                */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_basics)
{
    Basics value;
    {
        value.boolean = true;
        value.integer = 123;
        value.floating = 123.321;

        value.string = "abc";
        value.stringPtr = new std::string("def");
        value.stringShared = std::make_shared<std::string>("ghi");

        value.skip = 0;
        value.alias = 654;
        value.custom = { 789 };

        auto pInt = [](int64_t i) { return new int64_t(i); };

        value.vector = { 1, 2, 3 };
        value.vectorPtr = { pInt(4), pInt(5), pInt(6) };

        value.map = { {"abc", 10}, {"def", 20} };
        value.mapPtr = { {"hig", pInt(30)} };

        value.next = new Basics;
        value.next->integer = 10;

        value.next->next = new Basics;
        value.next->next->integer = 20;
    }

    checkFile("basics.json", value, true);
}
