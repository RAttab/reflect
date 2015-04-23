/* traits.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

namespace reflect{
namespace json {


/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

Traits::Traits() : skip(false) {}

Traits skip()
{
    Traits traits;
    traits.skip = true;
    return traits;
}

Traits alias(std::string alias)
{
    Traits traits;
    traits.alias = std::move(alias);
    return traits;
}

Traits custom(std::string parser, std::string printer)
{
    Traits traits;
    traits.parser = std::move(parser);
    traits.printer = std::move(printer);
    return traits;
}


} // namespace json
} // namespace reflect

reflectTypeImpl(reflect::json::Traits)
{
    reflectPlumbing();
    reflectField(skip);
    reflectField(alias);
    reflectField(parser);
    reflectField(printer);
}
