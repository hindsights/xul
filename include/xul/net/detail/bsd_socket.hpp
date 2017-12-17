#pragma once

#include <xul/config.hpp>
#include <xul/net/socket.hpp>
#include <xul/net/socket_address.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/noncopyable.hpp>

#if defined(XUL_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif


namespace xul {


/// bsd socket
class bsd_socket : public object_impl<socket>
{
public:
    explicit bsd_socket(int handle = -1)
    {
        m_handle = handle;
    }
    virtual ~bsd_socket()
    {
        close();
    }

    virtual int get_handle() const
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
            XUL_APP_REL_INFO("BSD: shutdown socket " << m_handle);
            ::shutdown(m_handle, 2);
            XUL_APP_REL_INFO("BSD: close socket " << m_handle);
#if defined(XUL_WINDOWS)
            ::closesocket(m_handle);
#else
            ::close(m_handle);
#endif
            m_handle = -1;
        }
    }
    virtual bool create(int domain, int type, int protocol)
    {
        close();
        m_handle = ::socket(domain, type, protocol);
        return is_open();
    }

    virtual int get_last_error() const
    {
#if defined(XUL_WINDOWS)
        return ::WSAGetLastError();
#else
        return errno;
#endif
    }

    virtual bool connect(const socket_address& addr)
    {
        return 0 == ::connect(m_handle, addr.get_data(), addr.get_length());
    }
    virtual int send(const uint8_t* data, int size)
    {
        return ::send(m_handle, (const char*)data, size, 0);
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
        return ::recv(m_handle, (char*)data, size, 0);
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
    virtual int accept(socket_address& addr)
    {
        socklen_t addrlen = addr.get_length();
        int ret = ::accept(m_handle, addr.get_data(), &addrlen);
        if (ret < 0)
            return ret;
        assert(addrlen == addr.get_length());
        return ret;
    }
    virtual bool bind(const socket_address& addr)
    {
        return 0 == ::bind(m_handle, addr.get_data(), addr.get_length());
    }
    virtual bool listen(int backlog)
    {
        return 0 == ::listen(m_handle, backlog);
    }

    virtual bool ioctl(int cmd, char* arg)
    {
        assert(is_open());
#if defined(XUL_WINDOWS)
        return ::ioctlsocket(m_handle, cmd, (u_long*)arg) != -1;
#else
        return ::ioctl(m_handle, cmd, (char*)arg) != -1;
#endif
    }

    virtual bool set_option(int level, int cmd, const void* data, int size)
    {
        assert(is_open());
#if defined(XUL_WINDOWS)
        return setsockopt(m_handle, level, cmd, (const char*)data, size) != -1;
#else
        return setsockopt(m_handle, level, cmd, (void*)data, size) != -1;
#endif
    }

    virtual bool get_local_address(socket_address& addr) const
    {
        socklen_t addrlen = addr.get_length();
        int ret = ::getsockname(m_handle, addr.get_data(), &addrlen);
        return 0 == ret;
    }
    virtual bool get_remote_address(socket_address& addr) const
    {
        socklen_t addrlen = addr.get_length();
        int ret = ::getpeername(m_handle, addr.get_data(), &addrlen);
        return 0 == ret;
    }

private:
    int m_handle;
};


}
