#include "SweepStudy.h"

// local includes
#include "SweepWork.h"

// Moose includes
#include "MooseMesh.h"
#include "AuxiliarySystem.h"

SweepStudy::SweepStudy(const libMesh::Parallel::Communicator & comm, const InputParameters & params, MooseMesh & mesh, AuxiliarySystem & aux_sys)
    : ParallelStudy<std::shared_ptr<SweepWork>, SweepWork, SweepStudy>(
      comm,
      this,
      params,
      "SweepStudy"),
    _mesh(mesh),
    _boundary_id(mesh.getBoundaryID(params.get<BoundaryName>("boundary"))),
    _aux_sys(aux_sys),
    _sys_num(aux_sys.number()),
    _var_num(aux_sys.sys().variable_number(params.get<VariableName>("variable"))),
  _pid(_mesh.processor_id())
{
}

void SweepStudy::generateWork()
{
  _node_to_elem_map = &_mesh.nodeToElemMap();
  _node_set_nodes = &_mesh.nodeSetNodes();

  // Grab the solution vector
  _solution = &_aux_sys.solution();

  // Grab all of the nodes on the boundary
  const auto & beginning_nodes = _node_set_nodes->at(0);

  for (const auto & n_id : beginning_nodes)
  {
    auto node = _mesh.nodePtr(n_id);

    // If the node exists on this processor and it's a node we own...
    if (node && node->processor_id() == _pid)
    {
      // Create Work
      std::shared_ptr<SweepWork> work = acquireParallelData(0);

      work->_processor_id = _pid;
      work->_current_value = 0;
      work->_current_node = n_id;

      auto & connected_elems = _node_to_elem_map->at(n_id);

      // There should be only one connected element (because we're on the end)
      if (connected_elems.size() != 1)
        mooseError("More than one connected element found for start of sweep");

      work->_current_elem = _mesh.elemPtr(connected_elems[0])->id();
      work->_should_continue = true;

      moveWorkToBuffer(work, 0);
    }
  }
}

void
SweepStudy::postExecute()
{
  _solution->close();
}

void
SweepStudy::postExecuteChunk(const work_iterator begin, const work_iterator end)
{
  for (auto it = begin; it != end; ++it)
  {
    auto & sweep_work = *it;

    // The SweepWork is not done
    if (sweep_work->_should_continue)
    {
      // If the SweepWork is going to another processor - move it to the send buffer
      const auto dest_pid = sweep_work->_processor_id;

      if (dest_pid != _pid)
        moveParallelDataToBuffer(sweep_work, dest_pid);
      else
        mooseError("Continuing SweepWork is not going to another processor after being swept");
    }
  }
}

bool
SweepStudy::workIsComplete(const std::shared_ptr<SweepWork> & sweep_work)
{
  // "Work" (a SweepWork) is considered complete in the parallel algorithm when it is done tracing
  return !sweep_work->_should_continue;
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
SweepStudy::executeWork(const std::shared_ptr<SweepWork> & sweep_work, const THREAD_ID /*tid*/)
{
  std::cout << "Executing Work!" << std::endl;

  // Incoming work is logically on the "left" side of each 1D element (node 0)
  // Let's handle that as a special case up front - then process all of the "right" nodes after that

  auto current_node_id = sweep_work->_current_node;

  // Incoming work should always be for a node that we own - but let's check
  mooseAssert(_mesh.nodePtr(current_node_id)->processor_id() == _pid, "Incoming work to sweeper is not for a local node!");

  // Get this node
  auto current_node = _mesh.nodePtr(current_node_id);

  // Get the dof_index for the variable at this node
  auto dof = current_node->dof_number(_sys_num, _var_num, 0);

  // Set the value of the first node
  _solution->set(dof, sweep_work->_current_value++);

  // Note: the element set for incoming work is to the "right" of the incoming node
  auto current_elem_id = sweep_work->_current_elem;
  auto current_elem = _mesh.elemPtr(current_elem_id);

  // Let's make sure that's true
  mooseAssert(current_elem->node_ptr(0) == _mesh.nodePtr(current_node_id), "Incoming node to sweeper is not for the 'left' node for the incoming elem!");

  // Get the node on the "right" of this element
  current_node = current_elem->node_ptr(1);

  // Iterate through all of the other nodes owned by this processor on the right of elements (node 1)
  while (true)
  {
    std::cerr << _pid << " Elem: " << current_elem->id() << std::endl;

    // Set the value at this node
    dof = current_node->dof_number(_sys_num, _var_num, 0);
    _solution->set(dof, sweep_work->_current_value++);

    // Get the element to the right
    current_elem = current_elem->neighbor_ptr(1);

    // Are we at the end?
    if (!current_elem)
    {
      sweep_work->_should_continue = false;
      break;
    }

    // Get the node on the right of the next elem
    current_node = current_elem->node_ptr(1);

    std::cerr << _pid << " current_node: " << current_node->id() << " pid: " << current_node->processor_id() << std::endl;
    std::cerr << _pid << " current_elem: " << current_elem->id() << " pid: " << current_elem->processor_id() << std::endl;

    // Is this node owned by another proc?
    if (current_node->processor_id() != _pid)
    {
      // Save out all of the current information to pass on
      sweep_work->_processor_id = current_node->processor_id();
      sweep_work->_current_node = current_node->id();

      mooseAssert(current_elem->neighbor_ptr(1), "Found off-processor node without element on other side");

      // Store the element to the _right_ of the current node
      sweep_work->_current_elem = current_elem->neighbor_ptr(1)->id();

      sweep_work->_should_continue = true;

      break;
    }
  }
}
