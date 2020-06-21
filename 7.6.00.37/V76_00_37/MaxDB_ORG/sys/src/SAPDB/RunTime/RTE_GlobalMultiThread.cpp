/****************************************************************************

  module      : RTE_GlobalMultiThread.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE Global Singleton
  description : Class definining RTE_IGlobal interfaces in multi threaded environment

                This is for multithreaded environment, that do not support 
                internal tasking

  last changed: 2000-07-11  13:47
  first created:2000-05-26  18:36

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp" // required for placement new
#include "RunTime/RTE_IGlobal.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/Threading/RTEThread_IContext.hpp"
#include "RunTime/RTE_MessageList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  class:       RTE_GlobalMultiThread
  description: TheRTE Singleton class
  
  This class contains all functions provided to all threads or tasks inside a process.
  
  The global variable 'TheRTE' is a pointer to the singleton instance.
  Instead of TheRTE 'RTE_IGlobal::Instance() could be used, but this seemed to long...
 */
class RTE_GlobalMultiThread : public RTE_IGlobal
{
public:
  /*!
    function: CurrentTaskContext
    description: Get current task context pointer

    If this function is called within a task, the pointer to the task context is returned.

    return value: Current Task Pointer or NULL if not called within a task
   */
  virtual RTETask_IContext   * CurrentTaskContext() { return NULL; }

  /*!
    function: CurrentThreadContext
    description: Get current thread context pointer

    If this function is called within a thread, the pointer to the thread context is returned.
    
    return value: Current Thread Pointer or NULL if not called within multithreading RTE
   */
  virtual RTEThread_IContext * CurrentThreadContext();

  /*!
    function: MapTaskContext
    description: Get current thread context pointer

    If this function is called with a valid TaskId, the pointer to the correspnding task context
    is returned.

    arguments: TaskId [in] The unique TaskId to map

    return value: Task Context pointer containing the TaskId or NULL if no matching TaskId found
   */
  virtual RTETask_IContext   * MapTaskContext(RTE_TaskId TaskId) { return NULL; }

  /*!
    Function: GetLastErrorMessage
    Description: Get copy of last error message list

    The internal stored message list is only meaningfull, if retrieved after an error
    was reported. You will always retrieve the complete last error message list, including all
    appended messages.
    
    Return value: Last Error Message list set by any error reporting call via SetLastErrorMessage()
   */
  virtual const SAPDBErr_MessageList & GetLastErrorMessage();

  /*!
    Function: SetLastErrorMessage
    Description: Get copy of last error message list

    The internal stored message list is only meaningfull, if retrieved after an error
    was reported. You will always retrieve the last error occured!

    Arguments: LastErrorMessage [in] Message to set or append
               DoAppend [in] Flag to indicate replacement or appending of LastErrorMessage
   */
  virtual void SetLastErrorMessage(const SAPDBErr_MessageList & LastErrorMessage, SAPDB_Bool DoAppend);

private:
  RTE_GlobalMultiThread();        // This class is a singleton

  RTE_TLSKey           m_ThreadKey;   // The Key storing the TLS Key to find current thread
  RTEThread_IContext **m_ThreadArray; // Pointer to the thread context pointer array
  SAPDBErr_MessageList m_LastErrorMessage; // Last error message if no threads used
  static RTE_IGlobal  *m_Instance;    // Pointer to the RTE_Global instance
  friend class RTE_IGlobal;
}; /* endclass: RTE_GlobalMultiThread */

RTE_IGlobal * RTE_GlobalMultiThread::m_Instance = (RTE_IGlobal *)0;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

//---------------------------------------------------------------------------
RTE_GlobalMultiThread::RTE_GlobalMultiThread() :
  m_LastErrorMessage(RTE_CONTEXT, SAPDBErr_MessageList::Info, 0, "", 0)
{
}
//---------------------------------------------------------------------------
RTEThread_IContext * RTE_GlobalMultiThread::CurrentThreadContext()
{
  // ask thread local storage...
  return reinterpret_cast<RTEThread_IContext *>(
                          RTEThread_IContext::GetTLS(m_ThreadKey));
}
//---------------------------------------------------------------------------
// Find the right context for the message and then get it
const SAPDBErr_MessageList & RTE_GlobalMultiThread::GetLastErrorMessage()
{
  RTEThread_IContext * Thread = CurrentThreadContext();

  if ( !Thread )
  {
    return m_LastErrorMessage;
  }

  return Thread->GetLastErrorMessage();
}
//---------------------------------------------------------------------------
// Find the right context for the message and then set it
void RTE_GlobalMultiThread::SetLastErrorMessage(const SAPDBErr_MessageList & LastErrorMessage, SAPDB_Bool DoAppend)
{
  RTEThread_IContext * Thread = CurrentThreadContext();

  if ( !Thread )
  {
    if ( DoAppend )
    {
      m_LastErrorMessage.AppendNewMessage(LastErrorMessage);
    }
    else
    {
      m_LastErrorMessage = LastErrorMessage;
    }
  }

  Thread->SetLastErrorMessage(LastErrorMessage, DoAppend);
}
//
// This static member function is defined in RTE_IGlobal class and implemented here
// since it needs size of RTE_Global class which is not visible to the interface.
//
RTE_IGlobal & RTE_IGlobal::Instance()
{
  RTE_IGlobal::Initialize();

  SAPDBERR_ASSERT_STATE(RTE_GlobalMultiThread::m_Instance);

  return *RTE_GlobalMultiThread::m_Instance;
}
//
// This static member function is defined in RTE_IGlobal class and implemented here
// since it needs size of RTE_Global class which is not visible to the interface.
//
void RTE_IGlobal::Initialize( )
{
  SAPDBMEM_STATIC_RAW_ALLOCATE( Space, sizeof(RTE_GlobalMultiThread) );

  if ( !RTE_GlobalMultiThread::m_Instance )
  {
    RTE_GlobalMultiThread::m_Instance = new (Space) RTE_GlobalMultiThread();
  }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
