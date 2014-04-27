/* test_types.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Implementation of the type reflections.
*/

#include "test_types.h"

#include "reflect/basics.h"
#include "reflect/plumbing.h"
#include "reflect/field.h"
#include "reflect/operators.h"


/******************************************************************************/
/* OBJECT                                                                     */
/******************************************************************************/

reflectTypeImpl(test::Object)
{
    reflectPlumbing();
    reflectCons(int);

    reflectField(value);
    reflectField(ref);
    reflectField(constRef);
    reflectField(rref);

    reflectOperators();
}


/******************************************************************************/
/* NOT COPIABLE                                                               */
/******************************************************************************/

reflectTypeImpl(test::NotCopiable)
{
    reflectPlumbing();
}


/******************************************************************************/
/* NOT MOVABLE                                                                */
/******************************************************************************/

reflectTypeImpl(test::NotMovable)
{
    reflectPlumbing();
}


/******************************************************************************/
/* NOT CONSTRUCTIBLE                                                          */
/******************************************************************************/

reflectTypeImpl(test::NotConstructible)
{
    reflectPlumbing();
}


/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

reflectTypeImpl(test::Interface)
{
    reflectFn(pureVirtual);
}

/******************************************************************************/
/* PARENT                                                                     */
/******************************************************************************/

reflectTypeImpl(test::Parent)
{
    reflectParent(test::Interface);
    reflectPlumbing();
    reflectCons(test::Object, int);

    reflectField(value);
    reflectField(shadowed);

    reflectFn(normalVirtual);
}


/******************************************************************************/
/* CHILD                                                                      */
/******************************************************************************/

reflectTypeImpl(test::Child)
{
    reflectParent(test::Parent);
    reflectPlumbing();
    reflectCons(test::Object, int);

    reflectField(childValue);
    reflectField(shadowed);

    reflectFn(normalVirtual);
}


/******************************************************************************/
/* CONVERTIBLE                                                                */
/******************************************************************************/

reflectTypeImpl(test::Convertible)
{
    reflectPlumbing();
    reflectField(value);
    reflectOpCast(int);
    reflectOpCast(test::Parent);
}
