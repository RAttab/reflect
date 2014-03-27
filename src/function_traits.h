/* type_traits.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 10 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type introspection utilities
*/


#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* GET RETURN VALUE                                                           */
/******************************************************************************/

template<typename> struct GetReturnValue;

template<typename Ret, typename... Args>
struct GetReturnValue<Ret(Args...)>
{
    typedef Ret type;
};


/******************************************************************************/
/* COUNT ARGUMENTS                                                            */
/******************************************************************************/

template<typename> struct CountArguments;

template<typename Ret, typename... Args>
struct CountArguments
{
    static constexpr size_t value = sizeof...(Args);
};


/******************************************************************************/
/* GET ARGUMENT                                                               */
/******************************************************************************/

namespace details {

template<size_t Index, typename Arg, typename... Rest>
struct ExtractArg : public ExtractArg<Index - 1, Rest...>
{};

template<typename Arg, typename... Rest>
struct ExtractArg<0, Arg, Rest...>
{
    typedef Arg type;
};

} // namespace details

template<size_t, typename> struct GetArgument;

template<size_t Index, typename Ret, typename... Args>
struct GetArgument<Index, Ret(Args...)>
{
    typedef typename details::ExtractArg<Index, Args...>::type type;
};


} // namespace reflect
