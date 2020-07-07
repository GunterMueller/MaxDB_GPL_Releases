#ifndef MSSIM_TASK_HPP
#define MSSIM_TASK_HPP

#include "LVCSimulator/MultiSessionSimulator/MSSim_Resource.hpp"

#include <set>

using namespace std; 

typedef void* TASKID; 
typedef void* SYNCID; 

enum MSSim_TaskStatusEnum {
  ts_created = 0,
  ts_running = 1,
  ts_runnable = 2, 
  ts_suspended = 3,
  ts_waiting = 4,
  ts_finished = 5, 
};

class MSSim_Task : public MSSim_WaitItem
{
public: 

  MSSim_Task( MSSim_Dispatcher* disp, TASKID id, SYNCID sync )
    : m_task_id( id )
    , m_sync( sync )
    , m_status( ts_created )
    , m_wait_status( ws_ok )
    , m_next( 0 )
    , MSSim_WaitItem( disp )
  {
  }

  TASKID getId()     { return m_task_id; }
  SYNCID getSync()   { return m_sync; }

  void itemWaiting( MSSim_WaitItem* res )
  {
    m_waiting_items.insert( res ); 
    m_wait_status = ws_ok; 
  }

  void itemAcquired( MSSim_WaitItem* res )
  {
    m_waiting_items.erase( res ); 
    m_acquired_items.insert( res ); 
  }

  void itemReleased( MSSim_WaitItem* res )
  {
    m_acquired_items.erase( res ); 
  }

  void itemCancelled( MSSim_WaitItem* res )
  {
    m_waiting_items.erase( res ); 
    m_wait_status = ws_cancelled; 
  }

  void setStatus( MSSim_TaskStatusEnum status ) { m_status = status; }
  MSSim_TaskStatusEnum getStatus()              { return m_status; }
  MSSim_WaitStatusEnum getWaitStatus()          { return m_wait_status; }

  void join() 
  {
    // wait for this task to finish
    acquire(); 
  }

  // acquire for thread means waiting until thread finishes (join)
  // no action has to be taken on acquire or release, because the 
  // task is finished at that point!
  void doAcquire( void* params ) { }
  void doRelease( void* params ) { } 

  bool isAvailable( void* params ) 
  {
    return false; 
  }

  void cleanup(); 
  ~MSSim_Task(); 

  MSSim_Task* m_next; 

private: 
  TASKID m_task_id; 
  SYNCID m_sync; 
  MSSim_TaskStatusEnum m_status; 
  MSSim_WaitStatusEnum m_wait_status; 
  set< MSSim_WaitItem* > m_waiting_items;   // items this task is waiting for
  set< MSSim_WaitItem* > m_acquired_items;  // items this task has already acquired
};

#endif // #ifndef MSSIM_TASK_HPP