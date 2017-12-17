#pragma once

#include <xul/config.hpp>

#ifdef XUL_WINDOWS
#include <xul/mswin/windows.hpp>
#else
#include <stdlib.h>
#include <unistd.h> // _SC_PAGESIZE
#endif

namespace xul {


class system_information
{
public:
    static int get_page_size()
    {
        static int s = 0;
        if (s != 0) return s;

#ifdef BOOST_WINDOWS
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        s = si.dwPageSize;
#else
        s = sysconf(_SC_PAGESIZE);
#endif
        // assume the page size is 4 kiB if we
        // fail to query it
        if (s <= 0) s = 4096;
        return s;
    }
};


}
