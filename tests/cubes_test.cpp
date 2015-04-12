/* config_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for the reflection-based configuration system.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "dsl/all.h"
#include "types/std/string.h"
#include "types/std/vector.h"
#include "utils/config/includes.h"

#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace reflect;


/******************************************************************************/
/* CUBE                                                                       */
/******************************************************************************/

struct Cube
{
    virtual void run() = 0;
};

reflectType(Cube) { reflectFn(run); }


/******************************************************************************/
/* TUBE                                                                       */
/******************************************************************************/

template<typename T>
struct Tube
{
    void connect(Tube<T>& tube) { connection = &tube; }

    void push(T v) { value = std::move(v); }
    T pull() const { return connection->value; }

private:
    T value;
    Tube<T>* connection;
};

reflectTemplate(Tube, ValueT)
{
    reflectPlumbing();

    reflectTypeTrait(tube);
    reflectCustom(valueType) { return type<ValueT>(); };

    reflectFn(push);
    reflectFn(pull);
    reflectFn(connect);
}


/******************************************************************************/
/* STRING CUBE                                                                */
/******************************************************************************/

struct StringCube : public Cube
{
    void run() { out.push(value); }

    std::string value;
    Tube<std::string> out;
};

reflectType(StringCube)
{
    reflectParent(Cube);

    reflectPlumbing();
    reflectAllocPlumbing();

    reflectField(value);
    reflectField(out);
}


/******************************************************************************/
/* CONCAT CUBE                                                                */
/******************************************************************************/

struct ConcatCube : public Cube
{
    void run()
    {
        out.push(inFirst.pull() + inSecond.pull());
    }

    Tube<std::string> inFirst;
    Tube<std::string> inSecond;
    Tube<std::string> out;
};

reflectType(ConcatCube)
{
    reflectParent(Cube);

    reflectPlumbing();
    reflectAllocPlumbing();

    reflectField(inFirst);
    reflectField(inSecond);
    reflectField(out);
}


/******************************************************************************/
/* PRINT CUBE                                                                 */
/******************************************************************************/

struct PrintCube : public Cube
{
    void run() { std::cerr << in.pull() << std::endl; }

    Tube<std::string> in;
};

reflectType(PrintCube)
{
    reflectParent(Cube);

    reflectPlumbing();
    reflectAllocPlumbing();

    reflectField(in);
}


/******************************************************************************/
/* RUN CUBE                                                                   */
/******************************************************************************/

struct RunCube : public Cube
{
    void run()
    {
        for (auto& cube : cubes) cube->run();
    }

    std::vector<Cube*> cubes;
};

reflectType(RunCube)
{
    reflectParent(Cube);

    reflectPlumbing();
    reflectAllocPlumbing();

    reflectField(cubes);
}


/******************************************************************************/
/* TEST                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(CubesAndTubes)
{
    std::ifstream ifs("tests/data/cubes.json");

    config::Config cubes;
    config::loadJson(cubes, ifs);

    (*cubes["runner"]).call<void>("run");
}
