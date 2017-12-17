#pragma once

#include <xul/data/string_table.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/macro/foreach.hpp>
#include <xul/std/strings.hpp>
#include <vector>
#include <functional>
#include <string>
#include <assert.h>


namespace xul {


template <typename CompareT = std::equal_to<std::string> >
class associative_string_array : public object_impl<string_table>
{
public:
    typedef typename string_table::entry_type entry_type;
    typedef associative_string_array<CompareT> this_type;

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
    typedef std::vector<string_pair_entry> storage_collection_type;
    class const_string_pair_iterator : public object_impl<const_iterator<const entry_type*> >
    {
    public:
        explicit const_string_pair_iterator(const storage_collection_type& items) : m_items(items), m_position(0)
        {
        }
        virtual bool available() const
        {
            return m_position < m_items.size();
        }
        virtual const entry_type* element() const
        {
            if (m_position >= m_items.size())
                return NULL;
            return &m_items[m_position];
        }
        virtual void next()
        {
            ++m_position;
        }
    private:
        const storage_collection_type& m_items;
        size_t m_position;
    };
    class string_pair_iterator : public object_impl<iterator<entry_type*> >
    {
    public:
        explicit string_pair_iterator(storage_collection_type& items) : m_items(items), m_position(0)
        {
        }
        virtual bool available() const
        {
            return m_position < m_items.size();
        }
        virtual entry_type* element()
        {
            if (m_position >= m_items.size())
                return NULL;
            return &m_items[m_position];
        }
        virtual void set_element(entry_type*)
        {
            assert(false);
        }
        virtual void next()
        {
            ++m_position;
        }
    private:
        storage_collection_type& m_items;
        size_t m_position;
    };

    typedef CompareT key_compare_type;

    virtual bool contains(const char* key) const
    {
        if (!key)
        {
            assert(false);
            return false;
        }
        for (typename storage_collection_type::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if (m_compare(iter->key, key))
                return true;
        }
        return false;
    }

    virtual const char* get(const char* key, const char* default_value) const
    {
        for (typename storage_collection_type::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
        {
            if (m_compare(iter->key, key))
                return iter->value.c_str();
        }
        return default_value;
    }
    virtual const char* get_ref(const char* key, const char* default_value)
    {
        return get(key, default_value);
    }
    virtual void set(const char* key, const char* val)
    {
        if (!key || !val)
        {
            assert(false);
        }
        std::string keystr(key);
        for (typename storage_collection_type::iterator iter = m_items.begin(); iter !=m_items.end(); ++iter)
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
        for (typename storage_collection_type::iterator iter = m_items.begin(); iter !=m_items.end(); ++iter)
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
        return new const_string_pair_iterator(m_items);
    }
    virtual iterator<entry_type*>* iterate_ref()
    {
        return new string_pair_iterator(m_items);
    }
    virtual string_table* clone() const
    {
        this_type* new_copy = new this_type;
        new_copy->m_items = m_items;
        return new_copy;
    }

    const storage_collection_type& get_items() const { return m_items; }
    storage_collection_type& get_items() { return m_items; }

private:
    storage_collection_type m_items;
    key_compare_type m_compare;
};

typedef associative_string_array<string_iequal_to> associative_istring_array;


}
