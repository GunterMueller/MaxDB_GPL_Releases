
#include "LVCSimulator/MultiSessionSimulator/MSSim_Task.hpp"

void MSSim_Task::cleanup() 
{
  {
    set< MSSim_WaitItem* >::iterator iter( m_acquired_items.begin() ); 
    for( ; iter != m_acquired_items.end(); ++iter )
    {
      (*iter)->cancel( this ); 
    }
    m_acquired_items.clear(); 
  }
  {
    set< MSSim_WaitItem* >::iterator iter( m_waiting_items.begin() ); 
    for( ; iter != m_waiting_items.end(); ++iter )
    {
      (*iter)->cancel( this ); 
    }
    m_waiting_items.clear(); 
  }
  cancelAll(); 
}

MSSim_Task::~MSSim_Task() 
{
  cleanup(); 
}
