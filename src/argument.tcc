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

// Compile-time optimization: Avoids the construction of Argument when possible.
template<typename T>
void
Argument::
fill(Argument& arg)
{
    typedef typename std::decay<T>::type CleanT;
    arg.type_ = reflect::type<CleanT>();
    arg.refType_ = makeRefType<T>();
    arg.isConst_ = reflect::isConst<T>();
}

template<typename T>
Argument
Argument::
make()
{
    typedef typename std::decay<T>::type CleanT;

    Argument arg;
    arg.type_ = reflect::type<CleanT>();
    arg.refType_ = makeRefType<T>();
    arg.isConst_ = reflect::isConst<T>();
    return arg;
}

template<typename T>
Argument
Argument::
make(T&& value)
{
    typedef typename std::decay<T>::type CleanT;

    Argument arg;
    arg.type_ = reflect::type<CleanT>();
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
