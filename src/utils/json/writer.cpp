/* writer.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 14 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

namespace reflect {
namespace json {

/******************************************************************************/
/* WRITER                                                                     */
/******************************************************************************/

namespace { std::string spaces(4096ULL, ' '); }

void
Writer::
seperator(char c)
{
    push(c);
    if (pretty()) push(' ');
}

void
Writer::
newline()
{
    if (!pretty()) return;

    push("\n");
    push(spaces.c_str(), indent_ * 4);
}

} // namespace json
} // namespace reflect

reflectTypeImpl(reflect::json::Writer)
{
    reflectPlumbing();
}
