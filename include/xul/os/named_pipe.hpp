#pragma once

#include <xul/util/trace.hpp>
#include <boost/config.hpp>
#include <boost/noncopyable.hpp>

#if defined(BOOST_WINDOWS)

#include <xul/mswin/windows.hpp>

namespace xul {


class named_pipe : private boost::noncopyable
{
public:
    typedef HANDLE native_type;

    named_pipe() : m_handle(INVALID_HANDLE_VALUE)
    {
    }
    ~named_pipe()
    {
        close();
    }

    bool is_open() const { return m_handle != INVALID_HANDLE_VALUE; }
    void close()
    {
        if (is_open())
        {
            ::CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }
    }

    bool create_input(const char* filename)
    {
        return create(filename, PIPE_ACCESS_INBOUND);
    }

    size_t read(void* buffer, size_t size)
    {
        assert(buffer != NULL && size > 0);
        assert(is_open());
        DWORD bytes = 0;
        if (::ReadFile(m_handle, buffer, size, &bytes, NULL))
        {
            //assert(bytes > 0);
            return bytes;
        }
        XULTRACE("file::read ReadFile failed. size=%u, bytes=%u error=%d\n", size, bytes, ::GetLastError());
        return 0;
    }


private:
    bool create(const char* filename, DWORD openMode)
    {
        assert(filename);
        if (NULL == filename)
            return false;
        assert(false == is_open());
        close();
        DWORD pipeMode = PIPE_TYPE_BYTE;
        DWORD maxInstances = 1;
        DWORD outBufferSize = 4096;
        DWORD inBufferSize = 4096;
        DWORD defaultTimeout = 5000;
        m_handle = ::CreateNamedPipe(filename, openMode, pipeMode, maxInstances, outBufferSize, inBufferSize, defaultTimeout, NULL);
        if (is_open())
            return true;
        int errcode = ::GetLastError();
        XULTRACEOUT("failed to create named pipe %s, errcode=%d\n", filename, errcode);
        return false;
    }

private:
    HANDLE m_handle;
};

}

#else

#include <sys/types.h>
#include <sys/stat.h>

namespace xul {

class named_pipe : private boost::noncopyable
{
public:
    typedef int native_type;

    named_pipe() : m_handle(-1)
    {
    }
    ~named_pipe()
    {
        close();
    }

    bool is_open() const { return m_handle != -1; }
    void close()
    {
        if (m_filename.size() > 0)
        {
        //    ::unlink(m_filename.c_str());
            m_filename.clear();
        }
        if (is_open())
        {
            ::close(m_handle);
            m_handle = -1;
        }
    }

    bool create_input(const char* filename)
    {
        return create(filename, O_RDONLY | O_NONBLOCK);
    }

    size_t read(void* buffer, size_t size)
    {
        assert(buffer != NULL && size > 0);
        assert(is_open());
        ssize_t bytes = ::read(m_handle, buffer, size);
        if (bytes > 0)
        {
            //assert(bytes > 0);
            return bytes;
        }
        XULTRACE("file::read ReadFile failed. size=%u, bytes=%d error=%d\n", size, bytes, errno);
        return 0;
    }


private:
    bool create(const char* filename, int openMode)
    {
        assert(filename);
        if (NULL == filename)
            return false;
        assert(false == is_open());
        close();
        if (mkfifo(filename, O_CREAT|O_EXCL) != 0)
        {
            int errcode = errno;
            XULTRACE("named_pipe::create failed %s %d %d\n", filename, 0777, errcode);
            if (errcode != EEXIST)
            {
                return false;
            }
        }
        m_handle = open(filename, openMode);
        if (false == is_open())
        {
            XULTRACE("named_pipe::create open failed %s %d %d\n", filename, openMode, errno);
            return false;
        }
        m_filename = filename;
        return true;
    }

private:
    std::string m_filename;
    int m_handle;
};

}

#endif

