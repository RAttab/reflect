/* field.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Field reflection
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

#define reflectFieldTrait(fn, trait)            \
    type_->field(#fn).addTrait(#trait)

#define reflectFieldValue(fn, trait, value)     \
    type_->field(#fn).addTrait(#trait, value)


/******************************************************************************/
/* FIELD                                                                      */
/******************************************************************************/

template<typename T, typename M>
void reflectField(Type* type, std::string name, M T::* field)
{
    type->addField(std::move(name), field);
}

#define reflectField(field)                             \
    ::reflect::reflectField(type_, #field, &T_::field)

} // reflect
