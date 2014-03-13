/* reflection.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 08 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection core.
*/

#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECTION                                                                 */
/******************************************************************************/

struct Reflection
{
    explicit Reflection(std::string id, Reflection* parent = nullptr) :
        id(std::move(id)), parent(parent)
    {}

    Reflection(Reflection&&) = delete;
    Reflection(const Reflection&) = delete;
    Reflection& operator=(Reflection&&) = delete;
    Reflection& operator=(const Reflection&) = delete;


private:

    std::string id;
    Reflection* parent;

    std::unordered_map<std::string, Functions> fields;
};


} // reflect
