
#include "LVCSimulator/MultiSessionSimulator/MSSim_Coordinator.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_PatchedSysFunc.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Task.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Trace.hpp"

#define CALL_INITIAL_DISPATCH_LIMIT 50
#define CALL_SLEEP_DISPATCH_FACTOR 200

#ifdef WIN32
#pragma warning( disable : 4355 ) // this used in base member initialization list
#endif

MSSim_Coordinator* MSSim_Coordinator::m_instance; 

struct MSSim_TaskArgs
{
  MSSim_TaskArgs( MSSim_Coordinator* sim
                , MSSim_Task* creator
                , MSSim_ThreadMainFct start_func
                , void* args 
                )
                : m_simulator( sim )
                , m_creator( creator )
                , m_task( 0 )
                , m_func( start_func )
                , m_args( args )
  { }

  MSSim_Coordinator* m_simulator; 
  MSSim_Task* m_creator; 
  MSSim_ThreadMainFct m_func; 
  MSSim_Task* m_task; 
  void* m_args; 
};

unsigned long MSSim_TaskMain( void* task_args )
{
  // get params
  MSSim_TaskArgs* args = (MSSim_TaskArgs*)task_args; 
  // signal creator
  MSSim_SemReleaseSysApi( args->m_creator->getSync(), 1 ); 
  MSSim_SemWaitSysApi( args->m_task->getSync() ); 

  // activate dispatcher before starting 
  args->m_simulator->getDispatcher().setEnabled( true ); 

  // call function
  int retcode; 
  retcode = (args->m_func)(args->m_args); 

  MSSim_Coordinator* simul = args->m_simulator; 

  simul->taskEnd(); 
  return retcode; 
}

MSSim_Coordinator::MSSim_Coordinator() 
: m_dispatcher( *this, CALL_INITIAL_DISPATCH_LIMIT ) 
{
  createSimTask( MSSim_ThreadCurrentSysApi() ); 

  // after this, the dispatcher is called regularly
  MSSim_InitializeCallback( m_dispatcher ); 

  // activate dispatcher
  m_dispatcher.setEnabled( true ); 
}

TASKID MSSim_Coordinator::taskCreate( MSSim_ThreadMainFct start_func, void* arg, bool suspended )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::taskCreate()" )

    m_dispatcher.setEnabled( false ); 
  MSSim_Task* current = m_dispatcher.getCurrentTask(); 
  MSSim_TaskArgs* task_params = new MSSim_TaskArgs( this, current, start_func, arg ); 
  // create thread suspended
  TASKID new_t = MSSim_ThreadCreateSysApi( MSSim_TaskMain, task_params ); 
  if( new_t )
  {
    MSSim_Task* simTask = createSimTask( new_t ); 
    task_params->m_task = simTask; 
    MSSim_ThreadResumeSysApi( new_t ); 
    // Wait until new task is in main routine
    MSSim_SemWaitSysApi( current->getSync() ); 
  }
  m_dispatcher.setEnabled( true ); 
  return new_t; 
}

void MSSim_Coordinator::taskYield()
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::taskYield()" )

  m_dispatcher.dispatch( m_dispatcher.getCurrentTask() ); 
}

void MSSim_Coordinator::taskSuspend( TASKID nativeTask )
{
}

void MSSim_Coordinator::taskResume( TASKID nativeTask )
{
}

void MSSim_Coordinator::taskJoin( TASKID nativeTask )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::taskJoin()" )

  MSSim_Task* simTask = getSimTask( nativeTask ); 
  if( simTask )
  {
    simTask->join(); 
  }
}

// normal thread end
void MSSim_Coordinator::taskEnd()
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::taskEnd()" )

  // dispatch without suspending current thread
  m_dispatcher.finishTask( m_dispatcher.getCurrentTask() ); 
  m_dispatcher.dispatch( 0 ); 
}

void MSSim_Coordinator::taskExit( int exitCode )
{
  taskEnd(); 
}

void MSSim_Coordinator::cleanupTask( MSSim_Task* task )
{
  if( task )
  {
    closeSimTask( task->getId() ); 
  }
}

