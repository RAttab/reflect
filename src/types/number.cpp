/* number.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 31 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Number reflection implementation.

   \todo Should try to emulate boost pp.
*/

#include "primitives.h"
#include "reflect/constructor.h"
#include "reflect/function.h"

#include <limits>

/******************************************************************************/
/* MASS TYPE MANIP TEMPLATE                                                   */
/******************************************************************************/
// Basic template I use to create new type dependent stuff.

/*
(          char        )
(   signed char        )
( unsigned char        )
(          char16_t    )
(          char32_t    )
(          wchar_t     )
(          short       )
( unsigned short       )
(          int         )
( unsigned int         )
(          long        )
( unsigned long        )
(          long long   )
( unsigned long long   )
(          float       )
(          double      )
(          long double )

*/

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

#define reflectNumOpBinaryImpl(op, type)                                \
    do {                                                                \
        auto fn = [] (T_ lhs, type rhs) { return lhs op rhs; };         \
        reflectLambda("operator" #op, fn);                              \
    } while(false)

#define reflectNumOpBinary(op)                                  \
    do {                                                        \
        reflectNumOpBinaryImpl(op,          char        );      \
        reflectNumOpBinaryImpl(op,   signed char        );      \
        reflectNumOpBinaryImpl(op, unsigned char        );      \
        reflectNumOpBinaryImpl(op,          char16_t    );      \
        reflectNumOpBinaryImpl(op,          char32_t    );      \
        reflectNumOpBinaryImpl(op,          wchar_t     );      \
        reflectNumOpBinaryImpl(op,          short       );      \
        reflectNumOpBinaryImpl(op, unsigned short       );      \
        reflectNumOpBinaryImpl(op,          int         );      \
        reflectNumOpBinaryImpl(op, unsigned int         );      \
        reflectNumOpBinaryImpl(op,          long        );      \
        reflectNumOpBinaryImpl(op, unsigned long        );      \
        reflectNumOpBinaryImpl(op,          long long   );      \
        reflectNumOpBinaryImpl(op, unsigned long long   );      \
        reflectNumOpBinaryImpl(op,          float       );      \
        reflectNumOpBinaryImpl(op,          double      );      \
        reflectNumOpBinaryImpl(op,          long double );      \
    } while (false)

#define reflectNumBitOpBinary(op)                               \
    do {                                                        \
        reflectNumOpBinaryImpl(op,   signed char        );      \
        reflectNumOpBinaryImpl(op,          char16_t    );      \
        reflectNumOpBinaryImpl(op,          char32_t    );      \
        reflectNumOpBinaryImpl(op,          wchar_t     );      \
        reflectNumOpBinaryImpl(op, unsigned char        );      \
        reflectNumOpBinaryImpl(op,          short       );      \
        reflectNumOpBinaryImpl(op, unsigned short       );      \
        reflectNumOpBinaryImpl(op,          int         );      \
        reflectNumOpBinaryImpl(op, unsigned int         );      \
        reflectNumOpBinaryImpl(op,          long        );      \
        reflectNumOpBinaryImpl(op, unsigned long        );      \
        reflectNumOpBinaryImpl(op,          long long   );      \
        reflectNumOpBinaryImpl(op, unsigned long long   );      \
    } while (false)

#define reflectNumOpSigned(op)                                  \
    do {                                                        \
        reflectNumOpBinaryImpl(op,   signed char        );      \
        reflectNumOpBinaryImpl(op,          short       );      \
        reflectNumOpBinaryImpl(op,          int         );      \
        reflectNumOpBinaryImpl(op,          long        );      \
        reflectNumOpBinaryImpl(op,          long long   );      \
        reflectNumOpBinaryImpl(op,          float       );      \
        reflectNumOpBinaryImpl(op,          double      );      \
        reflectNumOpBinaryImpl(op,          long double );      \
    } while (false)

#define reflectNumOpUnsigned(op)                                \
    do {                                                        \
        reflectNumOpBinaryImpl(op, unsigned char        );      \
        reflectNumOpBinaryImpl(op, unsigned short       );      \
        reflectNumOpBinaryImpl(op, unsigned int         );      \
        reflectNumOpBinaryImpl(op, unsigned long        );      \
        reflectNumOpBinaryImpl(op, unsigned long long   );      \
        reflectNumOpBinaryImpl(op,          float       );      \
        reflectNumOpBinaryImpl(op,          double      );      \
        reflectNumOpBinaryImpl(op,          long double );      \
    } while (false)


#define reflectNumOpAssignImpl(op, type)                                \
    do {                                                                \
        auto fn = [] (T_& lhs, type rhs) { return lhs op rhs; };        \
        reflectLambda("operator" #op, fn);                              \
    } while (false)

#define reflectNumOpAssign(op)                                  \
    do {                                                        \
        reflectNumOpAssignImpl(op,          char        );      \
        reflectNumOpAssignImpl(op,   signed char        );      \
        reflectNumOpAssignImpl(op, unsigned char        );      \
        reflectNumOpAssignImpl(op,          char16_t    );      \
        reflectNumOpAssignImpl(op,          char32_t    );      \
        reflectNumOpAssignImpl(op,          wchar_t     );      \
        reflectNumOpAssignImpl(op,          short       );      \
        reflectNumOpAssignImpl(op, unsigned short       );      \
        reflectNumOpAssignImpl(op,          int         );      \
        reflectNumOpAssignImpl(op, unsigned int         );      \
        reflectNumOpAssignImpl(op,          long        );      \
        reflectNumOpAssignImpl(op, unsigned long        );      \
        reflectNumOpAssignImpl(op,          long long   );      \
        reflectNumOpAssignImpl(op, unsigned long long   );      \
        reflectNumOpAssignImpl(op,          float       );      \
        reflectNumOpAssignImpl(op,          double      );      \
        reflectNumOpAssignImpl(op,          long double );      \
    } while (false)


#define reflectNumBitOpAssign(op)                               \
    do {                                                        \
        reflectNumOpAssignImpl(op,          char        );      \
        reflectNumOpAssignImpl(op,   signed char        );      \
        reflectNumOpAssignImpl(op, unsigned char        );      \
        reflectNumOpAssignImpl(op,          char16_t    );      \
        reflectNumOpAssignImpl(op,          char32_t    );      \
        reflectNumOpAssignImpl(op,          wchar_t     );      \
        reflectNumOpAssignImpl(op,          short       );      \
        reflectNumOpAssignImpl(op, unsigned short       );      \
        reflectNumOpAssignImpl(op,          int         );      \
        reflectNumOpAssignImpl(op, unsigned int         );      \
        reflectNumOpAssignImpl(op,          long        );      \
        reflectNumOpAssignImpl(op, unsigned long        );      \
        reflectNumOpAssignImpl(op,          long long   );      \
        reflectNumOpAssignImpl(op, unsigned long long   );      \
    } while (false)

#define reflectNumOpCast(type)                                          \
    do {                                                                \
        auto fn = [] (T_ value) { return (type) value; }                \
        reflectLambda("operator " #type "()", fn);                      \
    } while(false)


#define reflectNumLimits(name)                                          \
    do {                                                                \
        auto fn = [] { return std::numeric_limits<T_>::name; };         \
        reflectLambda(#name, fn);                                       \
    } while (false)

#define reflectNumLimitsFn(name)                                        \
    do {                                                                \
        auto fn = [] { return std::numeric_limits<T_>::name(); };       \
        reflectLambda(#name, fn);                                       \
    } while (false)



namespace {

#if 0

/******************************************************************************/
/* REFLECT NUMBER COMPARAISON                                                 */
/******************************************************************************/

template<typename T_,
    class = typename std::enable_if<std::numeric_limits<T_>::is_signed>::type>
void reflectNumberComparaison(reflect::Type* type_)
{
    reflectNumOpSigned(==);
    reflectNumOpSigned(!=);
    reflectNumOpSigned(<);
    reflectNumOpSigned(>);
    reflectNumOpSigned(<=);
    reflectNumOpSigned(>=);
}

template<typename T_,
    class = typename std::enable_if<!std::numeric_limits<T_>::is_signed>::type,
    class = void>
void reflectNumberComparaison(reflect::Type* type_)
{
    reflectNumOpUnsigned(==);
    reflectNumOpUnsigned(!=);
    reflectNumOpUnsigned(<);
    reflectNumOpUnsigned(>);
    reflectNumOpUnsigned(<=);
    reflectNumOpUnsigned(>=);
}


/******************************************************************************/
/* REFLECT NUMBER BIT OP                                                      */
/******************************************************************************/

template<typename T_,
    class = typename std::enable_if<std::numeric_limits<T_>::is_integer>::type>
void reflectNumberBitOp(reflect::Type* type_)
{
    reflectNumBitOpAssign(%=);
    reflectNumBitOpAssign(&=);
    reflectNumBitOpAssign(|=);
    reflectNumBitOpAssign(^=);
    reflectNumBitOpAssign(<<=);
    reflectNumBitOpAssign(>>=);

    reflectNumBitOpBinary(%);
    type_->add("operator~", [] (T_ value) { return ~value; });
    reflectNumBitOpBinary(&);
    reflectNumBitOpBinary(|);
    reflectNumBitOpBinary(^);
    reflectNumBitOpBinary(<<);
    reflectNumBitOpBinary(>>);
}

template<typename>
void reflectNumberBitOp(...) {}

#endif

/******************************************************************************/
/* REFLECT NUMBER                                                             */
/******************************************************************************/

template<typename T_>
void reflectNumber(reflect::Type* type_)
{
    (void) type_;

//    reflectConsBasics();

#if 0

    reflectNumOpCast(         bool       );
    reflectNumOpCast(         char       );
    reflectNumOpCast(  signed char       );
    reflectNumOpCast(unsigned char       );
    reflectNumOpCast(         char16_t   );
    reflectNumOpCast(         char32_t   );
    reflectNumOpCast(         wchar_t    );
    reflectNumOpCast(         short      );
    reflectNumOpCast(unsigned short      );
    reflectNumOpCast(         int        );
    reflectNumOpCast(unsigned int        );
    reflectNumOpCast(         long       );
    reflectNumOpCast(unsigned long       );
    reflectNumOpCast(         long long  );
    reflectNumOpCast(unsigned long long  );
    reflectNumOpCast(         float      );
    reflectNumOpCast(         double     );
    reflectNumOpCast(         long double);

#endif

#if 0

    reflectNumberComparaison<T_>(type_);
    reflectNumberBitOp<T_>(type_);

    reflectNumOpAssign(=);
    reflectNumOpAssign(+=);
    reflectNumOpAssign(-=);
    reflectNumOpAssign(*=);
    reflectNumOpAssign(/=);

    reflectNumOpBinary(+);
    reflectNumOpBinary(-);
    reflectNumOpBinary(*);
    reflectNumOpBinary(/);

    type_->add("operator!", [] (T_ value) { return !value; });
    reflectNumOpBinary(&&);
    reflectNumOpBinary(||);

    type_->add("operator++", [] (T_ value)      { return ++value; });
    type_->add("operator++", [] (T_ value, int) { return value++; });
    type_->add("operator--", [] (T_ value)      { return --value; });
    type_->add("operator--", [] (T_ value, int) { return value--; });

#endif

#if 1
    reflectNumLimits(is_signed);
    reflectNumLimits(is_integer);
    reflectNumLimits(is_exact);
    reflectNumLimits(has_infinity);
    reflectNumLimits(has_quiet_NaN);
    reflectNumLimits(has_signaling_NaN);
    // reflectNumLimits(has_denorm);
    reflectNumLimits(has_denorm_loss);
    // reflectNumLimits(has_round_style);
    reflectNumLimits(is_iec559);
    reflectNumLimits(is_bounded);
    reflectNumLimits(is_modulo);
    reflectNumLimits(digits);
    reflectNumLimits(digits10);
    reflectNumLimits(max_digits10);
    reflectNumLimits(radix);
    reflectNumLimits(min_exponent);
    reflectNumLimits(min_exponent10);
    reflectNumLimits(max_exponent);
    reflectNumLimits(max_exponent10);
    reflectNumLimits(traps);
    reflectNumLimits(tinyness_before);

#endif

#if 0
    reflectNumLimitsFn(min);
    reflectNumLimitsFn(lowest);
    reflectNumLimitsFn(max);
    reflectNumLimitsFn(epsilon);
    reflectNumLimitsFn(round_error);
    reflectNumLimitsFn(infinity);
    reflectNumLimitsFn(quiet_NaN);
    reflectNumLimitsFn(signaling_NaN);
    reflectNumLimitsFn(denorm_min);
#endif
}

} // namespace anonymous


/******************************************************************************/
/* IMPL                                                                       */
/******************************************************************************/

#define reflectNum(T)                                   \
    reflectClassImpl(T) { reflectNumber<T>(type_); }

// reflectNum(bool)

reflectNum(char)
reflectNum(signed char)
reflectNum(unsigned char)

reflectNum(char16_t)
reflectNum(char32_t)
reflectNum(wchar_t)

reflectNum(short)
reflectNum(unsigned short)

reflectNum(int)
reflectNum(unsigned int)

reflectNum(long)
reflectNum(unsigned long)

reflectNum(long long)
reflectNum(unsigned long long)

reflectNum(float)
reflectNum(double)
reflectNum(long double)


/******************************************************************************/
/* ALIASES                                                                    */
/******************************************************************************/

namespace {

#define reflectAlias(type)                      \
    reflect::Registry::alias<type>(#type)

struct ReflectNumberAlias
{
    ReflectNumberAlias()
    {

        reflectAlias( size_t);
        reflectAlias(ssize_t);

        reflectAlias( intmax_t);
        reflectAlias(uintmax_t);
        reflectAlias( intptr_t);
        reflectAlias(uintptr_t);

        reflectAlias( int8_t);
        reflectAlias(uint8_t);
        reflectAlias( int16_t);
        reflectAlias(uint16_t);
        reflectAlias( int32_t);
        reflectAlias(uint32_t);
        reflectAlias( int64_t);
        reflectAlias(uint64_t);

        reflectAlias( int_fast8_t);
        reflectAlias(uint_fast8_t);
        reflectAlias( int_fast16_t);
        reflectAlias(uint_fast16_t);
        reflectAlias( int_fast32_t);
        reflectAlias(uint_fast32_t);
        reflectAlias( int_fast64_t);
        reflectAlias(uint_fast64_t);

        reflectAlias( int_least8_t);
        reflectAlias(uint_least8_t);
        reflectAlias( int_least16_t);
        reflectAlias(uint_least16_t);
        reflectAlias( int_least32_t);
        reflectAlias(uint_least32_t);
        reflectAlias( int_least64_t);
        reflectAlias(uint_least64_t);

    }
} reflectNumberAlias;

} // namespace anonymous
