/* parser_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 18 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "utils/json/json.h"
#include "types/primitives.h"
#include "types/std/string.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;
using namespace reflect::json;


/******************************************************************************/
/* PATH PARSER                                                                */
/******************************************************************************/

struct PathParser
{
    reflect::json::Error parse(const std::string& str)
    {
        std::istringstream stream(str);
        Reader reader(stream);
        parse(reader, "");
        return reader.error();
    }

    void check(const std::vector< std::pair<std::string, Value> >& exp) const
    {
        auto printVec = [] (const std::vector< std::pair<std::string, Value> > v) {
            std::cerr << "\n";
            for (const auto& item : v)
                std::cerr << "    <" << item.first << "> -> " << item.second.typeId() << '\n';
        };

        std::cerr << "VALUE: "; printVec(items);
        std::cerr << "EXP:   "; printVec(exp);

        BOOST_CHECK_EQUAL(items.size(), exp.size());
        if (items.size() != exp.size()) return;

        for (size_t i = 0; i < items.size(); ++i) {
            BOOST_CHECK_EQUAL(items[i].first, exp[i].first);
            if (items[i].first != exp[i].first) continue;

            std::cerr << "<" << items[i].first << "> -> ";

            const Type* t = items[i].second.type();
            bool ok = false;

            if (t == type<void>())
                ok = checkVoid(items[i].second, exp[i].second);

            else if (t == type<bool>())
                ok = check<bool>(items[i].second, exp[i].second);

            else if (t == type<int64_t>())
                ok = check<int64_t>(items[i].second, exp[i].second);

            else if (t == type<double>())
                ok = check<double>(items[i].second, exp[i].second);

            else if (t == type<std::string>())
                ok = check<std::string>(items[i].second, exp[i].second);

            else BOOST_CHECK(false);

            BOOST_CHECK(ok);
            if (ok) std::cerr << "OK" << std::endl;
        }
    }

private:

    bool checkVoid(Value lhs, Value rhs) const
    {
        if (lhs.type() != type<void>() || rhs.type() != type<void>()) {
            std::cerr << "ERROR: type " << lhs.typeId() << " != " <<  rhs.typeId() << std::endl;
            return false;
        }

        return true;
    }

    template<typename T>
    bool check(Value lhs, Value rhs) const
    {
        if (lhs.type() != type<T>() || rhs.type() != type<T>()) {
            std::cerr << "ERROR: type " << lhs.typeId() << " != " <<  rhs.typeId() << std::endl;
            return false;
        }

        if (lhs.cast<T>() != rhs.cast<T>()) {
            std::cerr << "ERROR: value " << lhs.cast<T>() << " != " <<  rhs.cast<T>() << std::endl;
            return false;
        }

        return true;
    }

    void parse(Reader& reader, const std::string& prefix)
    {
        auto token = reader.peekToken();

        switch (token.type()) {

        case Token::Null:
            reader.nextToken();
            items.emplace_back(prefix, Value());
            return;

        case Token::Bool:
            items.emplace_back(prefix, Value(parseBool(reader)));
            return;

        case Token::Int:
            items.emplace_back(prefix, Value(parseInt(reader)));
            return;

        case Token::Float:
            items.emplace_back(prefix, Value(parseFloat(reader)));
            return;

        case Token::String:
            items.emplace_back(prefix, Value(parseString(reader)));
            return;

        case Token::ArrayStart:
            parseArray(reader, [&](size_t i) {
                        parse(reader, prefix + '.' + std::to_string(i)); });
            return;

        case Token::ObjectStart:
            parseObject(reader, [&] (const std::string& key) {
                        parse(reader, prefix + '.' + key);});
            return;

        default:
            reader.error("unknown token type %s at <%s>", token.print(), prefix);
            return;
        }
    }

    std::vector< std::pair<std::string, Value> > items;
};


void check(const std::string& str, std::vector< std::pair<std::string, Value> > exp)
{
    PathParser parser;

    std::cerr << "\nPARSING: " << str << std::endl;

    reflect::json::Error err = parser.parse(str);
    if (err) {
        std::cerr << "ERROR: " << err.what() << std::endl;
        BOOST_CHECK(!err);
        return;
    }

    parser.check(exp);
}

std::pair<std::string, Value>
p(std::string key, nullptr_t)
{
    return std::make_pair(key, Value());
}

std::pair<std::string, Value>
p(std::string key, bool value)
{
    return std::make_pair(key, Value(std::move(value)));
}

std::pair<std::string, Value>
p(std::string key, int value)
{
    return std::make_pair(key, Value(int64_t(value)));
}

std::pair<std::string, Value>
p(std::string key, double value)
{
    return std::make_pair(key, Value(std::move(value)));
}

std::pair<std::string, Value>
p(std::string key, const char* value)
{
    return std::make_pair(key, Value(std::string(value)));
}

/******************************************************************************/
/* TESTS                                                                      */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(test_basics)
{
    check("null", { p("", nullptr) });
    check("true", { p("", true) });
    check("false", { p("", false) });

    check("123", { p("", 123) });
    check("123.321", { p("", 123.321) });

    check("\"hello\"", { p("", "hello") });

    check("[]", {});
    check("[ true ]", { p(".0", true) });
    check("[ null, true, false, 123, 123.321, \"hello\" ]", {
                p(".0", nullptr), p(".1", true), p(".2", false),
                p(".3", 123), p(".4", 123.321), p(".5", "hello")});

    check("[ [ true ] ]", { p(".0.0", true) });
    check("[ 1, [ 2 ], [], 3 ]", {p(".0", 1), p(".1.0", 2), p(".3", 3) });

    check("{}", {});
    check("{ \"a\": true }", { p(".a", true) });
    check("{ \"a\": null, \"b\": true, \"c\": 123, \"d\": \"hello\" }", {
                p(".a", nullptr), p(".b", true), p(".c", 123), p(".d", "hello") });

    check("{ \"a\": { \"b\": true } }", { p(".a.b", true) });
    check("{ \"a\": [ 1, 2 ] }", { p(".a.0", 1), p(".a.1", 2) });
    check("{ \"a\": 1, \"b\": { \"c\": 2 }, \"d\": {}, \"e\": 3 }", {
                p(".a", 1), p(".b.c", 2), p(".e", 3) });

}
