#pragma once


#include <xul/os/module.hpp>
#include <xul/mswin/file_finder.hpp>
#include <xul/util/trace.hpp>
#include <imagehlp.h>
#include <share.h>

#include <boost/filesystem/path.hpp>



#pragma comment(lib, "version.lib")
#include <xul/macro/XUL_FILL_ZERO.hpp>
#include <xul/util/log.hpp>

#include <boost/noncopyable.hpp>

#include <tlhelp32.h>
#include <psapi.h>
#if defined(_WIN32_WCE)
//#pragma comment(lib, "toolhelp.lib")
#else
#pragma comment(lib, "psapi.lib")
#endif



namespace xul {


class FileVersionInfo
{
public:
    FileVersionInfo();
    explicit FileVersionInfo(LPCTSTR filename);

    bool Retrieve(LPCTSTR filename);

    const VS_FIXEDFILEINFO& GetFixedInfo() const
    {
        assert(!m_data.empty());
        return m_fixedInfo;
    }

private:
    std::string m_data;

    VS_FIXEDFILEINFO m_fixedInfo;
};



inline FileVersionInfo::FileVersionInfo()
{
    XUL_FILL_ZERO(m_fixedInfo);
}

inline FileVersionInfo::FileVersionInfo(LPCTSTR filename)
{
    Retrieve(filename);
}

inline bool FileVersionInfo::Retrieve(LPCTSTR filename)
{
    XUL_FILL_ZERO(m_fixedInfo);
    LPTSTR filepath = const_cast<LPTSTR>(filename);
    m_data.reserve(8 * 1024);
    DWORD dummyHandle = 0;
    DWORD size = ::GetFileVersionInfoSize(filepath, &dummyHandle);
    if (size <= 0)
    {
//        UTIL_ERROR("FileVersionInfo::Retrieve: GetFileVersionInfoSize failed " << make_tuple(::GetLastError(), dummyHandle));
        return false;
    }
    m_data.resize(size);
    if (!::GetFileVersionInfo(filepath, 0, size, &m_data[0]))
    {
//        UTIL_ERROR("FileVersionInfo::Retrieve: GetFileVersionInfo failed " << make_tuple(::GetLastError(), dummyHandle));
        return false;
    }
    void* buffer = NULL;
    UINT bufferLength = 0;
    const TCHAR* data = m_data.data();
    if (!::VerQueryValue(const_cast<TCHAR*>(data), TEXT("\\"), &buffer, &bufferLength))
    {
//        UTIL_ERROR("FileVersionInfo::Retrieve: VerQueryValue failed " << make_tuple(::GetLastError(), dummyHandle));
        return false;
    }
    if (bufferLength < sizeof(VS_FIXEDFILEINFO))
    {
//        UTIL_ERROR("FileVersionInfo::Retrieve: VerQueryValue failed buffer length error " << bufferLength);
        return false;
    }
    const VS_FIXEDFILEINFO* fixedInfo = static_cast<const VS_FIXEDFILEINFO*>(buffer);
    m_fixedInfo = *fixedInfo;
    return true;
}


struct KernelObjectHandleCloser
{
    static bool Close(HANDLE handle)
    {
        return ::CloseHandle(handle) != FALSE;
    }
};


template <typename HandleT, typename CloserT>
class HandleWrapper : private boost::noncopyable
{
public:
    HandleWrapper() : m_handle(NullHandle())
    {
    }
    ~HandleWrapper()
    {
        Close();
    }

    HandleT GetHandle() const    { return m_handle; }
    bool IsValid() const        { return GetHandle() != NullHandle(); }
    void Close()
    {
        if (IsValid())
        {
            CloserT::Close(m_handle);
            m_handle = NullHandle();
        }
    }
    void Attach(HandleT handle)
    {
        Close();
        m_handle = handle;
    }

    static HandleT NullHandle()
    {
        return NULL;
    }

private:
    HandleT    m_handle;
};


class ToolhelpSnapshot : public HandleWrapper<HANDLE, KernelObjectHandleCloser>
{
public:
    void Open(DWORD flags, DWORD pid = 0)
    {
        Attach(::CreateToolhelp32Snapshot(flags, pid));
    }
    void OpenProcesses()
    {
        Open(TH32CS_SNAPPROCESS);
    }
    void OpenHeaps(DWORD pid)
    {
        Open(TH32CS_SNAPHEAPLIST, pid);
    }
};


class ProcessWalker : private boost::noncopyable
{
public:
    ProcessWalker()
    {
        XUL_FILL_ZERO(m_item);
        m_item.dwSize = sizeof(m_item);
        m_snapshot.OpenProcesses();
        assert(m_snapshot.IsValid());
        m_isValid = (::Process32First(m_snapshot.GetHandle(), &m_item) != FALSE);
    }

