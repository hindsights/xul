#pragma once

/**
 * @file
 * @brief memory leak checker
 */

#include <boost/config.hpp>

#if defined(BOOST_WINDOWS)

#if defined(_DEBUG)

#pragma message("------ crt memory checking utilities")

#include <crtdbg.h>

class CrtDbgFlag
{
public:
    static void Set(int newFlag)
    {
        _CrtSetDbgFlag(newFlag);
    }
    static int Get()
    {
        return _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    }
    static void Enable(int flagBit)
    {
        Set(Get() | flagBit);
    }
    static void Disable(int flagBit)
    {
        Set(Get() & (~flagBit));
    }
    static void EnableLeakCheck()
    {
        Enable(_CRTDBG_LEAK_CHECK_DF);
    };
    static void EnableAlwaysCheck()
    {
        Enable(_CRTDBG_CHECK_ALWAYS_DF);
    }
};

#endif



#if defined(_DEBUG) && !defined(XUL_DISABLE_MEMORY_LEAK_CHECKING)

#  pragma message("------enable memory leak checking")
#  define XUL_MEMORY_LEAK_CHECKING_ENABLED

#  if defined(XUL_ENABLE_REDEFINE_NEW)

#    include <crtdbg.h>

#    include <new>
#    include <memory>
#    include <fstream>
#    include <xtree>

#    define XUL_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new XUL_DEBUG_NEW
#    pragma message("------redefine new as macro")

#  else

#    include <stdlib.h>
#    include <malloc.h>

#    define _CRTDBG_MAP_ALLOC
#    include <crtdbg.h>

#  endif

#define XUL_CHECK_MEMORY_LEAK()        do { CrtDbgFlag::EnableLeakCheck(); } while (0)

#else
#define XUL_CHECK_MEMORY_LEAK()
#endif


class MemoryLeakChecker
{
public:
    MemoryLeakChecker()
    {
        XUL_CHECK_MEMORY_LEAK();
    }
private:
    MemoryLeakChecker(const MemoryLeakChecker&);
    void operator=(const MemoryLeakChecker&);
};

#endif

