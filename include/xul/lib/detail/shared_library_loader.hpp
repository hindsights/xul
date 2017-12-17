#pragma once

#include <xul/config.hpp>
#include <xul/os/paths.hpp>
#include <xul/os/module.hpp>
#include <xul/lib/detail/library_repository_impl.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <list>
#include <assert.h>

namespace xul {


class shared_library_loader
{
public:
    static shared_library_loader& instance()
    {
        static shared_library_loader the_instance;
        return the_instance;
    }

    typedef int (__cdecl *func_lib_register_type)(library_repository*);
    typedef boost::shared_ptr<loadable_module> loadable_module_ptr;

    shared_library_loader()
    {
        m_repo = new library_repository_impl;
    }
    ~shared_library_loader()
    {
        m_repo.reset();
        m_modules.clear();
    }

    library_repository* get_repository()
    {
        return m_repo.get();
    }

    bool load(const char* name)
    {
        if (NULL == name)
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
        if (xul::paths::get_directory(namestr).empty() && false == xul::paths::has_extension(namestr))
        {
#ifdef XUL_WINDOWS
            namestr += ".dll";
#else
            namestr = "lib" + namestr + ".so";
#endif
        }
        return do_load(namestr);
    }

private:
    bool do_load(const std::string& filepath)
    {
        loadable_module_ptr dllmodule(new loadable_module);
        if (false == dllmodule->load(filepath.c_str()))
        {
            assert(false);
            return false;
        }
        func_lib_register_type func_reglib = (func_lib_register_type)dllmodule->get_export_item("lib_register");
        if (NULL == func_reglib)
        {
            assert(false);
            return false;
        }
        if (func_reglib(m_repo.get()) < 0)
        {
            return false;
        }
        m_modules.push_back(dllmodule);
        return true;
    }

private:
    boost::intrusive_ptr<library_repository_impl> m_repo;
    std::list<loadable_module_ptr> m_modules;
};


}
