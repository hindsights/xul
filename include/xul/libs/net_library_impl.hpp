
#include <xul/config.hpp>

#include <xul/net/net_library.hpp>
#include <xul/libs/asio/asio_library_impl.hpp>
#include <xul/net/detail/tcp_server_impl.hpp>
#include <xul/net/detail/dns_backup_impl.hpp>
#include <xul/net/http/detail/http_server_impl.hpp>
#include <xul/net/http/detail/simple_http_server_router_impl.hpp>
#include <xul/net/http/detail/http_connection_impl.hpp>
#include <xul/net/http/detail/background_http_requester_impl.hpp>
#include <xul/net/http/detail/concurrent_http_connection_impl.hpp>
#include <xul/net/http/http_client.hpp>
#include <xul/net/http/detail/chunked_http_filter.hpp>
#include <xul/net/http/detail/gzip_http_filter.hpp>
#include <xul/net/simple_uri.hpp>
#include <xul/net/http/http_response.hpp>
#include <xul/net/http/http_request.hpp>

#include <xul/lib/library.hpp>
#include <xul/lib/library_repository.hpp>
#include <xul/lib/detail/library_info_impl.hpp>
#include <xul/lang/detail/class_info_impl.hpp>
#include <xul/lang/detail/global_service_manager_impl.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


http_server* create_http_server(io_service* ios)
{
    return new http_server_impl(ios);
}

tcp_server* create_tcp_server(io_service* ios)
{
    return new tcp_server_impl(ios);
}

http_connection* create_http_connection(io_service* ios)
{
    return new detail::http_connection_impl(ios);
}

background_http_requester* create_background_http_requester(io_service* ios)
{
    return new detail::background_http_requester_impl(ios);
}

http_connection* create_concurrent_http_connection(io_service* ios)
{
    return new detail::concurrent_http_connection_impl(ios);
}

http_filter* create_chunked_http_filter()
{
    return new chunked_http_filter();
}

http_filter* create_gzip_http_filter()
{
    return new gzip_http_filter();
}

url_request* create_url_request()
{
    return new http_request;
}

url_response* create_url_response()
{
    return new http_response;
}


uri* create_uri()
{
    return new simple_uri;
}


}
