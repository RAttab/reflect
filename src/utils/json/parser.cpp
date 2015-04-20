/* parser.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2015
   FreeBSD-style copyright and disclaimer apply
*/

namespace reflect {
namespace json {
namespace {


/******************************************************************************/
/* PARSER                                                                     */
/******************************************************************************/

struct Parser
{
    virtual ~Parser() {};
    virtual void init(const Type*) {}
    virtual void parse(Reader& reader, Value& value) const = 0;
};

const Parser* getParser(const Type* type);


/******************************************************************************/
/* TypeDetails                                                                */
/******************************************************************************/

struct TypeParser
{
    void init(const Type* type)
    {
        movable = type->isMovable();
        parser = getParser(this->type = type);
    }

    bool movable;
    const Type* type;
    const Parser* parser;
};


/******************************************************************************/
/* BASIC PARSERS                                                              */
/******************************************************************************/

struct BoolParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        value.assign(parseBool(reader));
    }
};

struct IntParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        value.assign(parseInt(reader));
    }
};

struct FloatParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        value.assign(parseFloat(reader));
    }
};

struct StringParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        value.assign(parseString(reader));
    }
};


/******************************************************************************/
/* POINTER PARSER                                                             */
/******************************************************************************/

struct PointerParser : public Parser
{
    void init(const Type* type)
    {
        inner.init(type->pointee());
        isSmartPtr = type->is("smartPtr");
    }

    void parse(Reader& reader, Value& ptr) const
    {
        if (reader.peekToken().type() == Token::Null) {
            reader.nextToken();

            if (isSmartPtr) ptr.call<void>("reset");
            else ptr = inner.type->construct();

            return;
        }

        if (cast<bool>(ptr)) {
            Value pointee = *ptr;
            inner.parser->parse(reader, pointee);
        }

        else {
            Value value = inner.type->alloc();
            Value pointee = *value;
            inner.parser->parse(reader, pointee);

            if (isSmartPtr) ptr.call<void>("reset", value);
            else ptr.assign(value);
        }
    }

private:
    TypeParser inner;
    bool isSmartPtr;
};


/******************************************************************************/
/* ARRAY PARSER                                                               */
/******************************************************************************/

struct ArrayParser : public Parser
{
    void init(const Type* type)
    {
        inner.init(type->getValue<const Type*>("valueType"));
    }

    void parse(Reader& reader, Value& array) const
    {
        auto onItem = [&] (size_t) {
            Value item = inner.type->construct();

            inner.parser->parse(reader, item);
            if (!reader) return;

            if (inner.movable) item = item.rvalue();
            array.call<void>("push_back", item);
        };
        parseArray(reader, onItem);
    }

private:
    TypeParser inner;
};


/******************************************************************************/
/* MAP PARSER                                                                 */
/******************************************************************************/

struct MapParser : public Parser
{
    void init(const Type* type)
    {
        inner.init(type->getValue<const Type*>("valueType"));
    }

    void parse(Reader& reader, Value& map) const
    {
        // need to copy the key because we use the key only after we've parsed
        // the value.
        auto onField = [&] (std::string key) {
            Value value = inner.type->construct();

            inner.parser->parse(reader, value);
            if (!reader) return;

            if (inner.movable) value = value.rvalue();
            map[key].assign(value);
        };
        parseObject(reader, onField);
    }


private:
    TypeParser inner;
};


/******************************************************************************/
/* OBJECT PARSER                                                              */
/******************************************************************************/

struct ObjectParser : public Parser
{
    void init(const Type* type)
    {
        for (std::string key : type->fields()) {
            const Field& field = type->field(key);

            if (field.is("json")) {
                auto traits = field.getValue<Traits>("json");
                if (traits.skip) continue;
                if (!traits.alias.empty()) key = traits.alias;
            }

            if (fields.count(key))
                reflectError("duplicate json key <%s> in <%s>", key, type->id());

            TypeParser inner;
            inner.init(field.type());
            fields.emplace(key, inner);
        }
    }

    void parse(Reader& reader, Value& obj) const
    {
        auto onField = [&] (const std::string& key) {
            auto it = fields.find(key);
            if (it == fields.end()) return;

            Value field = obj.field(it->first);
            it->second.parser->parse(reader, field);
        };
        parseObject(reader, onField);
    }

private:
    std::unordered_map<std::string, TypeParser> fields;
};


/******************************************************************************/
/* CUSTOM PARSER                                                              */
/******************************************************************************/

struct CustomParser : public Parser
{
    void init(const Type* type)
    {
        auto traits = type->getValue<Traits>("json");
        if (!traits.parser.empty())
            reflectError("no parser function defined for <%s>", type->id());

        parser = &type->function(traits.parser).get<void(Reader&)>();
    }

    void parse(Reader& reader, Value& value) const
    {
        parser->call<void>(value, reader);
    }

private:
    const Function* parser;
};


/******************************************************************************/
/* VALUE PARSER                                                               */
/******************************************************************************/

struct ValueParser : public Parser
{
    void parse(Reader& reader, Value& value) const
    {
        Token token = reader.nextToken();

        if (token.type() == Token::Null);

        else if (token.type() == Token::Bool) value = Value(parseBool(reader));
        else if (token.type() == Token::Int) value = Value(parseInt(reader));
        else if (token.type() == Token::Float) value = Value(parseFloat(reader));
        else if (token.type() == Token::String) value = Value(parseString(reader));

        else if (token.type() == Token::ArrayStart) {
            std::vector<Value> array;

            auto onItem = [&] (size_t) {
                Value item;
                parse(reader, item);
                array.push_back(item);
            };
            parseArray(reader, onItem);

            value = Value(array);
        }

        else if (token.type() == Token::ObjectStart) {
            std::unordered_map<std::string, Value> obj;

            auto onField = [&] (std::string key) {
                Value field;
                parse(reader, field);
                obj.emplace(std::move(key), field);
            };
            parseObject(reader, onField);

            value = Value(obj);
        }

        else reader.error("unknown expected token <%s>", token.print());
    }
};


/******************************************************************************/
/* GET PARSER                                                                 */
/******************************************************************************/

const Parser* getParser(const Type* type)
{
    static std::unordered_map<const Type*, const Parser*> parsers;

    auto it = parsers.find(type);
    if (it != parsers.end()) return it->second;

    Parser* parser = nullptr;

    if (type->is("bool")) parser = new BoolParser;
    else if (type->is("float")) parser = new FloatParser;
    else if (type->is("integer")) parser = new IntParser;
    else if (type->is("string")) parser = new StringParser;

    else if (type->isPointer()) parser = new PointerParser;
    else if (type->is("map")) parser = new MapParser;
    else if (type->is("list")) parser = new ArrayParser;
    else if (type->is("json")) parser = new CustomParser;

    else if (type == reflect::type<void>()) parser = new ValueParser;

    else parser = new ObjectParser;

    parsers[type] = parser;
    parser->init(type);

    return parser;
}

const Parser* getParserLocked(const Type* type)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);

    return getParser(type);
}

} // namespace anonymous


/******************************************************************************/
/* PARSE                                                                      */
/******************************************************************************/

void parse(Reader& reader, Value& value)
{
    getParserLocked(value.type())->parse(reader, value);
}

} // namespace json
} // namespace reflect
