/* blah_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Blah
*/

#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "dsl/all.h"
#include "types/primitives.h"
#include "types/std/string.h"

#include <iostream>

using namespace std;
using namespace reflect;

struct Bar {};
struct Foo
{
    operator Bar() const { return Bar(); }
};

struct Baz : public Bar {};

void value(Bar) {}
void lRef(Bar&) {}
void constLRef(const Bar&) {}
void rRef(Bar&&) {}

Foo g;

Foo retValue() { return {}; }
Foo& retLRef() { return g; }
const Foo& retConstLRef() { return g; }


struct NoCons
{
    NoCons(const NoCons&) = default;
    NoCons& operator=(const NoCons&) = default;
    NoCons(NoCons&&) = default;
    NoCons& operator=(NoCons&&) = default;

    NoCons() = delete;
};

struct NoCopy
{
    NoCopy() = default;
    NoCopy(NoCopy&&) = default;
    NoCopy& operator=(NoCopy&&) = default;

    NoCopy(const NoCopy&) = delete;
    NoCopy& operator=(const NoCopy&) = delete;
};

struct NoMove
{
    NoMove() = default;
    NoMove(const NoMove&) = default;
    NoMove& operator=(const NoMove&) = default;

    NoMove(NoMove&&) = delete;
    NoMove& operator=(NoMove&&) = delete;
};


template<typename T>
void bleh()
{
    std::cerr << "def cons:  " << std::is_default_constructible<T>() << std::endl;
    std::cerr << "copy cons: " << std::is_copy_constructible<T>() << std::endl;
    std::cerr << "copy assg: " << std::is_copy_assignable<T>() << std::endl;
    std::cerr << "move cons: " << std::is_move_constructible<T>() << std::endl;
    std::cerr << "move assg: " << std::is_move_assignable<T>() << std::endl;
}

void ptr(int const*) {}


struct Foo2 {
    int bar(int a, int b) const { return a + b; }
    int baz;
};


reflectType(Foo2)
{
    reflectPlumbing();
    reflectField(baz);
    reflectFn(bar);
}


int main(int, char**)
{
    std::cerr << type("Foo2")->print() << std::endl;
    std::cerr << type("Foo2")->call<std::string>("baz_desc") << std::endl;

    printf("\ncopy:\n"); bleh<typename CleanType<int>::type>();
    printf("\nlref:\n"); bleh<typename CleanType<int&>::type>();
    printf("\ncref:\n"); bleh<typename CleanType<const int&>::type>();
    printf("\nrref:\n"); bleh<typename CleanType<int&&>::type>();

    std::cerr << std::endl;
    std::cerr << "copy: " << std::is_pointer<int*>::value << std::endl;
    std::cerr << "lref: " << std::is_pointer<int*&>::value << std::endl;
    std::cerr << "rref: " << std::is_pointer<int*&&>::value << std::endl;
    std::cerr << "cref: " << std::is_pointer<int* const &>::value << std::endl;
    std::cerr << std::endl;

    {
        Baz baz;
        constLRef(baz);
    }

    {
        Foo foo;
        value(foo);
        // lRef(foo);
        constLRef(foo);
        rRef(foo);
    }

    {
        const Foo foo;
        value(foo);
        // lRef(foo);
        constLRef(foo);
        rRef(foo);
    }

    {
        value(Foo());
        // lRef(Foo());
        constLRef(Foo());
        rRef(Foo());
    }

    {
        (Bar) retValue();
        const Bar& bar = retLRef();
        (void) bar;
        (const Bar&) retConstLRef();
    }

}
