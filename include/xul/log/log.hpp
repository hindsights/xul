#pragma once

#include <boost/config.hpp>

#include <xul/log/logging.hpp>
#include <xul/log/logger.hpp>
#include <xul/log/log_level.hpp>
#include <xul/util/trace.hpp>
#include <xul/data/tuple.hpp>
#include <xul/macro/variadic.hpp>

#ifdef BOOST_WINDOWS
#pragma warning(disable: 4127)
#endif

#include <string>

#include <sstream>

#if defined(BOOST_WINDOWS)
#define XUL_TRACEOUT(msg) do{\
    std::ostringstream temposs; \
    temposs << "trace: " << msg << std::endl; \
    ::OutputDebugStringA(temposs.str().c_str()); } while (false)
#else
#define XUL_TRACEOUT_LOG(msg) do{\
    std::ostringstream temposs; \
    temposs << "trace: " << msg; \
    printf("%s\n", temposs.str().c_str()); } while (false)
#endif


#if 1 //!defined(_WIN32_WCE) || 1
#if defined(XUL_ENABLE_TRACE_LOGOUT)
#define XUL_REL_LOG(thelog, context, level, msg) do \
    { \
    std::ostringstream temposs; \
    temposs << msg; \
    std::string msgstr = temposs.str(); \
    XULTRACE0(msgstr.c_str()); XULTRACE0("\n"); \
    } while (false)
#else
#define XUL_REL_LOG(thelog, context, level, msg) do \
    { \
        if (thelog && thelog->need_log(level)) { \
            std::ostringstream temposs; \
            temposs << msg; \
            std::string msgstr = temposs.str(); \
            thelog->log(context, level, msgstr.c_str(), msgstr.size()); \
        } \
    } while (false)
#endif
#else
#define XUL_REL_LOG(thelog, context, level, msg)        ((void)0)
#endif

#ifdef _DEBUG

#define XUL_TRACE_LOG(msg) XUL_TRACEOUT_LOG(msg)

#else

#define XUL_TRACE_LOG(msg)                            ((void)0)

#endif

//#define XUL_ENABLE_FULL_LOG


#if defined(_DEBUG) || defined(XUL_ENABLE_DEBUG_LOG) || defined(XUL_ENABLE_FULL_LOG)
#define XUL_DBG_LOG(category, context, level, msg) XUL_REL_LOG(category, context, level, msg)
#else
#define XUL_DBG_LOG(category, context, level, msg)        ((void)0)
#endif


#define XUL_LOG(category, context, level, msg)        XUL_DBG_LOG(category, context, level, msg)


#define XUL_LOG_FATAL(thelog, context, msg)            XUL_LOG(thelog, context, xul::LOG_FATAL, msg)
#define XUL_LOG_ERROR(thelog, context, msg)            XUL_LOG(thelog, context, xul::LOG_ERROR, msg)
#define XUL_LOG_EVENT(thelog, context, msg)            XUL_LOG(thelog, context, xul::LOG_EVENT, msg)
#define XUL_LOG_WARN(thelog, context, msg)            XUL_LOG(thelog, context, xul::LOG_WARNING, msg)
#define XUL_LOG_INFO(thelog, context, msg)            XUL_LOG(thelog, context, xul::LOG_INFO, msg)
#define XUL_LOG_DEBUG(thelog, context, msg)            XUL_LOG(thelog, context, xul::LOG_DEBUG, msg)

#define XUL_REL_LOG_FATAL(thelog, context, msg)        XUL_REL_LOG(thelog, context, xul::LOG_FATAL, msg)
#define XUL_REL_LOG_ERROR(thelog, context, msg)        XUL_REL_LOG(thelog, context, xul::LOG_ERROR, msg)
#define XUL_REL_LOG_EVENT(thelog, context, msg)        XUL_REL_LOG(thelog, context, xul::LOG_EVENT, msg)
#define XUL_REL_LOG_WARN(thelog, context, msg)        XUL_REL_LOG(thelog, context, xul::LOG_WARNING, msg)
#define XUL_REL_LOG_INFO(thelog, context, msg)        XUL_REL_LOG(thelog, context, xul::LOG_INFO, msg)
#define XUL_REL_LOG_DEBUG(thelog, context, msg)        XUL_REL_LOG(thelog, context, xul::LOG_DEBUG, msg)



#define XUL_FATAL_1(msg)            XUL_LOG_FATAL(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_ERROR_1(msg)            XUL_LOG_ERROR(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_EVENT_1(msg)            XUL_LOG_EVENT(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_WARN_1(msg)                XUL_LOG_WARN(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_INFO_1(msg)                XUL_LOG_INFO(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_DEBUG_1(msg)            XUL_LOG_DEBUG(get_xul_logger(), get_xul_logger_owner(), msg)

#define XUL_REL_FATAL_1(msg)        XUL_REL_LOG_FATAL(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_REL_ERROR_1(msg)        XUL_REL_LOG_ERROR(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_REL_EVENT_1(msg)        XUL_REL_LOG_EVENT(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_REL_WARN_1(msg)            XUL_REL_LOG_WARN(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_REL_INFO_1(msg)            XUL_REL_LOG_INFO(get_xul_logger(), get_xul_logger_owner(), msg)
#define XUL_REL_DEBUG_1(msg)        XUL_REL_LOG_DEBUG(get_xul_logger(), get_xul_logger_owner(), msg)



