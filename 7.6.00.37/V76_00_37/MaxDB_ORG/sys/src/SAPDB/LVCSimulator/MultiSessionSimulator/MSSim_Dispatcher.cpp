
#include "LVCSimulator/MultiSessionSimulator/MSSim_Dispatcher.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Coordinator.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Task.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Trace.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_PatchedSysFunc.hpp"

#include < set >

using namespace std; 

//*********************************************************************************

MSSim_Dispatcher::MSSim_Dispatcher( MSSim_Coordinator& task_handler, int dispatchLimit )
: m_task_handler( task_handler )
, m_dispatch_count( 0 )
, m_dispatch_limit( dispatchLimit )
, m_enabled( true )
, m_running( 0 )
{
}

void MSSim_Dispatcher::dispatch( MSSim_Task* current ) 
{

  if( m_running ) {
    if( ! m_runnable.first() )
    {
      // continue, when no other runnable tasks are available
      return; 
    }
    setRunnable( m_running ); 
  } 

  MSSim_Task* next_task = m_runnable.popFront(); 

  setRunning( next_task ); 
  m_dispatch_count = 0; 
  switchToTask( current, next_task ); 

  // activate handler after switch
  setEnabled( true ); 
}

void MSSim_Dispatcher::finishTask( MSSim_Task* task )
{
  task->cleanup(); 
  setFinished( task ); 
}

bool MSSim_Dispatcher::isFinished( MSSim_Task* task )
{
  return m_finished.contains( task ); 
}

void MSSim_Dispatcher::unregisterTask( MSSim_Task* task )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::unregisterTask()" )
  MSSIM_TRACE( TraceSimDetail, " task: " << task )

  removeInternal( task ); 
}

bool MSSim_Dispatcher::isAlive( MSSim_Task* task )
{
  if( m_running == task )
    return true; 

  if( m_waiting.contains( task ) )
    return true; 

  if( m_runnable.contains( task ) )
    return true; 

  return false; 
}

MSSim_Task* MSSim_Dispatcher::getTask( TASKID taskId )
{
  if( m_running->getId() == taskId )
    return m_running; 

  {
    MSSim_Task* task( m_waiting.first() ); 
    while( task )
    {
      if( task->getId() == taskId )
        return task; 
      task = task->m_next; 
    }
  }

  {
    MSSim_Task* task( m_runnable.first() ); 
    while( task )
    {
      if( task->getId() == taskId )
        return task; 
      task = task->m_next; 
    }
  }

  return 0; 
}

void MSSim_Dispatcher::suspend( MSSim_Task* task )
{
  if( task->getStatus() == ts_running )
  {
    setWaiting( task ); 
    dispatch( task ); 
  } else 
  {
    setWaiting( task ); 
  }
}

void MSSim_Dispatcher::resume( MSSim_Task* task )
{
  setRunnable( task ); 
}

void MSSim_Dispatcher::registerTask( MSSim_Task* task )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::registerTask()" )
  MSSIM_TRACE( TraceSimDetail, " task: " << task )

  if( ! m_running )
  {
    setRunning( task ); 
  } else {
    setRunnable( task ); 
  }
}

void MSSim_Dispatcher::acquireWaitItem( MSSim_WaitItem* res, void* params ) 
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::acquireWaitItem()" )
  MSSIM_TRACE( TraceSimDetail, " res: " << res << " params: " << params )

  {
    if( ! res->isAvailable( params ) )
    {
      res->pushWaitingTask( m_running ); 
      suspend( m_running ); 
      if( m_running->getWaitStatus() == ws_cancelled )
        return; 
    } 
  }
  {
    res->doAcquire( params ); 
  }
}

bool MSSim_Dispatcher::tryAcquireWaitItem( MSSim_WaitItem* res, void* params )
{
  if( ! res->isAvailable( params ) )
  {
    acquireWaitItem( res, params ); 
    return true; 
  }
  return false; 
}

void MSSim_Dispatcher::releaseWaitItem( MSSim_WaitItem* res, void* params )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::releaseWaitItem()" )
  MSSIM_TRACE( TraceSimDetail, " res: " << res << " params: " << params )

  res->doRelease( params ); 
  getCurrentTask()->itemReleased( res ); 
  // resume next waiting task 
  if( res->isAvailable() )
  {
    MSSim_Task* wTask = res->popWaitingTask(); 
    if( wTask )
    {
      // set waiting task runnable
      setRunnable( wTask, true ); 
      dispatch( getCurrentTask() ); 
    }
  }
}


void MSSim_Dispatcher::cancelWaitItem( MSSim_WaitItem* res, MSSim_Task* task )
{
  if( res->removeFromWaitList( task ) )
  {
    // is waiting
    setRunnable( task ); 
    task->itemCancelled( res ); 
  } else {
    // has acquired
    res->doRelease(); 
    task->itemReleased( res ); 
  }
}

void MSSim_Dispatcher::reset() 
{
  // reset internal state
  m_dispatch_count = 0; 
//  m_running = 0; 
//  m_waiting.clear(); 
//  m_runnable.clear(); 
//  m_finished.clear(); 
}

void MSSim_Dispatcher::setWaiting( MSSim_Task* task )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::setWaiting()" )
  MSSIM_TRACE( TraceSimDetail, "  task: " << task )

  removeInternal( task ); 
  task->setStatus( ts_waiting ); 
  m_waiting.pushBack( task ); 
}

void MSSim_Dispatcher::setRunnable( MSSim_Task* task, bool front )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::setRunnable()" )
  MSSIM_TRACE( TraceSimDetail, "  task: " << task )

  removeInternal( task ); 
  task->setStatus( ts_runnable ); 
  if( front )
  {
    m_runnable.pushFront( task );
  } else 
  {
    m_runnable.pushBack( task ); 
  }

  MSSIM_TRACE( TraceSimFunctionCall, "  running: " << m_running )
}

void MSSim_Dispatcher::setRunning( MSSim_Task* task )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::setRunning()" )
  MSSIM_TRACE( TraceSimDetail, "  task: " << task )

  removeInternal( task ); 
  task->setStatus( ts_running ); 
  m_running = task; 
}

void MSSim_Dispatcher::setFinished( MSSim_Task* task )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::setFinished()" )
  MSSIM_TRACE( TraceSimDetail, "  task: " << task )

  removeInternal( task ); 
  task->setStatus( ts_finished ); 
  m_finished.pushBack( task ); 
}

void MSSim_Dispatcher::switchToTask( MSSim_Task* from, MSSim_Task* to )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Dispatcher::switchToTask()" )
  MSSIM_TRACE( TraceSimDetail, " from: " << from  )
  MSSIM_TRACE( TraceSimDetail, " to  : " << to )

  MSSim_SemReleaseSysApi( to->getSync(), 1 ); 
  if( from )
  {
    MSSim_SemWaitSysApi( from->getSync() ); 
  }
}

bool MSSim_Dispatcher::removeInternal( MSSim_Task* task )
{
  if( task )
  {
    switch( task->getStatus() )
    {
    case ts_running: 
      m_running = 0; 
      return true; 
      break; 
    case ts_runnable: 
      return m_runnable.remove( task ); 
      break; 
    case ts_waiting: 
      return m_waiting.remove( task );  
      break; 
    case ts_finished: 
      return m_finished.remove( task ); 
    }
  }
  return false; 
}