    bool HasMore() const
    {
        return m_isValid;
    }
    const PROCESSENTRY32& Current() const
    {
        return m_item;
    }
    void MoveNext()
    {
        assert(HasMore());
        m_isValid = (::Process32Next(m_snapshot.GetHandle(), &m_item) != FALSE);
    }

private:
    ToolhelpSnapshot    m_snapshot;
    PROCESSENTRY32        m_item;
    bool                m_isValid;
};


class ModuleWalker : private boost::noncopyable
{
public:
    explicit ModuleWalker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
    {
        XUL_FILL_ZERO(m_item);
        m_item.dwSize = sizeof(MODULEENTRY32);
        m_snapshot.Open(TH32CS_SNAPMODULE, pid);
        assert(m_snapshot.IsValid());
        m_isValid = (FALSE != ::Module32First(m_snapshot.GetHandle(), &m_item));
    }
    ~ModuleWalker()
    {
    }

    bool HasMore() const
    {
        return m_isValid;
    }
    void MoveNext()
    {
        assert(HasMore());
        m_isValid = (FALSE != ::Module32Next(m_snapshot.GetHandle(), &m_item));
    }
    const MODULEENTRY32& Current() const
    {
        return m_item;
    }

private:
    ToolhelpSnapshot m_snapshot;
    MODULEENTRY32    m_item;
    bool    m_isValid;
};


class ThreadWalker : private boost::noncopyable
{
public:
    explicit ThreadWalker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
    {
        m_snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
        memset(&m_item, 0, sizeof(THREADENTRY32));
        m_item.dwSize = sizeof(THREADENTRY32);
        if (!IsSnapShotValid())
            return;
        m_isValid = (FALSE != ::Thread32First(m_snapShot, &m_item));
    }
    ~ThreadWalker()
    {
        ::CloseHandle(m_snapShot);
    }

    bool HasMore() const
    {
        return m_isValid;
    }
    void MoveNext()
    {
        assert(HasMore());
        m_isValid = (FALSE != ::Thread32Next(m_snapShot, &m_item));
    }
    const THREADENTRY32& Current() const
    {
        return m_item;
    }

protected:
    bool IsSnapShotValid() const
    {
        return m_snapShot != INVALID_HANDLE_VALUE;
    }

private:
    HANDLE    m_snapShot;
    THREADENTRY32    m_item;
    bool    m_isValid;
};


class DbghelpModule : private boost::noncopyable
{
public:
    typedef BOOL (__stdcall *FUNC_SymInitialize)(HANDLE, LPSTR, BOOL);
    typedef BOOL (__stdcall *FUNC_SymCleanup)(HANDLE);
    typedef BOOL (__stdcall *FUNC_StackWalk)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID,
        PREAD_PROCESS_MEMORY_ROUTINE,
        PFUNCTION_TABLE_ACCESS_ROUTINE,
        PGET_MODULE_BASE_ROUTINE,
        PTRANSLATE_ADDRESS_ROUTINE);
    typedef LPVOID (__stdcall *FUNC_SymFunctionTableAccess)(HANDLE, DWORD);
    typedef DWORD (__stdcall *FUNC_SymGetModuleBase)(HANDLE, DWORD);
    typedef BOOL (__stdcall *FUNC_SymGetSymFromAddr)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);


    DbghelpModule() : m_dbghelp(_T("dbghelp.dll"))
    {
        if (!m_dbghelp.is_open())
        {
            ::OutputDebugString(TEXT("Load dbghelp.dll failed\n"));
        }
        RetrieveFunc("SymInitialize", m_SymInitialize);
        RetrieveFunc("SymCleanup", m_SymCleanup);
        RetrieveFunc("StackWalk", m_StackWalk);
        RetrieveFunc("SymFunctionTableAccess", m_SymFunctionTableAccess);
        RetrieveFunc("SymGetModuleBase", m_SymGetModuleBase);
        RetrieveFunc("SymGetSymFromAddr", m_SymGetSymFromAddr);
    }


    static BOOL __stdcall SymInitialize(HANDLE hProcess, LPSTR UserSearchPath, BOOL fInvadeProcess)
    {
        FUNC_SymInitialize func = GetInstance().m_SymInitialize;
        if (func == NULL)
        {
            ::OutputDebugString(_T("DbghelpModule::SymInitialize, No SymInitialize\n"));
            return FALSE;
        }
        return (*func)(hProcess, UserSearchPath, fInvadeProcess);
    }
    static BOOL __stdcall SymCleanup(HANDLE hProcess)
    {
        FUNC_SymCleanup func = GetInstance().m_SymCleanup;
        if (func == NULL)
            return FALSE;
        return (*func)(hProcess);
    }
    static BOOL __stdcall StackWalk(
        DWORD MachineType, HANDLE hProcess, HANDLE hThread,
        LPSTACKFRAME StackFrame, LPVOID ContextRecord,
        PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
        PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
        PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
        PTRANSLATE_ADDRESS_ROUTINE TranslateAddress)
    {
        FUNC_StackWalk func = GetInstance().m_StackWalk;
        if (func == NULL)
            return FALSE;
        return (*func)(MachineType, hProcess, hThread, StackFrame, ContextRecord,
            ReadMemoryRoutine, FunctionTableAccessRoutine,
            GetModuleBaseRoutine, TranslateAddress);
    }
    static BOOL __stdcall SymGetSymFromAddr(HANDLE hProcess, DWORD dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_SYMBOL Symbol)
    {
        FUNC_SymGetSymFromAddr func = GetInstance().m_SymGetSymFromAddr;
        if (func == NULL)
            return FALSE;
        return (*func)(hProcess, dwAddr, pdwDisplacement, Symbol);
    }
    static LPVOID __stdcall SymFunctionTableAccess(HANDLE hProcess, DWORD dwAddr)
    {
        FUNC_SymFunctionTableAccess func = GetInstance().m_SymFunctionTableAccess;
        if (func == NULL)
            return FALSE;
        return (*func)(hProcess, dwAddr);
    }
    static DWORD __stdcall SymGetModuleBase(HANDLE hProcess, DWORD dwAddr)
    {
        FUNC_SymGetModuleBase func = GetInstance().m_SymGetModuleBase;
        if (func == NULL)
            return 0;
        return (*func)(hProcess, dwAddr);
    }

