#pragma once

#include <xul/lib/detail/library_repository_impl.hpp>
#include <xul/lib/library_manager.hpp>
#include <xul/libs/log/log_library_impl.hpp>
#include <xul/libs/net_library_impl.hpp>
#include <xul/libs/data_library_impl.hpp>
#include <xul/libs/util_library_impl.hpp>


namespace xul {


class static_library_loader
{
public:
    static static_library_loader& instance()
    {
        static static_library_loader the_instance;
        return the_instance;
    }
    static void load_repository()
    {
        instance().load();
        library_manager::set_repository(instance().get_repository());
    }

    static_library_loader()
    {
        m_repo = new library_repository_impl;
    }
    ~static_library_loader()
    {
        m_repo.reset();
    }

    library_repository* get_repository()
    {
        return m_repo.get();
    }

    bool load()
    {
#if 0
        boost::intrusive_ptr<xul::library_info> loglib = new log_library_info;
        m_repo->register_library(loglib.get());
        boost::intrusive_ptr<xul::library_info> asiolib = new asio_library_info;
        m_repo->register_library(asiolib.get());
        boost::intrusive_ptr<xul::library_info> netlib = new net_library_info;
        m_repo->register_library(netlib.get());
        boost::intrusive_ptr<xul::library_info> datalib = new data_library_info;
        m_repo->register_library(datalib.get());
#endif
        return true;
    }

private:
    boost::intrusive_ptr<library_repository_impl> m_repo;
};


}
