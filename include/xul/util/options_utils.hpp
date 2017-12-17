#pragma once

#include <xul/util/options.hpp>
#include <xul/util/options_wrapper.hpp>
#include <xul/data/map.hpp>
#include <xul/std/maps.hpp>
#include <xul/io/stdfile.hpp>
#include <xul/macro/iterate.hpp>
#include <xul/macro/foreach.hpp>
#include <xul/log/log.hpp>
#include <xul/util/trace.hpp>

#include <map>
#include <string>


namespace xul {


class options_utils
{
public:
    static void update( options* opts, const std::map<std::string, std::string>& params )
    {
        XUL_FOREACH_CONST(xul::string_dict, params, iter)
        {
            XULTRACEOUT("options.update %s=%s\n", iter->first.c_str(), iter->second.c_str());
            opts->set(iter->first.c_str(), iter->second.c_str());
        }
    }
    static void update(options* opts, const std::string& strparams)
    {
        xul::string_dict params;
        xul::string_dicts::parse(params, strparams, '&', '=');
        update(opts, params);
    }

    static void update( options* opts, const xul::string_table* params )
    {
        XUL_ITERATE(xul::string_table, params, iter)
        {
            const xul::string_table::entry_type* entry = iter->element();
            XULTRACEOUT("options.update %s=%s\n", entry->get_key(), entry->get_value());
            opts->set(entry->get_key(), entry->get_value());
        }
    }

    static bool parse_conf( options* opts1, const char* filepath )
    {
        //clear();
        xul::stdfile_reader fin;
        if ( false == fin.open_text(filepath) )
        {
            //XUL_APP_REL_ERROR("failedt o load config file " << filepath);
            return false;
        }

        std::string line;
        xul::options_wrapper opts(*opts1);

        while(fin.read_line(line)) {
            strings::trim_spaces(line);
            if (line.empty())
                continue;
            assert(line.size() > 0);
            if ('#' == line[0])
                continue;
            opts.parse(line);
        }
        return true;
    }

};


}
