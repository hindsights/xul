#pragma once

#include <xul/macro/foreach.hpp>
#include <xul/std/strings.hpp>
#include <vector>
#include <functional>
#include <string>


namespace xul {


template <typename KeyT, typename ValueT, typename CompareT = std::equal_to<KeyT> >
class associative_array
{
public:
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef CompareT key_compare_type;
    typedef std::vector<std::pair<KeyT, ValueT> > collection_type;

    ValueT lookup(const KeyT& key, const ValueT& default_value = ValueT()) const
    {
        for (typename collection_type::const_iterator iter = m_items.begin(); iter !=m_items.end(); ++iter)
        {
            if (m_compare(iter->first, key))
                return iter->second;
        }
        return default_value;
    }
    void update(const KeyT& key, const ValueT& val)
    {
        for (typename collection_type::iterator iter = m_items.begin(); iter !=m_items.end(); ++iter)
        {
            if (m_compare(iter->first, key))
            {
                iter->second = val;
                return;
            }
        }
        m_items.push_back(std::make_pair(key, val));
    }
    bool remove(const KeyT& key)
    {
        for (typename collection_type::iterator iter = m_items.begin(); iter !=m_items.end(); ++iter)
        {
            if (m_compare(iter->first, key))
            {
                m_items.erase(iter);
                return true;
            }
        }
        return false;
    }
    void clear()
    {
        m_items.clear();
    }

    const collection_type& get_items() const { return m_items; }
    collection_type& get_items() { return m_items; }

private:
    collection_type m_items;
    key_compare_type m_compare;
};


}
