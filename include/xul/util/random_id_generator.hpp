#pragma once

#include <xul/util/id_generator.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/unordered_set.hpp>
#include <xul/util/random.hpp>
#include <limits>

namespace xul {


template <typename ValueT>
class random_id_generator
{
public:
    random_id_generator()
    {
        set_range(std::numeric_limits<ValueT>::min(), std::numeric_limits<ValueT>::max());
    }

    void set_range(ValueT range_min, ValueT range_max)
    {
        m_min = range_min;
        m_max = range_max;
    }

    ValueT generate()
    {
        ValueT val = 0;
        for (int i = 0; i < 100; ++i)
        {
            if (do_generate(val))
                return val;
        }
        return 0;
    }
    void release(ValueT val)
    {
        m_ids.erase(val);
    }

private:
    bool do_generate(ValueT& val)
    {
        ValueT tempval = random_generator<ValueT>::generate();
        if (m_ids.find(tempval) != m_ids.end())
            return false;
        val = tempval;
        return true;
    }

private:
    boost::unordered_set<ValueT> m_ids;
    ValueT m_min;
    ValueT m_max;
};


class sequential_id_generator : public object_impl<id_generator>
{
public:
    sequential_id_generator()
    {
        set_range(0, UINT_MAX);
    }

    void set_range(uint32_t range_min, uint32_t range_max)
    {
        m_min = range_min;
        m_max = range_max;
        m_next = m_min;
    }

    uint32_t generate()
    {
        uint32_t val = m_next;
        ++m_next;
        return val;
    }
    void release(uint32_t val)
    {
    }

private:
    uint32_t m_min;
    uint32_t m_max;
    uint32_t m_next;
};


}
