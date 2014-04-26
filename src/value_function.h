/* value_function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Functor used to call reflected function through a uniform interface.

   This is the core magic templace of the reflection system whereby it allows us
   to call any function using the generic Value(Value...) interface. This is
   accomplished by having a specialized template for each reflected function
   type which all inherit from the same base class which implements the generic
   operator() overload through virtual function. This allows the child class to
   remember their original argument and return value types while still
   implementing the Value(Value...) generic interface. This is analogous to how
   std::function is implemented.

   To convert from a Value object to the remembered argument type, this template
   uses the cast<> utility which routes the conversion to the proper Value
   function (ie. cast<>, copy<> or move<>). The return value is wrapped up in a
   Value object before being returned which means that any temporaries will be
   stored in Value and is therefor safe to use by the caller.

*/

#include "reflect.h"
#pragma once

namespace reflect {


/******************************************************************************/
/* VALUE FUNCTION                                                             */
/******************************************************************************/

template<typename Vs> struct ValueFunctionBase;

template<typename... Values>
struct ValueFunctionBase< TypeVector<Values...> >
{
    virtual Value operator() (Values... values) = 0;

    /** Turns out that virtual destructors are absurdly expensive to compile.
        Not sure why but it might have to do with the templated nature of the
        class. Even there, 1.7s on a 5.4s compile is a little much.

        Instead we create a virtual free() function which is overrided to call
        the destructor of the child class. In other words, we manually do the
        job for the compiler...
     */
    virtual void free() = 0;
};


template<size_t Args>
struct ValueFunction :
        public ValueFunctionBase<typename RepeatType<Value, Args>::type>
{};


/******************************************************************************/
/* VALUE FUNCTION IMPL                                                        */
/******************************************************************************/

template<typename Fn, typename Values> struct ValueFunctionImpl;

template<typename Fn, typename... Values>
struct ValueFunctionImpl< Fn, TypeVector<Values...> > :
        public ValueFunction<sizeof...(Values)>
{
    typedef FunctionType<Fn> FnType;
    typedef typename FnType::Return Ret;

    ValueFunctionImpl(Fn fn) : fn(std::move(fn)) {}

    // Compile-time optimization. See ValueFunctionBase::free()
    virtual void free() { this->~ValueFunctionImpl(); }

    virtual Value operator() (Values... values)
    {
        typedef typename std::is_same<Ret, void>::type IsVoidRet;

        return call(IsVoidRet(), values...);
    }


private:

    Value call(std::true_type, Values&... values)
    {
        typedef typename FnType::type type;
        typedef typename FnType::Arguments Args;

        call(type(), Args(), values...);
        return Value();
    }

    Value call(std::false_type, Values&... values)
    {
        typedef typename FnType::type type;
        typedef typename FnType::Arguments Args;

        return Value(call(type(), Args(), values...));
    }


    template<typename... Args>
    Ret call(GlobalFunction, TypeVector<Args...>, Values&... values)
    {
        return (*fn)(cast<Args>(values)...);
    }

    template<typename Obj, typename... Args, typename... Rest>
    Ret call(
            MemberFunction, TypeVector<Obj, Args...>,
            Value& obj, Rest&... values)
    {
        return (cast<Obj&>(obj).*fn)(cast<Args>(values)...);
    }

    template<typename Obj, typename... Args, typename... Rest>
    Ret call(
            MemberFunction, TypeVector<const Obj, Args...>,
            Value& obj, Rest&... values)
    {
        return (cast<const Obj&>(obj).*fn)(cast<Args>(values)...);
    }

    template<typename... Args>
    Ret call(FunctorFunction, TypeVector<Args...>, Values&... values)
    {
        return fn(cast<Args>(values)...);
    }

    Fn fn;
};


/******************************************************************************/
/* MAKE VALUE FUNCTION                                                        */
/******************************************************************************/

template<typename Fn>
struct MakeValueFunction
{
    typedef FunctionType<Fn> FnType;
    typedef typename RepeatType<Value, FnType::ArgCount>::type Values;
    typedef ValueFunctionImpl<Fn, Values> type;
};


void* allocValueFunction(size_t size);
void freeValueFunction(void* fn);

template<typename Fn>
auto makeValueFunction(Fn fn) -> typename MakeValueFunction<Fn>::type*
{
    typedef typename MakeValueFunction<Fn>::type ValueFn;

    ValueFn* ptr = (ValueFn*) allocValueFunction(sizeof(ValueFn));
    new (ptr) ValueFn(std::move(fn));
    return ptr;
}


/******************************************************************************/
/* MAKE VALUE FUNCTION SAFE                                                   */
/******************************************************************************/

struct ValueFunctionDestructor
{
    void operator() (void* ptr) const;
};

// Exists for testing purposes only.
template<typename Fn>
auto makeValueFunctionSafe(Fn fn) ->
    std::unique_ptr<typename MakeValueFunction<Fn>::type, ValueFunctionDestructor>
{
    typedef typename MakeValueFunction<Fn>::type ValueFn;
    typedef std::unique_ptr<ValueFn, ValueFunctionDestructor> Ptr;

    return Ptr(makeValueFunction(std::move(fn)));
}

} // reflect
