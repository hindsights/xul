#pragma once

#include <xul/log/log_level.hpp>
#include <xul/data/enum.hpp>
#include <string>


namespace xul {


class log_level_metadata : public xul::enum_metadata
{
public:
    log_level_metadata()
    {
        add(LOG_FATAL, "FATAL");
        add(LOG_ERROR, "ERROR");
        add(LOG_EVENT, "EVENT");
        add(LOG_WARNING, "WARN");
        add(LOG_INFO, "INFO");
        add(LOG_DEBUG, "DEBUG");
    }
};


typedef xul::simple_enum_impl<log_level, log_level_metadata> log_levels;


}
