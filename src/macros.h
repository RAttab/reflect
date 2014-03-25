/* macros.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection macros
*/

#pragma once
#include "reflect.h"

/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

#define REFLECT_TYPE(_class_,_ns_)              \
    _ns_ ## :: ## _class_

#define REFLECT_ID(_class_,_ns_)                \
    #REFLECT_NAME(id,_class_,_ns_)


/******************************************************************************/
/* CLASSES                                                                    */
/******************************************************************************/


#define REFLECT_SPECIALIZATION(_class_,_ns_)                    \
    template<>                                                  \
    struct Reflect<REFLECT_TYPE(_class_,_ns_)>                  \
    {                                                           \
        static constexpr const char* id =                       \
            REFLECT_ID(_class_,_ns_);                           \
                                                                \
        static Reflection* create();                            \
    };


/******************************************************************************/
/* TOP LEVEL                                                                  */
/******************************************************************************/

#define REFLECT(_class_,_ns_)                                           \
    namespace reflect { REFLECT_SPECIALIZATION(_class_,_ns_) }          \
    template<>                                                          \
    Reflection* reflect::Reflect<REFLECT_TYPE(_class_,_ns_)>::create()
