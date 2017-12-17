#pragma once

#include <xul/macro/fill_zero.hpp>
#include <boost/noncopyable.hpp>
#include <assert.h>

#include <tlhelp32.h>
#include <psapi.h>
#if defined(_WIN32_WCE)
#pragma comment(lib, "toolhelp.lib")
#else
#pragma comment(lib, "psapi.lib")
#endif


namespace xul {


class kernel_object_handle_closer
{
public:
    static bool close(HANDLE handle)
    {
        return ::CloseHandle(handle) != FALSE;
    }
};

template <typename HandleT, typename CloserT>
class handle_wrapper : private boost::noncopyable
{
public:
    handle_wrapper() : m_handle(null_handle())
    {
    }
    ~handle_wrapper()
    {
        close();
    }

    HandleT native_handle() const    { return m_handle; }
    bool is_open() const        { return native_handle() != null_handle(); }
    void close()
    {
        if (is_open())
        {
            CloserT::close(m_handle);
            m_handle = null_handle();
        }
    }
    void reset(HandleT handle)
    {
        close();
        m_handle = handle;
    }

    static HandleT null_handle()
    {
        return NULL;
    }

private:
    HandleT    m_handle;
};



class system_snapshot : public handle_wrapper<HANDLE, kernel_object_handle_closer>
{
public:
    void open(DWORD flags, DWORD pid = 0)
    {
        reset(::CreateToolhelp32Snapshot(flags, pid));
    }
    void open_processes()
    {
        open(TH32CS_SNAPPROCESS);
    }
    void open_heaps(DWORD pid)
    {
        open(TH32CS_SNAPHEAPLIST, pid);
    }
    void open_threads(DWORD pid)
    {
        open(TH32CS_SNAPTHREAD, pid);
    }
    void open_modules(DWORD pid)
    {
        open(TH32CS_SNAPMODULE, pid);
    }
};


class process_walker : private boost::noncopyable
{
public:
    typedef PROCESSENTRY32 item_type;

    process_walker()
    {
        XUL_FILL_ZERO(m_item);
        m_item.dwSize = sizeof(m_item);
        m_snapshot.open_processes();
        assert(m_snapshot.is_open());
        m_isValid = (::Process32First(m_snapshot.native_handle(), &m_item) != FALSE);
    }

    bool has_more() const
    {
        return m_isValid;
    }
    const PROCESSENTRY32& current() const
    {
        return m_item;
    }
    void move_next()
    {
        assert(has_more());
        m_isValid = (::Process32Next(m_snapshot.native_handle(), &m_item) != FALSE);
    }

private:
    system_snapshot    m_snapshot;
    PROCESSENTRY32        m_item;
    bool                m_isValid;
};



class module_walker : private boost::noncopyable
{
public:
    typedef MODULEENTRY32 item_type;

    explicit module_walker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
    {
        XUL_FILL_ZERO(m_item);
        m_item.dwSize = sizeof(MODULEENTRY32);
        m_snapshot.open_modules(pid);
        assert(m_snapshot.is_open());
        m_isValid = (FALSE != ::Module32First(m_snapshot.native_handle(), &m_item));
    }
    ~module_walker()
    {
    }

    bool has_more() const
    {
        return m_isValid;
    }
    void move_next()
    {
        assert(has_more());
        m_isValid = (FALSE != ::Module32Next(m_snapshot.native_handle(), &m_item));
    }
    const MODULEENTRY32& current() const
    {
        return m_item;
    }

private:
    system_snapshot m_snapshot;
    MODULEENTRY32    m_item;
    bool    m_isValid;
};

class thread_walker : private boost::noncopyable
{
public:
    typedef THREADENTRY32 item_type;

    explicit thread_walker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
    {
        XUL_FILL_ZERO(m_item);
        m_item.dwSize = sizeof(MODULEENTRY32);
        m_snapshot.open_threads(pid);
        assert(m_snapshot.is_open());
        m_isValid = (FALSE != ::Thread32First(m_snapshot.native_handle(), &m_item));
    }
    ~thread_walker()
    {
    }

    bool has_more() const
    {
        return m_isValid;
    }
    void move_next()
    {
        assert(has_more());
        m_isValid = (FALSE != ::Thread32Next(m_snapshot.native_handle(), &m_item));
    }
    const THREADENTRY32& current() const
    {
        return m_item;
    }

private:
    system_snapshot m_snapshot;
    THREADENTRY32    m_item;
    bool    m_isValid;
};


}

