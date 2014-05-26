# Reflect #

Yet another reflection system for C++.


## Why? ##

The world of C++ already has several excellent reflection system so why build a
new one? The answer is that Reflect does things differently then the existing
reflection systems that I'm aware of. The two big differentiating features are
as follow:

* It reflects functions
* It uses a macro-based type description DSL

We'll dig into these features and see exactly why it makes Reflect extra special
in the following sections. But first, here's a few more buzzword to pad out the
description:

* C++11
* Extensible
* Non-intrusive
* Lazy type loading
* Self-contained (no external tool required)
* Dark template voodoo-magic

Before we get started, note that reflection systems are usually meant as a mean
to an end. In other words, they provide basic tools that can be used by library
writers to provide nice libraries for their users. As an example, Reflect
includes a [configuration library](src/utils/config) which uses a json file to
create, fill and link a bunch of objects at runtime. This is then used in the
[cubes and tubes example](tests/cubes_test.cpp) example to build a simple
pipeline framework from a [json file](tests/data/cubes.json).


## Function Reflection ##

Reflect started as an experiment to see if it was possible to reflect any
function and make a call to it through the reflection system. As is often the
case with C++, given enough template voodoo just about anything is possible. And
so Reflect was born.

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
not something we're used to being able to do. You may also be glad to hear that
it's completely type safe and Reflect will raise an error if you try to make an
invalid function call.

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
also keeps track of the reference type as well as the const-ness of each
function parameter. In other words, whenever you manipulate a reflected object,
you do so through function calls which are checked for type, reference-ness and
const-ness at runtime. The rules emulated relatively similar to what the C++
standard allows which means that Reflect also uses the proper hand-off method
for the giving argument-parameter pair (copy, move or reference).

What about that traits thing? Traits in Reflect are tags that can be associated
with types and functions. They allow library writters to define concepts which
can be used to create generic utilties. As an example, the `field` trait
indicates that either a getter or setter function is available in the overload
pool. This can then used be used by utilities like [`json`](src/utils/json) and
[`path`](src/utils/config/path.h) to navigate an object completely generically.

The list of features supported by Reflect is a bit too long to fit here but you
can find a brief overview in [this example](tests/demo_test.cpp). For those who
are curious to know what makes Reflect tick, head over to
[value_function.h](src/value_function.h) and enjoy the templates.


## Macro-Based DSL ##

The sad reality of C++ is that we have no builtin type introspection mechanism
which means that any reflection system will need to figure out a way to extract
a type's make up. Historically, reflection systems have used external tools to
either parse a DSL file or the original source code.

Reflect uses a different approach: it uses macros to define a DSL which is
compiled alongside the reflected type. The advantage of this approach is that we
re-use all the existing machinery of the toolchain that is already familliar. As
a bonus, this allows us to also use C++'s
[template-based introspection](src/function_type.h) mechanisms to simplify our
DSL down to its bare essentials.

The result, as we've seen in the example of the previous section, is pretty
clean and simple. It's also also easily extensible because the macros uses the
same simple interface that is available to the user of the library. As an
example, let's enhance the `reflectField` macro so that it also adds a text
description for the field:

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
	reflectField(baz, "well, that was easy");
    reflectFn(bar);
}

auto desc = type<Foo>->call<std::string>("baz_desc");
// desc = "well, that was easy"
```

The key takeaway here is that, if you can represent an extension as a function,
then Reflect supports it with no modifications to the core. Neat.


## So What's the Catch? ##

While Reflect is pretty nice overall, it does have one major downside:

    It makes compilers cry.

In other words, the build time for reflecting types can be quite high. Because
of this, there's been a considerable amount of effort dedicated to optimizing
for compilation times using a suite of [benchmark tests](tests/cpef). Note that
this is an ongoing area of development and there are known compilation
bottlenecks (reflection of template classes) which still need to be resolved.

Note that while compilation times can be high when reflecting types, Reflect can
seperate the declaration of the reflection from its definition. This means that
definitions will usually be compiled once and used many times at no additional
costs. It also helps to spread out the definitions over multiple compilation
units to parallelize the compilation.


## How Do I Use It? ##

The biggest requirement for Reflect is a C++11 compliant compiler and any
version of boost that has `boost-test`. Here's a rundown of known compilers that
can compile Reflect.

* gcc v4.7+
* clang v??
* mvcc v??

Reflect uses `cmake` as its build system which should be available in your
distribution or at the [cmake website](). Once you have it, building is
straightforward:

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

The headers for Reflect are in the src folder and organized into one main header
and 2 folders. Here's what a typical include set might look like.

```c++
#include "reflect/reflect.h"
#include "reflect/dsl/all.h"
#include "reflect/types/primitives.h"
#include "reflect/types/std/string.h"
```

The `reflect/reflect.h` header includes the core library. The `reflect/reflect`
folder contains the the macro based
