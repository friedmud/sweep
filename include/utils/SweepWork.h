#pragma once

class SweepWork
{
public:
  long unsigned int _current_value = 0;
  dof_id_type _current_node = INVALID_DOF_ID;
  dof_id_type _current_elem = INVALID_DOF_ID;
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

  static data_size = 3;
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
