/* pointer_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for pointer reflection.

   Note: Disabled tests are the ones that are currently failing due to
   lackluster pointer support.

   \todo Pointers arent' const-checked at the moment.
*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define REFLECT_USE_EXCEPTIONS 1

#include "reflect.h"
#include "test_types.h"
#include "types/primitives.h"
#include "types/std/smart_ptr.h"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace reflect;


/******************************************************************************/
/* BASICS                                                                     */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(basics)
{
    const Type* tPtr = type<int*>();
    BOOST_CHECK_EQUAL(type<int const*>(), tPtr);

    BOOST_CHECK(tPtr->is("pointer"));
    BOOST_CHECK_EQUAL(tPtr->call<const Type*>("pointee"), type<int>());

    const Type* tPtrPtr = type<int**>();
    BOOST_CHECK(tPtrPtr->is("pointer"));
    BOOST_CHECK_EQUAL(tPtrPtr->call<const Type*>("pointee"), tPtr);

    BOOST_CHECK_EQUAL(type<int const* const*>(), tPtrPtr);
    BOOST_CHECK_EQUAL(type<int      * const*>(), tPtrPtr);
    BOOST_CHECK_EQUAL(type<int const*      *>(), tPtrPtr);
}


/******************************************************************************/
/* POINTER                                                                    */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(pointer_test)
{
    typedef test::Object Obj;

    auto doPtr = [] (Obj* obj) { return obj; };
    Function ptrFn("ptr", doPtr);

    auto doPtrPtr = [] (Obj** obj) { return obj; };
    Function ptrPtrFn("ptrPtr", doPtrPtr);

    auto doConstPtr = [] (Obj const* const obj) { return obj; };
    Function constPtrFn("constPtr", doConstPtr);

    BOOST_CHECK_EQUAL(ptrFn.test<void(Obj       )>(), Match::None);
    BOOST_CHECK_EQUAL(ptrFn.test<void(Obj *     )>(), Match::Exact);
    BOOST_CHECK_EQUAL(ptrFn.test<void(Obj **    )>(), Match::None);
    // BOOST_CHECK_EQUAL(ptrFn.test<void(Obj const*)>(), Match::None);
    BOOST_CHECK_EQUAL(ptrFn.test<Obj       (Obj*)>(), Match::None);
    BOOST_CHECK_EQUAL(ptrFn.test<Obj *     (Obj*)>(), Match::Exact);
    BOOST_CHECK_EQUAL(ptrFn.test<Obj **    (Obj*)>(), Match::None);
    BOOST_CHECK_EQUAL(ptrFn.test<Obj const*(Obj*)>(), Match::Exact);

    BOOST_CHECK_EQUAL(ptrPtrFn.test<void(Obj       )>(), Match::None);
    BOOST_CHECK_EQUAL(ptrPtrFn.test<void(Obj *     )>(), Match::None);
    BOOST_CHECK_EQUAL(ptrPtrFn.test<void(Obj **    )>(), Match::Exact);
    BOOST_CHECK_EQUAL(ptrPtrFn.test<void(Obj const*)>(), Match::None);
    BOOST_CHECK_EQUAL(ptrPtrFn.test<Obj       (Obj**)>(), Match::None);
    BOOST_CHECK_EQUAL(ptrPtrFn.test<Obj *     (Obj**)>(), Match::None);
    BOOST_CHECK_EQUAL(ptrPtrFn.test<Obj **    (Obj**)>(), Match::Exact);
    BOOST_CHECK_EQUAL(ptrPtrFn.test<Obj const*(Obj**)>(), Match::None);

    BOOST_CHECK_EQUAL(constPtrFn.test<void(Obj       )>(), Match::None);
    BOOST_CHECK_EQUAL(constPtrFn.test<void(Obj *     )>(), Match::Exact);
    BOOST_CHECK_EQUAL(constPtrFn.test<void(Obj **    )>(), Match::None);
    BOOST_CHECK_EQUAL(constPtrFn.test<void(Obj const*)>(), Match::Exact);
    BOOST_CHECK_EQUAL(constPtrFn.test<Obj       (Obj const*)>(), Match::None);
    // BOOST_CHECK_EQUAL(constPtrFn.test<Obj *     (Obj const*)>(), Match::None);
    BOOST_CHECK_EQUAL(constPtrFn.test<Obj **    (Obj const*)>(), Match::None);
    BOOST_CHECK_EQUAL(constPtrFn.test<Obj const*(Obj const*)>(), Match::Exact);

}

