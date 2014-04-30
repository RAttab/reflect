/* demo_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Demo of the reflection system.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "reflect.h"
#include "reflect/all.h"
#include "types/std/map.h"
#include "types/std/smart_ptr.h"

#include <boost/test/unit_test.hpp>


/******************************************************************************/
/* TEST                                                                       */
/******************************************************************************/

namespace test {

int counter = 0;
int incCounter(int i) { return counter += i; }

} // namespace test

reflectScope(test)
{
    reflectGlobalVar(test::counter);
    reflectGlobalFn(test::incCounter);
}


/******************************************************************************/
/* ZORISH                                                                     */
/******************************************************************************/

namespace test {

struct Zorish
{
    Zorish() : value_(0) {}
    Zorish(int value) : value_(value) {}

    int value() const { return value_; }
    void value(int v) { value_ = v; }

    Zorish operator+ (const Zorish& other) const
    {
        return Zorish(value_ + other.value_);
    }

    operator int() const { return value_; }

private:
    int value_;
};

} // namespace test


reflectType(test::Zorish)
{
    reflectPlumbing();

    reflectCons(int);
    reflectOpCast(int);

    reflectField(value);
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
}


/******************************************************************************/
/* DEMO                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(demo)
{
    const reflect::Scope* sGlobal = reflect::scope();
    const reflect::Scope* sTest = reflect::scope("test");
    std::cerr << sGlobal->print() << std::endl;

    const reflect::Type* tThingy = reflect::type("test::Thingy");
    const reflect::Type* tZorish = reflect::type<test::Zorish>();
    std::cerr << tThingy->print() << std::endl;


    auto thingy = tThingy->call<reflect::Value>("create", 100);
    std::cerr << "ptr is " << thingy.type()->pointer()
        << " of " << thingy.type()->pointee()->id()
        << std::endl;


    reflect::Value bob = (*thingy).get<reflect::Value>("bob");

    BOOST_CHECK(bob.is("map"));
    BOOST_CHECK_EQUAL(
            bob.type()->call<const reflect::Type*>("keyType"),
            reflect::type<int>());

    auto keys = bob.call< std::vector<int> >("keys");

    std::cerr << "bob = { ";
    for (const auto& key : keys) {
        std::cerr << key << ":" << bob[key].get<int>() << " ";
    }
    std::cerr << "}" << std::endl;


    auto sum = tZorish->construct(10) + *thingy;
    std::cerr << "sum is a " << sum.typeId() << std::endl;

    sTest->call<void>("incCounter", sum);
    std::cerr << "counter is now " << test::counter << std::endl;
}
