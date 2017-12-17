#pragma once

#include <xul/macro/for_each.hpp>
#include <boost/checked_delete.hpp>
#include <set>


namespace xul {


class sets
{
public:
    template <typename KeyT, typename PredT, typename AllocatorT>
    static bool pop_front(std::set<KeyT, PredT, AllocatorT>& coll, KeyT& key)
    {
        typedef std::set<KeyT, PredT, AllocatorT> collection_type;
        if (coll.empty())
            return false;
        typename collection_type::iterator iter = coll.begin();
        assert(iter != coll.end());
        key = *iter;
        coll.erase(iter);
        return true;
    }

    template <typename KeyT, typename PredT, typename AllocatorT>
    static void erase_lower(std::set<KeyT, PredT, AllocatorT>& coll, const KeyT& lowerBound)
    {
        coll.erase(coll.begin(), coll.lower_bound(lowerBound));
    }

    template <typename KeyT, typename PredT, typename AllocatorT>
    static void erase_upper(std::set<KeyT, PredT, AllocatorT>& coll, const KeyT& upperBound)
    {
        coll.erase(coll.begin(), coll.upper_bound(upperBound));
    }

};

class ptr_sets
{
public:
    template <typename KeyT, typename PredT, typename AllocatorT>
    static void clear(std::set<KeyT, PredT, AllocatorT>& cont)
    {
        typedef std::set<KeyT, PredT, AllocatorT> container_type;
        for (typename container_type::const_iterator iter = cont.begin(); iter != cont.end(); ++iter)
        {
            boost::checked_delete(*iter);
        }
        cont.clear();
    }

    template <typename KeyT, typename PredT, typename AllocatorT>
    static bool erase(std::set<KeyT, PredT, AllocatorT>& cont,
        typename std::set<KeyT, PredT, AllocatorT>::iterator iter)
    {
        if (iter == cont.end())
            return false;
        boost::checked_delete(*iter);
        cont.erase(iter);
        return true;
    }

    template <typename KeyT, typename PredT, typename AllocatorT>
    static bool erase(std::set<KeyT, PredT, AllocatorT>& cont, const KeyT& key)
    {
        typename std::set<KeyT, PredT, AllocatorT>::iterator iter = cont.find(key);
        if (iter == cont.end())
            return false;
        boost::checked_delete(*iter);
        cont.erase(iter);
        return true;
    }
};

}

