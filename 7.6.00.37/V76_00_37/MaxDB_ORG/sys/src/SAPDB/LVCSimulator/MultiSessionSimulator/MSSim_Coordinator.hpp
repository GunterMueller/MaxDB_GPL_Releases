
#ifndef MSSIM_COORDINATOR_HPP
#define MSSIM_COORDINATOR_HPP

#include "LVCSimulator/MultiSessionSimulator/MSSim_Dispatcher.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Resource.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Task.hpp"

#include <windows.h>

#include < set >
#include < map >

typedef int (*MSSim_ThreadMainFct)(void*); 
typedef unsigned long (*MSSim_TaskMainFct)(void* task_args ); 

class MSSim_Coordinator
{
public: 
  friend class MSSim_Dispatcher; 

  TASKID taskCreate( MSSim_ThreadMainFct start_func, void* arg, bool suspended ); 

  void taskYield(); 
  void taskJoin( TASKID ); 
  void taskCancel( TASKID ); 
  void taskKill( TASKID ); 
  bool taskIsAlive( TASKID ); 
  void taskSleep( long duration ); 
  void taskWaitFor( void* ); 
  void taskExit( int exitCode ); 
  void taskSuspend( TASKID ); 
  void taskResume( TASKID ); 

  void rteReset(); 
  void rteSuspend();
  void rteResume(); 

  void* semCreate( int initial, int limit ); 
  bool  semClose( void* sem ); 
  bool  semRelease( void* sem, int count ); 

  void* mtxCreate( bool initialOwner ); 
  bool  mtxClose( void* mtx ); 
  bool  mtxRelease( void* mtx ); 

  void* critCreate(); 
  bool  critClose( void* sec ); 
  void  critEnter( void* sec ); 
  bool  critLeave( void* sec ); 

  void* closeHandle( void* ); 

  void* taskCurrent()
  {
    return m_dispatcher.getCurrentTask(); 
  }

  static MSSim_Coordinator* GetInstance() 
  {
    static initializing( false ); 
    if( initializing )
      return 0; 
    if( m_instance == 0 )
    {
      initializing = true; 
      m_instance = new MSSim_Coordinator(); 
      initializing = false; 
    }
    return m_instance; 
  }

  MSSim_Dispatcher& getDispatcher() { return m_dispatcher; }

  void taskEnd(); 

private: 

  // Singleton
  // access instance via GetInstance
  MSSim_Coordinator(); 

  void cleanupTask( MSSim_Task* ); 
  MSSim_WaitItem* getWaitItemFor( void* ); 

  MSSim_Task* createSimTask( TASKID nativeTask );
  bool closeSimTask( TASKID nativeTask ); 

  MSSim_Task* getSimTask( TASKID nativeTask )
  {
    map< TASKID, MSSim_Task* >::iterator iter( m_tasks.find( nativeTask ) ); 
    if( iter != m_tasks.end() )
    {
      return iter->second; 
    }
    return 0; 
  }

  MSSim_Dispatcher m_dispatcher; 
  set< MSSim_Semaphore* > m_semaphores; 
  set< MSSim_Mutex* > m_mutex; 
  map< CSEC_ID, MSSim_CriticalSection* > m_critical_sections; 
  map< TASKID, MSSim_Task* > m_tasks; 

  static MSSim_Coordinator* m_instance; 
};

#endif // #ifndef MSSIM_COORDINATOR_HPP

