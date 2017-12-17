#pragma once

#include <xul/log/log_filter.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


class log_filter_impl : public object_impl<log_filter>
{
public:
    //virtual void log(const boost::shared_ptr<const std::string>& msg) = 0;
    virtual void set_next(log_filter* filter)
    {
        m_next_filter = filter;
    }
    virtual log_filter* get_next() const
    {
        return m_next_filter.get();
    }
    virtual void set_parameter(const char* name, const char* val)
    {
        // do nothing if no parameter defined
    }
    virtual bool start()
    {
        // do nothing if no non-trivial startup actions
        return true;
    }
    virtual void stop()
    {
        // do nothing if no non-trivial cleanup actions
    }

protected:
    void invoke_next(int level, const char* msg, int len)
    {
        if (m_next_filter)
        {
            m_next_filter->log(level, msg, len);
        }
    }

protected:
    boost::intrusive_ptr<log_filter> m_next_filter;
};


}
