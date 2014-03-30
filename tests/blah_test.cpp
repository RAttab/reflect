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
    const int& foo() const { return foo_; }
    void foo(int i) { foo_ = i; }

    const int& blah() const { return foo_; }
    void bleh(int i) { foo_ = i; }

    void copy(int i) { woo = i; }
    int copy() const { return woo; }

    void lValue(int& i) { woo = i; }
    int& lValue() { return woo; }

    void constLValue(const int& i) { woo = i; }
    const int& constLValue() const { return woo; }

    void rValue(int&& i) { woo = std::move(i); }
    int rValue() { return std::move(woo); }

    int bar;

private:
    int foo_;
    int woo;
};


/******************************************************************************/
/* REFLECT GETTER                                                             */
/******************************************************************************/

template<typename T, typename Obj>
void reflectGetter(Type* type, const std::string& name, T (Obj::* getter)() const)
{
    type->add(name, getter);
}

template<typename T, typename Obj>
void reflectGetter(Type* type, const std::string& name, T (Obj::* getter)())
{
    type->add(name, getter);
}

template<typename T>
void reflectGetter(Type*, const std::string&, T) {}


/******************************************************************************/
/* REFLECT SETTER                                                             */
/******************************************************************************/

template<typename T, typename Obj>
void reflectSetter(Type* type, const std::string& name, void (Obj::* setter)(T))
{
    type->add(name, setter);
}

template<typename T>
void reflectSetter(Type*, const std::string&, T) {}


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
void reflectMember(Type* type, const std::string& name, T Obj::* field)
{
    type->add(name,
            [=] (const Foo& obj) -> const T& {
                return obj.*field;
            });

    type->add(name,
            [=] (Foo& obj, T value) {
                obj.*field = std::move(value);
            });
}

// Used to disambiguate fields that have both getter and setter.
template<typename T, typename Obj>
void reflectMember(Type*, const std::string&, void (Obj::*)(T)) {}

template<typename T>
void reflectMember(Type*, const std::string&, T) {}


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
    printf("\ngetter(foo)\n"); reflectGetter(type, "foo", &Foo::foo);
    printf("\nsetter(foo)\n"); reflectSetter(type, "foo", &Foo::foo);
    printf("\nmember(bar)\n"); reflectMember(type, "foo", &Foo::bar);

    printf("\nfield(foo)\n");  reflectField(type, Foo, foo);
    printf("\nfield(blah)\n"); reflectField(type, Foo, blah);
    printf("\nfield(bleh)\n"); reflectField(type, Foo, bleh);
    printf("\nfield(bar)\n");  reflectField(type, Foo, bar);

    printf("\nfield(copy)\n"); reflectField(type, Foo, copy);
    printf("\nfield(lValue)\n"); reflectField(type, Foo, lValue);
    printf("\nfield(constLValue)\n"); reflectField(type, Foo, constLValue);
    printf("\nfield(rValue)\n"); reflectField(type, Foo, rValue);
}

BOOST_AUTO_TEST_CASE(blah)
{
    Registry::get<Foo>();
}
