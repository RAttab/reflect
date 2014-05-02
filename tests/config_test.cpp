/* config_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 29 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for the reflection-based configuration system.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "reflect/type.h"
#include "reflect/field.h"
#include "reflect/function.h"
#include "reflect/plumbing.h"
#include "types/std/string.h"
#include "types/std/vector.h"
#include "utils/config/includes.h"

#include <boost/test/unit_test.hpp>
#include <fstream>

using namespace reflect;


/******************************************************************************/
/* TUBE                                                                       */
/******************************************************************************/

template<typename T>
struct Tube
{
    void push(T v) { value = std::move(v); }
    T pull() const { return connection->value; }

    void connect(Tube<T>& tube) { connection = &tube; }

private:
    T value;
    Tube<T>* connection;
};


namespace reflect {

template<typename T>
struct Reflect< Tube<T> >
{
    typedef Tube<T> T_;
    static std::string id() { return "Tube<" + typeId<T>() + ">"; }
    reflectTemplateLoader()

    static void reflect(Type* type_)
    {
        reflectPlumbing();
        reflectTrait(tube);
        reflectField(push);
        reflectField(pull);
        reflectFn(connect);
    }
};

} // namespace reflect


/******************************************************************************/
/* CONST CUBE                                                                 */
/******************************************************************************/

struct StringCube
{
    void run() { out.push(value); }

    std::string value;
    Tube<std::string> out;
};

reflectType(StringCube)
{
    reflectPlumbing();
    reflectAllocPlumbing();
    reflectField(value);
    reflectField(out);
    reflectFn(run);
}


/******************************************************************************/
/* CONCAT CUBE                                                                */
/******************************************************************************/

struct ConcatCube
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
    reflectPlumbing();
    reflectAllocPlumbing();
    reflectField(inFirst);
    reflectField(inSecond);
    reflectField(out);
    reflectFn(run);
}


/******************************************************************************/
/* PRINT CUBE                                                                 */
/******************************************************************************/

struct PrintCube
{
    void run() { std::cerr << in.pull() << std::endl; }

    Tube<std::string> in;
};

reflectType(PrintCube)
{
    reflectPlumbing();
    reflectAllocPlumbing();
    reflectField(in);
    reflectFn(run);
}


/******************************************************************************/
/* TEST                                                                       */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(CubesAndTubes)
{
    std::ifstream ifs("tests/data/cubes.json");

    config::Config cubes;

    std::cerr << "loading..." << std::endl;
    config::loadJson(cubes, ifs);

    (*cubes["hello"]).call<void>("run");
    (*cubes["world"]).call<void>("run");
    (*cubes["concat"]).call<void>("run");
    (*cubes["print"]).call<void>("run");

    // cuebs["runner"].call<void>("run");
}
