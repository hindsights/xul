#pragma once

#include <xul/net/http/detail/http_filter.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/log/log.hpp>
#include "zlib.h"
#include <assert.h>


namespace xul {


class gzip_http_filter : public object_impl<http_filter>
{
public:
    gzip_http_filter()
    {
        XUL_LOGGER_INIT("gzip_http_filter");
        XUL_DEBUG("new");
        m_content_length = -1;
        m_received_size = 0;
        m_buffer.resize(16*1024);
        d_stream.zalloc = Z_NULL;
        d_stream.zfree = Z_NULL;
        d_stream.opaque = Z_NULL;
        d_stream.next_in = Z_NULL;
        d_stream.avail_in = 0;
        int ret = inflateInit2(&d_stream, 47);
        if (ret != Z_OK)
        {
            XUL_ERROR("inflateInit2 error:" << ret);
            return;
        }
    }
    ~gzip_http_filter()
    {
        XUL_DEBUG("delete");
        inflateEnd(&d_stream);
    }
    virtual const char* name() const
    {
        return "gzip";
    }
    virtual bool process(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        XUL_DEBUG("process " << size);
        int realsize = size;
        if (m_content_length >= 0 && m_received_size + size > m_content_length)
            realsize = m_content_length - m_received_size;
        m_received_size += realsize;
        if (!ungzip(false, data, realsize, req, resp))
            return false;
        return check_finish(req, resp, data + realsize, size - realsize);
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
        m_received_size = 0;
        resp->ref_headers()->remove("Content-Length");
        resp->ref_headers()->remove("Content-Encoding");
        if (!m_next->init(req, resp))
            return false;
        return check_finish(req, resp, NULL, 0);
    }
    virtual bool finish(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        XUL_DEBUG("finish");
        if (!ungzip(true, NULL, 0, req, resp))
            return false;
        return m_next->finish(req, resp, data, size);
    }
    //virtual void abort(url_request* req, url_response* resp)
    //{
    //    m_next->abort(req, resp);
    //}
    virtual void handle_error(url_request* req, url_response* resp, int errcode)
    {
        XUL_DEBUG("handle_error " << errcode);
        m_next->handle_error(req, resp, errcode);
    }
    bool ungzip(bool eof, const uint8_t* source,int len, url_request* req, url_response* resp)
    {
        int ret;
        d_stream.next_in = const_cast<uint8_t*>(source);
        d_stream.avail_in = len;
        do
        {
            d_stream.next_out = m_buffer.data();
            d_stream.avail_out = m_buffer.size();
            ret = inflate(&d_stream, eof ? Z_FINISH : Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);
            switch (ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                //inflateEnd(&d_stream);
                handle_error(req, resp, ret);
                return false;
            }
            assert(d_stream.avail_out >= 0);
            int have = m_buffer.size() - d_stream.avail_out;
            assert(have >= 0);
            if (have > 0)
            {
                if (!m_next->process(req, resp, m_buffer.data(), have))
                    return false;
            }
        } while (d_stream.avail_out == 0);
        return true;
    }

    bool check_finish(url_request* req, url_response* resp, const uint8_t* data, int size)
    {
        if (m_content_length >= 0 && m_received_size >= m_content_length)
        {
            return finish(req, resp, data, size);
        }
        return true;
    }
private:
    XUL_LOGGER_DEFINE();
    boost::intrusive_ptr<http_filter> m_next;
    z_stream d_stream;
    int64_t m_content_length;
    int64_t m_received_size;
    byte_buffer m_buffer;
};


}
