#pragma once

#include "ParallelStudy.h"

// Local includes
#include "SweepWork.h"

// Forward declarations
class SweepWorkTracingStudy;
class TraceSweepWork;

class SweepStudy : public ParallelStudy<std::shared_ptr<SweepWork>, SweepWork, SweepStudy>
{
public:
  SweepStudy(const libMesh::Parallel::Communicator & comm, const InputParameters & params);

protected:
  void executeWork(const std::shared_ptr<SweepWork> & sweep_work, const THREAD_ID tid) override;
  void postReceiveParallelData(const parallel_data_iterator begin,
                               const parallel_data_iterator end) override;
  bool workIsComplete(const std::shared_ptr<SweepWork> & sweep_work) override;
  void postExecuteChunk(const work_iterator begin, const work_iterator end) override;
};
