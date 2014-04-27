/* scope.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Scope (aka. namespaces) reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* SCOPE                                                                      */
/******************************************************************************/

struct Scope
{
    Scope();
    Scope(const std::string& name, Scope* parent = nullptr);

    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;
    Scope(Scope&&) = delete;
    Scope& operator=(Scope&&) = delete;

    std::string id() const;
    const std::string& name() const { return name_; }

    bool isGlobal() const { return !parent_; }
    Scope* parent() const { return parent_; }

    std::vector<std::string> scopes() const;
    bool hasScope(const std::string& name) const;
    Scope* scope(const std::string& name) const;
    Scope* scope(const std::string& name);

    std::vector<std::string> types(bool includeScopes = false) const;
    bool hasType(const std::string& name);
    const Type* type(const std::string& name);

    void addType(const std::string& name);

    std::vector<std::string> functions(bool includeScopes = false) const;
    bool hasFunction(const std::string& name) const;
    const Overloads& function(const std::string& name) const;

    template<typename Fn>
    void addFunction(const std::string& name, Fn&& fn);
    void addFunction(const std::string& name, Function fn);

    template<typename Ret, typename... Args>
    Ret call(const std::string& fn, Args&&... args) const;

    std::string print(int indent = 0) const;

    static std::string join(const std::string& head, const std::string& tail);
    static std::pair<std::string, std::string> head(const std::string& name);
    static std::pair<std::string, std::string> tail(const std::string& name);

private:

    std::string name_;

    Scope* parent_;
    std::unordered_map<std::string, Scope*> scopes_;

    std::unordered_map<std::string, const Type*> types_;
    std::unordered_map<std::string, Overloads> functions_;
};

} // reflect
