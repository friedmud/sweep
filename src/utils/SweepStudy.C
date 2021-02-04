#include "SweepStudy.h"

// Local includes
#include "TraceRay.h"
#include "RayTracingStudy.h"

SweepStudy::SweepStudy(
    RayTracingStudy & ray_tracing_study,
    const std::vector<std::shared_ptr<TraceRay>> & threaded_trace_ray)
  : ParallelStudy<std::shared_ptr<Ray>, Ray, RayTracingStudy>(ray_tracing_study.comm(),
                                                              &ray_tracing_study,
                                                              ray_tracing_study.parameters(),
                                                              "SweepStudy"),
    _ray_tracing_study(ray_tracing_study),
    _threaded_trace_ray(threaded_trace_ray)
{
}

void
SweepStudy::postExecuteChunk(const work_iterator begin, const work_iterator end)
{
  for (auto it = begin; it != end; ++it)
  {
    std::shared_ptr<Ray> & ray = *it;

    // The Ray is done tracing
    if (!ray->shouldContinue())
    {
      _ray_tracing_study.onCompleteRay(ray);
      continue;
    }

    // If the Ray is going to another processor - move it to the send buffer
    const auto dest_pid = ray->currentElem()->processor_id();
    if (dest_pid != _pid)
      moveParallelDataToBuffer(ray, dest_pid);
    else
      mooseError("Continuing Ray is not going to another processor after being traced\n\n",
                 ray->getInfo());
  }
}

bool
SweepStudy::workIsComplete(const std::shared_ptr<Ray> & ray)
{
  // "Work" (a Ray) is considered complete in the parallel algorithm when it is done tracing
  return !ray->shouldContinue();
}

void
SweepStudy::postReceiveParallelData(const parallel_data_iterator begin,
                                          const parallel_data_iterator end)
{
  // Move all of the parallel data (Rays that are continuing to be traced on this processor)
  // directly into the work buffer
  moveContinuingWorkToBuffer(begin, end);
}

void
SweepStudy::executeWork(const std::shared_ptr<Ray> & ray, const THREAD_ID tid)
{
  if (!ray->shouldContinue())
    mooseError("Tracing Ray that should not continue\n\n", ray->getInfo());

  // If this is false, it means we have a Ray that is banked to go onto another processor
  if (ray->currentElem()->processor_id() == _pid)
    _threaded_trace_ray[tid]->trace(ray);
}

void
SweepStudy::moveWorkError(const MoveWorkError error, const std::shared_ptr<Ray> * ray) const
{
  std::stringstream oss;
  oss << "In method " << _ray_tracing_study.type() << "::addRay(s)ToBuffer:\n";

  if (error == MoveWorkError::DURING_EXECUTION_DISABLED)
  {
    oss << "Rays are being added to the buffer during propagation.\n\n";
    oss << "This capability must be enabled by setting the parameter\n";
    oss << "'allow_new_work_during_execution' to true.";
  }
  else if (error == MoveWorkError::PRE_EXECUTION_AND_EXECUTION_ONLY)
    oss << "Rays can only be added to the buffer during generateRays() and tracing.";
  else if (error == MoveWorkError::PRE_EXECUTION_ONLY)
    oss << "Rays can only be added to the buffer during generateRays().";
  else if (error == MoveWorkError::PRE_EXECUTION_THREAD_0_ONLY)
    oss << "Rays can only be added on thread 0 during generateRays() (not thread safe)";
  else if (error == CONTINUING_DURING_EXECUTING_WORK)
    ParallelStudy<std::shared_ptr<Ray>, Ray, RayTracingStudy>::moveWorkError(error, ray);

  if (ray)
    oss << "\n\n" << (*ray)->getInfo();

  mooseError(oss.str());
}