BOOST_AUTO_TEST_CASE(pointer_call)
{
    typedef test::Object Obj;

    auto doPtr = [] (Obj* obj) { return obj; };
    Function ptrFn("ptr", doPtr);

    auto doPtrPtr = [] (Obj** obj) { return obj; };
    Function ptrPtrFn("ptrPtr", doPtrPtr);

    auto doConstPtr = [] (Obj const* const obj) { return obj; };
    Function constPtrFn("constPtr", doConstPtr);

    Obj o;
    Obj* po = &o;
    Obj** ppo = &po;
    Obj const* cpo = &o;

    BOOST_CHECK_THROW(ptrFn.call<Obj*>(o  ), ReflectError);
    BOOST_CHECK_EQUAL(ptrFn.call<Obj*>(po ), doPtr(po));
    BOOST_CHECK_THROW(ptrFn.call<Obj*>(ppo), ReflectError);
    // BOOST_CHECK_THROW(ptrFn.call<Obj*>(cpo), ReflectError);
    BOOST_CHECK_THROW(ptrFn.call<Obj*>(Value(o  )), ReflectError);
    BOOST_CHECK_EQUAL(ptrFn.call<Obj*>(Value(po )), doPtr(po));
    BOOST_CHECK_THROW(ptrFn.call<Obj*>(Value(ppo)), ReflectError);
    // BOOST_CHECK_THROW(ptrFn.call<Obj*>(Value(cpo)), ReflectError);
    BOOST_CHECK_THROW(ptrFn.call<Obj       >(po), ReflectError);
    BOOST_CHECK_EQUAL(ptrFn.call<Obj *     >(po), doPtr(po));
    BOOST_CHECK_THROW(ptrFn.call<Obj **    >(po), ReflectError);
    BOOST_CHECK_EQUAL(ptrFn.call<Obj const*>(po), doPtr(po));

    BOOST_CHECK_THROW(ptrPtrFn.call<Obj**>(o  ), ReflectError);
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj**>(po ), ReflectError);
    BOOST_CHECK_EQUAL(ptrPtrFn.call<Obj**>(ppo), doPtrPtr(ppo));
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj**>(cpo), ReflectError);
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj**>(Value(o  )), ReflectError);
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj**>(Value(po )), ReflectError);
    BOOST_CHECK_EQUAL(ptrPtrFn.call<Obj**>(Value(ppo)), doPtrPtr(ppo));
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj**>(Value(cpo)), ReflectError);
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj       >(ppo), ReflectError);
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj *     >(ppo), ReflectError);
    BOOST_CHECK_EQUAL(ptrPtrFn.call<Obj **    >(ppo), doPtrPtr(ppo));
    BOOST_CHECK_THROW(ptrPtrFn.call<Obj const*>(ppo), ReflectError);

    BOOST_CHECK_THROW(constPtrFn.call<Obj const*>(o  ), ReflectError);
    BOOST_CHECK_EQUAL(constPtrFn.call<Obj const*>(po ), doConstPtr(po));
    BOOST_CHECK_THROW(constPtrFn.call<Obj const*>(ppo), ReflectError);
    BOOST_CHECK_EQUAL(constPtrFn.call<Obj const*>(cpo), doConstPtr(cpo));
    BOOST_CHECK_THROW(constPtrFn.call<Obj const*>(Value(o  )), ReflectError);
    BOOST_CHECK_EQUAL(constPtrFn.call<Obj const*>(Value(po )), doConstPtr(po));
    BOOST_CHECK_THROW(constPtrFn.call<Obj const*>(Value(ppo)), ReflectError);
    BOOST_CHECK_EQUAL(constPtrFn.call<Obj const*>(Value(cpo)), doConstPtr(cpo));
    BOOST_CHECK_THROW(constPtrFn.call<Obj       >(cpo), ReflectError);
    // BOOST_CHECK_THROW(constPtrFn.call<Obj *     >(cpo), ReflectError);
    BOOST_CHECK_THROW(constPtrFn.call<Obj **    >(cpo), ReflectError);
    BOOST_CHECK_EQUAL(constPtrFn.call<Obj const*>(cpo), doConstPtr(cpo));

}