private:
    template <typename FuncT>
    void RetrieveFunc(LPCSTR name, FuncT& func)
    {
        func = reinterpret_cast<FuncT>(m_dbghelp.get_export_item(name));
        if (func == NULL)
        {
            xul::tracer::trace("CrashHandler: RetrieveFunc failed %s\n", name);
        }
    }

    static DbghelpModule& GetInstance()
    {
        static DbghelpModule module;
        return module;
    }

    xul::loadable_module m_dbghelp;
    FUNC_SymInitialize    m_SymInitialize;
    FUNC_SymCleanup        m_SymCleanup;
    FUNC_StackWalk        m_StackWalk;
    FUNC_SymFunctionTableAccess        m_SymFunctionTableAccess;
    FUNC_SymGetModuleBase            m_SymGetModuleBase;
    FUNC_SymGetSymFromAddr            m_SymGetSymFromAddr;
};


class SymbolEngine
{
    HANDLE    m_hProcess;
public:
    SymbolEngine() : m_hProcess(NULL) { }

    BOOL SymInitialize(HANDLE hProcess , LPSTR UserSearchPath , BOOL fInvadeProcess)
    {
        m_hProcess = hProcess;
        if (DbghelpModule::SymInitialize(hProcess, UserSearchPath, fInvadeProcess))
            return TRUE;
        TCHAR msg[513];
        DWORD errcode = ::GetLastError();
        _sntprintf(msg, 512, _T("SymbolEngine::SymInitialize FAILED %d(%#lx)\n"), errcode, errcode);
        ::OutputDebugString(msg);
        return FALSE;
    }
    BOOL SymCleanup()
    {
        return DbghelpModule::SymCleanup(m_hProcess);
    }
    BOOL StackWalk(DWORD MachineType, HANDLE hProcess, HANDLE hThread,
        LPSTACKFRAME StackFrame, LPVOID ContextRecord)
    {
        return DbghelpModule::StackWalk(MachineType, hProcess, hThread, StackFrame, ContextRecord,
            0,
            DbghelpModule::SymFunctionTableAccess,
            DbghelpModule::SymGetModuleBase,
            0);
    }
    BOOL SymGetSymFromAddr(DWORD dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_SYMBOL Symbol)
    {
        return DbghelpModule::SymGetSymFromAddr(m_hProcess, dwAddr, pdwDisplacement, Symbol);
    }
};


