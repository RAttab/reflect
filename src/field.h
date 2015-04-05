/* field.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 22 Mar 2015
   FreeBSD-style copyright and disclaimer apply

   Field description.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* FIELD                                                                      */
/******************************************************************************/

struct Field : public Traits
{
    template<typename T, typename M>
    Field(std::string name, M T::* field);

    const std::string& name() const { return name_; }
    size_t offset() const { return offset_; }

    const Argument& argument() const { return arg; }
    const Type* type() const { return arg.type(); }

    std::string print() const;

private:
    std::string name_;
    Argument arg;
    size_t offset_;
};

} // reflect
