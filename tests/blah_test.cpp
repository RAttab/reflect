/* blah_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Experimental tests
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "types/primitives.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;

struct Foo
{
    Foo() : constField(0) {}

    int field;
    const int constField;

    const int& getter() const { return value; }
    void setter(int i) { value = i; }

    void copy(int i) { value = i; }
    int copy() const { return value; }

    void lValue(int& i) { value = i; }
    int& lValue() { return value; }

    void constLValue(const int& i) { value = i; }
    const int& constLValue() const { return value; }

    void rValue(int&& i) { value = std::move(i); }
    int rValue() { return std::move(value); }

    void function() { value += 1; };

private:
    int value;
};

namespace reflect {

/******************************************************************************/
/* REFLECT GETTER                                                             */
/******************************************************************************/

template<typename T, typename Obj>
void reflectGetter(Type* type, std::string name, T (Obj::* getter)() const)
{
    type->add(std::move(name), getter);
}

template<typename T, typename Obj>
void reflectGetter(Type* type, std::string name, T (Obj::* getter)())
{
    type->add(std::move(name), getter);
}

template<typename T>
void reflectGetter(Type*, std::string, T) {}


/******************************************************************************/
/* REFLECT SETTER                                                             */
/******************************************************************************/

template<typename T, typename Obj>
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
            [=] (const Foo& obj) -> const T& {
                return obj.*field;
            });

    type->add(std::move(name),
            [=] (Foo& obj, T value) {
                obj.*field = std::move(value);
            });
}

template<typename T, typename Obj,
    class = typename std::enable_if<IsMemberPtr<T, Obj>::value>::type>
void reflectMember(Type* type, std::string name, T const Obj::* field)
{
    type->add(std::move(name),
            [=] (const Foo& obj) -> const T& {
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

#define reflectField(type, T, field)                      \
    do {                                                  \
        reflectGetter(type, #field, &Foo::field);         \
        reflectSetter(type, #field, &Foo::field);         \
        reflectMember(type, #field, &Foo::field);         \
    } while(false)


/******************************************************************************/
/* REFLECT FN                                                                 */
/******************************************************************************/

template<typename Fn>
void reflectFunction(Type* type, std::string name, Fn fn)
{
    type->add(std::move(name), std::move(fn));
}

#define reflectFn(type, T, fn)                  \
    do {                                        \
        reflectFunction(type, #fn, &T::fn);     \
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

#define reflectCustom(type, T, name)            \
    reflectLambda(type, #name) += []

} // namespace reflect 


/******************************************************************************/
/* REFLECT FOO                                                                */
/******************************************************************************/

namespace reflect {

template<>
struct Reflect<Foo>
{
    static constexpr const char* id = "Foo";
    static Type* create() { return new Type(id); }
    static void reflect(Type* type);
};

} // namespace reflect

void
reflect::Reflect<Foo>::
reflect(Type* type)
{
    printf("\nfield(field)\n");       reflectField(type, Foo, field);
    printf("\nfield(constField)\n");  reflectField(type, Foo, constField);
    printf("\nfield(copy)\n");        reflectField(type, Foo, copy);
    printf("\nfield(lValue)\n");      reflectField(type, Foo, lValue);
    printf("\nfield(constLValue)\n"); reflectField(type, Foo, constLValue);
    printf("\nfield(rValue)\n");      reflectField(type, Foo, rValue);

    printf("\nfn(function)\n"); reflectField(type, Foo, function);

    printf("\nlambda(custom)\n"); 
    reflectCustom(type, Foo, "custom") (Foo& obj, int a, int b) {
        obj.setter(a + b);
    };
}

BOOST_AUTO_TEST_CASE(blah)
{
    Registry::get<Foo>();
}
