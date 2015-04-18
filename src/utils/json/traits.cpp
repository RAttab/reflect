/* traits.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

namespace reflect{
namespace json {

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