void MSSim_Coordinator::taskCancel( TASKID nativeTask )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::taskCancel" )

  closeSimTask( nativeTask ); 
}

void MSSim_Coordinator::taskKill( TASKID nativeTask )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::taskKill" )

  closeSimTask( nativeTask ); 
}

bool MSSim_Coordinator::taskIsAlive( TASKID nativeTask )
{
  MSSim_Task* task = getSimTask( nativeTask ); 
  if( task && task->getStatus() != ts_finished )
  {
    return true; 
  }
  return false; 
}

void* MSSim_Coordinator::closeHandle( void* anyHandle )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::closeHandle()" )

  { // is task?
    if( closeSimTask( anyHandle ) ) return anyHandle; 
  }
  { // is Semphore?
    if( semClose( anyHandle ) ) return 0; 
  }

  { // is Mutex?
    if( mtxClose( anyHandle ) ) return 0; 
  }

  // not a internal handle
  return anyHandle; 
}

void MSSim_Coordinator::taskSleep( long duration )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Coordinator::taskSleep()" )

  duration = duration * CALL_SLEEP_DISPATCH_FACTOR; 
  for( long i=0; i <= duration; ++i )
  {
    m_dispatcher.dispatch( m_dispatcher.getCurrentTask() ); 
  }
}

void MSSim_Coordinator::rteReset()
{
/*
  // deletes all internal state information
  {
    map< TASKID, MSSim_Task* >::iterator iter( m_tasks.begin() ); 
    for( ; iter != m_tasks.end(); ++iter )
    {
      MSSim_Task* t = iter->second; 
      delete t; 
    }
    m_tasks.clear(); 
  }
  {
    set< MSSim_Semaphore* >::iterator iter( m_semaphores.begin() ); 
    for( ; iter != m_semaphores.end(); ++iter )
    {
      delete (*iter);
    }
    m_semaphores.clear(); 
  }
  {
    set< MSSim_Mutex* >::iterator iter( m_mutex.begin() ); 
    for( ; iter != m_mutex.end(); ++iter )
    {
      delete (*iter); 
    }
    m_mutex.clear(); 
  }
*/
  m_dispatcher.reset(); 
}

void MSSim_Coordinator::rteSuspend()
{
  m_dispatcher.setEnabled( false ); 
}

void MSSim_Coordinator::rteResume()
{
  m_dispatcher.setEnabled( true ); 
}

void MSSim_Coordinator::taskWaitFor( void* wait_item )
{
  MSSim_WaitItem* wItem( getWaitItemFor( wait_item ) );
  if( wItem )
  {
    m_dispatcher.acquireWaitItem( wItem, wait_item ); 
  }
}

void* MSSim_Coordinator::semCreate( int initial, int limit )
{
  MSSim_Semaphore* sem = new MSSim_Semaphore( &m_dispatcher, initial, limit ); 
  m_semaphores.insert( sem ); 
  return sem; 
}

bool MSSim_Coordinator::semRelease( void* sem, int count )
{
  m_dispatcher.releaseWaitItem( (MSSim_WaitItem*)sem, (void*)count ); 
  return true; 
}

bool MSSim_Coordinator::semClose( void* sem )
{
  set< MSSim_Semaphore* >::iterator iter( m_semaphores.find( (MSSim_Semaphore*)sem ) ); 
  if( iter != m_semaphores.end() )
  {
    m_semaphores.erase( iter ); 
    delete sem; 
    return true; 
  }
  return false; 
}

void* MSSim_Coordinator::mtxCreate( bool initialOwner )
{
  MSSim_Mutex* mtx = new MSSim_Mutex( &m_dispatcher ); 
  if( initialOwner )
  {
    mtx->doAcquire( m_dispatcher.getCurrentTask() ); 
  }
  m_mutex.insert( mtx ); 
  return mtx; 
}

bool MSSim_Coordinator::mtxRelease( void* mtx  )
{
  m_dispatcher.releaseWaitItem( (MSSim_WaitItem*)mtx, m_dispatcher.getCurrentTask() ); 
  return true; 
}

