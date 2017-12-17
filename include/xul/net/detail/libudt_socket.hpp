#pragma once

#include <xul/config.hpp>
#include <xul/net/socket.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/log/log.hpp>
#include <boost/noncopyable.hpp>

#include <udt.h>


namespace xul {


/// bsd socket implemented in libudt
class libudt_socket : public object_impl<socket>
{
public:
    typedef UDTSOCKET handle_type;

    explicit libudt_socket(handle_type handle = -1)
    {
        m_handle = handle;
    }
    virtual ~libudt_socket()
    {
        close();
    }

    virtual handle_type get_handle() const
    {
        return m_handle;
    }
    virtual bool is_open() const
    {
        return m_handle >= 0;
    }
    virtual void close()
    {
        if (is_open())
        {
            XUL_APP_REL_INFO("UDT: close socket " << m_handle);
            UDT::close(m_handle);
            m_handle = -1;
        }
    }
    virtual bool create(int domain, int type, int protocol)
    {
        close();
        m_handle = UDT::socket(domain, type, protocol);
        return is_open();
    }

    virtual int get_last_error() const
    {
        return errno;
    }

    virtual bool connect(const socket_address& addr)
    {
        return 0 == UDT::connect(m_handle, addr.get_data(), addr.get_length());
    }
    virtual int send(const uint8_t* data, int size)
    {
        return UDT::send(m_handle, (const char*)data, size, 0);
    }
    virtual int send_n(const uint8_t* data, int size)
    {
        int sent_size = 0;
        while (sent_size < size)
        {
            int len = send(data + sent_size, size - sent_size);
            if (len <= 0)
                return len;
            sent_size += len;
        }
        return sent_size;
    }
    virtual int receive(uint8_t* data, int size)
    {
        return UDT::recv(m_handle, (char*)data, size, 0);
    }
    virtual int receive_n(uint8_t* data, int size)
    {
        int recv_size = 0;
        while (recv_size < size)
        {
            int len = receive(data + recv_size, size - recv_size);
            if (len <= 0)
                return len;
            recv_size += len;
        }
        return recv_size;
    }
    virtual handle_type accept(socket_address& addr)
    {
        int addrlen = addr.get_length();
        int ret = UDT::accept(m_handle, addr.get_data(), &addrlen);
        if (ret < 0)
            return ret;
        assert(addrlen == addr.get_length());
        return ret;
    }
    virtual bool bind(const socket_address& addr)
    {
        return 0 == UDT::bind(m_handle, addr.get_data(), addr.get_length());
    }
    virtual bool listen(int backlog)
    {
        return 0 == UDT::listen(m_handle, backlog);
    }

    virtual bool ioctl(int cmd, char* arg)
    {
        assert(is_open());
        assert(false);
        //return UDT::fnctl(m_handle, cmd, (char*)arg) != -1;
        return false;
    }
    virtual bool set_option(int level, int cmd, const void* data, int size)
    {
        return true;
    }

    virtual bool get_local_address(socket_address& addr) const
    {
        int addrlen = addr.get_length();
        int ret = UDT::getsockname(m_handle, addr.get_data(), &addrlen);
        return 0 == ret;
    }
    virtual bool get_remote_address(socket_address& addr) const
    {
        int addrlen = addr.get_length();
        int ret = UDT::getpeername(m_handle, addr.get_data(), &addrlen);
        return 0 == ret;
    }

private:
    handle_type m_handle;
};


}
