#pragma once

#include <xul/util/log/log_service.hpp>
#include <xul/util/logger.hpp>


namespace xul {


/// trivial implementation, doing nothing
class fake_log_service : public log_service
{
public:
    virtual bool start() { return true; }
    virtual void stop() { }
    virtual bool need_log(logger& a_logger, int level) { return false; }
    virtual void log(logger& a_logger, int level, const std::string& msg) { }
    virtual logger_ptr get_logger(const std::string& name) { return logger_ptr(new logger(*this, name)); }
    virtual void set_log_level(int level) { }

};


}
