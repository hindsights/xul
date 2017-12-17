#pragma once

#include <xul/lib/library_repository.hpp>
#include <xul/lib/library.hpp>
#include <xul/lang/class_info.hpp>
#include <xul/lib/library_info.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/lang/object_ptr.hpp>
#include <map>
#include <string>
#include <assert.h>


namespace xul {


class library_repository_impl : public object_impl<library_repository>
{
public:
    typedef boost::intrusive_ptr<class_info> class_info_ptr;
    typedef std::map<std::string, class_info_ptr> class_info_collection;
    typedef boost::intrusive_ptr<library_info> library_info_ptr;
    typedef std::map<std::string, library_info_ptr> library_info_collection;

    library_repository_impl()
    {
    }
    ~library_repository_impl()
    {
    }

    virtual bool register_class(class_info* info)
    {
        if (!info)
        {
            assert(false);
            return false;
        }
        const char* name = info->get_name();
        if (!name)
        {
            assert(false);
            return false;
        }
        std::string namestr(name);
        if (namestr.empty())
        {
            assert(false);
            return false;
        }
        class_info_ptr& item = m_classes[namestr];
        if (item)
        {
            assert(false);
            return false;
        }
        item = info;
        return true;
    }
    virtual bool register_library(library_info* info)
    {
        if (!info)
        {
            assert(false);
            return false;
        }
        const char* name = info->get_name();
        if (!name)
        {
            assert(false);
            return false;
        }
        std::string namestr(name);
        if (namestr.empty())
        {
            assert(false);
            return false;
        }
        library_info_ptr& item = m_libraries[namestr];
        if (item)
        {
            assert(false);
            return false;
        }
        item = info;
        info->register_classes(this);
        return true;
    }
    virtual class_info* find_class(const char* name)
    {
        if (!name)
        {
            assert(false);
            return NULL;
        }
        std::string namestr(name);
        if (namestr.empty())
        {
            assert(false);
            return NULL;
        }
        class_info_collection::const_iterator iter = m_classes.find(namestr);
        if (iter == m_classes.end())
            return NULL;
        return iter->second.get();
    }
    virtual object* create_object(const char* name)
    {
        class_info* info = find_class(name);
        if (NULL == info)
            return NULL;
        return info->create_object();
    }
    virtual library_info* find_library(const char* name)
    {
        if (!name)
        {
            assert(false);
            return NULL;
        }
        std::string namestr(name);
        if (namestr.empty())
        {
            assert(false);
            return NULL;
        }
        library_info_collection::const_iterator iter = m_libraries.find(namestr);
        if (iter == m_libraries.end())
            return NULL;
        return iter->second.get();
    }
    virtual library* load_library(const char* name)
    {
        library_info* info = find_library(name);
        if (NULL == info)
            return NULL;
        return info->load_library();
    }

private:
    class_info_collection m_classes;
    library_info_collection m_libraries;
};


}
