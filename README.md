# Reflect #

Yet another reflection system for C++.


## Why? ##

C++ already has several excellent reflection systems so why build a new one? To
put it simply, Reflect does things differently:

* It reflects functions
* It uses a macro-based type description DSL (domain-specific language)

We'll dig into these features and see exactly why it makes Reflect extra special
but first, here's a few more buzzword to pad out the description:

* C++11
* Extensible
* Non-intrusive
* Lazy type loading
* Self-contained (no external tool required)
* Imbued with dark template voodoo-magic

Before we get started, note that reflection systems are usually meant as a mean
to an end. In other words, they provide basic tools that can be used by library
writers to construct libraries for their users. As an example, Reflect includes
a [configuration library](src/utils/config) which uses a json file to create,
fill and link a bunch of objects at runtime. This is then used in the
[cubes and tubes example](tests/cubes_test.cpp) to build a simple pipeline
framework from a [json file](tests/data/cubes.json).


## Function Reflection ##

It all started as an experiment to see if it was possible to reflect any given
function and make a call to it in a type-safe manner. After having made the
required sacrifice to the proper template voodoo gods, Reflect was born.

Here's a small taste of function reflection:

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
assert(sum == 111);
```

As a C++ programmer, those last 3 lines of code might feel a little... off. It's
not something we're used to being able to do and, to make things even more
weird, those lines are also completely type-safe which means that Reflect will
raise an error if it detects an attempt at making an invalid function call.

To get a better idea of what's going on, let's ask Reflect to dump what it knows
about the `Foo` class.

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

Well this is interesting; there are only functions here even for our `baz` field
which was just a plain `int` in the original type. Not only that but Reflect
also keeps track of the reference-ness and the const-ness of each function
parameter. In other words, whenever a reflected object is manipulated, it is
done through function calls which are checked for type, reference-ness and
const-ness at runtime. The rules emulated are relatively similar to what you
would expect from a regular function call which means that the proper hand-off
method (copy, move or reference) is used for the given argument-parameter pair.

What about that traits thing? Traits in Reflect are tags that can be associated
with types and functions. They allow library writers to define concepts which
can be used to create generic utilities. As an example, the `field` trait
indicates that either a getter or setter function is available in the overload
pool. This can then used be used by utilities like [`json`](src/utils/json) and
[`path`](src/utils/config/path.h) to list the members of an object and navigate
it completely generically.

The list of features supported by Reflect is actually quite a bit longer then
what I can fit here but you can find a brief overview in
[this example](tests/demo_test.cpp). For those who are curious to know what
makes Reflect tick, head over to [value_function.h](src/value_function.h) and
enjoy the templates.


## Macro-Based DSL ##

The sad reality of C++ is that we have no built-in type introspection mechanism
which means that any reflection system will need to figure out a way to extract
a type's make up. Historically, reflection systems have used external tools to
process either a DSL file or the original source code.

Reflect takes another approach: it uses macros to define a DSL which is compiled
alongside the reflected type. The advantage is that we re-use all the existing
machinery of the tool-chains that is already familiar to the user. As a bonus,
this allows the DSL to use C++'s template-based introspection mechanisms to
simplify our DSL down to its bare essentials.

The result, as we've seen in the example of the previous section, is pretty
clean and simple. It's also easily extensible because the macros use the same
simple interface that is available to the user of the library. As an example,
let's enhance the `reflectField` macro so that it also adds a text description
for the field:

```c++
#define reflectFieldDesc(field, desc)                                   \
    do {                                                                \
        reflectField(field);                                            \
        type_->add(#field "_desc", [] { return std::string(desc); });   \
    } while(false)
```

Now let's modify our reflection of `Foo` to make use of our new macro:

```c++
reflectType(Foo)
{
    reflectPlumbing();
	reflectFieldDesc(baz, "well, that was easy");
    reflectFn(bar);
}

auto desc = type<Foo>()->call<std::string>("baz_desc");
assert(desc == "well, that was easy");
```

The key take-away here is that, if you can represent an extension as a function,
then Reflect supports it with no modifications to the core. Handy.


## So What's the Catch? ##

While Reflect is pretty nice overall, it does have one major downside:

    It makes compilers cry.

In other words, the build time for reflecting types can be quite high. Because
of this, there's been a considerable amount of effort dedicated to optimizing
for compilation times using a suite of [benchmark tests](tests/cperf). Note that
this is an ongoing area of development and there are known compilation
bottlenecks which still need to be resolved (eg. reflection of templated
classes).

While compilation times can be high when reflecting types, Reflect allows for
the separation of the declaration of the reflection from its definition. This
means that definitions, which are expensive, will only need to be compiled once
while the declaration, which are cheap, can be used many times at no additional
cost. It also helps if the definitions are spread out over multiple compilation
units to parallelize the compilation.


## How Do I Use It? ##

The requirements for Reflect are a C++11 compliant compiler and any version of
boost that has `boost-test`. Here's a rundown of known compilers that can
compile Reflect:

* gcc v4.7+
* clang v??
* mvcc v??

Reflect uses `cmake` as its build system which should be available in your
distribution's package manager or at the
[cmake website](http://www.cmake.org). Once that's dealt with, building is
straightforward:

```
cmake .
make -kj8
make test
```

This will produce the following libraries in the `lib` folder which you'll want
to link with:

```
libreflect.so
libreflect_std.so
libreflect_reflect.so
libreflect_primitives.so
```

The headers for Reflect are in the `src` folder and organized into one main header
and 2 sub-folders. Here's what a typical include set might look like.

```c++
#include "reflect.h"
#include "dsl/all.h"
#include "types/primitives.h"
#include "types/std/string.h"
```

The `reflect.h` header includes the core library, the `dsl` folder contains the
macros for the DSL and the `types` folder contains the default reflection for
commonly used types.

That's it!
