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
    typedef typename std::decay<T>::type CleanT;

    Argument arg;
    arg.type_ = reflect<CleanT>();
    arg.refType_ = makeRefType<T>();
    arg.isConst_ = reflect::isConst<T>();
    return arg;
}


template<typename T>
std::string printArgument()
{
    return Argument::make<T>().print();
}

} // reflect
