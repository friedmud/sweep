#include "SweepWork.h"

// Raytracing Includes
#include "RayTracingPackingUtils.h"

// MOOSE Includes
#include "DataIO.h"

namespace libMesh
{
namespace Parallel
{

const unsigned int Packing<std::shared_ptr<SweepWork>>::mixed_size =
  RayTracingPackingUtils::mixedPackSize<buffer_type>(
    (processor_id_type)0, (long unsigned int)0, (dof_id_type)0, (dof_id_type)0, (bool)0);

unsigned int
Packing<std::shared_ptr<SweepWork>>::packed_size(typename std::vector<buffer_type>::const_iterator)
{
  return mixed_size;
}

unsigned int
Packing<std::shared_ptr<SweepWork>>::packable_size(const std::shared_ptr<SweepWork> &, const void *)
{
  return mixed_size;
}

template <>
std::shared_ptr<SweepWork>
Packing<std::shared_ptr<SweepWork>>::unpack(std::vector<buffer_type>::const_iterator in, SweepStudy  * study)
{
  std::shared_ptr<SweepWork> sweep_work = study->acquireParallelData(0);

  // Unpack the data
  RayTracingPackingUtils::mixedUnpack<buffer_type>(in,
                                                   sweep_work->_processor_id,
                                                   sweep_work->_current_value,
                                                   sweep_work->_current_node,
                                                   sweep_work->_current_elem,
                                                   sweep_work->_should_continue);

  return sweep_work;
}

template <>
void
Packing<std::shared_ptr<SweepWork>>::pack(const std::shared_ptr<SweepWork> & sweep_work,
                                    std::back_insert_iterator<std::vector<buffer_type>> data_out,
                                    const SweepStudy *)
{
  RayTracingPackingUtils::mixedPack<buffer_type>(data_out,
                                                 sweep_work->_processor_id,
                                                 sweep_work->_current_value,
                                                 sweep_work->_current_node,
                                                 sweep_work->_current_elem,
                                                 sweep_work->_should_continue);
}

} // namespace Parallel

} // namespace libMesh

void
dataStore(std::ostream & stream, std::shared_ptr<SweepWork> & sweep_work, void * context)
{
  storeHelper(stream, sweep_work->_processor_id, context);
  storeHelper(stream, sweep_work->_current_value, context);
  storeHelper(stream, sweep_work->_current_node, context);
  storeHelper(stream, sweep_work->_current_elem, context);
  storeHelper(stream, sweep_work->_should_continue, context);
}

void
dataLoad(std::istream & stream, std::shared_ptr<SweepWork> & sweep_work, void * context)
{
  loadHelper(stream, sweep_work->_processor_id, context);
  loadHelper(stream, sweep_work->_current_value, context);
  loadHelper(stream, sweep_work->_current_node, context);
  loadHelper(stream, sweep_work->_current_elem, context);
  loadHelper(stream, sweep_work->_should_continue, context);
}
