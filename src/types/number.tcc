/* number.tcc                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 10 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Utilities for the number reflections

   Note that the number reflection are spread among multiple cpp files to help
   compilation times.

*/

#include "reflect.h"
#include "types/primitives.h"
#include "reflect/constructor.h"
#include "reflect/function.h"

#include <limits>

namespace reflect {

/******************************************************************************/
/* MASS TYPE MANIP TEMPLATE                                                   */
/******************************************************************************/
// Basic template I use to create new type dependent stuff.

/*

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
/* REFLECT HELPERS                                                            */
/******************************************************************************/

#define reflectNumOpCast(type)                                          \
    do {                                                                \
        auto fn = [] (T_ value) { return (type) value; };               \
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

#define reflectNum(T)                                   \
    reflectClassImpl(T) { reflect::reflectNumber<T>(type_); }


/******************************************************************************/
/* REFLECT NUMBER                                                             */
/******************************************************************************/

template<typename T_>
void reflectNumber(reflect::Type* type_)
{
    reflectConsBasics();

    reflectNumOpCast(         bool       );
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

    reflectNumLimitsFn(min);
    reflectNumLimitsFn(lowest);
    reflectNumLimitsFn(max);
    reflectNumLimitsFn(epsilon);
    reflectNumLimitsFn(round_error);
    reflectNumLimitsFn(infinity);
    reflectNumLimitsFn(quiet_NaN);
    reflectNumLimitsFn(signaling_NaN);
    reflectNumLimitsFn(denorm_min);
}

} // namespace reflect
