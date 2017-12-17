#pragma once

#include <xul/lib/library_repository.hpp>
#include <xul/lang/object_ptr.hpp>
#include <boost/noncopyable.hpp>


namespace xul {


class library_manager : private boost::noncopyable
{
public:
    static library_repository* get_repository()
    {
        return instance().m_repo.get();
    }

    static void set_repository(library_repository* repo)
    {
        instance().m_repo = repo;
    }

    template <typename ObjectT>
    static ObjectT* create_object(const char* name)
    {
        library_repository* repo = get_repository();
        if (!repo)
            return NULL;
        return static_cast<ObjectT*>(repo->create_object(name));
    }
    template <typename LibT>
    static LibT* load_library(const char* name)
    {
        library_repository* repo = get_repository();
        if (!repo)
            return NULL;
        return static_cast<LibT*>(repo->load_library(name));
    }

    static library_manager& instance()
    {
        static library_manager the_instance;
        return the_instance;
    }

protected:
    library_manager()
    {
    }
    ~library_manager()
    {
    }
    boost::intrusive_ptr<library_repository> m_repo;
};


}