const UINT CRASH_HANDLER_DISABLE = 0x01;

const UINT CRASH_HANDLER_SUPPRESS = 0x02;

const UINT CRASH_HANDLER_SILENT = 0x04;


const UINT CRASH_HANDLER_VERY_SILENT = CRASH_HANDLER_SILENT | CRASH_HANDLER_SUPPRESS;


class crash_handler
{
public:
    typedef std::string string;
    typedef std::string tstring;
    crash_handler() : m_file(NULL)
    {
        m_ExeFilename = xul::module().get_file_path();

        xul::module module;
        module.attach_to_self();
        boost::filesystem::path dllPath(module.get_file_path());
        dllPath.remove_filename();

        std::string dllPathString = dllPath.string();
        std::string pdbPath = dllPathString + ";.;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;";

        m_IsValid = m_Engine.SymInitialize(::GetCurrentProcess(), const_cast<char*>(dllPathString.c_str()), TRUE);
        if (!m_IsValid)
            return;

        m_log_folder = dllPath.string();

        dllPath /= "crash.log";
        std::string logfile = dllPath.string();
        m_file = _fsopen(logfile.c_str(), "a+t", _SH_DENYWR);

        SYSTEMTIME st;
        ::GetLocalTime(&st);
        LogMsg("Crash: %s\n", ::GetCommandLineA());
        LogMsg("Time : ");
        LogTime( st );
        LogMsg( "\n" );

        LogModules();
    }
    ~crash_handler()
    {
        LogMsg("Crash End.\n\n\n");
        if (m_file != NULL)
        {
            fflush(m_file);
            fclose(m_file);
            m_file = NULL;
        }
        m_Engine.SymCleanup();
    }

    void LogTime( const SYSTEMTIME& st )
    {
        LogMsg("%04d-%02d-%02d %02d:%02d:%02d.%03d",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond,
            st.wMilliseconds);
    }

    void set_log_folder(const std::string& logFolder)
    {
        m_log_folder = logFolder;
    }

    static crash_handler& instance()
    {
        static crash_handler handler;
        return handler;
    }

    SymbolEngine& GetSymbolEngine()
    {
        return m_Engine;
    }

    static LPTOP_LEVEL_EXCEPTION_FILTER install()
    {
        static bool isNotInstalled = true;
        LPTOP_LEVEL_EXCEPTION_FILTER oldHandler = NULL;
        if (isNotInstalled)
        {
            oldHandler = ::SetUnhandledExceptionFilter(UnhandledExceptionFilter);
            isNotInstalled = false;
        }
        return oldHandler;
    }
    static void uninstall(LPTOP_LEVEL_EXCEPTION_FILTER oldHandler)
    {
        ::SetUnhandledExceptionFilter(oldHandler);
    }

    static UINT& get_mode()
    {
        static UINT mode = CRASH_HANDLER_SILENT;
        return mode;
    }

    static void set_mode(UINT mode)
    {
        get_mode() = mode;
    }

    static void set_silent_mode(bool enable)
    {
        if (enable)
        {
            set_mode(get_mode() | CRASH_HANDLER_SILENT);
        }
        else
        {
            set_mode(get_mode() & (~CRASH_HANDLER_SILENT));
        }
    }

