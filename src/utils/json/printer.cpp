/* printer.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 15 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

namespace reflect {
namespace json {

/******************************************************************************/
/* GENERIC PRINTERS                                                           */
/******************************************************************************/

void printNull(Writer& writer) { formatNull(writer); }
void printBool(Writer& writer, bool value) { formatBool(writer, value); }
void printInt(Writer& writer, int64_t value) { formatInt(writer, value); }
void printFloat(Writer& writer, double value) { formatFloat(writer, value); }
void printString(Writer& writer, const std::string& value)
{
    formatString(writer, value);
}

void printBool(Writer& writer, const Value& value)
{
    formatBool(writer, value.cast<bool>());
}

void printInt(Writer& writer, const Value& value)
{
    formatInt(writer, value.cast<int64_t>());
}

void printFloat(Writer& writer, const Value& value)
{
    formatFloat(writer, value.cast<double>());
}

void printString(Writer& writer, const Value& value)
{
    formatString(writer, value.cast<std::string>());
}


/******************************************************************************/
/* PRINTER                                                                    */
/******************************************************************************/

namespace {

struct Printer
{
    virtual void print(Writer& writer, const Value& value) const = 0;
};

const Printer* getPrinter(const Type* type);


/******************************************************************************/
/* TypeDetails                                                                */
/******************************************************************************/

struct TypePrinter
{
    TypePrinter(const Type* type) :
        type(type), printer(getPrinter(type))
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
        else {
            Value pointee = *ptr;
            inner.printer->print(writer, pointee);
        }
    }

private:
    TypePrinter inner;
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
        auto onItem = [&] (size_t i) {
            Value item = array[i];
            inner.printer->print(writer, item);
        };
        printArray(writer, array.call<size_t>("size"), onItem);
    }

private:
    TypePrinter inner;
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

        auto onKey = [&] (const std::string& key) {
            Value value = map[key];
            inner.printer->print(writer, value);
        };
        printObject(writer, keys, onKey);
    }

private:
    TypePrinter inner;
};


/******************************************************************************/
/* OBJECT PRINTER                                                             */
/******************************************************************************/

struct ObjectPrinter : public Printer
{
    ObjectPrinter(const Type* type)
    {
        for (std::string& key : type->fields()) {
            const Field& field = type->field(key);

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
        auto onField = [&] (const std::string& key) {
            Value field = obj.field(key);
            fields.find(key)->second.printer->print(writer, field);
        };
        printObject(writer, keys, onField);
    }

private:
    std::vector<std::string> keys;
    std::unordered_map<std::string, TypePrinter> fields;
};


/******************************************************************************/
/* CUSTOME PRINTER                                                            */
/******************************************************************************/

struct CustomPrinter : public Printer
{
    CustomPrinter(const Type* type)
    {
        auto traits = type->getValue<Traits>("json");
        if (!traits.printer.empty())
            reflectError("no printer function defined for <%s>", type->id());

        printer = &type->function(traits.printer).get<void(Writer&)>();
    }

    void print(Writer& writer, const Value& value) const
    {
        printer->call<void>(value, writer);
    }

private:
    const Function* printer;
};


/******************************************************************************/
/* GET PRINTER                                                                */
/******************************************************************************/

const Printer* getPrinter(const Type* type)
{
    static std::mutex mutex;
    static std::unordered_map<const Type*, const Printer*> printers;

    std::lock_guard<std::mutex> guard(mutex);

    auto it = printers.find(type);
    if (it != printers.end()) return it->second;

    const Printer* printer = nullptr;

    if (type->is("bool")) printer = new BoolPrinter;
    else if (type->is("integer")) printer = new IntPrinter;
    else if (type->is("float")) printer = new FloatPrinter;
    else if (type->is("string")) printer = new StringPrinter;

    else if (type->isPointer()) printer = new PointerPrinter(type);
    else if (type->is("map")) printer = new MapPrinter(type);
    else if (type->is("list")) printer = new ArrayPrinter(type);
    else if (type->is("json")) printer = new CustomPrinter(type);

    else if (type == reflect::type<void>())
        reflectError("unable to print void value");

    else printer = new ObjectPrinter(type);

    printers[type] = printer;
    return printer;
}

} // namespace anonymous


/******************************************************************************/
/* PRINT                                                                      */
/******************************************************************************/

void print(Writer& writer, const Value& value)
{
    getPrinter(value.type())->print(writer, value);
}

} // namespace json
} // namespace reflect
