#pragma once

#include <xul/log/log_filter.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


class log_filter_chain_impl : public object_impl<log_filter_chain>
{
public:
    virtual bool is_empty() const
    {
        return NULL == m_head;
    }
    virtual void add_filter(log_filter* filter)
    {
        if (!filter)
        {
            assert(false);
            return;
        }
        if (m_tail)
        {
            m_tail->set_next(filter);
            m_tail = filter;
        }
        else
        {
            assert(!m_head);
            m_head = filter;
            m_tail = filter;
        }
    }
    virtual void log(int level, const char* msg, int len)
    {
        if (m_head)
        {
            m_head->log(level, msg, len);
        }
    }
    virtual bool start()
    {
        for (log_filter* filter = m_head.get(); filter; filter = filter->get_next())
        {
            if (!filter->start())
                return false;
        }
        return true;
    }
    virtual void stop()
    {
        for (log_filter* filter = m_head.get(); filter; filter = filter->get_next())
        {
            filter->stop();
        }
    }

protected:
    boost::intrusive_ptr<log_filter> m_head;
    boost::intrusive_ptr<log_filter> m_tail;
};


}
