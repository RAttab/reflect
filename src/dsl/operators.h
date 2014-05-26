/* operators.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 30 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection for operators.
*/

#include "reflect.h"
#pragma once

#include "dsl/function.h"

namespace reflect {

/******************************************************************************/
/* REFLECT OP NAME                                                            */
/******************************************************************************/

#define reflectOpNameImpl(name) reflectOp ## _ ## name
#define reflectOpName(name)     reflectOpNameImpl(name)


/******************************************************************************/
/* REFLECT OP                                                                 */
/******************************************************************************/

#define reflectOpFn(op, name)                                   \
    template<typename T, class = decltype(&T::op)>              \
    void reflectOpName(name) (Type* type, std::string name)     \
    {                                                           \
        reflectFunction(type, name, &T::op);                    \
    }                                                           \
                                                                \
    template<typename>                                          \
    void reflectOpName(name)(...) {}

#define reflectOp(op, name) reflect::reflectOpName(name)<T_>(type_, #op)


/******************************************************************************/
/* REFLECT OP TYPED                                                           */
/******************************************************************************/

#define reflectOpTypedFn(op, name, signature)                           \
    template<typename T,                                                \
        class = decltype( static_cast<signature>(&T::op) )>             \
    void reflectOpName(name) (Type* type, std::string name)             \
    {                                                                   \
        reflectFunction<signature>(type, name, &T::op);                 \
    }                                                                   \
                                                                        \
    template<typename>                                                  \
    void reflectOpName(name)(...) {}

#define reflectOpTyped(op, name) reflect::reflectOpName(name)<T_>(type_, #op)


/******************************************************************************/
/* REFLECT OP CAST                                                            */
/******************************************************************************/

#define reflectOpCast(type)                                     \
    type_->add("operator " #type "()", &T_::operator type)


/******************************************************************************/
/* ASSIGN                                                                     */
/******************************************************************************/

reflectOpFn(operator+=,  PlusAssign)
reflectOpFn(operator-=,  MinusAssign)
reflectOpFn(operator*=,  MultAssign)
reflectOpFn(operator/=,  DivAssign)
reflectOpFn(operator%=,  ModAssign)
reflectOpFn(operator&=,  BitAndAssign)
reflectOpFn(operator|=,  BitOrAssign)
reflectOpFn(operator^=,  BitXorAssign)
reflectOpFn(operator<<=, BitLShiftAssign)
reflectOpFn(operator>>=, BitRShiftAssign)

#define reflectOpAssign()                               \
    do {                                                \
        reflectOp(operator+=,  PlusAssign);             \
        reflectOp(operator-=,  MinusAssign);            \
        reflectOp(operator*=,  MultAssign);             \
        reflectOp(operator/=,  DivAssign);              \
        reflectOp(operator%=,  ModAssign);              \
        reflectOp(operator&=,  BitAndAssign);           \
        reflectOp(operator|=,  BitOrAssign);            \
        reflectOp(operator^=,  BitXorAssign);           \
        reflectOp(operator<<=, BitLShiftAssign);        \
        reflectOp(operator>>=, BitRShiftAssign);        \
    } while(false);


/******************************************************************************/
/* INC DEC                                                                    */
/******************************************************************************/

reflectOpTypedFn(operator++, IncPre, T& (T::*)())
reflectOpTypedFn(operator--, DecPre, T& (T::*)())
reflectOpTypedFn(operator++, IncPost, T (T::*)(int))
reflectOpTypedFn(operator--, DecPost, T (T::*)(int))

#define reflectOpIncDec()                       \
    do {                                        \
        reflectOp(operator++, IncPre);          \
        reflectOp(operator--, DecPre);          \
        reflectOp(operator++, IncPost);         \
        reflectOp(operator--, DecPost);         \
    } while(false);


/******************************************************************************/
/* ARITHMETIC                                                                 */
/******************************************************************************/

// \todo Add unary +/-
reflectOpFn(operator+,  Plus)
reflectOpFn(operator-,  Minus)
reflectOpFn(operator*,  Mult)
reflectOpFn(operator/,  Div)
reflectOpFn(operator%,  Mod)
reflectOpFn(operator~,  BitNot)
reflectOpFn(operator&,  BitAnd)
reflectOpFn(operator|,  BitOr)
reflectOpFn(operator^,  BitXor)
reflectOpFn(operator<<, BitLShift)
reflectOpFn(operator>>, BitRShift)

#define reflectOpArithmetic()                   \
    do {                                        \
        reflectOp(operator+,  Plus);            \
        reflectOp(operator-,  Minus);           \
        reflectOp(operator*,  Mult);            \
        reflectOp(operator/,  Div);             \
        reflectOp(operator%,  Mod);             \
        reflectOp(operator~,  BitNot);          \
        reflectOp(operator&,  BitAnd);          \
        reflectOp(operator|,  BitOr);           \
        reflectOp(operator^,  BitXor);          \
        reflectOp(operator<<, BitLShift);       \
        reflectOp(operator>>, BitRShift);       \
    } while(false);


/******************************************************************************/
/* LOGICAL                                                                    */
/******************************************************************************/

reflectOpFn(operator!,  LogicNot)
reflectOpFn(operator&&, LogicAnd)
reflectOpFn(operator||, LogicOr)

#define reflectOpLogical()                      \
    do {                                        \
        reflectOp(operator!,  LogicNot);        \
        reflectOp(operator&&, LogicAnd);        \
        reflectOp(operator||, LogicOr);         \
    } while(false);


/******************************************************************************/
/* COMPARAISON                                                                */
/******************************************************************************/

reflectOpFn(operator==, EqComp)
reflectOpFn(operator!=, NEComp)
reflectOpFn(operator<,  LTComp)
reflectOpFn(operator>,  GTComp)
reflectOpFn(operator<=, LEComp)
reflectOpFn(operator>=, GEComp)

#define reflectOpComparaison()                  \
    do {                                        \
        reflectOp(operator==, EqComp);          \
        reflectOp(operator!=, NEComp);          \
        reflectOp(operator<,  LTComp);          \
        reflectOp(operator>,  GTComp);          \
        reflectOp(operator<=, LEComp);          \
        reflectOp(operator>=, GEComp);          \
    } while(false);


/******************************************************************************/
/* OTHER                                                                      */
/******************************************************************************/

reflectOpFn(operator(),  Function)
reflectOpFn(operator[],  Array)
reflectOpFn(operator*,   Indirection)

#define reflectOpOther()                                \
    do {                                                \
        reflectOp(operator(),  Function);               \
        reflectOp(operator[],  Array);                  \
        reflectOp(operator*,   Indirection);            \
    } while(false);


/******************************************************************************/
/* ALL                                                                        */
/******************************************************************************/

#define reflectOperators()                      \
    reflectOpAssign()                           \
    reflectOpIncDec()                           \
    reflectOpArithmetic()                       \
    reflectOpLogical()                          \
    reflectOpComparaison()                      \
    reflectOpOther()

} // reflect
