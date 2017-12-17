#pragma once

#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <xul/config.hpp>
#include <stdint.h>


#if defined(BOOST_WINDOWS)

#include <xul/mswin/windows.hpp>

namespace xul {

class thread : private boost::noncopyable
{
public:
    typedef DWORD id_type;

    static void sleep(uint32_t milliseconds)
    {
        ::Sleep(milliseconds);
    }

    static id_type current_thread_id()
    {
        return ::GetCurrentThreadId();
    }
};

}

#else

#include <unistd.h>
//#include <pthread.h>
#include <sys/types.h>
#if defined(XUL_MACH)
#include <unistd.h>
#include <pthread.h>
#else
#include <linux/unistd.h>
#include <sys/syscall.h>
#endif

namespace xul {

class thread : private boost::noncopyable
{
public:
    typedef unsigned long id_type;

    static void sleep(uint32_t milliseconds)
    {
        //assert(milliseconds < 1 * 1000 * 1000);
        usleep(milliseconds * 1000);
    }

#if defined(XUL_MACH)
    static id_type current_thread_id()
    {
        //return pthread_self();
        return pthread_mach_thread_np(pthread_self());
    }
#else
    static id_type current_thread_id()
    {
        //return pthread_self();
        return syscall(__NR_gettid);
    }
#endif
};

}

#endif

