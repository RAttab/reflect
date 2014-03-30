/* type.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 08 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Type of a type.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* TYPE                                                                       */
/******************************************************************************/

struct Type
{
    explicit Type(std::string id, Type* parent = nullptr) :
        id_(std::move(id)), parent_(parent)
    {}

    Type(Type&&) = delete;
    Type(const Type&) = delete;
    Type& operator=(Type&&) = delete;
    Type& operator=(const Type&) = delete;

    const std::string& id() const { return id_; }

    Type* parent() const { return parent_; }
    void parent(Type* parent) { parent_ = parent; }

    template<typename T>
    bool isConvertibleTo()
    {
        return isConvertibleTo(type<T>());
    }
    bool isConvertibleTo(Type* other);

    // \todo implement
    bool isCopiable() const { return true; }
    bool isMovable() const { return true; }

    template<typename Fn>
    void add(std::string name, Fn rawFn)
    {
        Function fn(name, std::move(rawFn));
        std::cerr << "add(" << name << "): " << signature(fn) << std::endl;
    }

private:

    std::string id_;
    Type* parent_;

    std::unordered_map<std::string, Functions> fields;
};


} // reflect
