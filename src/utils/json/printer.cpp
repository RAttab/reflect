/* printer.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2015
   FreeBSD-style copyright and disclaimer apply

   Printer implementation.
*/

namespace reflect {
namespace json {
namespace {


/******************************************************************************/
/* PRINTER                                                                    */
/******************************************************************************/

struct Printer { virtual void print(Writer& writer, const Value& value) const = 0; };

const Printer* printer(const Type* type)
{
    static std::mutex mutex;
    static std::unordered_map<const Printer*, const Printer*> printers;

    std::lock_guard<std::mutex> guard(mutex);

    auto it = printers.find(type);
    if (it != printers.end()) return it->second;

    const Printer* = nullptr;

    if (type->is("bool")) printer = new BoolPrinter();
    else if (type->is("integer")) printer = new IntPrinter();
    else if (type->is("float")) printer = new FloatPrinter();
    else if (type->is("string")) printer = new StringPrinter();
    else if (type->isPointer()) printer = new PointerPrinter();

    else if (type->is("map")) printer = new MapPrinter(type);
    else if (type->is("list")) printer = new ArrayPrinter(type);
    else if (type->is("json")) printer = new CustomPrinter(type);

    else if (type == reflect::type<void>())
        writer.error("unable to print void value");

    else printer = new ObjectPrinter(type);

    printers[type] = printer;
    return printer;
}


/******************************************************************************/
/* TypeDetails                                                                */
/******************************************************************************/

struct TypeDetails
{
    TypeDetails(const Type* type) :
        type(type), printer(printer(type))
    {}

    const Type* type;
    const Printer* printer;
};


/******************************************************************************/
/* BASIC PRINTERS                                                             */
/******************************************************************************/

struct BoolPrinter : public Printer
{
    void print(Writer& writer, const Value& value) const
    {
        printBool(writer, value);
    }
};

struct IntPrinter : public Printer
{
    void print(Writer& writer, const Value& value) const
    {
        printInt(writer, value);
    }
};

struct FloatPrinter : public Printer
{
    void print(Writer& writer, const Value& value) const
    {
        printFloat(writer, value);
    }
};

struct StringPrinter : public Printer
{
    void print(Writer& writer, const Value& value) const
    {
        printString(writer, value);
    }
};


/******************************************************************************/
/* POINTER PRINTER                                                            */
/******************************************************************************/

struct PointerPrinter : public Printer
{
    PointerPrinter(const Type* type) : inner(type->pointee()) {}

    void print(Writer& writer, const Value& ptr) const
    {
        if (ptr.cast<bool>()) printNull(writer);
        else inner.printer->print(writer, *ptr);
    }

private:
    TypeDetails inner;
};


/******************************************************************************/
/* ARRAY PRINTER                                                              */
/******************************************************************************/

struct ArrayPrinter : public Printer
{
    ArrayPrinter(const Type* type) :
        inner(type->getValue<const Type*>("valueType"))
    {}

    void print(Writer& writer, const Value& array) const
    {
        onItem = [&] (size_t i) {
            inner.printer->print(writer, array[i]);
        };
        parseArray(writer, array.call<size_t>("size"), onItem);
    }

private:
    TypeDetails inner;
};


/******************************************************************************/
/* MAP PRINTER                                                                */
/******************************************************************************/

struct MapPrinter : public Printer
{
    MapPrinter(const Type* type) :
        inner(type->getValue<const Type*>("valueType"))
    {}

    void print(Writer& writer, const Value& map) const
    {
        auto keys = map.call< std::vector<std::string> >("keys");
        
        onKey = [&] (const std::string& key) {
            inner.printer->print(writer, map[field]);
        };
        printObject(writer, keys, onKey);
    }

private:
    TypeDetails inner;
};


/******************************************************************************/
/* OBJECT PRINTER                                                             */
/******************************************************************************/

struct ObjectPrinter : public Printer
{
    ObjectPrinter(const Type* type)
    {
        for (const std::string& key : type->fields()) {
            const Field& field = type->field(field);

            if (field.is("json")) {
                auto traits = field.getValue<Traits>("json");
                if (traits.skip) continue;
                if (!traits.alias.empty()) key = traits.alias;
            }

            if (fields.count(key))
                reflectError("duplicate json key <%s> in <%s>", key, type->id());

            keys.push_back(key);
            fields.emplace(key, field.type());
        }

        std::sort(keys.begin(), keys.end());
    }
        
    void print(Writer& writer, const Value& obj) const
    {
        onField = [&] (const std::string& key) {
            Value field = obj.field(key);
            if (field.isPointer() && !field) continue;

            fields.find(key)->second.printer->print(writer, field);
        };
        printObject(writer, keys, onField);
    }

private:
    std::vector<std::string> keys;
    std::unordered_map<std::string, TypeDetails> fields;
};


/******************************************************************************/
/* CUSTOME PRINTER                                                            */
/******************************************************************************/

struct CustomPrinter : public Printer
{
    CustomePrinter(const Type* type)
    {
        auto traits = type->getValue<Traits>("json");
        if (!traits.printer.empty())
            reflectError("no printer function defined for <%s>", type->id());

        printer = &type->function(traits.printer).get<void(Writer&)>();
    }

    void print(Writer& writer, const Value& value) const
    {
        printer.call<void>(value, writer);
    }

private:
    const Function* printer;
};

} // namespace anonymous


/******************************************************************************/
/* PRINT                                                                      */
/******************************************************************************/

void print(Writer& writer, const Value& value)
{
    printer(value.type())->print(writer, value);
}

} // namespace json
} // namespace reflect
