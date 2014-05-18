Reflect
=======

Yet another reflection system for C++.

Why?
====

I can hear your cries of anguish: Why another relfection system? Doesn't the
world have enough of them already? Well yes except that reflect does thing a bit
differently then the other reflection systems that I've encountered so far:

* Reflects functions
* Macro-based type description DSL

The next sections will delve into why these two features makes reflect extra
special.

Function Reflection
===================

Reflect started as an experiment to see if it was possible to reflect any
function and then call it through the reflection system. As usual in C++, just
about anything is possible given enough templating voodoo and so this library
was born. Here's a small taste of function reflection:

```c++
struct Foo
{
    int bar(int a, int b) const { return a + b; }
	int baz;
};

reflectType(Foo)
{
    reflectPlumbing();
	reflectField(baz);
    reflectFn(bar);
}

Value vFoo = type("Foo")->construct();
int sum = vFoo.call<int>("bar", 1, vFoo.call<Value>("bar", 10, 100));
// sum == 111;
```

As a C++ programer, there's something about that code that just feels wrong. It
looks like somebody misplaced some python in C++! That can't possibly work...
can it? Not only does it work but it's also completely type safe (take that
python!).

To get a better idea of what's going on, let's ask Reflect to dump what it knows
about the Foo class.

```c++
std::cerr << type("Foo")->print() << std::endl;
```
```
type Foo
{
    bar:
        int(Foo const&, int, int)
    baz:
        traits: [ field ]
        int&(Foo&)
        int const&(Foo const&)
        void(Foo&, int)
    operator=:
        Foo&(Foo&, Foo const&)
        Foo&(Foo&, Foo&&)
    Foo:
        Foo()
        Foo(Foo const&)
        Foo(Foo&&)
}
```

Well this is interesting; there are only functions here even for our `baz`
field. Not only that but Reflect also keeps track of the reference type as well
as the const-ness of each function parameter.

The takeway here is that, if you're capable of representing it as a function
then Reflect can handle it. You can find a brief overview of the supported
features in [this example](tests/demo_test.cpp). For those who wants to see what
makes Reflect tick, head over to [value_function.h](src/value_function.h).


Macro-Based DSL
===============

The sad reality of C++ is that we have no builtin type introspection mechanism
which means that any reflection system will need to figure out a way to extract
the make up of a type
