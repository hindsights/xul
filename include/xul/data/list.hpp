#pragma once

//#include <xul/data/collection.hpp>
#include <xul/lang/object.hpp>
#include <xul/data/iterator.hpp>
#include <xul/data/element_traits.hpp>


namespace xul {


//template <typename T, typename TraitsT = element_traits<T> >
template <typename T>
class list : public object
{
public:
    typedef element_traits<T> traits_type;
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    virtual void clear() = 0;
    virtual const_iterator<typename traits_type::const_accessor_type>* iterate() const = 0;
    virtual iterator<T>* iterate_ref() = 0;
    virtual void add(T elem) = 0;
    virtual typename traits_type::const_accessor_type get(int index) const = 0;
    virtual typename traits_type::accessor_type get_ref(int index) = 0;
};


}
