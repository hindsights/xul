#pragma once

#include <xul/log/log.hpp>
#include <boost/detail/lightweight_mutex.hpp>
#include <map>


namespace xul {


template <typename T>
class handle_map
{
public:
    
    long add(T obj)
    {
        boost::detail::lightweight_mutex::scoped_lock lock(m_lock);
        // try first 100 handles
        for (long i = 1; i < 100; ++i)
        {
            if (m_handles.find(i) == m_handles.end())
            {
                XUL_REL_TRACE("handle_map.add handle %ld %p\n", i, obj.get());
                m_handles[i] = obj;
                return i;
            }
        }
        assert(!m_handles.empty());
        long handle = m_handles.rbegin()->first + 1;
        assert(m_handles.find(handle) == m_handles.end());
        if (handle > 1000000)
        {
            XUL_REL_TRACE("handle_map.add handle is too large %ld %p", handle, obj.get());
            return -1;
        }
        XUL_REL_TRACE("handle_map.add large handle %ld %p", handle, obj.get());
        m_handles[handle] = obj;
        return handle;
    }
    
    T find(int handle)
    {
        boost::detail::lightweight_mutex::scoped_lock lock(m_lock);
        typename std::map<int, T>::const_iterator iter = m_handles.find(handle);
        if (iter == m_handles.end())
        {
            XUL_REL_TRACE("handle_map.find not found %ld\n", handle);
            return T();
        }
        XUL_REL_TRACE("handle_map found  %ld %p\n", handle, iter->second.get());
        return iter->second;
    }
    T remove(int handle)
    {
        boost::detail::lightweight_mutex::scoped_lock lock(m_lock);
        typename std::map<int, T>::iterator iter = m_handles.find(handle);
        if (iter == m_handles.end())
        {
            XUL_REL_TRACE("handle_map not found %ld\n", handle);
            return T();
        }
        T obj = iter->second;
        XUL_REL_TRACE("handle_map found  %ld %p\n", handle, obj.get());
        m_handles.erase(iter);
        return obj;
    }
    
private:
    std::map<int, T> m_handles;
    boost::detail::lightweight_mutex m_lock;
};


}
