#pragma once

#include <xul/net/message_socket.hpp>
#include <xul/lang/object_ptr.hpp>
#include <xul/util/singleton.hpp>
#include <xul/net/io_service.hpp>
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>


namespace xul {


class daemon_controller;


class daemon_controller_listener
{
public:
    virtual void on_daemon_fork_succeeded(daemon_controller* sender, int index, pid_t pid) = 0;
    virtual void on_daemon_fork_failed(daemon_controller* sender, int index, int errcode) = 0;
    virtual void on_daemon_child_forked(daemon_controller* sender, int index) = 0;
};


class dummy_daemon_controller_listener : public daemon_controller_listener, public singleton<dummy_daemon_controller_listener>
{
public:
    virtual void on_daemon_fork_succeeded(daemon_controller* sender, int index, pid_t pid) { }
    virtual void on_daemon_fork_failed(daemon_controller* sender, int index, int errcode) { }
    virtual void on_daemon_child_forked(daemon_controller* sender, int index) { }
};


class daemon_controller
{
public:
    class daemon_worker_info
    {
    public:
        int pid;
        boost::intrusive_ptr<message_socket> socket;

        daemon_worker_info()
        {
            pid = 0;
        }
    };
    typedef boost::shared_ptr<daemon_worker_info> daemon_worker_info_ptr;

    explicit daemon_controller(io_service* ios)
    {
        m_ios = ios;
        set_listener(NULL);
        init();
    }

    message_socket* get_socket()
    {
        return m_socket.get();
    }

    void set_listener(daemon_controller_listener* listener)
    {
        m_listener = listener ? listener : &dummy_daemon_controller_listener::instance();
    }

    bool fork_children(int count)
    {
        assert(count > 0 && count < 100);
        m_children_pids.resize(count, 0);
        for (int i = 0; i < count; ++i)
        {
            printf("fork %d\n", getpid());
            int socks[2] = { -1, -1 };
            int ret = ::socketpair(AF_INET, SOCK_DGRAM, 0, socks);
            if (ret < 0)
            {
                printf("socketpair failed %d %d\n", ret, errno);
                continue;
            }
            pid_t pid = fork();
            if (pid < 0)
            {
                m_listener->on_daemon_fork_failed(this, i, pid);
            }
            else if (0 == pid)
            {
                m_socket = m_ios->recreate_message_socket(socks[1]);
                m_listener->on_daemon_child_forked(this, i);
                return false;
            }
            else
            {
                m_children_pids[i] = pid;
                m_socket = m_ios->recreate_message_socket(socks[0]);
                m_listener->on_daemon_fork_succeeded(this, i, pid);
            }
        }
        return true;
    }

private:
    void init()
    {
        //signal(SIGCHLD, &daemon_controller::handle_signal_child_die);
        struct sigaction sig, oldsig;
        memset(&sig, 0, sizeof(sig));
        memset(&oldsig, 0, sizeof(oldsig));
        sig.sa_sigaction = &daemon_controller::handle_sigaction_child_die;
        sigaction(SIGCHLD, &sig, &oldsig);
    }
    static void handle_signal_child_die(int x)
    {
        printf("handle_signal_child_die %d %d\n", getpid(), x);
    }
    static void handle_sigaction_child_die(int signum, siginfo_t* info, void* context)
    {
        printf("handle_sigaction_child_die %d %d %d %d\n", getpid(), signum, info->si_errno, info->si_code);
    }

private:
    daemon_controller_listener* m_listener;
    std::vector<daemon_worker_info_ptr> m_workers;
    daemon_worker_info m_worker;
    //boost::intrusive_ptr<message_socket> m_socket;
    boost::intrusive_ptr<io_service> m_ios;
};


}
