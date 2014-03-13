/* macros.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 13 Mar 2014
   FreeBSD-style copyright and disclaimer apply

   Reflection macros
*/

#pragma once


/******************************************************************************/
/* UTILS                                                                      */
/******************************************************************************/

#define REFLECT_NAME(_prefix_,_class_,_ns_)     \
    _prefix_ ## _ ## _ns_ ## _ ## _class_

#define REFLECT_TYPE(_class_,_ns_)              \
    _ns_ ## :: ## _class_

#define REFLECT_ID(_class_,_ns_)                \
    #REFLECT_NAME(id,_class_,_ns_)


/******************************************************************************/
/* CLASSES                                                                    */
/******************************************************************************/

#define REFLECT_ID_CLASS(_class_,_ns_)                  \
    template<>                                          \
    struct ReflectionId<REFLECT_TYPE(_class_,_ns_)>     \
    {                                                   \
        static constexpr std::string value =            \
            REFLECT_ID(_class_,_ns_);                   \
    };


#define REFLECT_REGISTER(_class_,_ns_)                          \
    struct REFLECT_NAME(Register,_class_,_ns_)                  \
    {                                                           \
        REFLECT_NAME(Register,_class_,_ns_)()                   \
        {                                                       \
            std::string id = REFLECT_ID(_class_,_ns_);          \
            ReflectionRegistry::add(new Reflection(id));        \
            REFLECT_NAME(init,_class_,_ns_)();                  \
        }                                                       \
    } REFLECT_NAME(register,_class_,_ns_);


/******************************************************************************/
/* TOP LEVEL                                                                  */
/******************************************************************************/

#define REFLECT(_class_,_ns_)                                   \
    namespace reflect {                                         \
    REFLECT_ID_CLASS(_class_,_ns_)                              \
    namespace info {                                            \
        void REFLECT_NAME(init,_class_,_ns_)();                 \
        REFLECT_REGISTER(_class_,_ns_)                          \
    }}                                                          \
    void reflect::info::REFLECT_NAME(init,_class_,_ns_)()
