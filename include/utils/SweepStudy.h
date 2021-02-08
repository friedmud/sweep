#pragma once

#include "ParallelStudy.h"

// Local includes
#include "SweepWork.h"

// Forward declarations
class MooseMesh;
class AuxiliarySystem;

class SweepStudy : public ParallelStudy<std::shared_ptr<SweepWork>, SweepWork, SweepStudy>
{
public:
  SweepStudy(const libMesh::Parallel::Communicator & comm, const InputParameters & params, MooseMesh & mesh, AuxiliarySystem & aux_sys);

  /**
   * Call after preExecute(), but before execute(), to actually create all of the work to do
   */
  void generateWork();

  /**
   * Call after execute()
   */
  void postExecute();

protected:
  void executeWork(const std::shared_ptr<SweepWork> & sweep_work, const THREAD_ID tid) override;
  void postReceiveParallelData(const parallel_data_iterator begin,
                               const parallel_data_iterator end) override;
  bool workIsComplete(const std::shared_ptr<SweepWork> & sweep_work) override;
  void postExecuteChunk(const work_iterator begin, const work_iterator end) override;

  MooseMesh & _mesh;

  BoundaryID _boundary_id;

  const std::map<dof_id_type, std::vector<dof_id_type>> * _node_to_elem_map;
  const std::map<boundary_id_type, std::vector<dof_id_type>> * _node_set_nodes;

  AuxiliarySystem & _aux_sys;

  unsigned int _sys_num;
  unsigned int _var_num;

  NumericVector<Number> * _solution;

  processor_id_type _pid;
};
