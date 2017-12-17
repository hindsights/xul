#pragma once

#include <xul/data/iterator.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


template <typename ContainerT, typename TraitsT>
class sequential_container_iterator : public object_impl<iterator<typename TraitsT::accessor_type> >
{
public:
    ContainerT& owner;
    typename ContainerT::iterator position;

    explicit sequential_container_iterator(ContainerT& o)
        : owner(o)
        , position(o.begin())
    {
    }
    virtual bool available() const
    {
        return position != owner.end();
    }
    virtual typename TraitsT::accessor_type element()
    {
        assert(available());
        return TraitsT::get_accessor(*position);
    }
    virtual void set_element(typename TraitsT::input_type src)
    {
        *position = src;
    }
    virtual void next()
    {
        assert(available());
        ++position;
    }

};


template <typename ContainerT, typename TraitsT>
class const_sequential_container_iterator : public object_impl<const_iterator<typename TraitsT::const_accessor_type> >
{
public:
    const ContainerT& owner;
    typename ContainerT::const_iterator position;

    explicit const_sequential_container_iterator(const ContainerT& o)
        : owner(o)
        , position(o.begin())
    {
    }
    virtual bool available() const
    {
        return position != owner.end();
    }
    virtual typename TraitsT::const_accessor_type element() const
    {
        assert(available());
        return TraitsT::get_const_accessor(*position);
    }
    virtual void next()
    {
        assert(available());
        ++position;
    }

};


}