/******************************************************************************/
/* SHARED PTR                                                                 */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(sharedPtr)
{
    typedef test::NotConstructible Obj;

    const Type* tSharedPtr = type< std::shared_ptr<Obj> >();
    std::cerr << tSharedPtr->print() << std::endl;

    BOOST_CHECK(tSharedPtr->is("pointer"));
    BOOST_CHECK_EQUAL(tSharedPtr->call<const Type*>("pointee"), type<Obj>());

    Obj* po = Obj::make();
    Value vSharedPtr = tSharedPtr->construct(po);
    BOOST_CHECK_THROW(vSharedPtr.get< std::shared_ptr<int> >(), ReflectError);

    auto& obj = (*vSharedPtr).get<Obj>();
    auto& sharedPtr = vSharedPtr.get< std::shared_ptr<Obj> >();

    BOOST_CHECK(vSharedPtr); // operator bool()
    BOOST_CHECK(vSharedPtr == sharedPtr); // operator==
    BOOST_CHECK_EQUAL(&obj, po);
    BOOST_CHECK_EQUAL(sharedPtr.get(), po);
    BOOST_CHECK_EQUAL(vSharedPtr.call<Obj*>("get"), po);

    vSharedPtr.call<void>("reset", Obj::make());

    BOOST_CHECK(vSharedPtr); // operator bool()
    BOOST_CHECK_NE(vSharedPtr.call<Obj*>("get"), po);

    vSharedPtr.call<void>("reset");
    BOOST_CHECK(!vSharedPtr); // operator bool()
    BOOST_CHECK(vSharedPtr.call<Obj*>("get") == nullptr);
}


/******************************************************************************/
/* UNIQUE PTR                                                                 */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(uniquePtr)
{
    typedef test::NotConstructible Obj;

    const Type* tUniquePtr = type< std::unique_ptr<Obj> >();
    std::cerr << tUniquePtr->print() << std::endl;

    BOOST_CHECK(tUniquePtr->is("pointer"));
    BOOST_CHECK_EQUAL(tUniquePtr->call<const Type*>("pointee"), type<Obj>());

    Obj* po = Obj::make();
    Value vUniquePtr = tUniquePtr->construct(po);
    BOOST_CHECK_THROW(vUniquePtr.get< std::unique_ptr<int> >(), ReflectError);

    auto& obj = (*vUniquePtr).get<Obj>();
    auto& uniquePtr = vUniquePtr.get< std::unique_ptr<Obj> >();

    BOOST_CHECK(vUniquePtr); // operator bool()
    BOOST_CHECK(vUniquePtr == uniquePtr); // operator==
    BOOST_CHECK_EQUAL(&obj, po);
    BOOST_CHECK_EQUAL(uniquePtr.get(), po);
    BOOST_CHECK_EQUAL(vUniquePtr.call<Obj*>("get"), po);

    vUniquePtr.call<void>("reset", Obj::make());

    BOOST_CHECK(vUniquePtr); // operator bool()
    BOOST_CHECK_NE(vUniquePtr.call<Obj*>("get"), po);

    vUniquePtr.call<void>("reset");
    BOOST_CHECK(!vUniquePtr); // operator bool()
    BOOST_CHECK(vUniquePtr.call<Obj*>("get") == nullptr);
}


/******************************************************************************/
/* TODO                                                                       */
/******************************************************************************/

#if 0 // \todo Useful use cases that are currenty not supported.

BOOST_AUTO_TEST_CASE(null)
{
    Function nullFn("null", [] (test::Object* o) {});
    nullFn.call<void>(nullptr);
}

// should also work with smart ptr.
BOOST_AUTO_TEST_CASE(parentChild)
{
    auto doParent = [] (test::Parent* p) { return p; };
    Function parentFn("parent", doParent);

    auto doChild = [] (test::Child* c) { return c; };
    Function childFn("child", doChild);

    test::Parent parent;
    test::Child child;

    BOOST_CHECK_EQUAL(childFn.call<test::Child>(&child), doChild(&child));
    BOOST_CHECK_EQUAL(childFn.call<test::Child>(&parent), doChild(&parent));
    BOOST_CHECK_EQUAL(parentFn.call<test::Parent>(&child), doParent(&child));
    BOOST_CHECK_EQUAL(parentFn.call<test::Parent>(&parent), doParent(&parent));
}

#endif
