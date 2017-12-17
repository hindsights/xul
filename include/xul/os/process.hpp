#pragma once

#include <xul/util/log.hpp>
#include <xul/macro/fill_zero.hpp>
#include <boost/config.hpp>

#include <boost/algorithm/string/split.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <stdint.h>
#include <string>

#include <vector>


#if defined(BOOST_WINDOWS)

#include <xul/mswin/process_object.hpp>

namespace xul {


class process : private boost::noncopyable
{
public:
    typedef HANDLE native_type;
    typedef DWORD pid_type;
    typedef DWORD tid_type;

    process()
    {
    }
    ~process()
    {
        this->close();
    }

    static DWORD current_process_id()
    {
        return ::GetCurrentProcessId();
    }
    static DWORD current_thread_id()
    {
        return ::GetCurrentThreadId();
    }

    bool join( uint32_t milliseconds )
    {
        return m_object.join(milliseconds);
    }

    bool join()
    {
        return m_object.join();
    }

    void close()
    {
        if (is_open())
        {
            if ( is_alive() )
            {
                bool success = this->kill(123);
                assert( success );
            }
        }
        m_object.close();
    }

    HANDLE native_handle() const { return m_object.native_handle(); }
    DWORD native_id() const { return m_object.native_id(); }

    xul::mswin::process_object& native_object() { return m_object; }

    bool is_open() const
    {
        return m_object.is_open();
    }

    bool is_alive() const
    {
        return m_object.is_alive();
    }

    ULONG get_exit_code() const
    {
        return m_object.get_exit_code();
    }

    bool kill(DWORD exitCode)
    {
        return m_object.kill(exitCode);
    }

    bool start( const std::string& cmdline, STARTUPINFO& si, bool inheritHandles = false, bool createNoWindow = false )
    {
        assert( false == is_open() );
        assert( false == is_alive() );
        this->close();
        std::string cmdlinestr = cmdline;
        PROCESS_INFORMATION pi = { 0 };
        DWORD creationFlags = NORMAL_PRIORITY_CLASS;
        if (createNoWindow)
        {
            creationFlags |= CREATE_NO_WINDOW;
        }
        if ( FALSE == ::CreateProcess( NULL, &cmdlinestr[0], NULL, NULL, inheritHandles ? TRUE : FALSE, creationFlags, NULL, NULL, &si, &pi) )
        {
            APP_ERROR("failed to create process " << cmdlinestr << " errcode=" << ::GetLastError());
            return false;
        }
        m_object.attach(pi.hProcess, pi.dwProcessId);
        CloseHandle(pi.hThread);
        return true;
    }

    bool start( const std::string& cmdline, int outhandle, int inhandle = -1, int errhandle = -1, bool inheritHandles = false, bool createNoWindow = false )
    {
        return this->start(cmdline, get_pipe_handle(outhandle), get_pipe_handle(inhandle), get_pipe_handle(errhandle), inheritHandles);
    }

    bool start(const std::string& cmdline, HANDLE outhandle = INVALID_HANDLE_VALUE, HANDLE inhandle = INVALID_HANDLE_VALUE, HANDLE errhandle = INVALID_HANDLE_VALUE, bool inheritHandles = false, bool createNoWindow = false)
    {
        STARTUPINFO si;
        XUL_FILL_ZERO( si );
        si.cb = sizeof( si );
        if (INVALID_HANDLE_VALUE != outhandle)
        {
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdOutput = outhandle;
            inheritHandles = true;
        }
        else
        {
            //si.hStdOutput = ::GetStdHandle( STD_OUTPUT_HANDLE );
        }
        if (INVALID_HANDLE_VALUE != inhandle)
        {
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdInput = inhandle;
            inheritHandles = true;
        }
        else
        {
            //si.hStdInput = ::GetStdHandle( STD_INPUT_HANDLE );
        }
        if (INVALID_HANDLE_VALUE != errhandle)
        {
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdError = errhandle;
            inheritHandles = true;
        }
        else
        {
            //si.hStdError = ::GetStdHandle( STD_ERROR_HANDLE );
        }
        return this->start( cmdline, si, inheritHandles );
    }

    static HANDLE get_pipe_handle( int fd )
    {
        if (-1 == fd)
            return INVALID_HANDLE_VALUE;
        HANDLE h = reinterpret_cast<HANDLE>( _get_osfhandle( fd ) );
        SetHandleInformation( h, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );
        //::DuplicateHandle( )
        return h;
    }

protected:

private:
    xul::mswin::process_object m_object;
};


}

#else

#include <sys/wait.h>
#include <sys/types.h>

namespace xul {

class process : private boost::noncopyable
{
public:
    process() : m_pid( 0 )
    {
    }
    ~process()
    {
        this->close();
    }

    bool is_open() const
    {
        return m_pid > 0;
    }

    bool is_alive() const
    {
        if ( false == is_open() )
            return false;
        int s;
        if (::waitpid(m_pid, &s, 0) == -1)
            return false;
        return WIFSTOPPED(s);
    }

    bool kill()
    {
        return 0 == ::kill( m_pid, 9 );
    }

    bool start( const std::string& cmdline, int outhandle = -1, int inhandle = -1, int errhandle = -1)
    {
        printf("fork ok, execute command %s\n", cmdline.c_str());
        std::vector<std::string> tokens;
        boost::split(tokens, cmdline, boost::is_space());
        std::vector<char*> args;
        assert(tokens.size() >= 1);
        args.resize(tokens.size() + 1);
        for (size_t index = 0; index < tokens.size(); ++index)
        {
            args[index] = const_cast<char*>(tokens[index].c_str());
        }
        args[tokens.size()] = NULL;

        this->close();
        pid_t pid = fork ();
        if ( pid < (pid_t) 0 )
        {
            // The fork failed.
            fprintf (stderr, "Fork failed.\n");
            return false;
        }
        if ( pid > (pid_t)0 )
        {
            // This is the parent process.Close other end first.
            return true;
        }
        // This is the child process.Close other end first.
        //set_pipe_handle( outhandle, 1 );
        //set_pipe_handle( inhandle, 0 );
        //set_pipe_handle( errhandle, 2 );
        int ret = execv( tokens[0].c_str(), &args[0]);
        printf("fork ok, execute complete %s %d %d\n", tokens[0].c_str(), ret, errno);
        return true;
    }

    bool join()
    {
        int s;
        if (::waitpid(m_pid, &s, 0) == -1)
            return false;
        return true;
    }

    void close()
    {
        if (is_open())
        {
            if ( is_alive() )
            {
                bool success = this->kill();
                assert( success );
            }
            m_pid = 0;
        }
    }

protected:
    static void set_pipe_handle( int fd, int dest )
    {
        if ( fd != -1 )
        {
            dup2( fd, dest );
        }
    }

private:
    pid_t m_pid;
};

}

#endif

