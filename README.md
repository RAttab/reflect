# Reflect #

Yet another reflection system for C++.

## Why? ##

I can hear your cries of anguish: Why another relfection system? Doesn't the
world have enough of them already? Well yes except that reflect does thing a bit
differently then the other reflection systems that I've encountered so far:

* Reflects functions
* Macro-based type description DSL

The next sections will delve into why these two features makes reflect extra
special.


## Function Reflection ##

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

Value foo = type("Foo")->construct();
int sum = foo.call<int>("bar", 1, foo.call<Value>("bar", 10, 100));
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
as the const-ness of each function parameter. In other words, whenever you
manipulate a reflected object, you do so through function calls which are
typed-check at runtime. Not only that but while making the function calls,
Reflect also checks the reference type and const-ness of the source and target
parameter to determine what type of parameter hand-off should be used (copy,
move or reference). Now that's pretty neat.

Reflect also has support for traits which is a rather simple feature that allows
you to add tags to your types and functions. This allows library writters to
define concepts which can be used to create generic utilties. As an example,
you'll notice that our member `baz` has the `field` trait which indicates that
either a getter or setter function is available for that member. This is then
used by the [`json`](src/utils/json) and [`path`](src/utils/config/path.h)
utility to navigate the object without knowing what the object actually is.

The goal of Reflect is to provide a solid flexible base which can be used to
build highly generic libraries. An example, reflect also includes a
[configuration library](src/utils/config) which uses a json file to create, fill
and link a bunch of objects at runtime. This is used in the
[cubes and tubes](tests/cubes_test.cpp) example to build a simple pipeline
framework.

You can find a brief demo of the reflect core in
[this example](tests/demo_test.cpp). For those who wants to see what makes
Reflect tick, head over to [value_function.h](src/value_function.h).


## Macro-Based DSL ##

The sad reality of C++ is that we have no builtin type introspection mechanism
which means that any reflection system will need to figure out a way to extract
information related to a type. Historically, reflection frameworks have used
external tools to either parse a DSL file or the original source code.

Reflect uses a different approach: use macros to define a DSL which is compiled
alongside the reflected type. The advantage of this approach is that we re-use
all the existing machinery of existing toolchains. As a bonus, this allows us to
also use C++'s [template-based introspection](src/function_type.h) mechanisms to
simplify our DSL down to its bare essentials.

The result, as we've seen in the example of the previous section, is pretty
clean and simple. Not only that but it's also easily extensible because it's all
C++ and macros which you can easily add to. As an example, let's enhanced the
`reflectField` macro so that it also adds a text description for the field:

```c++
#define reflectFieldDesc(field, desc)                                   \
    do {                                                                \
        reflectField(field);                                            \
        type_->add(#field "_desc", [] { return std::string(desc); });   \
    } while(false);
```

Now let's modify our reflection of our `Foo` class to make use of our new macro:

```c++
reflectType(Foo)
{
    reflectPlumbing();
	reflectField(baz, "reflection is fuuuuun.");
    reflectFn(bar);
}

auto desc = type<Foo>->call<std::string>("baz_desc");
// desc = "reflection is fuuuuun."
```

The takeaway here is that if you can represent it as a function, Reflect
supports it.


## So What's the Catch? ##

While Reflect is pretty nice overall, it does have one major downside:

    It makes compilers cry.

In other words, the build time for reflecting types can be quite high. Because
of this, there's been a considerable of effort in Reflect to optimize for
compilation times.

Currently the biggest compilation time gains are made by seperating the
declaration of a reflection from it's definition so it only has to be compiled
once. It also helps to split up reflected types into multiple compilation units
to parallelize the compilation. Reflect is also continuously hand optimized with
the help of [benchmark tests](tests/cperf). Unfortunately, this is a rather slow
and tedious process and C++ currently has virtually no tools to help out so
results have been a mixed so far.


## Build ##

Reflect is entirely build on top of C++11 and should therefor run anywhere where
C++11 is supported. Here's a rundown of known compilers that can compile reflect.

* gcc v4.7+
* clang v??
* mvcc v??

The only external dependency at the moment is `boost-test` which is used for all
the reflect tests. Once that's taken care of, here's how you build:

```
cmake .
make -kj8
make test
```

This will produce the following libraries which you'll want to link with:

```
libreflect.so
libreflect_std.so
libreflect_reflect.so
libreflect_primitives.so
```
