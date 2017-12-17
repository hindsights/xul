#pragma once

#include <xul/net/uri.hpp>
#include <xul/os/paths.hpp>
#include <xul/data/numerics.hpp>
#include <xul/std/strings.hpp>
#include <xul/std/maps.hpp>
#include <xul/net/well_known_ports.hpp>
#include <xul/net/url_encoding.hpp>
#include <xul/data/string_table.hpp>
#include <xul/io/output_streams.hpp>
#include <xul/data/numerics.hpp>
#include <xul/macro/iterate.hpp>
#include <string>


namespace xul {


/// interface for uri
class simple_uri : public object_impl<uri>
{
public:
    static boost::intrusive_ptr<uri> parse_uri(const char* s)
    {
        boost::intrusive_ptr<uri> u = new simple_uri;
        if (!u->parse(s))
            return boost::intrusive_ptr<uri>();
        return u;
    }

    simple_uri() : m_params(create_associative_string_array())
    {
        m_port = 0;
    }

    void clear()
    {
        m_original_string.clear();
        m_scheme.clear();
        m_port = 0;
        m_host.clear();
        m_username.clear();
        m_password.clear();
        m_full_path.clear();
        m_path.clear();
        m_filename.clear();
        m_query.clear();
        m_fragment.clear();
        m_params->clear();
    }

    virtual const char* get_original_string() const { return m_original_string.c_str(); }
    virtual const char* get_scheme() const { return m_scheme.c_str(); }
    virtual const char* get_host() const { return m_host.c_str(); }
    virtual const char* get_username() const { return m_username.c_str(); }
    virtual const char* get_password() const { return m_password.c_str(); }
    virtual int get_port() const { return m_port; }

    /// path and query part
    virtual const char* get_full_path() const { return m_full_path.c_str(); }
    virtual const char* get_path() const { return m_path.c_str(); }
    virtual const char* get_filename() const { return m_filename.c_str(); }
    virtual const char* get_query() const { return m_query.c_str(); }
    virtual const char* get_fragment() const { return m_fragment.c_str(); }
    virtual const string_table* get_params() const { return m_params.get(); }
    virtual void set_param(const char* name, const char* val)
    {
        m_params->set(name, val);
        assemble_query();
    }
    virtual void remove_param(const char* name)
    {
        m_params->remove(name);
        assemble_query();
    }

    virtual const char* get_param(const char* name) const
    {
        return m_params->get(name, "");
    }
    virtual bool has_param(const char* name) const
    {
        return m_params->contains(name);
    }

    virtual bool parse(const char* urlstr)
    {
        clear();
        if (!urlstr)
        {
            assert(false);
            return false;
        }
        std::string s(urlstr);
        m_original_string = s;
        size_t schemePos = s.find("://");
        if (schemePos != std::string::npos)
        {
            for (size_t pos = 0; pos < schemePos; ++pos)
            {
                if (false == isalpha(s[pos]))
                {
                    return parse_path(s, 0);
                }
            }
            m_scheme = s.substr(0, schemePos);
            return parse_with_scheme(s, schemePos + 3);
        }
        return parse_path(s, 0);
    }
    void write(std::ostream& os) const
    {
        os << m_original_string;
    }
    virtual uri* clone() const
    {
        simple_uri* new_copy = new simple_uri;
        new_copy->m_original_string = m_original_string;
        new_copy->m_scheme = m_scheme;
        new_copy->m_username = m_username;
        new_copy->m_password = m_password;
        new_copy->m_host = m_host;
        new_copy->m_port = m_port;
        new_copy->m_full_path = m_full_path;
        new_copy->m_filename = m_filename;
        new_copy->m_path = m_path;
        new_copy->m_query = m_query;
        new_copy->m_fragment = m_fragment;
        new_copy->m_params = m_params->clone();
        return new_copy;
    }

    std::string str() const
    {
        return m_original_string;
    }

