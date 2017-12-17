#pragma once

#include <xul/util/program_options.hpp>

#include <sstream>


namespace xul {


/// simple program parsing utility
class simple_program_options : public program_options
{
public:

public:
    const std::vector<std::string>& get_arguments() const { return m_args; }
    
    bool load_strings(const std::vector<std::string>& args)
    {
        return do_load(args);
    }
    bool load_string(const std::string& arg)
    {
        return load(arg);
    }
    /// load from command line string
    bool load(const std::string& arg)
    {
        m_args.clear();
        boost::split(m_args, arg, boost::is_space());
        //boost::algorithm::split(m_params, arg, boost::is_any_of(" \t"));
        return m_args.size() > 0;
    }

    /// load from command line argv
    bool load(int argc, char** argv)
    {
        return load(argc, (const char* const*)argv);
    }
    bool load(int argc, const char* const* argv)
    {
        m_args.clear();
        if (argc <= 0)
            return false;
        m_args.clear();
        m_args.reserve(argc);
        for (int i = 0; i < argc; ++i)
        {
            m_args.push_back(argv[i]);
        }
        return true;
    }

    bool has_option( const std::string& name ) const
    {
        return std::find(m_args.begin(), m_args.end(), name) != m_args.end();
    }

    /// get option by name, return default value if option is not found
    std::string get_option( const std::string& name, const std::string& defaultVal = std::string() ) const
    {
        std::string result;
        std::vector<std::string>::const_iterator pos = std::find(m_args.begin(), m_args.end(), name);
        if (pos != m_args.end())
        {
            ++pos;
            if (pos != m_args.end())
            {
                return *pos;
            }
        }
        return defaultVal;
    }

protected:
    virtual bool do_load(int argc, const char* const* argv)
    {
        m_args.clear();
        if (argc < 0)
            return false;
        m_args.clear();
        m_args.reserve(argc);
        for (int i = 0; i < argc; ++i)
        {
            m_args.push_back(argv[i]);
        }
        return true;
    }

    virtual bool do_load(const std::vector<std::string>& args)
    {
        m_args = args;
        return false == m_args.empty();
    }

    virtual std::string do_assemble() const
    {
        std::ostringstream oss;
        for (size_t index = 0; index < m_args.size(); ++index)
        {
            const std::string& arg = m_args[index];
            if (arg.find_first_of(" \t") != std::string::npos)
            {
                oss << "\"" << arg << "\"" << " ";
            }
            else
            {
                oss << arg << " ";
            }
        }
        return oss.str();
    }

    virtual std::string do_get_option( const std::string& name, const std::string& defaultVal ) const
    {
        std::string result;
        std::vector<std::string>::const_iterator pos = std::find(m_args.begin(), m_args.end(), name);
        if (pos != m_args.end())
        {
            ++pos;
            if (pos != m_args.end())
            {
                return *pos;
            }
        }
        return defaultVal;
    }

    virtual bool do_has_option( const std::string& name ) const
    {
        return std::find(m_args.begin(), m_args.end(), name) != m_args.end();
    }

private:
    std::vector<std::string> m_args;
};


}
