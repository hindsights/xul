#pragma once

#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <assert.h>
#include <xul/util/trace.hpp>

#if defined(BOOST_WINDOWS)

#include <xul/mswin/windows.hpp>

namespace xul {


/// ms windows module
class module
{
public:
    typedef HMODULE handle_type;

    explicit module(HMODULE hModule = NULL) : m_handle(hModule)
    {
    }
    ~module()
    {
    }

    static module self()
    {
        module thisModule;
        thisModule.attach_to_self();
        return thisModule;
    }

    /// find module handle by memory address
    static HMODULE query_module_handle(PVOID address)
    {
        MEMORY_BASIC_INFORMATION meminfo = { 0 };
        const size_t sizeof_meminfo = sizeof(MEMORY_BASIC_INFORMATION);
        if (sizeof_meminfo != ::VirtualQuery(address, &meminfo, sizeof_meminfo))
        {
            TRACE(_T("VirtualQuery failed.\n"));
            return NULL;
        }
        return static_cast<HMODULE>(meminfo.AllocationBase);
    }

    bool attach_to_self()
    {
        return attach_address(&module::query_module_handle);
    }

    /// attach module handle by memory address
    bool attach_address(PVOID address)
    {
        m_handle = query_module_handle(address);
        if (m_handle != NULL)
        {
            //assert( false == get_file_path().empty() );
        }
        return this->is_open();
    }

    HMODULE native_handle() const { return m_handle; }

    bool is_open() const { return m_handle != NULL; }

#ifdef _WIN32_WCE
    /// get dll's export item
    FARPROC get_export_item(LPCWSTR name)
    {
        if (!is_open())
            return NULL;
        return ::GetProcAddress(m_handle, name);
    }
#else
/// get dll's export item
    FARPROC get_export_item(const char* name)
    {
        if (!is_open())
            return NULL;
        return ::GetProcAddress(m_handle, name);
    }
#endif

    std::string get_file_path() const
    {
        const size_t max_path_size = 4095;
        char pathstr[max_path_size + 1];
        DWORD len = ::GetModuleFileNameA( m_handle, pathstr, max_path_size );
        if ( 0 == len || len > max_path_size )
        {
            TRACE(_T("GetModuleFileName failed %p %lu %d\n"), m_handle, len, ::GetLastError());
            return std::string();
        }
        assert( '\0' == pathstr[len] );
        return std::string(pathstr, len);
    }

protected:
    HMODULE m_handle;
};


class loadable_module : public module, private boost::noncopyable
{
public:
    loadable_module()
    {
    }

    explicit loadable_module(const char* filename)
    {
        m_handle = ::LoadLibrary(filename);
    }

    ~loadable_module()
    {
        close();
    }

    bool load(const char* filename)
    {
        assert(!is_open());
        close();
        m_handle = ::LoadLibrary(filename);
        return is_open();
    }

    /// release handle
    void close()
    {
        if (is_open())
        {
            ::FreeLibrary(m_handle);
            m_handle = NULL;
        }
    }
};


}


#else

#include <dlfcn.h>

namespace xul {


/// posix module
class module
{
public:
    typedef void* HMODULE;
    typedef void* handle_type;

    explicit module(HMODULE hModule = NULL) : m_handle(hModule)
    {
    }
    ~module()
    {
    }

    HMODULE native_handle() const { return m_handle; }

    bool is_open() const { return m_handle != NULL; }

    void* get_export_item(const char* name)
    {
        if (!is_open())
            return NULL;
        return ::dlsym(native_handle(), name);
    }



protected:
    HMODULE m_handle;
};


class loadable_module : public module, private boost::noncopyable
{
public:
    loadable_module()
    {
    }

    explicit loadable_module(const char* filename)
    {
        m_handle = ::dlopen(filename, RTLD_NOW);
    }

    ~loadable_module()
    {
        close();
    }

    bool load(const char* filename)
    {
        assert(!is_open());
        close();
        m_handle = ::dlopen(filename, RTLD_NOW);
        if (is_open())
            return true;
        XULTRACE("dlopen failed file=%s error=%s\n", filename, dlerror());
        return false;
    }

    void close()
    {
        if (is_open())
        {
            int err = ::dlclose(m_handle);
            m_handle = NULL;
            assert(0 == err);
        }
    }
};


}

#endif


