/* map.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   std::map reflection.
*/

#pragma once

#include "reflect.h"
#include "dsl/basics.h"
#include "dsl/template.h"
#include "dsl/function.h"
#include "dsl/operators.h"

#include <map>

namespace reflect {

/******************************************************************************/
/* GENERIC MAP REFLECT                                                        */
/******************************************************************************/

namespace details {

template<typename T_, typename KeyT, typename ValueT>
void reflectMap(Type* type_)
{
    reflectPlumbing();

    reflectTypeTrait(map);
    reflectTypeValue(keyType, type<KeyT>());
    reflectTypeValue(valueType, type<ValueT>());

    reflectFn(size);
    reflectCustom(count) (const T_& value, const KeyT& k) -> size_t {
        return value.count(k);
    };
    reflectCustom(operator[]) (T_& value, const KeyT& k) -> ValueT& {
        return value[k];
    };
    reflectCustom(at) (const T_& value, const KeyT& k) -> const ValueT& {
        auto it = value.find(k);
        if (it != value.end()) return it->second;

        reflectError("accessing unknown key in const map");
    };

    reflectCustom(keys) (const T_& value) -> std::vector<KeyT> {
        std::vector<KeyT> result;
        result.reserve(value.size());

        for (auto& item : value) result.push_back(item.first);

        return result;
    };
}

} // namespace details


/******************************************************************************/
/* REFLECT MAP                                                                */
/******************************************************************************/

template<typename KeyT, typename ValueT>
struct Reflect< std::map<KeyT, ValueT> >
{
    typedef std::map<KeyT, ValueT> T_;
    static std::string id()
    {
        return "std::map<" + typeId<KeyT>() + "," + typeId<ValueT>() + ">";
    }

    reflectTemplateLoader()

    static void reflect(Type* type)
    {
        details::reflectMap<T_, KeyT, ValueT>(type);
    }
};


/******************************************************************************/
/* REFLECT UNORDERED MAP                                                      */
/******************************************************************************/

template<typename KeyT, typename ValueT>
struct Reflect< std::unordered_map<KeyT, ValueT> >
{
    typedef std::unordered_map<KeyT, ValueT> T_;
    static std::string id()
    {
        return "std::unordered_map<" + typeId<KeyT>() + "," + typeId<ValueT>() + ">";
    }

    reflectTemplateLoader()

    static void reflect(Type* type)
    {
        details::reflectMap<T_, KeyT, ValueT>(type);
    }
};

} // reflect
