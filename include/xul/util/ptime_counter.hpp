#pragma once

#include <xul/util/ptime.hpp>
#include <xul/std/strings.hpp>
#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <stdint.h>


namespace xul {


class ptime_counter
{
public:
    ptime_counter()
    {
        sync();
    }
    explicit ptime_counter(const boost::posix_time::ptime& t) : m_time(t)
    {
    }

    bool operator<(const ptime_counter& t) const
    {
        return m_time < t.m_time;
    }

    void sync()
    {
        m_time = get_system_time();
    }
    int64_t elapsed() const
    {
        return get_elapsed_time().total_milliseconds();
    }
    int64_t get_elapsed() const
    {
        return get_elapsed_time().total_milliseconds();
    }
    uint32_t elapsed32() const
    {
        return static_cast<uint32_t>( get_elapsed() );
    }
    uint32_t get_elapsed32() const
    {
        return static_cast<uint32_t>( get_elapsed() );
    }
    int elapsed_seconds() const
    {
        return get_elapsed_time().total_seconds();
    }
    int get_elapsed_seconds() const
    {
        return get_elapsed_time().total_seconds();
    }

    boost::posix_time::time_duration elapsed_time() const
    {
        return get_system_time() - m_time;
    }
    boost::posix_time::time_duration get_elapsed_time() const
    {
        return get_system_time() - m_time;
    }

    const boost::posix_time::ptime& get_time() const
    {
        return m_time;
    }

    static boost::posix_time::ptime get_system_time()
    {
        return boost::posix_time::microsec_clock::universal_time();
    }
    std::string str() const
    {
        std::ostringstream os;
        os << m_time;
        return os.str();
    }

private:
    boost::posix_time::ptime m_time;
};


/// time counter based on boost.ptime(unit: millisecond)
class local_ptime_counter
{
public:
    local_ptime_counter()
    {
        sync();
    }
    explicit local_ptime_counter(const boost::posix_time::ptime& t) : m_time(t)
    {
    }

    bool operator<(const local_ptime_counter& t) const
    {
        return m_time < t.m_time;
    }

    void sync()
    {
        m_time = get_system_time();
    }
    int64_t elapsed() const
    {
        return (get_system_time() - m_time).total_milliseconds();
    }
    int64_t get_elapsed() const
    {
        return (get_system_time() - m_time).total_milliseconds();
    }
    uint32_t elapsed32() const
    {
        return static_cast<uint32_t>( elapsed() );
    }

    boost::posix_time::time_duration elapsed_time() const
    {
        return get_system_time() - m_time;
    }
    boost::posix_time::time_duration get_elapsed_time() const
    {
        return get_system_time() - m_time;
    }

    const boost::posix_time::ptime& get_time() const
    {
        return m_time;
    }

    static boost::posix_time::ptime get_system_time()
    {
        return boost::posix_time::microsec_clock::local_time();
    }

private:
    boost::posix_time::ptime m_time;
};


inline std::ostream& operator<<(std::ostream& os, const ptime_counter& t)
{
    return os << t.get_time();
}

inline std::ostream& operator<<(std::ostream& os, const local_ptime_counter& t)
{
    return os << t.get_time();
}


}
