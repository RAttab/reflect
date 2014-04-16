/* namespace.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Namespace reflection.
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* NAMESPACE                                                                  */
/******************************************************************************/

struct Namespace
{
    Namespace(const std::string& name, Namespace* parent = nullptr);

    Namespace(const Namespace&) = delete;
    Namespace& operator=(const Namespace&) = delete;
    Namespace(Namespace&&) = delete;
    Namespace& operator=(Namespace&&) = delete;

    std::string id() const;
    const std::string& name() const { return name_; }

    bool isGlobal() const { return !parent_; }
    Namespace* parent() const { return parent_; }

    std::vector<std::string> subs() const;
    bool hasSub(const std::string& name) const;
    Namespace* sub(const std::string& name) const;
    Namespace* sub(const std::string& name);

    std::vector<std::string> types(bool includeSubs = false) const;
    bool hasType(const std::string& name);
    const Type* type(const std::string& name);

    void addType(const std::string& name);

    std::vector<std::string> functions(bool includeSubs = false) const;
    bool hasFunction(const std::string& name) const;
    const Functions& function(const std::string& name) const;

    template<typename Fn>
    void addFunction(const std::string& name, Fn&& fn)
    {
        addFunction(name, Function(tail(name), std::forward<Fn>(fn)));
    }
    void addFunction(const std::string& name, Function fn);

    std::string print(int indent = 0) const;

private:

    static std::string join(const std::string& head, const std::string& tail);
    static std::pair<std::string, std::string> head(const std::string& name);
    static std::pair<std::string, std::string> tail(const std::string& name);

    std::string name_;

    Namespace* parent_;
    std::unordered_map<std::string, Namespace*> subs_;

    std::unordered_map<std::string, const Type*> types_;
    std::unordered_map<std::string, Functions> functions_;
};

} // reflect
