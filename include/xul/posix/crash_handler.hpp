#pragma once

#include <xul/std/containers.hpp>
#include <xul/os/paths.hpp>
#include <xul/io/stdfile.hpp>
#include <xul/data/date_time.hpp>
#include <boost/noncopyable.hpp>
#include <set>
#include <signal.h>
#include <dlfcn.h>

#ifndef ANDROID
#include <execinfo.h>
#else
#include <unwind.h>

typedef struct {
    size_t count;
    size_t ignore;
    const void** addrs;
} stack_crawl_state_t;

inline _Unwind_Reason_Code trace_function(_Unwind_Context *context, void *arg)
{
    printf("trace_function %p\n", arg);
    stack_crawl_state_t* state = (stack_crawl_state_t*)arg;
    printf("trace_function %d\n", state->count);
    if (state->count) {
        void* ip = (void*)_Unwind_GetIP(context);
        printf("trace_function ip %p\n", ip);
        if (ip) {
            if (state->ignore) {
                state->ignore--;
            } else {
                state->addrs[0] = ip;
                state->addrs++;
                state->count--;
            }
        }
    }
    return _URC_NO_REASON;
}

inline int backtrace(const void** addrs, size_t size)
{
    stack_crawl_state_t state;
    state.count = size;
    state.ignore = 0;
    state.addrs = addrs;
    _Unwind_Backtrace(trace_function, (void*)&state);
    return size - state.count;
}

#endif



namespace xul {


class crash_handler : private boost::noncopyable
{
public:
    static void install()
    {
        instance().enable_signal(SIGSEGV);
        instance().enable_signal(SIGBUS);
    }

    static crash_handler& instance()
    {
        static crash_handler handler;
        return handler;
    }
    static void set_silent_mode(bool val)
    {
        return;
    }

//private:
    static void handle_signal(int signum)
    {
        fprintf(stderr, "signal raised %d\n", signum);
        instance().log_signal(signum);
        exit(1111);
    }
    void print_trace (xul::stdfile& fout)
     {
        const void *array[64];
        size_t size;
        size_t i;
        size = backtrace (array, 64);
        //strings = backtrace_symbols (array, size);
        fout.write_format("Obtained %zd stack frames.\n", size);
        for (i = 0; i < size; i++)
            fout.write_format ("%p\n", array[i]);
     }

    void log_signal(int signum)
    {
        printf("log_signal %d\n", signum);
        std::string logFilePath = xul::paths::join(m_log_folder, "crash.log");
        xul::stdfile_writer fout;
        if (false == fout.open_text_appending(logFilePath.c_str()))
        {
            printf("failed open file %s\n", logFilePath.c_str());
            return;
        }
        std::string timestr = xul::date_time::now().str();
        fout.write_format("crash:\n");
        fout.write_format("time: %s\n", timestr.c_str());
        print_trace(fout);
        fout.write("\ncrash end.\n\n");
        fout.flush();
        fout.close();
        printf("write crash.log ok.\n");
    }

private:
    crash_handler()
    {
        m_log_folder = ".";
    }

    ~crash_handler()
    {
    }

    void enable_signal(int signum)
    {
        if (xul::containers::contains(m_signals, signum))
            return;
        signal(signum, handle_signal);
        m_signals.insert(signum);
    }
    void disable_signal(int signum)
    {
    }

public:
    void set_log_folder(const std::string& logFolder)
    {
        m_log_folder = logFolder;
    }

private:
    std::set<int> m_signals;
    std::string m_log_folder;
};


}
