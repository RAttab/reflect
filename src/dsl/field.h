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

template<typename T>
void reflectField(Type* type, std::string name, size_t offset)
{
    type->addField<T>(std::move(name), offset);
}

#define reflectField(field)                             \
    ::reflect::reflectField<decltype(T_::field)>(type_, #field, offsetof(T_, field))

} // reflect
