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

    int bar;

private:
    int foo_;
};

namespace reflect {

template<>
struct Reflect<Foo>
{
    static constexpr const char* id = "Foo";
    static Type* create() { return new Type(id); }
    static void reflect(Type* type);
};

} // namespace reflect


template<typename T, typename Obj>
void reflectGetter(Type* type, const std::string& name, T (Obj::* getter)() const)
{
    type->add(name, getter);
}

template<typename T>
void reflectGetter(Type*, const std::string&, T) {}


template<typename T, typename Obj>
void reflectSetter(Type* type, const std::string& name, void (Obj::* setter)(T))
{
    type->add(name, setter);
}

template<typename T>
void reflectSetter(Type*, const std::string&, T) {}


#define reflectField(type, T, field)                                    \
    do {                                                                \
        type->add(#field,                                               \
                [] (const T& obj) -> const decltype(Foo::field)& {      \
                    return obj.field;                                   \
                });                                                     \
        type->add(#field,                                               \
                [] (T& obj, decltype(Foo::field) value) {               \
                    obj.field = std::move(value);                       \
                });                                                     \
    } while(false)

#define reflectGetSet(type, T, field)                     \
    do {                                                  \
        reflectGetter(type, #field, &Foo::field);         \
        reflectSetter(type, #field, &Foo::field);         \
    } while(false);

void
reflect::Reflect<Foo>::
reflect(Type* type)
{
    printf("\ngetter(foo)\n"); reflectGetter(type, "foo", &Foo::foo);
    printf("\nsetter(foo)\n"); reflectSetter(type, "foo", &Foo::foo);
    printf("\nfield(bar)\n"); reflectField(type, Foo, bar);

    printf("\ngetset(foo)\n"); reflectGetSet(type, Foo, foo);
    printf("\ngetset(blah)\n"); reflectGetSet(type, Foo, blah);
    printf("\ngetset(bleh)\n"); reflectGetSet(type, Foo, bleh);
    printf("\ngetset(bar)\n"); reflectGetSet(type, Foo, bar);
}



BOOST_AUTO_TEST_CASE(blah)
{
    Registry::get<Foo>();
}
