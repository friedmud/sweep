#include "SweepWork.h"

unsigned int
Packing<std::shared_ptr<SweepWork>>::packed_size(typename std::vector<buffer_type>::const_iterator in)
{
  return data_size;
}

unsigned int
Packing<std::shared_ptr<SweepWork>>::packable_size(const std::shared_ptr<SweepWork> & sweep_work, const void *)
{
  return data_size;
}

template <>
std::shared_ptr<SweepWork>
Packing<std::shared_ptr<SweepWork>>::unpack(std::vector<buffer_type>::const_iterator in, SweepStudy  * study)
{
  sted::shared_ptr<SweepWork> sweep_work = study->acquireParallelData(0);

  // Unpack the data
  *in++ >> sweep_work->_current_value;
  *in++ >> sweep_work->_current_node;
  *in++ >> sweep_work->_current_elem;

  return sweep_work;
}

template <>
void
Packing<std::shared_ptr<SweepWork>>::pack(const std::shared_ptr<SweepWork> & sweep_work,
                                    std::back_insert_iterator<std::vector<buffer_type>> data_out,
                                    const void *)
{
  data_out = static_cast<buffer_type>(sweep_work->_current_value);
  data_out = static_cast<buffer_type>(sweep_work->_current_node);
  data_out = static_cast<buffer_type>(sweep_work->_current_elem);
}

} // namespace Parallel

} // namespace libMesh

void
dataStore(std::ostream & stream, std::shared_ptr<SweepWork> & sweep_work, void * context)
{
  storeHelper(stream, sweep_work->_current_value, context);
  storeHelper(stream, sweep_work->_current_node, context);
  storeHelper(stream, sweep_work->_current_elem, context);
}

void
dataLoad(std::istream & stream, std::shared_ptr<SweepWork> & sweep_work, void * context)
{
  loadHelper(stream, sweep_work->_current_value, context);
  loadHelper(stream, sweep_work->_current_node, context);
  loadHelper(stream, sweep_work->_current_elem, context);
}
