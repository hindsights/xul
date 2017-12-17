#pragma once

#include <xul/data/numerics.hpp>
#include <xul/data/enum.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include <string>


namespace xul {


/// base class for program options
class program_options
{
public:
    static void split_command_line(const std::string& cmdline, std::vector<std::string>& args)
    {
        args.clear();
        bool  inQuotationMark = false;
        std::string token;
        for (size_t i = 0; i < cmdline.size(); ++i) {
            char a = cmdline[i];
            if (inQuotationMark) {
                if (a == '\"') {
                    inQuotationMark = false;
                    args.push_back(token);
                    token.clear();
                } else {
                    token.push_back(a);
                }
            } else {
                switch(a) {
                case '\"':
                    inQuotationMark = true;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if (false == token.empty()) {
                        args.push_back(token);
                        token.clear();
                    }
                    break;
                default:
                    token.push_back(a);
                    break;
                }
            }
        }
        if (false == token.empty()) {
            args.push_back(token);
            token.clear();
        }
    }


    virtual ~program_options() { }

    bool load(const std::string& cmdline)
    {
        std::vector<std::string> args;
        boost::split(args, cmdline, boost::is_space());
        //boost::algorithm::split(m_params, arg, boost::is_any_of(" \t"));
        return do_load(args);
    }

    bool load(int argc, char** argv)
    {
        return do_load(argc, (const char* const*)argv);
    }
    bool load(int argc, const char* const* argv)
    {
        return do_load(argc, argv);
    }

    std::string assemble() const
    {
        return do_assemble();
    }

    /// does the specified option exist
    bool has_option( const std::string& name ) const
    {
        return do_has_option(name);
    }

    /// get string value of a specified option
    std::string get_option( const std::string& name, const std::string& defaultVal = std::string() ) const
    {
        return do_get_option(name, defaultVal);
    }

    template <typename ValueT>
    ValueT get_numeric_option(const std::string& name, ValueT defaultVal) const
    {
        std::string s = do_get_option(name, std::string());
        if (s.empty())
            return defaultVal;
        return xul::numerics::parse<ValueT>(s, defaultVal);
    }

    template <typename EnumT>
    typename EnumT::enum_type get_enum_option(const std::string& name, typename EnumT::enum_type defaultVal) const
    {
        std::string s = do_get_option(name, std::string());
        if (s.empty())
            return defaultVal;
        return EnumT::parse(s, defaultVal);
    }

protected:
    virtual bool do_load(int argc, const char* const* argv) = 0;
    virtual bool do_load(const std::vector<std::string>& args) = 0;
    virtual std::string do_assemble() const = 0;
    virtual std::string do_get_option( const std::string& name, const std::string& defaultVal ) const = 0;
    virtual bool do_has_option( const std::string& name ) const = 0;
};


}