#define XUL_FATAL_2(context, msg)            XUL_LOG_FATAL(context->get_xul_logger(), context, msg)
#define XUL_ERROR_2(context, msg)            XUL_LOG_ERROR(context->get_xul_logger(), context, msg)
#define XUL_EVENT_2(context, msg)            XUL_LOG_EVENT(context->get_xul_logger(), context, msg)
#define XUL_WARN_2(context, msg)            XUL_LOG_WARN(context->get_xul_logger(), context, msg)
#define XUL_INFO_2(context, msg)            XUL_LOG_INFO(context->get_xul_logger(), context, msg)
#define XUL_DEBUG_2(context, msg)            XUL_LOG_DEBUG(context->get_xul_logger(), context, msg)

#define XUL_REL_FATAL_2(context, msg)        XUL_REL_LOG_FATAL(context->get_xul_logger(), context, msg)
#define XUL_REL_ERROR_2(context, msg)        XUL_REL_LOG_ERROR(context->get_xul_logger(), context, msg)
#define XUL_REL_EVENT_2(context, msg)        XUL_REL_LOG_EVENT(context->get_xul_logger(), context, msg)
#define XUL_REL_WARN_2(context, msg)        XUL_REL_LOG_WARN(context->get_xul_logger(), context, msg)
#define XUL_REL_INFO_2(context, msg)        XUL_REL_LOG_INFO(context->get_xul_logger(), context, msg)
#define XUL_REL_DEBUG_2(context, msg)        XUL_REL_LOG_DEBUG(context->get_xul_logger(), context, msg)


#define XUL_FATAL(...)            XUL_MACRO_DISPATCH(XUL_FATAL_, __VA_ARGS__)
#define XUL_ERROR(...)            XUL_MACRO_DISPATCH(XUL_ERROR_, __VA_ARGS__)
#define XUL_EVENT(...)            XUL_MACRO_DISPATCH(XUL_EVENT_, __VA_ARGS__)
#define XUL_WARN(...)            XUL_MACRO_DISPATCH(XUL_WARN_, __VA_ARGS__)
#define XUL_INFO(...)            XUL_MACRO_DISPATCH(XUL_INFO_, __VA_ARGS__)
#define XUL_DEBUG(...)            XUL_MACRO_DISPATCH(XUL_DEBUG_, __VA_ARGS__)

#define XUL_REL_FATAL(...)            XUL_MACRO_DISPATCH(XUL_REL_FATAL_, __VA_ARGS__)
#define XUL_REL_ERROR(...)            XUL_MACRO_DISPATCH(XUL_REL_ERROR_, __VA_ARGS__)
#define XUL_REL_EVENT(...)            XUL_MACRO_DISPATCH(XUL_REL_EVENT_, __VA_ARGS__)
#define XUL_REL_WARN(...)            XUL_MACRO_DISPATCH(XUL_REL_WARN_, __VA_ARGS__)
#define XUL_REL_INFO(...)            XUL_MACRO_DISPATCH(XUL_REL_INFO_, __VA_ARGS__)
#define XUL_REL_DEBUG(...)            XUL_MACRO_DISPATCH(XUL_REL_DEBUG_, __VA_ARGS__)


#define XUL_LIBXUL_FATAL(msg)            XUL_LOG_FATAL(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_ERROR(msg)            XUL_LOG_ERROR(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_EVENT(msg)            XUL_LOG_EVENT(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_WARN(msg)            XUL_LOG_WARN(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_INFO(msg)            XUL_LOG_INFO(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_DEBUG(msg)            XUL_LOG_DEBUG(xul::logging::get_libxul_logger(), 0, msg)

#define XUL_LIBXUL_REL_FATAL(msg)        XUL_REL_LOG_FATAL(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_REL_ERROR(msg)        XUL_REL_LOG_ERROR(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_REL_EVENT(msg)        XUL_REL_LOG_EVENT(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_REL_WARN(msg)        XUL_REL_LOG_WARN(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_REL_INFO(msg)        XUL_REL_LOG_INFO(xul::logging::get_libxul_logger(), 0, msg)
#define XUL_LIBXUL_REL_DEBUG(msg)        XUL_REL_LOG_DEBUG(xul::logging::get_libxul_logger(), 0, msg)


#define XUL_APP_FATAL(msg)            XUL_LOG_FATAL(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_ERROR(msg)            XUL_LOG_ERROR(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_EVENT(msg)            XUL_LOG_EVENT(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_WARN(msg)            XUL_LOG_WARN(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_INFO(msg)            XUL_LOG_INFO(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_DEBUG(msg)            XUL_LOG_DEBUG(xul::logging::get_app_logger(), 0, msg)

#define XUL_APP_REL_FATAL(msg)        XUL_REL_LOG_FATAL(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_REL_ERROR(msg)        XUL_REL_LOG_ERROR(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_REL_EVENT(msg)        XUL_REL_LOG_EVENT(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_REL_WARN(msg)        XUL_REL_LOG_WARN(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_REL_INFO(msg)        XUL_REL_LOG_INFO(xul::logging::get_app_logger(), 0, msg)
#define XUL_APP_REL_DEBUG(msg)        XUL_REL_LOG_DEBUG(xul::logging::get_app_logger(), 0, msg)


//#define XUL_LOGGER_DEFINE()            const xul::logger_ptr& get_xul_logger() const { return m_xul_logger; } mutable xul::logger_ptr m_xul_logger;
#define XUL_LOGGER_DEFINE()    \
    xul::logger* get_xul_logger() const { return m_xul_logger.get(); }\
    const void* get_xul_logger_owner() const { return this; }\
    mutable boost::intrusive_ptr<xul::logger> m_xul_logger
