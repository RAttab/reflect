/* argument.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   template implementation for argument thingy.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* ARGUMENT                                                                   */
/******************************************************************************/

template<typename T>
Argument
Argument::
make()
{
    Argument arg;
    Argument::fill<T>(arg);
    return arg;
}

// Compile-time optimization: Avoids the construction of Argument when possible.
template<typename T>
void
Argument::
fill(Argument& arg)
{
    arg.type_ = reflect::type<T>();
    arg.refType_ = makeRefType<T>();
    arg.isConst_ = reflect::isConst<T>();
}

template<typename T>
Argument
Argument::
make(T&& value)
{
    Argument arg;
    arg.type_ = reflect::type<T>();
    arg.refType_ = makeRefType(std::forward<T>(value));
    arg.isConst_ = reflect::isConst(std::forward<T>(value));
    return arg;
}


template<typename T>
bool
Argument::
isConvertibleTo() const
{
    return isConvertibleTo(Argument::make<T>());
}


template<typename T>
std::string printArgument()
{
    return Argument::make<T>().print();
}

} // reflect
