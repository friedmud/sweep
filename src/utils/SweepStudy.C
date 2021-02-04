#include "SweepStudy.h"

// local includes
#include "SweepWork.h"

SweepStudy::SweepStudy(const libMesh::Parallel::Communicator & comm, const InputParameters & params)
    : ParallelStudy<std::shared_ptr<SweepWork>, SweepWork, SweepStudy>(
      comm,
      this,
      params,
      "SweepStudy")
{
}

void
SweepStudy::postExecuteChunk(const work_iterator begin, const work_iterator end)
{
  for (auto it = begin; it != end; ++it)
  {
    std::shared_ptr<SweepWork> & sweep_work = *it;

    // The SweepWork is not done tracing
    if (sweep_work->shouldContinue())
    {
      // If the SweepWork is going to another processor - move it to the send buffer
      const auto dest_pid = sweep_work->currentElem()->processor_id();
      if (dest_pid != _pid)
        moveParallelDataToBuffer(sweep_work, dest_pid);
      else
        mooseError("Continuing SweepWork is not going to another processor after being traced\n\n",
                   sweep_work->getInfo());
    }
  }
}

bool
SweepStudy::workIsComplete(const std::shared_ptr<SweepWork> & sweep_work)
{
  // "Work" (a SweepWork) is considered complete in the parallel algorithm when it is done tracing
  return !sweep_work->shouldContinue();
}

void
SweepStudy::postReceiveParallelData(const parallel_data_iterator begin,
                                          const parallel_data_iterator end)
{
  // Move all of the parallel data (SweepWorks that are continuing to be traced on this processor)
  // directly into the work buffer
  moveContinuingWorkToBuffer(begin, end);
}

void
SweepStudy::executeWork(const std::shared_ptr<SweepWork> & sweep_work, const THREAD_ID tid)
{
  //_threaded_trace_sweep_work[tid]->trace(sweep_work);
}
