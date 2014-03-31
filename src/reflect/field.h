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
void reflectGetter(Type* type, std::string name, T (Obj::* getter)() const)
{
    type->add(std::move(name), getter);
}

template<typename T, typename Obj,
    class = typename std::enable_if<!std::is_same<T, void>::value>::type>
void reflectGetter(Type* type, std::string name, T (Obj::* getter)())
{
    type->add(std::move(name), getter);
}

template<typename T>
void reflectGetter(Type*, std::string, T) {}


/******************************************************************************/
/* REFLECT SETTER                                                             */
/******************************************************************************/

template<typename T, typename Obj,
    class = typename std::enable_if<!std::is_same<T, void>::value>::type>
void reflectSetter(Type* type, std::string name, void (Obj::* setter)(T))
{
    type->add(std::move(name), setter);
}

template<typename T>
void reflectSetter(Type*, std::string, T) {}


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
void reflectMember(Type* type, std::string name, T Obj::* field)
{
    type->add(std::move(name),
            [=] (const Obj& obj) -> const T& {
                return obj.*field;
            });

    type->add(std::move(name),
            [=] (Obj& obj, T value) {
                obj.*field = std::move(value);
            });
}

template<typename T, typename Obj,
    class = typename std::enable_if<IsMemberPtr<T, Obj>::value>::type>
void reflectMember(Type* type, std::string name, T const Obj::* field)
{
    type->add(std::move(name),
            [=] (const Obj& obj) -> const T& {
                return obj.*field;
            });
}

// Used to disambiguate fields that have both getter and setter.
template<typename T, typename Obj>
void reflectMember(Type*, std::string, void (Obj::*)(T)) {}

template<typename T>
void reflectMember(Type*, std::string, T) {}


/******************************************************************************/
/* REFLECT FIELD                                                              */
/******************************************************************************/

#define reflectField(field)                             \
    do {                                                \
        reflectGetter(type_, #field, &T_::field);       \
        reflectSetter(type_, #field, &T_::field);       \
        reflectMember(type_, #field, &T_::field);       \
    } while(false)


} // reflect