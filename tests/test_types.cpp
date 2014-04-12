/* test_types.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Implementation of the type reflections.
*/

#include "test_types.h"

#include "reflect/basics.h"
#include "reflect/field.h"
#include "reflect/operators.h"
// #include "types/primitives.h"


/******************************************************************************/
/* OBJECT                                                                     */
/******************************************************************************/

#if 0

reflectClassImpl(test::Object)
{
    reflectConsBasics();

    reflectField(value);
    reflectField(ref);
    reflectField(constRef);
    reflectField(rref);

    reflectOperators();
}

/******************************************************************************/
/* PARENT                                                                     */
/******************************************************************************/

reflectClassImpl(test::Parent)
{
    reflectConsBasics();
    reflectField(value);
}


/******************************************************************************/
/* CHILD                                                                      */
/******************************************************************************/

reflectClassImpl(test::Child)
{
    reflectParent(test::Parent);
    reflectConsBasics();
    reflectField(childValue);
}


/******************************************************************************/
/* CONVERTIBLE                                                                */
/******************************************************************************/

reflectClassImpl(test::Convertible)
{
    reflectConsBasics();
    reflectField(value);
    reflectOpCast(test::Parent);
}

#endif
