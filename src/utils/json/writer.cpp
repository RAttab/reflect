/* writer.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   JSON Writer interface.
*/

#include "json.h"
#pragma once

namespace reflect {
namespace json {


/******************************************************************************/
/* WRITER                                                                     */
/******************************************************************************/


struct Writer
{
    enum Options
    {
        Pretty        = 1 << 0,
        EncodeUnicode = 1 << 1,

        Default = 0,
    };

    Writer(std::ostream& stream, Options options = Default) :
        stream(stream),
        buffer_(128),
        options(options)
    {}
    
    bool operator() const { return error_ && stream; }

    template<typename... Args>
    void error(const char* fmt, Args&&... args);
    const Error& error() const { return error_; }

    std::string& buffer() { return buffer_; }

    static const char* indent = "    ";

    bool pretty() const { return options & Pretty; }
    bool encodeUnicode() const { return options & EncodeUnicode; }

};

} // namespace json
} // namespace reflect