    virtual void assemble(output_stream* os) const
    {
        if (false == m_host.empty())
        {
            if (false == m_scheme.empty())
            {
                *os << m_scheme << "://";
            }
            *os << m_host;
            if ( m_port > 0 && !("http" == m_scheme && 80 == m_port) )
                *os << ':' << numerics::format(m_port);
        }
        *os << m_path;
        if (false == m_query.empty())
            *os << '?' << m_query;
        if (false == m_fragment.empty())
            *os << '#' << m_fragment;
    }
    virtual void print(output_stream* os) const
    {
        assemble(os);
    }
    virtual void encode()
    {
        XUL_ITERATE_REF(string_table, m_params, iter)
        {
            string_table::entry_type* entry = iter->element();
            std::string encoded_val = url_encoding::upper_case().encode(entry->get_value());
            entry->set_value(encoded_val.c_str());
        }
        assemble_query();
        m_original_string = printables::to_string(*this);
    }

private:
    void assemble_full_path()
    {
        m_full_path = m_query.empty() ? m_path : m_path + "?" + m_query;
    }
    void assemble_query()
    {
        std::ostringstream os;
        bool is_first = true;
        XUL_ITERATE(string_table, m_params, iter)
        {
            const string_table::entry_type* entry = iter->element();
            if (!is_first)
                os << "&";
            os << entry->get_key() << '=' << entry->get_value();
            is_first = false;
        }
        m_query = os.str();
        assemble_full_path();
    }
    bool parse_fragment(const std::string& s, size_t offset)
    {
        if (offset != std::string::npos)
            m_fragment = s.substr(offset + 1);
        return true;
    }
    bool parse_params(const std::string& s, size_t offset, size_t end)
    {
        m_query = xul::strings::extract_string(s, offset + 1, end);
        xul::string_dicts::parse(*m_params, m_query, '&', '=');
        XUL_ITERATE_REF(string_table, m_params, iter)
        {
            string_table::entry_type* entry = iter->element();
            std::string decoded_val = url_encoding::decode(entry->get_value());
            entry->set_value(decoded_val.c_str());
        }
        return true;
    }
    bool parse_path(const std::string& s, size_t offset)
    {
        if (offset == std::string::npos)
        {
            return true;
        }
        //size_t pathEndPos = std::string::npos;
        size_t paramsPos = s.find('?', offset);
        if (paramsPos != std::string::npos)
        {
            size_t fragmentPos = s.find('#', paramsPos + 1);
            parse_fragment(s, fragmentPos);
            parse_params(s, paramsPos, fragmentPos);
            m_path = s.substr(offset, paramsPos - offset);
        }
        else
        {
            size_t fragmentPos = s.find('#', offset);
            parse_fragment(s, fragmentPos);
            m_path = xul::strings::extract_string(s, offset, fragmentPos);
        }
        assemble_full_path();
        m_filename = xul::paths::get_filename(m_path);
        return true;
    }
    bool parse_host_and_port(const std::string& s, size_t start, size_t end)
    {
        size_t pos = s.find(':', start);
        if (pos != std::string::npos && pos < end)
        {
            m_host = s.substr(start, pos - start);
            pos++;
            std::string portstr = s.substr(pos, end - pos);
            if (false == xul::numerics::try_parse<int>(portstr, m_port))
                return false;
        }
        else
        {
            m_host = s.substr(start, end - start);
            if (false == m_scheme.empty())
            {
                m_port = well_known_ports::get_port(m_scheme, 0);
            }
        }
        return true;
    }
    bool parse_with_scheme(const std::string& s, size_t offset)
    {
        size_t pathPos = s.find('/', offset);
        if (pathPos != std::string::npos)
        {
            if (pathPos > offset)
            {
                if (false == parse_host_and_port(s, offset, pathPos))
                    return false;
            }
            return parse_path(s, pathPos);
        }
        return parse_path(s, pathPos);
    }

private:
    std::string m_original_string;
    std::string m_scheme;
    std::string m_username;
    std::string m_password;
    std::string m_host;
    int m_port;

    std::string m_full_path;
    std::string m_path;
    std::string m_filename;
    std::string m_query;
    std::string m_fragment;
    boost::intrusive_ptr<string_table> m_params;
};

inline std::ostream& operator<<(std::ostream& os, const uri& url)
{
    os << url.get_original_string();
    return os;
}


}


#ifdef XUL_RUN_TEST

#include <xul/util/test_case.hpp>

