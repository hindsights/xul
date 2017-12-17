#pragma once

#include <limits.h>


namespace xul {


enum log_level
{
    LOG_OFF = INT_MAX,
    LOG_FATAL = 50000,
    LOG_ERROR = 40000,
    LOG_EVENT = 35000,
    LOG_WARNING = 30000,
    LOG_INFO = 20000,
    LOG_DEBUG = 10000,
    LOG_NOT_SET = 0,
};


}
