/* plumbing.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Dec 2015
   FreeBSD-style copyright and disclaimer apply
*/

#include "reflect.h"
#pragma once

namespace reflect {

/******************************************************************************/
/* DEFAULT CONSTRUCTIBLE                                                      */
/******************************************************************************/

struct DefaultConstructible : public Concept
{
    static const std::string id() { return "default_constructible"; }
    virtual Value constructDefault() const = 0;
};

template<typename T>
struct DefaultConstructibleT : public DefaultConstructible
{
    Value constructDefault() const { return Value(T{}); }
};


/******************************************************************************/
/* DEFAULT ALLOCABLE                                                          */
/******************************************************************************/

struct DefaultAllocable : public Concept
{
    static const std::string id() { return "default_allocable"; }
    virtual Value allocDefault() const = 0;
};

template<typename T>
struct DefaultAllocableT : public DefaultAllocable
{
    Value allocDefault() const { return Value(new T); }
};


/******************************************************************************/
/* COPY CONSTRUCTIBLE                                                         */
/******************************************************************************/

struct CopyConstructible : public Concept
{
    static const std::string id() { return "copy_constructible"; }
    virtual Value constructCopy(const Value& other) const = 0;
};

template<typename T>
struct CopyConstructibleT : public CopyConstructible
{
    virtual Value constructCopy(const Value& other) const
    {
        return Value(T(other.get<T>()));
    }
};


/******************************************************************************/
/* COPY ALLOCABLE                                                         */
/******************************************************************************/

struct CopyAllocable : public Concept
{
    static const std::string id() { return "copy_allocable"; }
    virtual Value allocCopy(const Value& other) const = 0;
};

template<typename T>
struct CopyAllocableT : public CopyAllocable
{
    virtual Value allocCopy(const Value& other) const
    {
        return Value(new T(other.get<T>()));
    }
};


/******************************************************************************/
/* COPY ASIGNABLE                                                             */
/******************************************************************************/

struct CopyAssignable : public Concept
{
    static const std::string id() { return "copy_assignable"; }

    virtual void assignCopy(Value& lhs, const Value& rhs) const = 0;
};

template<typename T>
struct CopyAssignableT : public CopyAssignable
{
    virtual void assignCopy(Value& lhs, const Value& rhs) const
    {
        lhs.as<T>() = rhs.get<T>();
    }
};

/******************************************************************************/
/* MOVE CONSTRUCTIBLE                                                         */
/******************************************************************************/

struct MoveConstructible : public Concept
{
    static const std::string id() { return "move_constructible"; }
    virtual Value constructMove(Value& other) const = 0;
};

template<typename T>
struct MoveConstructibleT : public MoveConstructible
{
    virtual Value constructMove(Value& other) const
    {
        return Value(T(std::move(other.as<T>())));
    }
};


/******************************************************************************/
/* MOVE ALLOCABLE                                                         */
/******************************************************************************/

struct MoveAllocable : public Concept
{
    static const std::string id() { return "move_allocable"; }
    virtual Value allocMove(Value& other) const = 0;
};

template<typename T>
struct MoveAllocableT : public MoveAllocable
{
    virtual Value allocMove(Value& other) const
    {
        return Value(new T(std::move(other.as<T>())));
    }
};


/******************************************************************************/
/* MOVE ASIGNABLE                                                             */
/******************************************************************************/

struct MoveAssignable : public Concept
{
    static const std::string id() { return "move_assignable"; }

    virtual void assignMove(Value& lhs, Value& rhs) const = 0;
};

template<typename T>
struct MoveAssignableT : public MoveAssignable
{
    virtual void assignMove(Value& lhs, Value& rhs) const
    {
        lhs.as<T>() = std::move(rhs.as<T>());
    }
};


/******************************************************************************/
/* CONSTRUCT                                                                  */
/******************************************************************************/

template<typename Ret>
Ret construct(const Type* type)
{
    Value ret = type->concept<DefaultConstructible>().constructDefault();
    return retCast<Ret>(ret);
}

template<typename Ret, typename Arg>
Ret construct(const Type* type, Arg&& arg)
{
    static const Argument moveTarget(type, RefType::RValue, false);
    if (isCastable<Arg>(moveTarget)) {
        Value vArg = cast(arg, moveTarget);
        Value ret = type->concept<MoveConstructible>().constructMove(vArg);
        return retCast<Ret>(ret);
    }
    
    static const Argument copyTarget(type, RefType::LValue, true);
    if (isCastable<Arg>(copyTarget)) {
        Value vArg = cast(arg, copyTarget);
        Value ret = type->concept<CopyConstructible>().constructCopy(vArg);
        return retCast<Ret>(ret);
    }

    return type->call<Ret>(type->id(), std::forward<Arg>(arg));
}

template<typename Ret, typename... Args>
Ret construct(const Type* type, Args&&... args)
{
    return type->call<Ret>(type->id(), std::forward<Args>(args)...);
}


/******************************************************************************/
/* ALLOC                                                                      */
/******************************************************************************/

template<typename Ret>
Ret alloc(const Type* type)
{
    return retCast<Ret>(type->concept<DefaultAllocable>().allocDefault());
}

template<typename Ret, typename Arg>
Ret alloc(const Type* type, Arg&& arg)
{
    static const Argument moveTarget(type, RefType::RValue, false);
    if (isCastable<Arg>(moveTarget)) {
        Value vArg = cast(arg, moveTarget);
        Value ret = type->concept<MoveAllocable>().allocMove(vArg);
        return retCast<Ret>(ret);
    }
    
    static const Argument copyTarget(type, RefType::LValue, true);
    if (isCastable<Arg>(copyTarget)) {
        Value vArg = cast(arg, copyTarget);
        Value ret = type->concept<CopyAllocable>().allocCopy(vArg);
        return retCast<Ret>(ret);
    }

    return type->call<Ret>(type->id(), std::forward<Arg>(arg));
}

template<typename Ret, typename... Args>
Ret alloc(const Type* type, Args&&... args)
{
    return type->call<Ret>(type->id(), std::forward<Args>(args)...);
}


/******************************************************************************/
/* ASSIGN                                                                     */
/******************************************************************************/

template<typename Arg>
void assign(const Type* type, Value& object, Arg&& arg)
{
    static const Argument moveTarget(type, RefType::LValue, true);
    if (isCastable<Arg>(moveTarget)) {
        Value vArg = cast(arg, moveTarget);
        type->concept<MoveAssignable>().assignMove(object, vArg);
        return;
    }
    
    static const Argument copyTarget(type, RefType::LValue, true);
    if (isCastable<Arg>(copyTarget)) {
        Value vArg = cast(arg, copyTarget);
        type->concept<CopyAssignable>().assignCopy(object, vArg);
        return;
    }

    type->call<void>("operator=", std::forward<Arg>(arg));
}

} // namespace reflect
