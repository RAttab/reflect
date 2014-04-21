/* pointer_test.cpp                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 20 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Tests for pointer reflection.

   Note: Disabled tests are the ones that are currently failing due to
   lackluster pointer support.

   \todo Pointers aren't const-checked at the moment.

   Turns out that this is rather difficult to get right because with pointers we
   get multi-layering of constness which makes it impossible to have a single
   bool represent const-ness. As an example:

   int const*      * const*      &
   int      * const*      * const&

   Should they both be const? What would that even mean? What we'd need is a
   list of argument objects that understands both pointer and references. But
   that's just way too complicated for the small feature gain we'd get.

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
    std::cerr << tPtr->print() << std::endl;

    BOOST_CHECK_EQUAL(type<int const*>(), tPtr);

    BOOST_CHECK(tPtr->isPointer());
    BOOST_CHECK_EQUAL(tPtr->pointer(), "*");
    BOOST_CHECK_EQUAL(tPtr->pointee(), type<int>());

    const Type* tPtrPtr = type<int**>();
    BOOST_CHECK(tPtrPtr->isPointer());
    BOOST_CHECK_EQUAL(tPtrPtr->pointer(), "*");
    BOOST_CHECK_EQUAL(tPtrPtr->pointee(), tPtr);

    BOOST_CHECK_EQUAL(type<int const* const*>(), tPtrPtr);
    BOOST_CHECK_EQUAL(type<int      * const*>(), tPtrPtr);
    BOOST_CHECK_EQUAL(type<int const*      *>(), tPtrPtr);

    auto argPtr = Argument::make<int*>();
    auto argPtrPtr = Argument::make<int**>();
    auto argSharedPtr = Argument::make< std::shared_ptr<int> >();
    BOOST_CHECK_EQUAL(argPtr.isConvertibleTo(argPtr), Match::Exact);
    BOOST_CHECK_EQUAL(argPtr.isConvertibleTo(argPtrPtr), Match::None);
    BOOST_CHECK_EQUAL(argPtr.isConvertibleTo(argSharedPtr), Match::None);
    BOOST_CHECK_EQUAL(argSharedPtr.isConvertibleTo(argSharedPtr), Match::Exact);
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
    BOOST_CHECK_EQUAL(ptrFn.test<void(Obj const*)>(), Match::Exact);
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
    BOOST_CHECK_EQUAL(constPtrFn.test<Obj *     (Obj const*)>(), Match::Exact);
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
    BOOST_CHECK_EQUAL(ptrFn.call<Obj*>(cpo), doPtr(po)); // Not an error.
    BOOST_CHECK_THROW(ptrFn.call<Obj*>(Value(o  )), ReflectError);
    BOOST_CHECK_EQUAL(ptrFn.call<Obj*>(Value(po )), doPtr(po));
    BOOST_CHECK_THROW(ptrFn.call<Obj*>(Value(ppo)), ReflectError);
    BOOST_CHECK_EQUAL(ptrFn.call<Obj*>(Value(cpo)), doPtr(po)); // not an error.
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
                      constPtrFn.call<Obj *     >(cpo);
    BOOST_CHECK_THROW(constPtrFn.call<Obj **    >(cpo), ReflectError);
    BOOST_CHECK_EQUAL(constPtrFn.call<Obj const*>(cpo), doConstPtr(cpo));

}


/******************************************************************************/
/* CHILD PARENT                                                               */
/******************************************************************************/

BOOST_AUTO_TEST_CASE(childParent_test)
{
    using namespace test;

    auto doChild = [] (Child* c) { return c; };
    Function childFn("child", doChild);

    auto doParent = [] (Parent* p) { return p; };
    Function parentFn("parent", doParent);

    typedef std::shared_ptr<Child> SmartChild;
    auto doSmartChild = [] (SmartChild c) { return c; };
    Function smartChildFn("smartChild", doSmartChild);

    typedef std::shared_ptr<Parent> SmartParent;
    auto doSmartParent = [] (SmartParent p) { return p; };
    Function smartParentFn("smartParent", doSmartParent);

    BOOST_CHECK_EQUAL(childFn.test<Child*(Child*)>(), Match::Exact);
    BOOST_CHECK_EQUAL(childFn.test<Child*(Parent*)>(), Match::None);
    BOOST_CHECK_EQUAL(childFn.test<Child*(SmartChild)>(), Match::None);
    BOOST_CHECK_EQUAL(childFn.test<Parent*(Child*)>(), Match::Exact);
    BOOST_CHECK_EQUAL(childFn.test<SmartChild(Child*)>(), Match::None);

    BOOST_CHECK_EQUAL(parentFn.test<Parent*(Child*)>(), Match::Partial);
    BOOST_CHECK_EQUAL(parentFn.test<Parent*(Parent*)>(), Match::Exact);
    BOOST_CHECK_EQUAL(parentFn.test<Parent*(SmartParent)>(), Match::None);
    BOOST_CHECK_EQUAL(parentFn.test<Child*(Parent*)>(), Match::None);
    BOOST_CHECK_EQUAL(parentFn.test<SmartParent(Parent*)>(), Match::None);

    BOOST_CHECK_EQUAL(smartChildFn.test<SmartChild(Child*)>(), Match::None);
    BOOST_CHECK_EQUAL(smartChildFn.test<SmartChild(SmartChild)>(), Match::Exact);
    BOOST_CHECK_EQUAL(smartChildFn.test<SmartChild(SmartParent)>(), Match::None);
    BOOST_CHECK_EQUAL(smartChildFn.test<Child*(SmartChild)>(), Match::None);
    BOOST_CHECK_EQUAL(smartChildFn.test<SmartParent(SmartChild)>(), Match::Exact);

    BOOST_CHECK_EQUAL(smartParentFn.test<SmartParent(Parent*)>(), Match::None);
    BOOST_CHECK_EQUAL(smartParentFn.test<SmartParent(SmartChild)>(), Match::Partial);
    BOOST_CHECK_EQUAL(smartParentFn.test<SmartParent(SmartParent)>(), Match::Exact);
    BOOST_CHECK_EQUAL(smartParentFn.test<Parent*(SmartParent)>(), Match::None);
    BOOST_CHECK_EQUAL(smartParentFn.test<SmartChild(SmartParent)>(), Match::None);
}