    static void set_suppressed_mode(bool enable)
    {
        if (enable)
        {
            set_mode(get_mode() | CRASH_HANDLER_SUPPRESS);
        }
        else
        {
            set_mode(get_mode() & (~CRASH_HANDLER_SUPPRESS));
        }
    }

protected:
    static LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* excep)
    {
        LONG result = EXCEPTION_CONTINUE_SEARCH;
        __try
        {
            ::OutputDebugString(_T("CrashHandler::UnhandledExceptionFilter\n"));
            crash_handler& handler = instance();
            if (handler.m_IsValid)
            {
                result = handler.Handle(excep);
            }
            else
            {
                ::OutputDebugString(_T("CrashHandler is not valid. So continue normal processing.\n"));
                result = EXCEPTION_CONTINUE_SEARCH;
            }
            ::OutputDebugString(_T("CrashHandler::UnhandledExceptionFilter OK.\n"));
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            ::OutputDebugString(_T("CrashHandler::UnhandledExceptionFilter error \n"));
        }
        return result;
    }

    static bool IsFlagSet(UINT flag)
    {
        assert(flag > 0);
        UINT mode = get_mode();
        return (mode & flag) != 0;
    }

    LONG Handle(EXCEPTION_POINTERS* excep)
    {
        ::OutputDebugString(_T("CrashHandler::Handle.\n"));
        const EXCEPTION_RECORD* info = excep->ExceptionRecord;
        UINT_PTR args[2] = { 0, 0 };
        if (info->NumberParameters > 0)
        {
            args[0] = info->ExceptionInformation[0];
            if (info->NumberParameters > 1)
                args[1] = info->ExceptionInformation[1];
        }
        LogMsg("ExceptionInfo: Code = 0x%p, Flags = 0x%p, Address = 0x%p, Args =[ %d, 0x%p ]\n",
            info->ExceptionCode, info->ExceptionFlags, info->ExceptionAddress, args[0], args[1]);
        PrintRegisters(excep->ContextRecord);
        PrintCallStack(excep);

        LogMsg("\n\n");
        ::OutputDebugString(_T("CrashHandler::Handle complete.\n"));

        typedef HANDLE (WINAPI * FUNC_OpenThread)(DWORD, BOOL, DWORD);
        FUNC_OpenThread funcOpenThread = reinterpret_cast<FUNC_OpenThread>(::GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "OpenThread"));
        if (funcOpenThread != NULL)
        {
            DWORD pid = ::GetCurrentProcessId();
            for (ThreadWalker walker; walker.HasMore(); walker.MoveNext())
            {
                const THREADENTRY32& item = walker.Current();
                if (item.th32ThreadID == ::GetCurrentThreadId() || item.th32OwnerProcessID != pid)
                    continue;
                HANDLE hThread = funcOpenThread(THREAD_SUSPEND_RESUME, FALSE, item.th32ThreadID);
                if (hThread != NULL)
                {
                    //::OutputDebugString("suspend thread\n");
                    ::SuspendThread(hThread);
                    ::CloseHandle(hThread);
                }
            }
        }

        UINT mode = get_mode();
        bool isSilent = IsFlagSet(CRASH_HANDLER_SILENT);
        bool isSuppressed = IsFlagSet(CRASH_HANDLER_SUPPRESS);

        if (!isSilent)
        {
            ::MessageBox(
                NULL,
                _T("Application has encountered a problem and needs to close. We are sorry for the inconvenience.\n")
                _T("Error information has been recorded in file 'crash.log'."),
                m_ExeFilename.c_str(),
                MB_OK | MB_ICONERROR | MB_TOPMOST
            );
        }
        else
        {
            ::OutputDebugStringA("Application crashed, but crash handler did not display.\n");
        }

        return isSuppressed ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
    }

