/* callable.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* CALLABLE                                                                   */
/******************************************************************************/

struct Callable
{
    virtual Signature signature() const = 0;
    virtual Value call(const Arguments& args) const = 0;

    template<typename Ret, typename Args>
    Ret call(Args&&...) const
    {

    }
};


} // namespace reflect
