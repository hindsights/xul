#pragma once

#include <xul/data/pool_buffer.hpp>
#include <xul/lang/object_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <stdint.h>


namespace xul {


class pool;
typedef boost::shared_ptr<pool> pool_ptr;

class block_pool;
//typedef boost::shared_ptr<block_pool> block_pool_ptr;

typedef boost::intrusive_ptr<pool_buffer> pool_buffer_ptr;
typedef boost::intrusive_ptr<const pool_buffer> const_pool_buffer_ptr;


}
