/* utils.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 25 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#pragma once

#include "utils/json.h"

#include <boost/test/unit_test.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>

/******************************************************************************/
/* CHECK FILE                                                                 */
/******************************************************************************/

void checkFile(const std::string& file, const std::string& value, bool noSpace = false)
{
    std::stringstream ss;
    std::ifstream stream("tests/utils/json/" + file);

    while (stream) {
        char c = stream.get();
        if (noSpace && std::isspace(c)) continue;
        if (stream) ss.put(c);
    }

    BOOST_CHECK_EQUAL(ss.str(), value);
}


template<typename T>
void checkFile(const std::string& file, const T& value, bool noSpace = false)
{
    using namespace reflect::json;

    Writer::Options options = Writer::Default;
    if (noSpace) options = Writer::Options(options | Writer::Pretty);

    std::stringstream ss;
    Writer writer(ss, options);

    auto err = print(writer, value);
    if (err) reflectError("unable to print value: %s", err.what());

    checkFile(file, ss.str(), noSpace);
}
