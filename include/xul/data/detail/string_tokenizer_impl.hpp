#pragma once

#include <xul/data/string_tokenizer.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/data/buffer.hpp>


namespace xul { namespace detail {


class string_tokenizer_impl : public object_impl<string_tokenizer>
{
public:
    string_tokenizer_impl()
    {
        reset();
        m_include_delimiter = false;
        m_max_buffer_size = 10240;
    }
    virtual void reset()
    {
        m_data.clear();
        m_temp_buffer = NULL;
        m_temp_size = 0;
        m_offset = 0;
    }
    virtual void set_max_buffer_size(size_t size)
    {
        if (size < 1024 || size > 4 * 1024 * 1024)
            return;
        m_max_buffer_size = size;
    }
    virtual void set_include_delimiter(bool includes)
    {
        m_include_delimiter = includes;
    }
    virtual bool is_delimiter_included() const
    {
        return m_include_delimiter;
    }
    virtual void set_delimiter(const char* data, size_t size)
    {
        assert(size < 10);
        m_delimiter.assign(data, size);
    }
    virtual void feed(const char* data, size_t size)
    {
        m_data.append(data, size);
    }
    virtual bool next_token(const char** pos, size_t* len)
    {
        if (m_data.empty() || m_offset >= m_data.size())
            return false;
        const char* data = m_data.data() + m_offset;
        const char* data_end = m_data.data() + m_data.size();
        const char* delimiter_pos = std::search(data, data_end, m_delimiter.begin(), m_delimiter.end());
        if (delimiter_pos == data_end)
        {
            return false;
        }
        int token_len = delimiter_pos - data;
        m_offset += token_len;
        if (m_include_delimiter)
        {
            delimiter_pos += m_delimiter.size();
            m_offset += m_delimiter.size();
        }
        *pos = data;
        *len = delimiter_pos - data;
        return true;
    }
    virtual bool get_remains(const char** pos, size_t* len)
    {
        if (m_data.empty() || m_offset >= m_data.size())
            return false;
        assert(pos);
        assert(len);
        *pos = m_data.data() + m_offset;
        *len = m_data.size() - m_offset;
        return true;
    }

private:
    std::string m_delimiter;
    std::string m_data;
    const char* m_temp_buffer;
    size_t m_temp_size;
    size_t m_offset;
    bool m_include_delimiter;
    size_t m_max_buffer_size;
};


} }
