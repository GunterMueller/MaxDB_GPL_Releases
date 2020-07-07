
#ifndef MSSIM_RESOURCE_HPP
#define MSSIM_RESOURCE_HPP

#include "LVCSimulator/MultiSessionSimulator/MSSim_List.hpp"

#include <deque>

class MSSim_Task; 
class MSSim_Dispatcher; 

using namespace std; 

typedef void* CSEC_ID; 

enum MSSim_WaitStatusEnum
{
  ws_ok = 0,
  ws_cancelled = 1
};

class MSSim_WaitItem
{
public: 
  friend class MSSim_Dispatcher; 
  
  MSSim_WaitItem( MSSim_Dispatcher* disp )
    : m_dispatcher( disp )
  { }

  virtual ~MSSim_WaitItem(); 

  bool removeFromWaitList( MSSim_Task* task ); 

  MSSim_Task* popWaitingTask(); 
  void pushWaitingTask( MSSim_Task* task ); 


  void release( void* = 0 ); 
  void acquire( void* = 0 ); 
  bool tryAcquire( void* = 0 ); 
  void cancel( MSSim_Task* ); 
  void cancelAll(); 

  virtual bool isAvailable( void* = 0 ) = 0; 
  virtual void doRelease( void* = 0 ) = 0; 
  virtual void doAcquire( void* = 0 ) = 0; 

private: 

  deque< MSSim_Task* > m_wait_list; 
//  MSSim_List< MSSim_Task > m_wait_list; 
  MSSim_Dispatcher* m_dispatcher; 
};

class MSSim_Semaphore : public MSSim_WaitItem
{
public: 

  MSSim_Semaphore( MSSim_Dispatcher* disp, int initial, int limit ) 
    : m_value( initial )
    , m_limit( limit )
    , MSSim_WaitItem( disp )
  { }

  void doAcquire( void* params )
  {
    --m_value; 
  }

  void doRelease( void* params ) 
  {
    m_value += (int)params; 
  }

  bool isAvailable( void* params )
  {
    return ( m_value > 0 ); 
  }

private: 
  int m_value; 
  int m_limit; 
};

class MSSim_Mutex : public MSSim_WaitItem
{
public: 

  MSSim_Mutex( MSSim_Dispatcher* disp ) 
    : m_owner( 0 )
    , MSSim_WaitItem( disp )
  { }

  void doAcquire( void* params )
  {
    //todo++ check if is available
    m_owner = (MSSim_Task*)params; 
  }

  void doRelease( void* params ) 
  {
    //todo++ check owner
    m_owner = 0; 
  }

  bool isAvailable( void* params )
  {
    return ( m_owner == 0 ); 
  }

  MSSim_Task* getOwner() 
  {
    return m_owner; 
  }

private: 
  MSSim_Task* m_owner; 
};

class MSSim_CriticalSection : public MSSim_WaitItem
{
public: 

  MSSim_CriticalSection( MSSim_Dispatcher* disp, CSEC_ID id = 0 ) 
    : m_owner( 0 )
    , MSSim_WaitItem( disp )
    , m_id( id )
  { }

  void doAcquire( void* params )
  {
    //todo++ check if is available
    m_owner = (MSSim_Task*)params; 
  }

  void doRelease( void* params ) 
  {
    //todo++ check owner
    m_owner = 0; 
  }

  bool isAvailable( void* params )
  {
    return ( m_owner == 0 ); 
  }

  void setId( void* id )
  {
    m_id = id; 
  }

  void* getId() 
  {
    return m_id; 
  }

  MSSim_Task* getOwner() 
  {
    return m_owner; 
  }

private: 
  MSSim_Task* m_owner; 
  CSEC_ID m_id; 
};


#endif // #ifndef MSSIM_RESOURCE_HPP