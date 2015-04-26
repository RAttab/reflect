/* signature.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 26 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* SIGNATURE                                                                  */
/******************************************************************************/

typedef std::vector<Value> Arguments;

struct Signature
{
    Argument ret;
    std::vector<Argument> params;

    static Signature make(Argument ret, std::vector<Argument> params)
    {
        Signature result;
        result.ret = std::move(ret);
        result.params = std::move(params);
        return result;
    }
};

} // namespace reflect
