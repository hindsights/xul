#pragma once

#include <xul/config.hpp>

#include <xul/data/tchar.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <stdarg.h>
#include <assert.h>


#if defined(XUL_WINDOWS)
#include <xul/mswin/windows.hpp>
#elif defined(ANDROID)
#include <android/log.h>
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif


namespace xul {


class tracer
{
public:
#if defined(XUL_WINDOWS) && !defined(XUL_CONSOLE_TRACE)
    static void output(const wchar_t* str)
    {
        assert(str != NULL);
        ::OutputDebugStringW(str);
    }
    static void trace(const wchar_t* formatString, ...)
    {

        const size_t max_size = 4096;
        int nBuf;
        wchar_t szBuffer[max_size + 1] = { 0 };

        va_list args;
        va_start(args, formatString);
        nBuf = _vsnwprintf(szBuffer, max_size, formatString, args);
        va_end(args);

        // was there an error? was the expanded string too long?
        assert(nBuf >= 0);

        output(szBuffer);

    }
#if !defined(_WIN32_WCE)
    static void output(const char* str)
    {
        assert(str != NULL);
        ::OutputDebugStringA(str);
    }
    static void trace(const char* formatString, ...)
    {

        const size_t max_size = 4096;
        int nBuf;
        char szBuffer[max_size + 1] = { 0 };

        va_list args;
        va_start(args, formatString);
        nBuf = vsnprintf(szBuffer, max_size, formatString, args);
        va_end(args);

        // was there an error? was the expanded string too long?
        assert(nBuf >= 0);

        output(szBuffer);

    }
#endif
#elif defined(ANDROID)
    static void output(const char* str)
    {
        assert(str != NULL);
        __android_log_write(ANDROID_LOG_INFO, "trace", str);
    }
    static void trace(const char* formatString, ...)
    {
        assert(formatString != NULL);
        va_list args;
        va_start(args, formatString);
        __android_log_vprint(ANDROID_LOG_INFO, "trace", formatString, args);
        va_end(args);
    }
#else
    static void output(const char* str)
    {
        assert(str != NULL);
        fprintf(stderr, "%s", str);
    }
    static void trace(const char* formatString, ...)
    {
        assert(formatString != NULL);
        va_list args;
        va_start(args, formatString);
        vprintf(formatString, args);
        va_end(args);
    }
#endif
};


class scoped_tracer : private boost::noncopyable
{
public:
    explicit scoped_tracer(const std::string& msg) : m_msg(msg)
    {
        xul::tracer::trace("%s begin\n", m_msg.c_str());
    }
    ~scoped_tracer()
    {
        xul::tracer::trace("%s end\n", m_msg.c_str());
    }

private:
    std::string m_msg;
};


}



#ifdef _DEBUG
#  define XULTRACE        ::xul::tracer::trace
#  define XULTRACE0        ::xul::tracer::output
#else
#  define XULTRACE        1 ? (void)0 : ::xul::tracer::trace
#  define XULTRACE0        1 ? (void)0 : ::xul::tracer::output
#define XUL_TRACE ::xul::tracer::trace
#endif

#define XUL_REL_TRACE ::xul::tracer::trace


#ifndef TRACE
#define TRACE XULTRACE
#endif

#ifndef TRACE0
#define TRACE0 XULTRACE0
#endif

#define XULTRACEOUT    ::xul::tracer::trace
//#define TRACEOUT    ::xul::tracer::trace


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
