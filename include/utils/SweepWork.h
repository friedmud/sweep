#pragma once

#include "libmesh/dof_object.h"

class SweepWork
{
public:
  Real  /* processor_id_type */_processor_id = Moose::INVALID_PROCESSOR_ID;
  Real  /* long unsigned int */ _current_value = 0;
  Real  /* dof_id_type */ _current_node = DofObject::invalid_id;
  Real  /* dof_id_type */ _current_elem = DofObject::invalid_id;
  Real  /* bool */ _should_continue = true;
};

/**
 * The following methods are specializations for using the Parallel::packed_range_* routines
 * for a vector of SweepWorks
 */
namespace libMesh
{
namespace Parallel
{
template <>
class Packing<std::shared_ptr<SweepWork>>
{
public:
  typedef Real buffer_type;

  static const unsigned int data_size = 3;
  static unsigned int packed_size(typename std::vector<Real>::const_iterator in);
  static unsigned int packable_size(const std::shared_ptr<SweepWork> & ray, const void *);

  template <typename Iter, typename Context>
  static void pack(const std::shared_ptr<SweepWork> & object, Iter data_out, const Context * context);

  template <typename BufferIter, typename Context>
  static std::shared_ptr<SweepWork> unpack(BufferIter in, Context * context);
};

} // namespace Parallel

} // namespace libMesh

// For restart
void dataStore(std::ostream & stream, std::shared_ptr<SweepWork> & ray, void * context);
void dataLoad(std::istream & stream, std::shared_ptr<SweepWork> & ray, void * context);
