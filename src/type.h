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

    template<typename Fn>
    void add(const std::string& name, Fn rawFn);

    std::vector<std::string> functions() const;
    bool hasFunction(const std::string& fn) const;
    const Overloads& function(const std::string& fn) const;

    std::vector<std::string> fields() const;
    bool hasField(const std::string& field) const;
    const Overloads& field(const std::string& field) const;
    const Type* fieldType(const std::string& field) const;

    void addTrait(std::string trait);
    bool is(const std::string& trait) const;
    std::vector<std::string> traits() const;

    template<typename T>
    bool isParentOf() const { return isParentOf(type<T>()); }
    bool isParentOf(const Type* other) const;

    template<typename T>
    bool isChildOf() const { return isChildOf(type<T>()); }
    bool isChildOf(const Type* other) const;

    template<typename T>
    bool hasConverter() const { return hasConverter(type<T>()); }
    bool hasConverter(const Type* other) const;

    template<typename T>
    const Function& converter() const { return converter(type<T>()); }
    const Function& converter(const Type* other) const;

    bool isCopiable() const;
    bool isMovable() const;

    template<typename... Args>
    Value construct(Args&&... args) const;

    template<typename Ret, typename... Args>
    Ret call(const std::string& field, Args&&... args) const;

    std::string print(size_t indent = 0) const;

private:

    void functions(std::vector<std::string>& result) const;
    void fields(std::vector<std::string>& result) const;

    std::string id_;
    Type* parent_;

    std::unordered_set<std::string> traits_;
    std::unordered_map<std::string, Overloads> fns_;
};


} // reflect
