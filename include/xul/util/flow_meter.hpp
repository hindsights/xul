#pragma once

#include <xul/util/time_counter.hpp>
#include <algorithm>
#include <vector>
#include <utility>
#include <string.h>
#include <stdint.h>
#include <assert.h>


namespace xul {


class flow_meter
{
public:
    class meter_count
    {
    public:
        int bytes;
        int packets;

        meter_count()
        {
            clear();
        }
        void clear()
        {
            bytes = 0;
            packets = 0;
        }

        void operator+=(const meter_count& x)
        {
            this->bytes += x.bytes;
            this->packets += x.packets;
        }
        void operator-=(const meter_count& x)
        {
            this->bytes -= x.bytes;
            this->packets -= x.packets;
        }
    };

    explicit flow_meter(int unit_count = 5)
        : m_counts(unit_count)
    {
        assert(unit_count > 0);
        clear();
    }

    void add(int count)
    {
        assert(count >= 0);
        m_latest.bytes += count;
        ++m_latest.packets;
        m_recent.bytes += count;
        ++m_recent.packets;
        m_total_bytes += count;
        ++m_total_packets;
    }

    // should be called once every second
    void update()
    {
        m_last_update_time.sync();
        int position = m_update_times % m_counts.size();
        assert(m_recent.bytes >= 0);
        assert(m_recent.packets >= 0);
        m_recent -= m_counts[position];
        assert(m_recent.bytes >= 0);
        assert(m_recent.packets >= 0);
        m_counts[position] = m_latest;
        m_latest.clear();
        ++m_update_times;
    }
    int get_rate() const
    {
        assert(m_update_times >= 0);
        if (0 == m_update_times)
            return m_latest.bytes;
        int times = m_update_times < m_counts.size() ? m_update_times : m_counts.size();
        int used_time = times * 1000 + m_last_update_time.get_elapsed();
        assert(used_time <= (m_counts.size() + 1) * 1000 + 500);
        assert(m_recent.bytes >= 0);
        int rate = (m_recent.bytes) * 1000LL / used_time;
        assert(rate >= 0);
        return rate;
    }
    int get_packet_rate() const
    {
        assert(m_update_times >= 0);
        if (0 == m_update_times)
            return m_latest.packets;
        int times = m_update_times < m_counts.size() ? m_update_times : m_counts.size();
        int used_time = times * 1000 + m_last_update_time.get_elapsed();
        assert(used_time <= (m_counts.size() + 1) * 1000 + 500);
        assert(m_recent.packets >= 0);
        int rate = (m_recent.packets) * 1000LL / used_time;
        assert(rate >= 0);
        return rate;
    }
    int get_recent_bytes() const { return m_recent.bytes; }
    int get_recent_packets() const { return m_recent.packets; }
    int64_t get_total_bytes() const { return m_total_bytes; }
    int64_t get_total_packets() const { return m_total_packets; }

    void clear()
    {
        for (int i = 0; i < m_counts.size(); ++i)
        {
            m_counts[i].clear();
        }
        m_total_bytes = 0;
        m_total_packets = 0;
        m_recent.clear();
        m_latest.clear();
        m_update_times = 0;
        m_last_update_time.sync();
    }

private:
    time_counter32 m_last_update_time;
    // history of rates a few seconds back
    std::vector<meter_count> m_counts;
    int m_update_times;
    int64_t m_total_bytes;
    int64_t m_total_packets;
    meter_count m_recent;
    meter_count m_latest;
};


class simple_flow_meter
{
public:
    typedef flow_meter::meter_count meter_count;

    explicit simple_flow_meter(int unit_count = 5)
        : m_counts(unit_count)
    {
        assert(unit_count > 0);
        clear();
    }

    void add(int count)
    {
        assert(count >= 0);
        int64_t timediff = m_start_time.get_elapsed();
        int posdiff = (timediff + 1000 - 1) / 1000;
        int pos = posdiff % m_counts.size();
        if (pos != m_last_position)
        {
            step_next(pos);
        }
        m_counts[pos].bytes += count;
        m_counts[pos].packets++;
        m_recent.bytes += count;
        m_recent.packets++;
        m_total_bytes += count;
        ++m_total_packets;
    }

    // should be called once every second
    void update()
    {
        int64_t timediff = m_start_time.get_elapsed();
        int posdiff = (timediff + 1000 - 1) / 1000;
        int pos = posdiff % m_counts.size();
        if (pos != m_last_position)
        {
            step_next(pos);
        }
    }
    int get_rate() const
    {
        assert(m_update_times >= 0);
        if (0 == m_update_times)
            return m_latest.bytes;
        int times = m_update_times < m_counts.size() ? m_update_times : m_counts.size();
        assert(m_recent.bytes >= 0);
        int rate = (m_recent.bytes) / times;
        assert(rate >= 0);
        return rate;
    }
    int get_packet_rate() const
    {
        assert(m_update_times >= 0);
        if (0 == m_update_times)
            return m_latest.packets;
        int times = m_update_times < m_counts.size() ? m_update_times : m_counts.size();
        assert(m_recent.packets >= 0);
        int rate = (m_recent.packets) / times;
        assert(rate >= 0);
        return rate;
    }
    int get_recent_bytes() const { return m_recent.bytes; }
    int get_recent_packets() const { return m_recent.packets; }
    int64_t get_total_bytes() const { return m_total_bytes; }
    int64_t get_total_packets() const { return m_total_packets; }

    void clear()
    {
        for (int i = 0; i < m_counts.size(); ++i)
        {
            m_counts[i].clear();
        }
        m_total_bytes = 0;
        m_total_packets = 0;
        m_recent.clear();
        m_latest.clear();
        m_update_times = 0;
        m_last_update_time.sync();
        m_start_time.sync();
        m_last_position = 0;
    }

private:
    void step_next(int pos)
    {
        assert(pos != m_last_position);
        if (pos < m_last_position)
            pos += m_counts.size();
        for (int i = m_last_position + 1; i <= pos; ++i)
        {
            assert(m_recent.bytes >= 0);
            assert(m_recent.packets >= 0);
            int offset = i % m_counts.size();
            m_recent -= m_counts[offset];
            assert(m_recent.bytes >= 0);
            assert(m_recent.packets >= 0);
            m_counts[offset].clear();
            ++m_update_times;
        }
        m_last_position = pos % m_counts.size();
    }

private:
    time_counter m_start_time;
    time_counter32 m_last_update_time;
    // history of rates a few seconds back
    std::vector<meter_count> m_counts;
    int m_update_times;
    int m_last_position;
    int64_t m_total_bytes;
    int64_t m_total_packets;
    meter_count m_recent;
    meter_count m_latest;
};


}
