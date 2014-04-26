/* make_function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 27 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Creates an std::function of the correct type out of any callable object.

   The fun part is that it does all the type deduction for you and can therefor
   be used like so:

       auto blah = makeFunction(foo);
       blah(1, 2);

   Pretty convenient and all it requires is a few sacritfic to the altar of the
   dark templating gods. I'm pretty sure it will also fail spectacularly in all
   kinds of mundane situations.

*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* MAKE FUNCTION                                                              */
/******************************************************************************/

// C++14 return type deduction would be handy right about now...
template<typename Fn>
auto makeFunction(Fn&& fn) -> typename FunctionType<Fn>::StdFn
{
    typedef typename FunctionType<Fn>::StdFn StdFn;
    return StdFn(std::forward<Fn>(fn));
}

} // reflect
