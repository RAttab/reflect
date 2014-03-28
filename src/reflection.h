/* reflection.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 08 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection core.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECTION                                                                 */
/******************************************************************************/

struct Reflection
{
    explicit Reflection(std::string id, Reflection* parent = nullptr) :
        id_(std::move(id)), parent_(parent)
    {}

    Reflection(Reflection&&) = delete;
    Reflection(const Reflection&) = delete;
    Reflection& operator=(Reflection&&) = delete;
    Reflection& operator=(const Reflection&) = delete;

    const std::string& id() const { return id_; }

    Reflection* parent() const { return parent_; }
    void parent(Reflection* parent) { parent_ = parent; }

    template<typename T>
    bool isConvertibleTo()
    {
        return isConvertibleTo(reflect<T>());
    }
    bool isConvertibleTo(Reflection* other);

private:

    std::string id_;
    Reflection* parent_;

    std::unordered_map<std::string, Functions> fields;
};


} // reflect
