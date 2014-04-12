/* argument.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Tuple of Type, RefType and isConst.

   \todo Need to find a better name.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* ARGUMENT                                                                   */
/******************************************************************************/

struct Argument
{
    Argument();
    Argument(const Type* type, RefType refType, bool isConst);

    template<typename T>
    static Argument make();

    template<typename T>
    static Argument make(T&&);

    const Type* type() const { return type_; }
    RefType refType() const { return refType_; }
    bool isConst() const { return isConst_; }
    bool isVoid() const;

    bool isConstRef() const;
    bool isTemporary() const;

    std::string print() const;

    template<typename T>
    bool isConvertibleTo() const;
    bool isConvertibleTo(const Argument& other) const;

private:
    const Type* type_;
    RefType refType_;
    bool isConst_;
};

template<typename T>
std::string printArgument();
std::string printArgument(Type* type, RefType refType, bool isConst);

} // reflect
