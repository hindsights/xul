#pragma once

#include <xul/mswin/windows.hpp>
#include <boost/noncopyable.hpp>
#include <assert.h>



namespace xul {


class event : private boost::noncopyable
{
public:
    event() : m_handle(NULL)
    {
    }
    ~event()
    {
        this->close();
    }
    bool is_open() const { return m_handle != NULL; }

    void close()
    {
        if (is_open())
        {
            ::CloseHandle(m_handle);
            m_handle = NULL;
        }
    }

    BOOL signal()
    {
        assert(is_open());
        return ::SetEvent(m_handle);
    }
    BOOL reset()
    {
        assert(is_open());
        return ::ResetEvent(m_handle);
    }
    BOOL pulse()
    {
        assert(is_open());
        return ::PulseEvent(m_handle);
    }

    DWORD wait()
    {
        return wait(INFINITE);
    }
    DWORD wait(DWORD milliseconds)
    {
        return ::WaitForSingleObject(m_handle, milliseconds);
    }
    DWORD get_state()
    {
        return this->wait(0);
    }
    bool is_signaled()
    {
        return get_state() == WAIT_OBJECT_0;
    }
    HANDLE native_handle() const { return m_handle; }

    bool create(bool isManualReset, bool initialState = false, LPCTSTR name = NULL)
    {
        this->close();
        attach(::CreateEvent(NULL, isManualReset ? TRUE : FALSE, initialState ? TRUE : FALSE, name));
        return is_open();
    }

    bool open(LPCTSTR name, DWORD desiredAccess, bool inheritHandle = false)
    {
        assert(name != NULL);
        this->close();
        attach(::OpenEvent(desiredAccess, inheritHandle ? TRUE : FALSE, name));
        return is_open();
    }

protected:
    void attach(HANDLE hEvent)
    {
        assert(!is_open());
        m_handle = hEvent;
    }

private:
    HANDLE    m_handle;
};


class manual_reset_event : public event
{
public:
    explicit manual_reset_event(BOOL initState = FALSE, LPCTSTR name = NULL)
    {
        attach(::CreateEvent(NULL, TRUE, initState, name));
        assert(is_open());
    }
};

class auto_reset_event : public event
{
public:
    explicit auto_reset_event(BOOL initState = FALSE, LPCTSTR name = NULL)
    {
        attach(::CreateEvent(NULL, FALSE, initState, name));
        assert(is_open());
    }
};



class wait_handle
{
public:
    static DWORD wait_any(const event* handles, DWORD count, DWORD milliseconds = INFINITE)
    {
        return ::WaitForMultipleObjects(count, reinterpret_cast<const HANDLE*>(handles), FALSE, milliseconds);
    }
};

}

