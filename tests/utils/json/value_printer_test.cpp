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
/* TESTS                                                                      */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_full)
{
    Basics value;
    Basics::construct(value);
    checkFile("value_printer_full.json", value);
}

BOOST_AUTO_TEST_CASE(test_compact)
{
    Basics value;
    Basics::construct(value);
    checkFile("value_printer_compact.json", value, true);
}
