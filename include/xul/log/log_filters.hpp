#pragma once


namespace xul {


class log_filter;


#ifdef ANDROID
log_filter* create_android_log_appender();
#endif

log_filter* create_console_log_appender();
log_filter* create_asio_log_transmitter();
log_filter* create_file_log_appender();


}
