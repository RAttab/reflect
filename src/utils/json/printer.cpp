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
    formatBool(writer, cast<bool>(value));
}

void printInt(Writer& writer, const Value& value)
{
    formatInt(writer, cast<int64_t>(value));
}

void printFloat(Writer& writer, const Value& value)
{
    formatFloat(writer, cast<double>(value));
}

void printString(Writer& writer, const Value& value)
{
    formatString(writer, cast<std::string>(value));
}


namespace {


/******************************************************************************/
/* CUSTOM PRINTER                                                             */
/******************************************************************************/

std::string customPrinter(const Type* type)
{
    if (!type->is("json")) return "";
    return type->getValue<json::Traits>("json").printer;
}


/******************************************************************************/
/* PRINTER                                                                    */
/******************************************************************************/

struct Printer
{
    virtual ~Printer() {}

    virtual void init(const Type*) {}
    virtual bool isEmpty(const Value&) const { return false; }
    virtual void print(Writer& writer, const Value& value) const = 0;
};

const Printer* getPrinter(const Type* type);


/******************************************************************************/
/* TypeDetails                                                                */
/******************************************************************************/

struct TypePrinter
{
    void init(const Type* type)
    {
        printer = getPrinter(this->type = type);
    }

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
    bool isEmpty(const Value& value) const
    {
        return cast<const int64_t&>(value) == 0;
    }

    void print(Writer& writer, const Value& value) const
    {
        printInt(writer, value);
    }
};

struct FloatPrinter : public Printer
{
    bool isEmpty(const Value& value) const
    {
        return cast<const double&>(value) == 0;
    }

    void print(Writer& writer, const Value& value) const
    {
        printFloat(writer, value);
    }
};

struct StringPrinter : public Printer
{
    bool isEmpty(const Value& value) const
    {
        return value.call<size_t>("size") == 0;
    }

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
    void init(const Type* type) { inner.init(type->pointee()); }

    bool isEmpty(const Value& ptr) const
    {
        return !cast<bool>(ptr) || inner.printer->isEmpty(*ptr);
    }

    void print(Writer& writer, const Value& ptr) const
    {
        if (!cast<bool>(ptr)) {
            printNull(writer);
            return;
        }

        Value pointee = *ptr;
        inner.printer->print(writer, pointee);
    }

private:
    TypePrinter inner;
};


/******************************************************************************/
/* ARRAY PRINTER                                                              */
/******************************************************************************/

struct ArrayPrinter : public Printer
{
    void init(const Type* type)
    {
        inner.init(type->getValue<const Type*>("valueType"));
    }

    bool isEmpty(const Value& array) const
    {
        return array.call<size_t>("size") == 0;
    }

    void print(Writer& writer, const Value& array) const
    {
        auto printFn = [&] (size_t i) {
            Value item = array.call<Value>("at", i);
            inner.printer->print(writer, item);
        };
        printArray(writer, array.call<size_t>("size"), printFn);
    }

private:
    TypePrinter inner;
};


/******************************************************************************/
/* MAP PRINTER                                                                */
/******************************************************************************/

struct MapPrinter : public Printer
{
    void init(const Type* type)
    {
        inner.init(type->getValue<const Type*>("valueType"));
    }

    bool isEmpty(const Value& map) const
    {
        return map.call<size_t>("size") == 0;
    }

    void print(Writer& writer, const Value& map) const
    {
        auto keys = map.call< std::vector<std::string> >("keys");

        auto printFn = [&] (const std::string& key) {
            Value value = map.call<Value>("at", key);
            inner.printer->print(writer, value);
        };
        printObject(writer, keys, printFn);
    }

private:
    TypePrinter inner;
};


/******************************************************************************/
/* OBJECT PRINTER                                                             */
/******************************************************************************/

struct ObjectPrinter : public Printer
{
    void init(const Type* type)
    {
        for (std::string& key : type->fields()) {
            const Field& field = type->field(key);

            std::string alias = key;
            bool isSkipEmpty = false;

            if (field.is("json")) {
                auto traits = field.getValue<Traits>("json");
                if (traits.skip) continue;
                if (!traits.alias.empty()) alias = traits.alias;
                isSkipEmpty = traits.skipEmpty;
            }

            if (fields.count(key))
                reflectError("duplicate json key <%s> in <%s>", key, type->id());

            keys[alias] = key;
            sortedKeys.push_back(alias);
            if (isSkipEmpty) skipEmpty.insert(alias);

            TypePrinter printer;
            printer.init(field.type());
            fields.emplace(key, printer);
        }

        std::sort(sortedKeys.begin(), sortedKeys.end());
    }

    void print(Writer& writer, const Value& obj) const
    {
        auto getField = [&] (const std::string& alias) {
            auto keyIt = keys.find(alias);
            if (keyIt == keys.end())
                reflectError("unknown key for alias <%s>", alias);

            const std::string& key = keyIt->second;

            auto fieldIt = fields.find(key);
            if (fieldIt == fields.end())
                reflectError("unknown field <%s> for alias <%s>", key, alias);

            return fieldIt;
        };

        auto skipFn = [&] (const std::string& alias) {
            if (!writer.compact() && !skipEmpty.count(alias))
                return false;

            auto it = getField(alias);

            Value field = obj.field(it->first);
            return it->second.printer->isEmpty(field);
        };

        auto printFn = [&] (const std::string& alias) {
            auto it = getField(alias);

            Value field = obj.field(it->first);
            it->second.printer->print(writer, field);
        };

        printObject(writer, sortedKeys, printFn, skipFn);
    }

private:
    std::vector<std::string> sortedKeys;
    std::unordered_set<std::string> skipEmpty;
    std::unordered_map<std::string, std::string> keys;
    std::unordered_map<std::string, TypePrinter> fields;
};


/******************************************************************************/
/* CUSTOM PRINTER                                                             */
/******************************************************************************/

struct CustomPrinter : public Printer
{
    void init(const Type* type)
    {
        std::string name = customPrinter(type);
        if (name.empty())
            reflectError("no printer function defined for <%s>", type->id());

        printer = &type->function(name).get<void(Value, Writer&)>();
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
    static std::unordered_map<const Type*, const Printer*> printers;

    auto it = printers.find(type);
    if (it != printers.end()) return it->second;

    Printer* printer = nullptr;

    if (type->is("bool")) printer = new BoolPrinter;
    else if (type->is("integer")) printer = new IntPrinter;
    else if (type->is("float")) printer = new FloatPrinter;
    else if (type->is("string")) printer = new StringPrinter;

    else if (type->isPointer()) printer = new PointerPrinter;
    else if (type->is("map")) printer = new MapPrinter;
    else if (type->is("list")) printer = new ArrayPrinter;

    else if (!customPrinter(type).empty()) printer = new CustomPrinter;
    else if (type == reflect::type<void>())
        reflectError("unable to print void value");

    else printer = new ObjectPrinter;

    printers[type] = printer;
    printer->init(type);

    return printer;
}

const Printer* getPrinterLocked(const Type* type)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);

    return getPrinter(type);
}

} // namespace anonymous


/******************************************************************************/
/* PRINT                                                                      */
/******************************************************************************/

void print(Writer& writer, const Value& value)
{
    getPrinterLocked(value.type())->print(writer, value);
}

} // namespace json
} // namespace reflect
