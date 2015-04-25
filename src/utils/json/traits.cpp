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

Traits
Traits::
operator| (const Traits& other) const
{
    Traits result = *this;

    result.skip = skip || other.skip;
    result.skipEmpty = skipEmpty || other.skipEmpty;

    if (result.alias.empty()) result.alias = other.alias;
    if (result.parser.empty()) result.parser = other.parser;
    if (result.printer.empty()) result.printer = other.printer;

    return result;
}

Traits skip()
{
    Traits traits;
    traits.skip = true;
    return traits;
}

Traits skipEmpty()
{
    Traits traits;
    traits.skipEmpty = true;
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