//    void PrintAddress(ostream& os, const char* name, const ADDRESS& addr)
//    {
//        const void* p = reinterpret_cast<const void*>(addr.Offset);
//        os << name << "\tAddress=0x" << p << ", Segment=" << addr.Segment << ", Mode=" << addr.Mode << endl;
//    }

    void PrintRegisters(PCONTEXT context)
    {
        LogMsg("eip = 0x%p  ebp = 0x%p  esp = 0x%p  eflags = 0x%p cflags = 0x%p\n",
            context->Eip, context->Ebp, context->Esp, context->EFlags, context->ContextFlags);
        LogMsg("eax = 0x%p  ebx = 0x%p  ecx = 0x%p  edx = 0x%p  esi = 0x%p  edi = 0x%p\n",
            context->Eax, context->Ebx, context->Ecx, context->Edx, context->Esi, context->Edi);
        LogMsg(" cs = 0x%p   ss = 0x%p   ds = 0x%p   es = 0x%p   fs = 0x%p   gs = 0x%p\n",
            context->SegCs, context->SegSs, context->SegDs, context->SegEs, context->SegFs, context->SegGs);
    }

    void PrintAddress(const char* name, const ADDRESS& addr)
    {
        const DWORD max_size = 1024;
        char buf[max_size + 1] = { 0 };    //buffer used by IMAGEHLP_SYMBOL
        IMAGEHLP_SYMBOL* symbol = reinterpret_cast<IMAGEHLP_SYMBOL*>(buf);
        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
        symbol->MaxNameLength = max_size - sizeof(IMAGEHLP_SYMBOL);
        symbol->Address = addr.Offset;
        DWORD displacement = 0;
        const char* symName = "[null]";
        if (m_Engine.SymGetSymFromAddr(addr.Offset, &displacement, symbol))
            symName = symbol->Name;

        tstring filename;
        xul::module module;
        module.attach_address(reinterpret_cast<void*>(addr.Offset));
        if (module.is_open())
        {
            filename = boost::filesystem::path(module.get_file_path()).filename();
        }
        LogMsg("0x%p 0x%p  0x%08x  %2d  %-16s  %s\n",
            addr.Offset, module.native_handle(), displacement, addr.Mode, filename.c_str(), symbol->Name);
    }
    void PrintAddressHeader()
    {
        LogMsg("Address     Displacement  Mode    Module       Name\n");
    }

    void PrintCallStack(EXCEPTION_POINTERS* excep)
    {
        SymbolEngine& engine = GetSymbolEngine();

        CONTEXT* pContext = excep->ContextRecord;
//        m_Log << "Crashed at 0x" << excep->ExceptionRecord->ExceptionAddress
//            << ", ExceptionCode=0x" << hex << excep->ExceptionRecord->ExceptionCode << endl;
        HANDLE hProcess = ::GetCurrentProcess();
        STACKFRAME sf;
        memset(&sf,0,sizeof(sf));

        sf.AddrPC.Offset = pContext->Eip;
        sf.AddrPC.Mode = AddrModeFlat;
        sf.AddrStack.Offset = pContext->Esp;
        sf.AddrStack.Mode = AddrModeFlat;
        sf.AddrFrame.Offset = pContext->Ebp;
        sf.AddrFrame.Mode = AddrModeFlat;
        DWORD dwMachineType = IMAGE_FILE_MACHINE_I386;
        for (int stackFrames = 0; stackFrames < 500; ++stackFrames)
        {
            if (!m_Engine.StackWalk(dwMachineType,hProcess,GetCurrentThread(),&sf,pContext))
                break;
            PrintAddress("AddrPC", sf.AddrPC);
    //        PrintAddress("  AddrReturn ", sf.AddrReturn);
    //        PrintAddress("\tAddrFrame", sf.AddrFrame);
    //        PrintAddress("\tAddrStack", sf.AddrStack);
        }
    }

    void LogMsg(const char* format, ...)
    {
        va_list arglist;
        va_start(arglist, format);
        if (m_file == NULL)
            return;
        vfprintf(m_file, format, arglist);
        fflush(m_file);
        //vfprintf(stderr, format, arglist);
    }
    void LogModule(HMODULE hModule, LPCTSTR filename, DWORD size)
    {
        if (hModule != NULL)
        {
            DWORD filesize = 0;
            xul::file_finder finder;
            if (finder.find(filename))
            {
                filesize = finder.get_size();
            }
            char verstr[256] = { 0 };
            FileVersionInfo versionInfo;
            if (versionInfo.Retrieve(filename))
            {
                const VS_FIXEDFILEINFO& ver = versionInfo.GetFixedInfo();
                _snprintf(verstr, 255, "%d.%d.%d.%d",
                    HIWORD(ver.dwFileVersionMS),
                    LOWORD(ver.dwFileVersionMS),
                    HIWORD(ver.dwFileVersionLS),
                    LOWORD(ver.dwFileVersionLS));
            }
            LogMsg("0x%p %8d %8d   %-18s %s\n", hModule, size, filesize, verstr, filename);
        }
    }
    void LogModule(LPCTSTR filename)
    {
        LogModule(::GetModuleHandle(filename), filename, 0);
    }
    void LogModules()
    {
        LogMsg( "Module List:\n" );
        TCHAR path[MAX_PATH * 4 + 1] = { 0 };
        for (ModuleWalker walker; walker.HasMore(); walker.MoveNext())
        {
            const MODULEENTRY32& item = walker.Current();
            assert((void*)item.hModule == item.modBaseAddr);
            bool success = (::GetLongPathName(item.szExePath, path, MAX_PATH * 4) > 0);
            LogModule(item.hModule, success ? path : item.szExePath, item.modBaseSize);
        }
    }

private:
    BOOL            m_IsValid;
    SymbolEngine    m_Engine;
    std::string            m_ExeFilename;
    FILE*            m_file;
    std::string        m_log_folder;

};


class crash_handler_installer
{
public:
    crash_handler_installer()
    {
        m_oldHandler = crash_handler::install();
    }
    ~crash_handler_installer()
    {
        crash_handler::uninstall(m_oldHandler);
    }

private:
    LPTOP_LEVEL_EXCEPTION_FILTER m_oldHandler;
};


}
