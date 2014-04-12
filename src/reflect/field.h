/* field.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Field reflection
*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* REFLECT GETTER                                                             */
/******************************************************************************/

template<typename T, typename Obj,
    class = typename std::enable_if<!std::is_same<T, void>::value>::type>
void reflectGetter(Type* type, const char* name, T (Obj::* getter)() const)
{
    type->add(name, getter);
}

template<typename T, typename Obj,
    class = typename std::enable_if<!std::is_same<T, void>::value>::type>
void reflectGetter(Type* type, const char* name, T (Obj::* getter)())
{
    type->add(name, getter);
}

template<typename...>
void reflectGetter(...) {}


/******************************************************************************/
/* REFLECT SETTER                                                             */
/******************************************************************************/

template<typename T, typename Obj,
    class = typename std::enable_if<!std::is_same<T, void>::value>::type>
void reflectSetter(Type* type, const char* name, void (Obj::* setter)(T))
{
    type->add(name, setter);
}

template<typename...>
void reflectSetter(...) {}


/******************************************************************************/
/* REFLECT MEMBER                                                             */
/******************************************************************************/

/** getter and setter functions also match on T Obj::* so we gotta rule them out
    using sfinae.
 */
template<typename T, typename Obj>
struct IsMemberPtr
{
    // Putting the decltype statement in the parameter makes gcc 4.7 cry.
    template<typename U, typename V, class = decltype(((V*)0)->*((U V::*)0))>
    static std::true_type test(void*);

    template<typename, typename>
    static std::false_type test(...);

    typedef decltype(test<T, Obj>(0)) type;
    static constexpr bool value = type::value;
};

template<typename T, typename Obj,
    class = typename std::enable_if<IsMemberPtr<T, Obj>::value>::type>
void reflectMember(Type* type, const char* name, T Obj::* field)
{
    auto getter = [=] (const Obj& obj) -> const T& { return obj.*field; };
    type->add(name, reflect::Function(name, getter));

    auto setter = [=] (Obj& obj, T value) { obj.*field = std::move(value); };
    type->add(name, reflect::Function(name, setter));
}

template<typename T, typename Obj,
    class = typename std::enable_if<IsMemberPtr<T, Obj>::value>::type>
void reflectMember(Type* type, const char* name, T const Obj::* field)
{
    auto fn = [=] (const Obj& obj) -> const T& { return obj.*field; };
    type->add(name, reflect::Function(name, fn));
}

// Used to disambiguate fields that have both getter and setter.
template<typename T, typename Obj>
void reflectMember(Type*, std::string, void (Obj::*)(T)) {}

template<typename...>
void reflectMember(...) {}


/******************************************************************************/
/* REFLECT FIELD                                                              */
/******************************************************************************/

#define reflectField(field)                                     \
    do {                                                        \
        reflect::reflectGetter(type_, #field, &T_::field);      \
        reflect::reflectSetter(type_, #field, &T_::field);      \
        reflect::reflectMember(type_, #field, &T_::field);      \
    } while(false)


} // reflect
