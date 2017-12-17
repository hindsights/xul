#pragma once

#include <xul/std/containers.hpp>
#include <xul/std/strings.hpp>
#include <xul/std/tstring.hpp>
#include <xul/data/string_table.hpp>
#include <boost/checked_delete.hpp>
#include <map>


namespace xul {



typedef std::map<std::string, std::string> string_dict;
typedef std::map<std::wstring, std::wstring> wstring_dict;
typedef std::map<tstring, tstring> tstring_dict;


class multimaps
{
public:
    template <typename KeyT, typename ValueT, typename TraitsT, typename AllocatorT>
    static ValueT get(const std::multimap<KeyT, ValueT, TraitsT, AllocatorT>& coll, const KeyT& key, const ValueT& defaultVal = ValueT())
    {
        typename std::multimap<KeyT, ValueT, TraitsT, AllocatorT>::const_iterator iter = coll.find(key);
        if (iter == coll.end())
            return defaultVal;
        return iter->second;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static typename std::multimap<KeyT, ValueT, PredT, AllocatorT>::iterator
        find(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
    {
        typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
        typename collection_type::iterator iterUpperBound = coll.upper_bound(key);
        for (typename collection_type::iterator iter = coll.lower_bound(key); iter != iterUpperBound; ++iter)
        {
            if (iter->second == value)
                return iter;
        }
        return coll.end();
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static bool erase(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
    {
        typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
        typename collection_type::iterator iter = multimaps::find(coll, key, value);
        if (iter == coll.end())
            return false;
        coll.erase(iter);
        return true;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static bool insert(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
    {
        typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
        typename collection_type::iterator iter = multimaps::find(coll, key, value);
        if (iter != coll.end())
            return false;
        coll.insert(make_pair(key, value));
        return true;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static void update(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& oldKey, const KeyT& newKey, const ValueT& value)
    {
        multimaps::erase(coll, oldKey, value);
        coll.insert(make_pair(newKey, value));
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static bool update_if_exist(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& oldKey, const KeyT& newKey, const ValueT& value)
    {
        if (multimaps::erase(coll, oldKey, value))
        {
            coll.insert(make_pair(newKey, value));
            return true;
        }
        return false;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static bool pop_front(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, ValueT& val)
    {
        typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
        if (coll.empty())
            return false;
        typename collection_type::iterator iter = coll.begin();
        assert(iter != coll.end());
        val = iter->second;
        coll.erase(iter);
        return true;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static typename std::multimap<KeyT, ValueT, PredT, AllocatorT>::const_iterator
        max_less_than_or_equal(const std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key)
    {
        typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
        typename collection_type::const_iterator lowerBound = coll.lower_bound(key);
        if (lowerBound == coll.end())
        {
            if (!coll.empty())
            {
                --lowerBound;
                assert(coll.key_comp()(lowerBound->first, key));
            }
        }
        else
        {
            if (lowerBound->first != key)
            {
                assert(coll.key_comp()(key, lowerBound->first));
                if (lowerBound == coll.begin())
                    lowerBound = coll.end();
                else
                {
                    --lowerBound;
                    assert(coll.key_comp()(lowerBound->first, key));
                }
            }
        }
        return lowerBound;
    }
};



class ptr_maps
{
public:
    template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
    static TypeT find(const std::map<KeyT, TypeT, PredT, AllocatorT>& cont, const KeyT& key)
    {
        typename std::map<KeyT, TypeT, PredT, AllocatorT>::const_iterator pos = cont.find(key);
        if (pos == cont.end())
            return TypeT();
        return pos->second;
    }

    template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
    static void clear(std::map<KeyT, TypeT, PredT, AllocatorT>& cont)
    {
        typedef std::map<KeyT, TypeT, PredT, AllocatorT> map_type;
        for (typename map_type::const_iterator iter = cont.begin(); iter != cont.end(); ++iter)
        {
            delete_element(iter->second);
        }
        cont.clear();
    }

    template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
    static bool erase(std::map<KeyT, TypeT, PredT, AllocatorT>& cont,
        typename std::map<KeyT, TypeT, PredT, AllocatorT>::iterator iter)
    {
        if (iter == cont.end())
            return false;
        delete_element(iter->second);
        cont.erase(iter);
        return true;
    }

    template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
    static bool erase(std::map<KeyT, TypeT, PredT, AllocatorT>& cont, const KeyT& key)
    {
        typename std::map<KeyT, TypeT, PredT, AllocatorT>::iterator iter = cont.find(key);
        if (iter == cont.end())
            return false;
        delete_element(iter->second);
        cont.erase(iter);
        return true;
    }
};

class ptr_multimaps
{
public:
    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static ValueT find(std::multimap<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
    {
        typedef std::multimap<KeyT, ValueT, PredT, AllocatorT> collection_type;
        typename collection_type::iterator iter = multimaps::find(coll, key, value);
        if (iter == coll.end())
            return ValueT();
        return iter->second;
    }

};


class maps : public containers
{
public:
    template <typename KeyT, typename ValueT, typename TraitsT, typename AllocatorT>
    static bool try_get(ValueT& val, const std::map<KeyT, ValueT, TraitsT, AllocatorT>& coll, const KeyT& key)
    {
        typename std::map<KeyT, ValueT, TraitsT, AllocatorT>::const_iterator iter = coll.find(key);
        if (iter == coll.end())
            return false;
        val = iter->second;
        return true;
    }
    template <typename KeyT, typename ValueT, typename TraitsT, typename AllocatorT>
    static ValueT get(const std::map<KeyT, ValueT, TraitsT, AllocatorT>& coll, const KeyT& key, const ValueT& defaultVal = ValueT())
    {
        typename std::map<KeyT, ValueT, TraitsT, AllocatorT>::const_iterator iter = coll.find(key);
        if (iter == coll.end())
            return defaultVal;
        return iter->second;
    }

    template <typename KeyT, typename ValueT, typename TraitsT, typename AllocatorT>
    static bool try_get(ValueT& val, const std::multimap<KeyT, ValueT, TraitsT, AllocatorT>& coll, const KeyT& key)
    {
        typename std::multimap<KeyT, ValueT, TraitsT, AllocatorT>::const_iterator iter = coll.find(key);
        if (iter == coll.end())
            return false;
        val = iter->second;
        return true;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static ValueT get_value(const std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& defaultVal = ValueT())
    {
        return get(coll, key, defaultVal);
    }


    template <typename KeyT, typename TypeT, typename PredT, typename AllocatorT>
    static size_t count_range(const std::map<KeyT, TypeT, PredT, AllocatorT>& cont, const KeyT& rangeMin, const KeyT& rangeMax)
    {
        assert(!cont.key_comp()(rangeMax, rangeMin));
        return std::distance(cont.lower_bound(rangeMin), cont.upper_bound(rangeMax));
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static typename std::map<KeyT, ValueT, PredT, AllocatorT>::iterator
        find(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value = ValueT())
    {
        return coll.find(key);
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static bool erase(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
    {
        return coll.erase(key) == 1;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static bool insert(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& value)
    {
        typedef std::map<KeyT, ValueT, PredT, AllocatorT> collection_type;
        std::pair<typename collection_type::iterator, bool> res = coll.insert(make_pair(key, value));
        return res.second;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static void erase_lower(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& bound)
    {
        coll.erase(coll.begin(), coll.lower_bound(bound));
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static void erase_non_upper(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& bound)
    {
        coll.erase(coll.begin(), coll.upper_bound(bound));
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static void erase_upper(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& bound)
    {
        coll.erase(coll.upper_bound(bound), coll.end());
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static void erase_non_lower(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& bound)
    {
        coll.erase(coll.lower_bound(bound), coll.end());
    }

    /// find the biggest element which <= key
    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static typename std::map<KeyT, ValueT, PredT, AllocatorT>::iterator find_lower_floor(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key)
    {
        typedef std::map<KeyT, ValueT, PredT, AllocatorT> map_type;
        typename map_type::iterator iter = coll.upper_bound(key);
        if (coll.begin() == iter)
        {
            return coll.end();
        }
        --iter;
        return iter;
    }

    /// find the biggest element which <= key
    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static typename std::map<KeyT, ValueT, PredT, AllocatorT>::const_iterator find_lower_floor(const std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key)
    {
        typedef std::map<KeyT, ValueT, PredT, AllocatorT> map_type;
        typename map_type::const_iterator iter = coll.upper_bound(key);
        if (coll.begin() == iter)
        {
            return coll.end();
        }
        --iter;
        return iter;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static ValueT get_lower_floor(std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& defval = ValueT())
    {
        typedef std::map<KeyT, ValueT, PredT, AllocatorT> map_type;
        typename map_type::iterator iter = find_lower_floor(coll, key);
        if (coll.end() == iter)
            return defval;
        return iter->second;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static ValueT get_lower_floor(const std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& key, const ValueT& defval = ValueT())
    {
        typedef std::map<KeyT, ValueT, PredT, AllocatorT> map_type;
        typename map_type::const_iterator iter = find_lower_floor(coll, key);
        if (coll.end() == iter)
            return defval;
        return iter->second;
    }


    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static typename std::map<KeyT, ValueT, PredT, AllocatorT>::iterator get_last_valid(std::map<KeyT, ValueT, PredT, AllocatorT>& coll)
    {
        typename std::map<KeyT, ValueT, PredT, AllocatorT>::iterator iter = coll.end();
        if (!coll.empty())
        {
            --iter;
        }
        return iter;
    }

    template <typename KeyT, typename ValueT, typename PredT, typename AllocatorT>
    static typename std::map<KeyT, ValueT, PredT, AllocatorT>::const_iterator get_last_valid(const std::map<KeyT, ValueT, PredT, AllocatorT>& coll, const KeyT& bound)
    {
        typename std::map<KeyT, ValueT, PredT, AllocatorT>::iterator iter = coll.end();
        if (!coll.empty())
        {
            --iter;
        }
        return iter;
    }
};

class string_dicts
{
public:
    template <typename PredT, typename AllocT>
    static std::string get_value(const std::map<std::string, std::string, PredT, AllocT>& dict, const std::string& key)
    {
        return maps::get_value(dict, key);
    }

    template <typename PredT, typename AllocT>
    static std::wstring get_value(const std::map<std::wstring, std::wstring, PredT, AllocT>& dict, const std::wstring& key)
    {
        return maps::get_value(dict, key);
    }

    template <typename PredT, typename AllocT>
    static size_t parse(std::map<std::string, std::string, PredT, AllocT>& dict, const std::string& src, char firstKey, char secondKey)
    {
        std::vector<std::string> strs;
        xul::strings::split(std::back_inserter(strs), src, firstKey);
        size_t count = 0;
        for (size_t index = 0; index < strs.size(); ++index)
        {
            std::pair<std::string, std::string> keyVal = xul::strings::split_pair(strs[index], secondKey);
            if (keyVal.first.size() > 0)
            {
                dict[keyVal.first] = keyVal.second;
                ++count;
            }
        }
        return count;
    }

    static size_t parse(string_table& dict, const std::string& src, char firstKey, char secondKey)
    {
        std::vector<std::string> strs;
        xul::strings::split(std::back_inserter(strs), src, firstKey);
        size_t count = 0;
        for (size_t index = 0; index < strs.size(); ++index)
        {
            std::pair<std::string, std::string> keyVal = xul::strings::split_pair(strs[index], secondKey);
            if (keyVal.first.size() > 0)
            {
                dict.set(keyVal.first.c_str(), keyVal.second.c_str());
                ++count;
            }
        }
        return count;
    }
};

}

