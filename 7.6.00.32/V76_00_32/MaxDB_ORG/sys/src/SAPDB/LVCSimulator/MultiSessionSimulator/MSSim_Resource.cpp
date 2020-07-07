
#include "LVCSimulator/MultiSessionSimulator/MSSim_Resource.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Dispatcher.hpp"

bool MSSim_WaitItem::removeFromWaitList( MSSim_Task* task )
{
  deque< MSSim_Task* >::iterator iter( m_wait_list.begin() ); 
  for( ; iter != m_wait_list.end(); ++iter )
  {
    if( *iter == task )
    {
      m_wait_list.erase( iter ); 
      return true; 
    }
  }
  return false; 
}

void MSSim_WaitItem::release( void* params ) 
{
  m_dispatcher->releaseWaitItem( this, params ); 
}

void MSSim_WaitItem::acquire( void* params ) 
{
  m_dispatcher->acquireWaitItem( this, params ); 
}

bool MSSim_WaitItem::tryAcquire( void* params ) 
{
  return m_dispatcher->tryAcquireWaitItem( this, params ); 
}

MSSim_Task* MSSim_WaitItem::popWaitingTask()
{
  if( m_wait_list.size() )
  {
    MSSim_Task* front = m_wait_list.front(); 
    m_wait_list.pop_front(); 
    return front; 
  }
  return 0; 
}

void MSSim_WaitItem::cancel( MSSim_Task* task ) 
{
  m_dispatcher->cancelWaitItem( this, task ); 
}

void MSSim_WaitItem::cancelAll() 
{
  while( m_wait_list.size() > 0 )
  {
    m_dispatcher->cancelWaitItem( this, m_wait_list.front() ); 
  }
}

void MSSim_WaitItem::pushWaitingTask( MSSim_Task* task )
{
  m_wait_list.push_back( task ); 
}

MSSim_WaitItem::~MSSim_WaitItem() 
{
  cancelAll(); 
}