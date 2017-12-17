#pragma once

#include <xul/lib/library_info.hpp>
#include <xul/lang/class_info.hpp>
#include <xul/lang/object_impl.hpp>
#include <string>
#include <vector>


namespace xul {


class library_info_impl : public object_impl<library_info>
{
public:
    typedef boost::intrusive_ptr<class_info> class_info_ptr;

    explicit library_info_impl(const std::string& name, unsigned int version) : m_name(name), m_version(version) { }

    virtual const char* get_name() const { return m_name.c_str(); }
    virtual unsigned int get_version() const { return m_version; }
    virtual int register_classes(xul::library_repository* repo)
    {
        int lib_count = 0;
        for (size_t index = 0; index < m_classes.size(); ++index)
        {
            if (repo->register_class(m_classes[index].get()))
            {
                ++lib_count;
            }
        }
        return lib_count;
    }

protected:
    void add_class(xul::class_info* info)
    {
        m_classes.push_back(class_info_ptr(info));
    }

private:
    std::string m_name;
    unsigned int m_version;
    std::vector<class_info_ptr> m_classes;
};


}
