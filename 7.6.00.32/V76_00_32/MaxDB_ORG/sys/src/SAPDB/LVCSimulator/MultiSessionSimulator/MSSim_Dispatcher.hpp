
#ifndef MSSIM_DISPATCHER_HPP
#define MSSIM_DISPATCHER_HPP

#include "LVCSimulator/MultiSessionSimulator/MSSim_List.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Task.hpp"

class MSSim_Coordinator; 
class MSSim_WaitItem; 
class MSSim_Task; 

class MSSim_Dispatcher
{
public: 
  MSSim_Dispatcher( MSSim_Coordinator& task_handler, int dispatchLimit ); 

  void tryDispatch()
  {
    if( m_enabled )
    {
      ++m_dispatch_count; 
      if( m_dispatch_count > m_dispatch_limit )
      {
        dispatch( getCurrentTask() ); 
      }
    }
  }

  void dispatch( MSSim_Task* ); 
  void setDispatchLimit( size_t limit ); 

  void registerTask( MSSim_Task* ); 
  void unregisterTask( MSSim_Task* ); 
  void finishTask( MSSim_Task* ); 
  bool isFinished( MSSim_Task* ); 

  bool isAlive( MSSim_Task* ); 
  MSSim_Task* getTask( TASKID taskId );

  void setEnabled( bool enabled )  { m_enabled = enabled; }
  bool isEnabled()                 { return m_enabled; }

  void reset(); 

  void acquireWaitItem( MSSim_WaitItem*, void* params ); 
  bool tryAcquireWaitItem( MSSim_WaitItem*, void* params ); 
  void releaseWaitItem( MSSim_WaitItem*, void* params ); 
  void cancelWaitItem( MSSim_WaitItem*, MSSim_Task* ); 

  MSSim_Task* getCurrentTask() { return m_running; }

private: 

  void resume( MSSim_Task* ); 
  void suspend( MSSim_Task* ); 

  void switchToTask( MSSim_Task* from, MSSim_Task* to ); 

  void setWaiting( MSSim_Task* ); 
  void setRunnable( MSSim_Task*, bool addToront = false ); 
  void setRunning( MSSim_Task* ); 
  void setFinished( MSSim_Task* ); 
  bool removeInternal( MSSim_Task* ); 

  MSSim_Coordinator& m_task_handler; 
  size_t m_dispatch_count; 
  size_t m_dispatch_limit; 
  bool m_enabled; 

  MSSim_Task* m_running; 
  MSSim_List< MSSim_Task > m_runnable; 
  MSSim_List< MSSim_Task > m_waiting; 
  MSSim_List< MSSim_Task > m_finished; 
};

/*
class MSSim_NoDispatchingScope
{
public: 
  MSSim_NoDispatchingScope( MSSim_Dispatcher* disp ) 
    : m_disp( disp )
  {
    if( m_disp->isEnabled() )
    {
      m_disp->setEnabled( false ); 
      m_changed = true; 
    }
  }

  ~MSSim_NoDispatchingScope() 
  {
    if( m_changed )
    {
      m_disp->setEnabled( true ); 
    }
  }

private: 
  MSSim_Dispatcher* m_disp; 
  bool m_changed; 
};
*/
#endif // #ifndef MSSIM_DISPATCHER_HPP