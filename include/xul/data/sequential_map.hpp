#pragma once

#include <xul/data/map.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/macro/foreach.hpp>
#include <xul/std/strings.hpp>
#include <vector>
#include <functional>
#include <string>
#include <assert.h>


namespace xul {


template <typename KeyT, typename ValueT, typename KeyStorageTraitsT, typename ValueStorageTraitsT >
class sequential_map : public object_impl<map<KeyT, ValueT> >
{
public:
    typedef typename map<KeyT, ValueT>::entry_type entry_type;

    class string_pair_entry : public entry_type
    {
    public:
        std::string key;
        std::string value;

        string_pair_entry(const char* k, const char* v) : key(k), value(v)
        {
        }

        virtual const char* get_key() const { return key.c_str(); }
        virtual const char* get_value() const { return value.c_str(); }
        virtual const char* ref_value() { return value.c_str(); }
        virtual void set_value(const char* s) { value = s ? s : ""; }
    };
    class const_string_pair_iterator : public object_impl<const_iterator<const string_pair_entry*> >
    {
    public:
        explicit const_string_pair_iterator(OwnerT* owner) : m_owner(owner), m_position(0)
        {
        }
        virtual bool available() const
        {
            return m_position < m_owner->get_items().size();
        }
        virtual ElemT* element()
        {
            if (m_position >= m_owner->get_items().size())
                return NULL;
            return &m_owner->get_items()[m_position];
        }
        virtual void next()
        {
            ++m_position;
        }
    private:
        OwnerT* m_owner;
        size_t m_position;
    };
    template <typename OwnerT, typename ElemT, typename BaseT>
    class string_pair_array_iterator : public object_impl<BaseT>
    {
    public:
        explicit string_pair_array_iterator(OwnerT* owner) : m_owner(owner), m_position(0)
        {
        }
        virtual bool available() const
        {
            return m_position < m_owner->get_items().size();
        }
        virtual ElemT* element()
        {
            if (m_position >= m_owner->get_items().size())
                return NULL;
            return &m_owner->get_items()[m_position];
        }
        virtual void next()
        {
            ++m_position;
        }
    private:
        OwnerT* m_owner;
        size_t m_position;
    };

    typedef CompareT key_compare_type;
    typedef std::vector<string_pair_entry> collection_type;

    virtual bool contains(const char* key) const
    {
        if (!key)
        {
            assert(false);
            return false;
        }
        for (typename collection_type::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if (m_compare(iter->key, key))
                return true;
        }
        return false;
    }

    virtual const char* get(const char* key, const char* default_value) const
    {
        for (typename collection_type::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if (m_compare(iter->key, key))
                return iter->value.c_str();
        }
        return default_value;
    }
    virtual void set(const char* key, const char* val)
    {
        if (!key || !val)
        {
            assert(false);
        }
        std::string keystr(key);
        for (typename collection_type::iterator iter = m_items.begin(); iter !=m_items.end(); ++iter)
        {
            if (m_compare(iter->key, keystr))
            {
                iter->value = val;
                return;
            }
        }
        m_items.push_back(string_pair_entry(key, val));
    }
    virtual bool remove(const char* key)
    {
        if (!key)
            return false;
        std::string keystr(key);
        for (typename collection_type::iterator iter = m_items.begin(); iter !=m_items.end(); ++iter)
        {
            if (m_compare(iter->key, keystr))
            {
                m_items.erase(iter);
                return true;
            }
        }
        return false;
    }
    virtual void clear()
    {
        m_items.clear();
    }
    virtual int size() const
    {
        return m_items.size();
    }
    virtual const_iterator<const entry_type*>* iterate() const
    {
        return new string_pair_array_iterator<const this_type, const string_table_entry>(this);
    }
    virtual iterator<entry_type*>* iterate_ref()
    {
        return new string_pair_array_iterator<this_type, string_table_entry>(this);
    }
    virtual string_table* clone() const
    {
        this_type* new_copy = new this_type;
        new_copy->m_items = m_items;
        return new_copy;
    }

    const collection_type& get_items() const { return m_items; }
    collection_type& get_items() { return m_items; }

private:
    collection_type m_items;
    key_compare_type m_compare;
};

typedef associative_string_array<string_iequal_to> associative_istring_array;


}
