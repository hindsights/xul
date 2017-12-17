#pragma once

#include <xul/util/log/log_service.hpp>
#include <xul/util/log/fake_log_service.hpp>
#include <xul/util/trace.hpp>
#include <xul/io/stdfile.hpp>
#include <xul/util/time_counter.hpp>


namespace xul {


/// console output logging
class console_log_service : public fake_log_service
{
public:
    virtual bool need_log(int category, int level) { return true; }
    virtual void log(int category, int level, const std::string& msg)
    {
        printf("%s\n", msg.c_str());
    }

};


/// simple file output logging
class simple_file_log_service : public fake_log_service
{
public:
    explicit simple_file_log_service(const char* filename)
    {
        m_fout.open_text(filename);
    }
    ~simple_file_log_service()
    {
        m_fout.close();
    }
    virtual bool need_log(int category, int level) { return true; }
    virtual void log(int category, int level, const std::string& msg)
    {
        m_fout.write(msg.data(), msg.size());
        m_fout.write("\n");
        if (m_start_time.get_elapsed() > 5 * 1000)
        {
            m_fout.flush();
            m_start_time.sync();
        }
    }

private:
    xul::stdfile_writer m_fout;
    xul::time_counter32 m_start_time;
};


/// trace output logging
class trace_log_service : public fake_log_service
{
public:
    trace_log_service()
    {
    }
    ~trace_log_service()
    {
    }
    virtual bool need_log(int category, int level) { return true; }
    virtual void log(int category, int level, const std::string& msg)
    {
        xul::tracer::output(msg.c_str());
        xul::tracer::output("\n");
    }
};


}
