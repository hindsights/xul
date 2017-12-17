#pragma once

#include <xul/config.hpp>

#ifdef XUL_WINDOWS
#include <xul/mswin/windows.hpp>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#else
#include <stdlib.h>
#include <unistd.h> // _SC_PAGESIZE
#include <sys/time.h>
#include <sys/resource.h>
#endif

namespace xul {


#ifdef XUL_WINDOWS

class process_status
{
public:
    process_status()
    {
        retrieve(GetCurrentProcess());
    }

    bool retrieve(HANDLE handle)
    {
        m_info.cb = sizeof(m_info);
        return FALSE != ::GetProcessMemoryInfo(handle, &m_info, sizeof(m_info));
    }
    int64_t get_memory_usage() const
    {
        return m_info.PrivateUsage;
    }
    const PROCESS_MEMORY_COUNTERS_EX& get_info() const
    {
        return m_info;
    }

private:
    PROCESS_MEMORY_COUNTERS_EX m_info;
};

#else

class process_status
{
public:
    process_status()
    {
        retrieve(RUSAGE_SELF);
    }

    bool retrieve(int who)
    {
        return 0 == ::getrusage(who, &m_info);
    }
    int64_t get_memory_usage() const
    {
        return m_info.ru_idrss;
    }
    const PROCESS_MEMORY_COUNTERS_EX& get_info() const
    {
        return m_counters;
    }

private:
    struct rusage m_info;
};

#endif


}
