#pragma once

#include <xul/util/options.hpp>


namespace xul {


class log_config : public xul::options
{
public:
    std::string base_directory;
    std::string name;
    std::string config_file;
    
    bool enable_console_log;
    bool enable_file_log;
    bool enable_android_log;
    int log_level;
    
    int file_roll_size;
    int file_roll_count;
    bool file_append;
};


log_config* create_log_config();
    

}
