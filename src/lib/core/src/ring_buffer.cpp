#include "ring_buffer.h"
#include <common/exceptions.h>
#include <string.h>
#include <rte_ring.h>
#include <rte_errno.h>

namespace Core
{

Ring_buffer_base::Ring_buffer_base(std::string name, size_t size)
{
  _rte_ring = rte_ring_create(name.c_str(),/* debug name */
                              size, /* q depth */
                              SOCKET_ID_ANY,
                              0);
                              
  if(_rte_ring == nullptr && rte_errno == 17)
    throw General_exception("rte_ring_create failed (name=%s) exists!", name.c_str());
  else if(_rte_ring == nullptr)
    throw General_exception("rte_ring_create failed (size=%lu) for Ring_buffer_base class (%d)", size, rte_errno);
  
  if(option_DEBUG)
    PLOG("Ring_buffer_base [%s]: allocated at %p size=%lu", name.c_str(), _rte_ring, size);

}


Ring_buffer_base::Ring_buffer_base(std::string name, int index, size_t size)
{
  char name_sz[255];
  sprintf(name_sz,"%s-%d",name.c_str(), index);
  
  _rte_ring = rte_ring_create(name_sz,/* debug name */
                              size, /* q depth */
                              SOCKET_ID_ANY,
                              0);
  if(_rte_ring == nullptr && rte_errno == 17)
    throw General_exception("rte_ring_create failed (name=%s) exists!", name_sz);
  else if(_rte_ring == nullptr)
    throw General_exception("rte_ring_create failed (size=%lu) for Ring_buffer_base class (%d)", size, rte_errno);
 
  if(option_DEBUG)
    PLOG("Ring_buffer_base [%s]: allocated at %p size=%lu", name_sz, _rte_ring, size);    
}


Ring_buffer_base::~Ring_buffer_base() {
  if(option_DEBUG)
    PLOG("Ring_buffer_base: freeing at %p", _rte_ring);
  rte_ring_free(_rte_ring);
}


int Ring_buffer_base::full()
{
  assert(_rte_ring);
  return rte_ring_full(_rte_ring);
}


int Ring_buffer_base::empty()
{
  assert(_rte_ring);
  return rte_ring_empty(_rte_ring);
}


int Ring_buffer_base::sp_enqueue(void* elem)
{
  assert(_rte_ring);
  assert(elem);
  return rte_ring_sp_enqueue(_rte_ring,(void*) elem);
}


int Ring_buffer_base::sc_dequeue(void*& elem)
{
  assert(_rte_ring);
  return rte_ring_sc_dequeue(_rte_ring, (void**)&elem);
}


int Ring_buffer_base::mp_enqueue(void* elem)
{
  assert(_rte_ring);
  assert(elem);
  return rte_ring_mp_enqueue(_rte_ring,(void*) elem);
}


int Ring_buffer_base::mc_dequeue(void*& elem)
{
  assert(_rte_ring);
  return rte_ring_mc_dequeue(_rte_ring, (void**)&elem);
}

std::atomic<uint64_t> Ring_buffer_base::_static_name_index;


}
