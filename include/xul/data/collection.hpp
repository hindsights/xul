#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/iterator.hpp>
#include <xul/data/element_traits.hpp>


namespace xul {


template <typename T, typename TraitsT = element_traits<T> >
class collection : public object
{
public:
    typedef T element_type;
    typedef TraitsT traits_type;

    virtual int get_count() const = 0;
    virtual iterator<typename TraitsT::const_accessor_type>* iterate() const = 0;
    virtual iterator<typename TraitsT::accessor_type>* iterate_ref() = 0;
};


}
