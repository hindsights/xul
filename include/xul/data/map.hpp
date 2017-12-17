#pragma once

//#include <xul/data/collection.hpp>
#include <xul/lang/object.hpp>
#include <xul/data/iterator.hpp>
#include <xul/data/element_traits.hpp>


namespace xul {


template <typename KeyT, typename ValueT>
class const_map_entry
{
public:
    virtual KeyT get_key() const = 0;
    virtual ValueT get_value() const = 0;
};

template <typename KeyT, typename ValueT, typename ValueTraitsT>
class map_entry : public const_map_entry<KeyT, typename ValueTraitsT::const_accessor_type>
{
public:
    virtual typename ValueTraitsT::accessor_type ref_value() = 0;
    virtual void set_value(typename ValueTraitsT::input_type val) = 0;
};


//template <typename KeyT, typename ValueT, typename ValueTraitsT = element_traits<ValueT> >
template <typename KeyT, typename ValueT>
class map : public object
{
public:
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef element_traits<ValueT> value_traits_type;
    //typedef const_map_entry<KeyT, typename ValueTraitsT::const_accessor_type> const_entry_type;
    typedef map_entry<KeyT, ValueT, value_traits_type> entry_type;

    virtual int size() const = 0;
    virtual void clear() = 0;
    virtual map<KeyT, ValueT>* clone() const = 0;
    virtual const_iterator<const entry_type*>* iterate() const = 0;
    virtual iterator<entry_type*>* iterate_ref() = 0;
    virtual bool contains(KeyT key) const = 0;
    virtual void set(KeyT key, ValueT val) = 0;
    virtual bool remove(KeyT key) = 0;
    virtual typename value_traits_type::const_accessor_type get(KeyT key, typename value_traits_type::input_type default_val) const = 0;
    virtual typename value_traits_type::accessor_type get_ref(KeyT key, typename value_traits_type::input_type default_val) = 0;

};


}
