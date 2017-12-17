#pragma once

#include <xul/lang/object.hpp>


namespace xul {


template <typename T>
class const_iterator : public object
{
public:
    virtual bool available() const = 0;
    virtual void next() = 0;
    virtual T element() const = 0;
};


template <typename T>
class iterator : public object
{
public:
    virtual bool available() const = 0;
    virtual void next() = 0;
    virtual T element() = 0;
    virtual void set_element(T src) = 0;
};


}
