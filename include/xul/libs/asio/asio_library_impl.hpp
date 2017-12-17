
#include <xul/config.hpp>
#include <xul/net/io_library.hpp>
#include <xul/net/io_service.hpp>
#include <xul/libs/asio/asio_service.hpp>
#include <xul/lib/library.hpp>
#include <xul/lib/library_repository.hpp>
#include <xul/lib/detail/library_info_impl.hpp>
#include <xul/lang/detail/class_info_impl.hpp>
#include <xul/lang/detail/global_service_manager_impl.hpp>
#include <xul/lang/object_impl.hpp>

#include <xul/lang/object_ptr.hpp>


namespace xul {


io_service* create_io_service()
{
    return new asio_service;
}


}
