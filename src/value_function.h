/* value_function.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Functor used to proxy all function calls.
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
/* TEST                                                                       */
/******************************************************************************/

template<typename Vs> struct Bar;

template<typename... Values>
struct Bar< TypeVector<Values...> >
{
    // virtual ~Bar() {}
    virtual Value operator() (Values...) = 0;
};

template<size_t N> 
struct Foo : public Bar<typename RepeatType<Value, N>::type>
{
};


template<typename Fn, typename Vs> struct Blah;

template<typename Fn, typename... Values> 
struct Blah< Fn, TypeVector<Values...> > // : public Foo<sizeof...(Values)>
{
    virtual Value operator() (Values...) { return Value(); }
};



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
        return call(IsVoidRet(), std::move(values)...);
    }


private:

#if 1
    Value call(std::true_type, Values... values)
    {
        typedef typename FnType::type type;
        typedef typename FnType::Arguments Args;

        call(type(), Args(), std::move(values)...);
        return Value();
    }

    Value call(std::false_type, Values... values)
    {
        typedef typename FnType::type type;
        typedef typename FnType::Arguments Args;

        return Value(call(type(), Args(), std::move(values)...));
    }


    template<typename... Args>
    Ret call(GlobalFunction, TypeVector<Args...>, Values... values)
    {
        return (*fn)(cast<Args>(values)...);
    }

    template<typename Obj, typename... Args, typename... Rest>
    Ret call(
            MemberFunction, TypeVector<Obj, Args...>, 
            Value obj, Rest... values)
    {
        return (cast<Obj&>(obj).*fn)(cast<Args>(values)...);
    }

    template<typename Obj, typename... Args, typename... Rest>
    Ret call(
            MemberFunction, TypeVector<const Obj, Args...>, 
            Value obj, Rest... values)
    {
        return (cast<const Obj&>(obj).*fn)(cast<Args>(values)...);
    }

    template<typename... Args>
    Ret call(FunctorFunction, TypeVector<Args...>, Values... values)
    {
        return fn(cast<Args>(values)...);
    }
#endif

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


template<typename Fn>
auto makeValueFunction(Fn fn) -> typename MakeValueFunction<Fn>::type*
{
    typedef FunctionType<Fn> FnType;
    typedef typename RepeatType<Value, FnType::ArgCount>::type Values;
    typedef ValueFunctionImpl<Fn, TypeVector<> > ValueFn;

    (void) fn;
    ValueFn* ptr = (ValueFn*) std::malloc(sizeof(ValueFn));

    // new (ptr) Blah<Fn, Values>();

    new (ptr) ValueFn(std::move(fn));
    // (*ptr)();
    return ptr;
}

template<typename Fn>
auto makeValueFunctionSafe(Fn fn) ->
    std::unique_ptr<typename MakeValueFunction<Fn>::type>
{
    typedef typename MakeValueFunction<Fn>::type ValueFn;
    return std::unique_ptr<ValueFn>(makeValueFunction(std::move(fn)));
}

inline void freeValueFunction(void* fn)
{
    (void) fn;
    typedef ValueFunction<0> Fn;
    // delete static_cast<Fn*>(fn);
}


} // reflect