namespace xul {
class uri_test_case : public xul::test_case
{
protected:
    virtual void do_run()
    {
        test_full();
        test_no_host();
        test_no_fragment();
        test_no_query();
        test_simple();
        test_simple_with_host();
        test_no_port_http();
        test_no_port_rtsp();
        test_no_port_others();
        test_no_path();
        test_host_only();
    }
    void test_full()
    {
        xul::simple_uri url;
        assert(url.parse("http://www.host.com:9988/dir1/dir2/filename.ext?key1=val1&key2=val2&key3=val3#fragment"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "www.host.com");
        assert(url.get_port() == 9988);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 3);
        assert_equal(url.get_param("key1"), "val1");
        assert_equal(url.get_param("key2"), "val2");
        assert_equal(url.get_param("key3"), "val3");
        assert_equal(url.get_fragment(), "fragment");
        assert_equal(url.get_query(), "key1=val1&key2=val2&key3=val3");
        assert_equal(url.get_scheme(), "http");
    }
    void test_no_host()
    {
        xul::simple_uri url;
        assert(url.parse("/dir1/dir2/filename.ext?key1=val1&key2=val2&key3=val3#fragment"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "");
        assert(url.get_port() == 0);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 3);
        assert_equal(url.get_param("key1"), "val1");
        assert_equal(url.get_param("key2"), "val2");
        assert_equal(url.get_param("key3"), "val3");
        assert_equal(url.get_fragment(), "fragment");
        assert_equal(url.get_query(), "key1=val1&key2=val2&key3=val3");
        assert_equal(url.get_scheme(), "");
    }
    void test_no_fragment()
    {
        xul::simple_uri url;
        assert(url.parse("/dir1/dir2/filename.ext?key1=val1&key2=val2&key3=val3"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "");
        assert(url.get_port() == 0);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 3);
        assert_equal(url.get_param("key1"), "val1");
        assert_equal(url.get_param("key2"), "val2");
        assert_equal(url.get_param("key3"), "val3");
        assert_equal(url.get_fragment(), "");
        assert_equal(url.get_query(), "key1=val1&key2=val2&key3=val3");
        assert_equal(url.get_scheme(), "");
    }
    void test_no_query()
    {
        xul::simple_uri url;
        assert(url.parse("/dir1/dir2/filename.ext#fragment"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "");
        assert(url.get_port() == 0);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 0);
        assert_equal(url.get_fragment(), "fragment");
        assert_equal(url.get_query(), "");
        assert_equal(url.get_scheme(), "");
    }
    void test_simple()
    {
        xul::simple_uri url;
        assert(url.parse("/dir1/dir2/filename.ext"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "");
        assert(url.get_port() == 0);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 0);
        assert_equal(url.get_fragment(), "");
        assert_equal(url.get_query(), "");
        assert_equal(url.get_scheme(), "");
    }
    void test_simple_with_host()
    {
        xul::simple_uri url;
        assert(url.parse("http://www.host.com:34671/dir1/dir2/filename.ext"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "www.host.com");
        assert(url.get_port() == 34671);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 0);
        assert_equal(url.get_fragment(), "");
        assert_equal(url.get_query(), "");
        assert_equal(url.get_scheme(), "http");
    }
    void test_no_port_http()
    {
        xul::simple_uri url;
        assert(url.parse("http://www.host.com/dir1/dir2/filename.ext"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "www.host.com");
        assert(url.get_port() == 80);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 0);
        assert_equal(url.get_fragment(), "");
        assert_equal(url.get_query(), "");
        assert_equal(url.get_scheme(), "http");
    }
    void test_no_port_rtsp()
    {
        xul::simple_uri url;
        assert(url.parse("rtsp://www.host.com/dir1/dir2/filename.ext"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "www.host.com");
        assert(url.get_port() == 554);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 0);
        assert_equal(url.get_fragment(), "");
        assert_equal(url.get_query(), "");
        assert_equal(url.get_scheme(), "rtsp");
    }
    void test_no_port_others()
    {
        xul::simple_uri url;
        assert(url.parse("xyz://www.host.com/dir1/dir2/filename.ext"));
        assert_equal(url.get_filename(), "filename.ext");
        assert_equal(url.get_host(), "www.host.com");
        assert(url.get_port() == 0);
        assert_equal(url.get_path(), "/dir1/dir2/filename.ext");
        assert(url.get_params()->size() == 0);
        assert_equal(url.get_fragment(), "");
        assert_equal(url.get_query(), "");
        assert_equal(url.get_scheme(), "xyz");
    }
    void test_no_path()
    {
        xul::simple_uri url;
        assert(url.parse("xyz://www.host.com"));
    }
    void test_host_only()
    {
        xul::simple_uri url;
        assert(url.parse("www.host.com/abc"));
        assert_equal(url.get_host(), "");
        assert(url.get_port() == 0);
        assert_equal(url.get_path(), "www.host.com/abc");
    }
    void test_encode()
    {
        xul::simple_uri url;
        assert(url.parse("/dir1/dir2/filename.ext?key1=val1 val2&key2=val2&key3=val3"));
        url.encode();
        assert_equal(url.get_param("key1"), "val1%20val2");
        assert_equal(url.get_original_string(), "/dir1/dir2/filename.ext?key1=val1%20val2&key2=val2&key3=val3");
    }
    void assert_equal(const char* s1, const char* s2)
    {
        assert(std::string(s2) == s1);
    }
};
XUL_TEST_SUITE_REGISTRATION(uri_test_case);
}


#endif