bool MSSim_Coordinator::mtxClose( void* mtx )
{
  set< MSSim_Mutex* >::iterator iter( m_mutex.find( (MSSim_Mutex*)mtx ) ); 
  if( iter != m_mutex.end() )
  {
    m_mutex.erase( iter ); 
    delete mtx; 
    return true; 
  }
  return false; 
}

void* MSSim_Coordinator::critCreate()
{
/*
  MSSim_CriticalSection* sec = new MSSim_CriticalSection( &m_dispatcher ); 
  m_critical_sections.insert( sec, sec ); 
  return sec; 
*/
  return 0; 
}

bool  MSSim_Coordinator::critClose( void* sec )
{
/*
  set< MSSim_CriticalSection* >::iterator iter( m_critical_sections.find( (MSSim_CriticalSection*)sec ) ); 
  if( iter != m_critical_sections.end() )
  {
    m_critical_sections.erase( iter ); 
    delete sec; 
    return true; 
  }
*/
  return false; 
}

void  MSSim_Coordinator::critEnter( void* id )
{
  map< CSEC_ID, MSSim_CriticalSection* >::iterator iter( m_critical_sections.find( id ) ); 
  MSSim_CriticalSection* sec( 0 ); 
  if( iter == m_critical_sections.end() || iter->second == 0 )
  {
    // create critical section lazy
    sec = new MSSim_CriticalSection( &m_dispatcher, id ); 
    m_critical_sections[ id ] = sec;
  } else {
    sec = iter->second;
    // do nothing if current task has already entered
    if( sec->getOwner() == m_dispatcher.getCurrentTask() )
      return; 
  }
  m_dispatcher.acquireWaitItem( sec, m_dispatcher.getCurrentTask() ); 
}


bool  MSSim_Coordinator::critLeave( void* id )
{
  // should be available
  MSSim_CriticalSection* crit = m_critical_sections[ id ]; 
  if( crit )
  {
    m_dispatcher.releaseWaitItem( crit, m_dispatcher.getCurrentTask() ); 
  }
  m_critical_sections.erase( id ); 
//  m_critical_sections[ id ] = 0; 
  return true; 
}

MSSim_WaitItem* MSSim_Coordinator::getWaitItemFor( void* res ) 
{
  if( m_dispatcher.isAlive( (MSSim_Task*)res ) ) 
    return (MSSim_WaitItem*)res;  
  if( m_semaphores.find( (MSSim_Semaphore*)res ) != m_semaphores.end() ) 
    return (MSSim_WaitItem*)res; 
  if( m_mutex.find( (MSSim_Mutex*)res ) != m_mutex.end() ) 
    return (MSSim_WaitItem*)res; 
//  if( m_critical_sections.find( (MSSim_CriticalSection*)res ) != m_critical_sections.end() ) 
//    return (MSSim_WaitItem*)res; 

  // last try to interprete res as taskid
  return m_dispatcher.getTask( (TASKID)res ); 
}

MSSim_Task* MSSim_Coordinator::createSimTask( TASKID nativeTask )
{
  void* sem = MSSim_SemCreateSysApi( 0, 1 ); 
  MSSim_Task* new_task = new MSSim_Task( &m_dispatcher, nativeTask, sem ); 
  m_tasks[ nativeTask ] = new_task; 
  m_dispatcher.registerTask( new_task ); 
  return new_task; 
}

bool MSSim_Coordinator::closeSimTask( TASKID nativeTask )
{
  map< TASKID, MSSim_Task* >::iterator iter( m_tasks.find( nativeTask ) ); 
  if( iter != m_tasks.end() )
  {
    MSSim_Task* simTask = iter->second; 
    simTask->cleanup(); 
    MSSim_SemCloseSysApi( simTask->getSync() ); 
    m_dispatcher.unregisterTask( simTask ); 
    delete simTask; 
    m_tasks.erase( iter ); 
  }
  return false; 

}


//**************************************************************************

void MSSim_Activate()
{
  MSSim_Coordinator::GetInstance(); 
}

void MSSim_Suspend() 
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->rteSuspend(); 
}

void MSSim_Resume() 
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->rteResume(); 
}

void MSSim_Reset() 
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->rteReset(); 
}