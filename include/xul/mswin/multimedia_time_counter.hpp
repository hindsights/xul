#pragma once

#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")


namespace xul {


class multimedia_time_counter
{
public:
    static DWORD get_realtime_count()
    {
        return ::timeGetTime();
    }

    multimedia_time_counter()
    {
        sync();
    }

    DWORD get_count() const
    {
        return m_time;
    }

    DWORD elapsed() const
    {
        return get_realtime_count() - m_time;
    }

    void sync()
    {
        m_time = get_realtime_count();
    }

private:
    DWORD m_time;
};


}



