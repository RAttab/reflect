/* demo_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Demo of the reflection system.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "reflect.h"
#include "dsl/all.h"
#include "types/std/map.h"
#include "types/std/smart_ptr.h"

#include <boost/test/unit_test.hpp>

using namespace reflect;

/******************************************************************************/
/* TEST                                                                       */
/******************************************************************************/

namespace test {

int counter = 0;
int inc(int i) { return counter += i; }

} // namespace test

reflectScope(test)
{
    reflectGlobalVar(test::counter);
    reflectGlobalFn(test::inc);
}


/******************************************************************************/
/* ZORISH                                                                     */
/******************************************************************************/

namespace test {

struct Zorish
{
    Zorish() : value(0) {}
    Zorish(int value) : value(value) {}

    int value;

    Zorish operator+ (const Zorish& other) const
    {
        return Zorish(value + other.value);
    }

    operator int() const { return value; }
};

} // namespace test


reflectType(test::Zorish)
{
    reflectPlumbing();
    reflectCons(int);

    reflectField(value);

    reflectOpCast(int);
    reflectOpArithmetic();
}


/******************************************************************************/
/* THINGY                                                                     */
/******************************************************************************/

namespace test {

struct Thingy : public Zorish
{
    std::map<int, int> bob;

    Thingy(int value) : Zorish(value) {}

    static std::shared_ptr<Thingy> create(int value)
    {
        auto thingy = std::make_shared<Thingy>(value);
        thingy->bob[10] = 100;
        thingy->bob[20] = 200;
        return thingy;
    }
};

} // namespace test


reflectType(test::Thingy)
{
    reflectPlumbing();
    reflectParent(test::Zorish);

    reflectCons(int);
    reflectField(bob);
    reflectFn(create);

    reflectCustom(weee) (test::Thingy& obj, int i) {
        return obj.bob[i] + i;
    };

    reflectTypeTrait(thing);
    reflectFnTrait(weee, interesting);
}


/******************************************************************************/
/* SCOPE                                                                      */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(scope_)
{
    const Scope* sGlobal = scope();
    const Scope* sTest = scope("test");
    BOOST_CHECK_EQUAL(sGlobal->scope("test"), sTest);

    std::cerr << sGlobal->print() << std::endl;

    sTest->call<void>("inc", 10);
    BOOST_CHECK_EQUAL(test::counter, 10);
}


/******************************************************************************/
/* TYPE                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(type_)
{
    const Type* tThingy = type("test::Thingy");
    BOOST_CHECK_EQUAL(type<test::Thingy>(), tThingy);

    std::cerr << tThingy->print() << std::endl;
    BOOST_CHECK(tThingy->isChildOf<test::Zorish>());

    Value vThingy = tThingy->construct(10);
    BOOST_CHECK(tThingy->is("thing"));

    BOOST_CHECK(tThingy->hasField("value"));
    BOOST_CHECK_EQUAL(tThingy->field("value").type(), type<int>());
    BOOST_CHECK_EQUAL(vThingy.field<int>("value"), 10);

    BOOST_CHECK(tThingy->function("weee").is("interesting"));
    BOOST_CHECK(tThingy->hasFunction("weee"));
}


/******************************************************************************/
/* CONTAINERS                                                                 */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(containers)
{
    const Type* tThingy = type<test::Thingy>();

    Value thingy = tThingy->call<Value>("create", 10);
    Value bob = (*thingy).field<Value>("bob");

    BOOST_CHECK(bob.is("map"));
    BOOST_CHECK_EQUAL(bob.type()->getValue<const Type*>("keyType"), type<int>());

    auto keys = bob.call< std::vector<int> >("keys");

    std::cerr << "bob = { ";
    for (const auto& key : keys) {
        std::cerr << key << ":" << bob[key].get<int>() << " ";
    }
    std::cerr << "}" << std::endl;
}


/******************************************************************************/
/* OPERATORS                                                                  */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(operators)
{
    const Type* tThingy = type<test::Thingy>();
    const Type* tZorish = type<test::Zorish>();

    auto sum = Value(1) + tZorish->construct(10) + tThingy->construct(100);
    std::cerr << "sum is a " << sum.typeId() << std::endl;
    std::cerr << "sum = " << sum.get<int>() << std::endl;
}
