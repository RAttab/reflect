/* reflection.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection utilities
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* REFLECT PARENT                                                             */
/******************************************************************************/

template<typename T>
void reflectParent_(Type* type)
{
    type->parent(reflect::type<T>());
}

#define reflectParent(parent)                                           \
    do {                                                                \
        reflectStaticAssert((std::is_base_of<parent, T_>::value));      \
        reflectParent_<parent>(type_);                                  \
    } while(false)


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


/******************************************************************************/
/* REFLECT FN                                                                 */
/******************************************************************************/

template<typename Fn>
void reflectFunction_(Type* type, std::string name, Fn fn)
{
    type->add(std::move(name), std::move(fn));
}

#define reflectFunction(fn)                     \
    do {                                        \
        reflectFunction_(type_, #fn, &T_::fn);  \
    } while(false);


/******************************************************************************/
/* REFLECT CUSTOM                                                             */
/******************************************************************************/

struct AddLambdaToType
{
    AddLambdaToType(Type* type, std::string name) :
        type(type), name(std::move(name))
    {}

    template<typename Fn>
    void operator+= (Fn fn)
    {
        type->add(std::move(name), std::move(fn));
    }

private:
    Type* type;
    std::string name;
};

AddLambdaToType reflectLambda(Type* type, std::string name)
{
    return AddLambdaToType(type, std::move(name));
}

#define reflectCustom(name)                     \
    reflectLambda(type_, #name) += []


/******************************************************************************/
/* REFLECT CLASS                                                              */
/******************************************************************************/

#define reflectClassDecl(_type_)                                        \
    namespace reflect {                                                 \
    template<>                                                          \
    struct Reflect<_type_>                                              \
    {                                                                   \
        typedef _type_ T_;                                              \
        static constexpr const char* id = reflectStringify(_type_);     \
                                                                        \
        static void reflect(Type*);                                     \
    };                                                                  \
    } // namespace reflect


#define reflectLoader(_type_)                                   \
    namespace reflect {                                         \
    template<>                                                  \
    struct Loader<_type_>                                       \
    {                                                           \
        Loader()                                                \
        {                                                       \
            typedef Reflect<_type_> type;                       \
            Registry::add(type::id, [] (Type* type_) {          \
                        type::reflect(type_);                   \
                    });                                         \
        }                                                       \
    };                                                          \
    namespace { Loader<_type_> reflectUniqueName(loader); }     \
    } // namespace reflect


#define reflectClassImpl(_type_)                        \
    reflectLoader(_type_)                               \
                                                        \
    void                                                \
    reflect::Reflect<_type_>::                          \
    reflect(reflectUnused Type* type_)

#define reflectClass(_type_)                  \
    reflectClassDecl(_type_)                  \
    reflectClassImpl(_type_)

} // reflect
