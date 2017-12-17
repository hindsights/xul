#include <xul/static_lib_loader.hpp>
#include <xul/lib/detail/static_library_loader.hpp>

void xul_load_lib()
{
    xul::static_library_loader::load_repository();
}