BOOST_AUTO_TEST_CASE(childParent_call)
{
    using namespace test;

    auto doChild = [] (Child* c) { return c; };
    Function childFn("child", doChild);

    auto doParent = [] (Parent* p) { return p; };
    Function parentFn("parent", doParent);

    typedef std::shared_ptr<Child> SmartChild;
    auto doSmartChild = [] (SmartChild c) { return c; };
    Function smartChildFn("smartChild", doSmartChild);

    typedef std::shared_ptr<Parent> SmartParent;
    auto doSmartParent = [] (SmartParent p) { return p; };
    Function smartParentFn("smartParent", doSmartParent);

    Child child;
    Child* pc = &child;
    SmartChild sc(new Child);

    Parent parent;
    Parent* pp = &parent;
    SmartParent sp(new Parent);

    BOOST_CHECK_EQUAL(childFn.call<Child*>(pc), doChild(pc));
    BOOST_CHECK_THROW(childFn.call<Child*>(pp), ReflectError);
    BOOST_CHECK_THROW(childFn.call<Child*>(sc), ReflectError);
    BOOST_CHECK_EQUAL(childFn.call<Parent*>(pc), doChild(pc));
    BOOST_CHECK_THROW(childFn.call<SmartChild>(pc), ReflectError);

    BOOST_CHECK_EQUAL(parentFn.call<Parent*>(pc), doParent(pc));
    BOOST_CHECK_EQUAL(parentFn.call<Parent*>(pp), doParent(pp));
    BOOST_CHECK_THROW(parentFn.call<Parent*>(sp), ReflectError);
    BOOST_CHECK_THROW(parentFn.call<Child*>(pp), ReflectError);
    BOOST_CHECK_THROW(parentFn.call<SmartParent>(pp), ReflectError);

    BOOST_CHECK_THROW(smartChildFn.call<SmartChild>(pc), ReflectError);
    BOOST_CHECK_EQUAL(smartChildFn.call<SmartChild>(sc), doSmartChild(sc));
    BOOST_CHECK_THROW(smartChildFn.call<SmartChild>(sp), ReflectError);
    BOOST_CHECK_THROW(smartChildFn.call<Child*>(sc), ReflectError);
    BOOST_CHECK_EQUAL(smartChildFn.call<SmartParent>(sc), doSmartChild(sc));

    BOOST_CHECK_THROW(smartParentFn.call<SmartParent>(pp), ReflectError);
    BOOST_CHECK_EQUAL(smartParentFn.call<SmartParent>(sc), doSmartParent(sc));
    BOOST_CHECK_EQUAL(smartParentFn.call<SmartParent>(sp), doSmartParent(sp));
    BOOST_CHECK_THROW(smartParentFn.call<Parent*>(sp), ReflectError);
    BOOST_CHECK_THROW(smartParentFn.call<SmartChild>(sp), ReflectError);
}


/******************************************************************************/
/* SMART PTR                                                                  */
/******************************************************************************/

template<typename Ptr>
void testSmartPtr(const std::string& pointer)
{
    typedef test::NotConstructible Obj;

    const Type* tPtr = type<Ptr>();
    std::cerr << tPtr->print() << std::endl;

    BOOST_CHECK(tPtr->isPointer());
    BOOST_CHECK_EQUAL(tPtr->pointer(), pointer);
    BOOST_CHECK_EQUAL(tPtr->pointee(), type<Obj>());

    Obj* po = Obj::make();
    Value vPtr = tPtr->construct(po);

    auto& obj = (*vPtr).get<Obj>();
    auto& sharedPtr = vPtr.get<Ptr>();

    BOOST_CHECK(vPtr); // operator bool()
    BOOST_CHECK(vPtr == sharedPtr); // operator==
    BOOST_CHECK_EQUAL(&obj, po);
    BOOST_CHECK_EQUAL(sharedPtr.get(), po);
    BOOST_CHECK_EQUAL(vPtr.call<Obj*>("get"), po);

    vPtr.call<void>("reset", Obj::make());

    BOOST_CHECK(vPtr); // operator bool()
    BOOST_CHECK_NE(vPtr.call<Obj*>("get"), po);

    vPtr.call<void>("reset");
    BOOST_CHECK(!vPtr); // operator bool()
    BOOST_CHECK(vPtr.call<Obj*>("get") == nullptr);

}

BOOST_AUTO_TEST_CASE(sharedPtr)
{
    typedef test::NotConstructible Obj;
    testSmartPtr< std::shared_ptr<Obj> >("std::shared_ptr");
}

BOOST_AUTO_TEST_CASE(uniquePtr)
{
    typedef test::NotConstructible Obj;
    testSmartPtr< std::unique_ptr<Obj> >("std::unique_ptr");
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

#endif
