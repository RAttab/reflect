/* traits.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 22 Mar 2015
   FreeBSD-style copyright and disclaimer apply

   Traits container
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* TRAITS                                                                     */
/******************************************************************************/

struct Traits
{
    template<typename T>
    void addTrait(const std::string& trait, T&& value);
    void addTrait(const std::string& trait, Value value = {});

    std::vector<std::string> traits() const;

    bool is(const std::string& trait) const;

    template<typename Ret>
    Ret getValue(const std::string& trait) const;

protected:

    std::string print() const;

private:
    std::unordered_map<std::string, Value> traits_;
};

} // namespace reflect
