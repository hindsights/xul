#pragma once

#include <xul/data/list.hpp>
#include <xul/data/sequential_container_iterator.hpp>
#include <xul/data/element_storage_traits.hpp>
#include <xul/lang/object_impl.hpp>
#include <vector>


namespace xul {


template <typename T, typename TraitsT = element_storage_traits<T> >
class array_list : public object_impl<list<T> >
{
public:
    typedef typename TraitsT::storage_type storage_type;
    virtual bool empty() const
    {
        return m_items.empty();
    }
    virtual int size() const
    {
        return m_items.size();
    }
    virtual void clear()
    {
        m_items.clear();
    }
    virtual const_iterator<typename TraitsT::const_accessor_type>* iterate() const
    {
        return new const_sequential_container_iterator<std::vector<storage_type>, TraitsT>(m_items);
    }
    virtual iterator<typename TraitsT::accessor_type>* iterate_ref()
    {
        return new sequential_container_iterator<std::vector<storage_type>, TraitsT>(m_items);
    }
    virtual void add(typename TraitsT::accessor_type elem)
    {
        m_items.push_back(TraitsT::store(elem));
    }
    virtual typename TraitsT::const_accessor_type get(int index) const
    {
        return TraitsT::get_const_accessor(m_items[index]);
    }
    virtual typename TraitsT::accessor_type get_ref(int index)
    {
        return TraitsT::get_accessor(m_items[index]);
    }

private:
    std::vector<storage_type> m_items;
};


}
