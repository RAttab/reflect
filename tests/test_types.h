/* test_types.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 12 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   Various types used for testing.
*/

#pragma once

#include "reflect.h"
#include "reflect/class.h"

namespace test {

/******************************************************************************/
/* OBJECT                                                                     */
/******************************************************************************/

struct Object
{
    Object() : value_(0) {}
    Object(int i) : value_(i) {}

    Object(const Object& other) : value_(other.value_) {}
    Object(Object&& other) : value_(other.value_) { other.value_ = 0; }

    Object& operator=(const Object& other)
    {
        value_ = other.value_;
        return *this;
    }

    Object& operator=(Object&& other)
    {
        value_ = other.value_;
        other.value_ = 0;
        return *this;
    }

    int value() const { return value_; }
    void value(int v) { value_ = v; }

    int& ref() { return value_; }
    void ref(int& v) { value_ = v; }

    const int& constRef() { return value_; }
    void constRef(const int& v) { value_ = v; }

    void rref(int&& v)
    {
        value_ = v;
        v = 0;
    }

    Object& operator+= (int v) { value_ += v; return *this; }
    Object& operator++ () { value_++; return *this; }
    Object& operator++ (int) { value_++; return *this; }
    Object operator+ (int v) const { return Object(value_ + v); }
    bool operator== (const Object& other) const
    {
        return value_ == other.value_;
    }

private:
    int value_;
};

std::ostream& operator<<(std::ostream& stream, const test::Object& obj)
{
    stream << "<Object: " << obj.value() << ">";
    return stream;
}


/******************************************************************************/
/* NOT COPIABLE                                                               */
/******************************************************************************/

struct NotCopiable
{
    NotCopiable() {}

    NotCopiable(const NotCopiable&) = delete;
    NotCopiable& operator=(const NotCopiable&) = delete;

    NotCopiable(NotCopiable&&) = default;
    NotCopiable& operator=(NotCopiable&&) = default;
};


/******************************************************************************/
/* NOT MOVABLE                                                                */
/******************************************************************************/

struct NotMovable
{
    NotMovable() {}

    NotMovable(const NotMovable&) = delete;
    NotMovable& operator=(const NotMovable&) = delete;

    NotMovable(NotMovable&&) = delete;
    NotMovable& operator=(NotMovable&&) = delete;
};


/******************************************************************************/
/* NOT CONSTRUCTIBLE                                                          */
/******************************************************************************/

struct NotConstructible
{
    NotConstructible make() { return NotConstructible(); }

    NotConstructible(const NotConstructible&) = delete;
    NotConstructible& operator=(const NotConstructible&) = delete;

    NotConstructible(NotConstructible&&) = default;
    NotConstructible& operator=(NotConstructible&&) = default;

private:
    NotConstructible() {}
};


/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

struct Interface
{
    virtual void pureVirtual() = 0;
};


/******************************************************************************/
/* PARENT                                                                     */
/******************************************************************************/

struct Parent : public Interface
{
    Parent() : value(0), shadowed(0) {}
    Parent(Object c, int s) : value(c), shadowed(s) {}

    Object value;
    int shadowed;

    virtual void pureVirtual() {}
    virtual void normalVirtual() {}

    bool operator== (const Parent& other) const
    {
        return value == other.value
            && shadowed == other.shadowed;
    }
};

std::ostream& operator<<(std::ostream& stream, const Parent& obj)
{
    stream << "<Parent: " << obj.value << ", " << obj.shadowed << ">";
    return stream;
}


/******************************************************************************/
/* CHILD                                                                      */
/******************************************************************************/

struct Child : public Parent
{
    Child() : childValue(0), shadowed(0) {}
    Child(Object c, int s) : childValue(c), shadowed(s) {}

    Object childValue;
    int shadowed;

    virtual void normalVirtual() {}

    bool operator== (const Child& other) const
    {
        return Parent::operator==(other)
            && childValue == other.childValue
            && shadowed == other.shadowed;
    }
};

std::ostream& operator<<(std::ostream& stream, const Child& obj)
{
    stream << "<Child: "
        << ((const test::Parent&) obj)
        << ", " << obj.childValue
        << ", " << obj.shadowed << ">";
    return stream;
}


/******************************************************************************/
/* CONVERTIBLE                                                                */
/******************************************************************************/

struct Convertible
{
    Object value;

    operator int() { return value.value(); }
    operator Parent()
    {
        Parent p;
        p.value = value;
        p.shadowed = value.value();
        return std::move(p);
    }
};

} // namespace test


/******************************************************************************/
/* REFLECT                                                                    */
/******************************************************************************/

reflectClassDecl(test::Object)

reflectClassDecl(test::NotCopiable)
reflectClassDecl(test::NotMovable)
reflectClassDecl(test::NotConstructible)

reflectClassDecl(test::Interface)
reflectClassDecl(test::Parent)
reflectClassDecl(test::Child)
reflectClassDecl(test::Convertible)
