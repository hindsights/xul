#pragma once

#include <xul/data/map.hpp>
#include <xul/data/element_storage_traits.hpp>
#include <xul/lang/object_impl.hpp>
#include <map>


namespace xul {


template <typename KeyT, typename ValueT, typename KeyStorageTraitsT = key_element_storage_traits<KeyT>, typename ValueStorageTraitsT = element_storage_traits<ValueT> >
class tree_map : public object_impl<map<KeyT, ValueT> >
{
public:
    typedef tree_map<KeyT, ValueT, KeyStorageTraitsT, ValueStorageTraitsT> this_type;
    typedef typename KeyStorageTraitsT::storage_type key_storage_type;
    typedef typename ValueStorageTraitsT::storage_type value_storage_type;
    typedef std::map<key_storage_type, value_storage_type, typename KeyStorageTraitsT::comparer_type> storage_map_type;
    typedef typename map<KeyT, ValueT>::entry_type entry_type;

private:
    class const_item_entry : public entry_type
    {
    public:
        typedef const storage_map_type& container_ref_type;
        typedef typename storage_map_type::const_iterator iterator_type;
        iterator_type position;
        ValueT dummy;

        explicit const_item_entry(iterator_type pos) : position(pos), dummy()
        {
        }

        virtual KeyT get_key() const
        {
            return KeyStorageTraitsT::get_const_accessor(position->first);
        }
        virtual typename ValueStorageTraitsT::const_accessor_type get_value() const
        {
            return ValueStorageTraitsT::get_const_accessor(position->second);
        }
        virtual typename ValueStorageTraitsT::accessor_type ref_value()
        {
            assert(false);
            return dummy;
        }
        virtual void set_value(typename ValueStorageTraitsT::input_type val) { assert(false); }
        void move_next()
        {
            ++position;
        }
    };

    class item_entry : public entry_type
    {
    public:
        typedef storage_map_type& container_ref_type;
        typedef typename storage_map_type::iterator iterator_type;
        iterator_type position;

        explicit item_entry(iterator_type pos) : position(pos)
        {
        }

        virtual KeyT get_key() const
        {
            return KeyStorageTraitsT::get_const_accessor(position->first);
        }
        virtual typename ValueStorageTraitsT::const_accessor_type get_value() const
        {
            return ValueStorageTraitsT::get_const_accessor(position->second);
        }
        virtual typename ValueStorageTraitsT::accessor_type ref_value()
        {
            return ValueStorageTraitsT::get_accessor(position->second);
        }
        virtual void set_value(typename ValueStorageTraitsT::accessor_type val)
        {
            position->second = val;
        }
        void move_next()
        {
            ++position;
        }
    };

    class const_item_iterator : public object_impl<const_iterator<const entry_type*> >
    {
    public:
        explicit const_item_iterator(const storage_map_type& items)
            : m_items(items)
            , m_entry(items.begin())
        {
        }
        virtual bool available() const
        {
            return m_entry.position != m_items.end();
        }
        virtual const entry_type* element() const
        {
            assert(available());
            return &m_entry;
        }
        virtual void next()
        {
            m_entry.move_next();
        }
    private:
        const storage_map_type& m_items;
        const_item_entry m_entry;
    };

    class item_iterator : public object_impl<iterator<entry_type*> >
    {
    public:
        explicit item_iterator(storage_map_type& items)
            : m_items(items)
            , m_entry(items.begin())
        {
        }
        virtual bool available() const
        {
            return m_entry.position != m_items.end();
        }
        virtual entry_type* element()
        {
            assert(available());
            return &m_entry;
        }
        virtual void set_element(entry_type*)
        {
            assert(false);
        }
        virtual void next()
        {
            m_entry.move_next();
        }
    private:
        storage_map_type& m_items;
        item_entry m_entry;
    };

public:
    virtual int size() const
    {
        return m_items.size();
    }
    virtual const_iterator<const entry_type*>* iterate() const
    {
        return new const_item_iterator(m_items);
    }
    virtual iterator<entry_type*>* iterate_ref()
    {
        return new item_iterator(m_items);
    }
    virtual bool contains(KeyT key) const
    {
        return m_items.find(KeyStorageTraitsT::store(key)) != m_items.end();
    }
    virtual void set(KeyT key, ValueT val)
    {
        m_items[KeyStorageTraitsT::store(key)] = val;
    }
    virtual typename ValueStorageTraitsT::const_accessor_type get(KeyT key, typename ValueStorageTraitsT::input_type default_val) const
    {
        typename storage_map_type::const_iterator iter = m_items.find(KeyStorageTraitsT::store(key));
        if (iter != m_items.end())
            return ValueStorageTraitsT::get_const_accessor(iter->second);
        return ValueT();
    }
    virtual typename ValueStorageTraitsT::accessor_type get_ref(KeyT key, typename ValueStorageTraitsT::input_type default_val)
    {
        typename storage_map_type::iterator iter = m_items.find(KeyStorageTraitsT::store(key));
        if (iter != m_items.end())
            return ValueStorageTraitsT::get_accessor(iter->second);
        return default_val;
    }
    virtual void clear()
    {
        m_items.clear();
    }
    virtual map<KeyT, ValueT>* clone() const
    {
        this_type* copy = new this_type;
        copy->m_items = m_items;
        return copy;
    }
    virtual bool remove(KeyT key)
    {
        int count = m_items.erase(key);
        assert(0 == count || 1 == count);
        return 1 == count;
    }

private:
    storage_map_type m_items;
};


}
