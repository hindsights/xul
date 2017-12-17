#pragma once

#include <xul/net/http/detail/http_filter.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/log/log.hpp>
#include "zlib.h"
#include <assert.h>


namespace xul {


class chunked_http_filter : public object_impl<http_filter>
{
public:
    chunked_http_filter()
    {
        XUL_LOGGER_INIT("chunked_http_filter");
        XUL_DEBUG("new");
        m_buffer.resize(16*1024);
        reset_state();
    }
    ~chunked_http_filter()
    {
        XUL_DEBUG("delete");
    }
    virtual const char* name() const
    {
        return "chunked";
    }
    virtual bool process(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        XUL_DEBUG("process " << size);
        const uint8_t* realdata = data;
        int realsize = size;
        while (realsize > 0)
        {
            int ret = decode_once(req, resp, realdata, realsize);
            if (ret <= 0)
                break;
            realdata += ret;
            realsize -= ret;
            m_received_size += ret;
        }
        return true;
    }
    virtual void set_next(http_filter* next_filter)
    {
        assert(next_filter);
        XUL_DEBUG("set_next " << next_filter->name() << " " << next_filter << " " << m_next);
        m_next = next_filter;
    }
    virtual bool init(url_request* req, url_response* resp)
    {
        XUL_DEBUG("init");
        m_content_length = url_messages::get_content_length(*resp);
        assert(m_content_length < 0);
        reset_state();
        resp->ref_headers()->remove("Content-Length");
        resp->ref_headers()->remove("Content-Encoding");
        if (!m_next->init(req, resp))
            return false;
        return true;
    }
    virtual bool finish(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        XUL_DEBUG("finish");
        return m_next->finish(req, resp, data, size);
    }
    //virtual void abort(url_request* req, url_response* resp)
    //{
    //    m_next->abort(req, resp);
    //}
    virtual void handle_error(url_request* req, url_response* resp, int errcode)
    {
        XUL_DEBUG("handle_error " << errcode);
        reset_state();
        m_next->handle_error(req, resp, errcode);
    }
    void reset_state()
    {
        m_received_chunk_size = 0;
        m_current_chunk_size = -1;
        m_content_length = -1;
        m_last_chunk_size = -1;
        m_received_size = 0;
        m_buffer.resize(0);
    }

private:
    int decode_once(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        if (0 == m_current_chunk_size)
        {
            return decode_chunk_trailer(req, resp, data, size);
        }
        if (m_current_chunk_size > 0)
        {
            return decode_chunk_data(req, resp, data, size);
        }
        return decode_chunk_start(req, resp, data, size);
    }
    int decode_chunk_data(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        assert(m_received_chunk_size >= 0 && m_received_chunk_size < m_current_chunk_size);
        int remaining_size = m_current_chunk_size - m_received_chunk_size;
        int read_size = 0;
        int read_chunk_size = 0;
        if (size >= remaining_size + 2)
        {
            // chunk data and ending tag is complete
            if (data[remaining_size] != '\r' || data[remaining_size + 1] != '\n')
                return -1;
            read_chunk_size = remaining_size;
            read_size = read_chunk_size + 2;
            m_last_chunk_size = m_current_chunk_size;
            m_current_chunk_size = -1;
            m_received_chunk_size = 0;
        }
        else if (size < remaining_size)
        {
            // chunk data is incomplete, process current data
            read_chunk_size = size;
            read_size = size;
            m_received_chunk_size += size;
        }
        else
        {
            // chunk data is complete, but ending tag is not
            return 0;
        }
        m_next->process(req, resp, data, read_chunk_size);
        return read_size;
    }
    int decode_chunk_trailer(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        if (size < 2)
        {
            return 0;
        }
        if (data[0] == '\r' && data[1] == '\n')
        {
            //m_next->finish(req, resp, data + 2, size - 2);
            m_next->finish(req, resp, data + 2, 0);
            return 2;
        }
        const uint8_t crln2[4] = { '\r', '\n', '\r', '\n' };
        const uint8_t* pos = std::search(data, data + size, crln2, crln2 + 4);
        if (pos == data + size)
        {
//            return 0;
        }
        m_next->process(req, resp, data, 0);
        return data + size - pos;
    }
    int decode_chunk_start(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        const uint8_t crln[2] = { '\r', '\n' };
        const uint8_t* pos = std::search(data, data + size, crln, crln + 2);
        if (pos == data + size)
        {
            return 0;
        }
        assert(pos - data + 2 <= size);
        std::string chunklenstr(reinterpret_cast<const char*>(data), pos - data);
        int64_t chunksize = -1;
        if (sscanf(chunklenstr.c_str(), "%llx", &chunksize) != 1)
            return -1;
        m_current_chunk_size = chunksize;
        XUL_APP_DEBUG("decode_chunk_start " << chunklenstr);
        return chunklenstr.size() + 2;
    }

private:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<http_filter> m_next;
    int64_t m_content_length;
    int64_t m_received_size;
    byte_buffer m_buffer;
    /// size of last chunk, used for check eof
    int64_t m_last_chunk_size;
    int64_t m_current_chunk_size;
    int64_t m_received_chunk_size;
};


}
