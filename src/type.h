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

struct Type : public Traits
{
    explicit Type(std::string id);

    Type(Type&&) = delete;
    Type(const Type&) = delete;
    Type& operator=(Type&&) = delete;
    Type& operator=(const Type&) = delete;

    const std::string& id() const { return id_; }
    const Type* parent() const { return parent_; }
    void parent(Type* parent) { parent_ = parent; }

    template<typename Fn>
    void addFunction(const std::string& name, Fn&& rawFn);
    void addFunction(const std::string& name, Function&& fn);

    std::vector<std::string> functions() const;
    bool hasFunction(const std::string& fn) const;
    Overloads& function(const std::string& fn);
    const Overloads& function(const std::string& fn) const;

    template<typename T, typename M>
    void addField(const std::string& name, M T::* rawField);
    void addField(const std::string& name, Field&& field);

    std::vector<std::string> fields() const;
    bool hasField(const std::string& field) const;
    Field& field(const std::string& field);
    const Field& field(const std::string& field) const;

    bool isPointer() const;
    std::string pointer() const;
    const Type* pointee() const;
    void setPointer(std::string pointer, const Type* pointee);

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

    template<typename... Args>
    Value alloc(Args&&... args) const;

    template<typename Ret, typename... Args>
    Ret call(const std::string& fn, Args&&... args) const;

    std::string print(size_t indent = 0) const;

private:

    void functions(std::vector<std::string>& result) const;
    void fields(std::vector<std::string>& result) const;

    std::string id_;
    Type* parent_;

    std::string pointer_;
    const Type* pointee_;

    std::unordered_map<std::string, Field> fields_;
    std::unordered_map<std::string, Overloads> fns_;
};


} // reflect
