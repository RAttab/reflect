/* blah_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Blah
*/

#define REFLECT_USE_EXCEPTIONS 1

#include <iostream>

// #include "reflect.h"
// #include "reflect/all.h"
// #include "types/primitives.h"

using namespace std;

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
struct Blah
{

    static struct Loader
    {
        Loader() { std::cerr << "@@@ Blah loaded\n"; }
    } loader;
};

template<typename T>
typename Blah<T>::Loader Blah<T>::loader = {};


int main(int, char**)
{
    (void) Blah<int>::loader;

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
