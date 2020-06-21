/*!**************************************************************************

  module      : RTE_IGlobal.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE Global Singleton
  description : Class definining global RTE interface

                This interface defines functions that are needed on the global
                scope.

  last changed: 2000-07-26  15:00
  see also    : example.html ...
  first created:2000-03-14  11:03

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


#ifndef RTE_IGLOBAL_HPP
#define RTE_IGLOBAL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdlib.h> // size_t
#include "RunTime/RTE_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class RTE_ISystem;
class RTETask_IContext;
class RTEThread_IContext;
class SAPDBErr_MessageList;

/*!
  class:       RTE_IGlobal
  Description: TheRTE Singleton class

  This class contains all functions provided to all threads or tasks inside a process.

  The global definition 'TheRTE' is the Reference to the singleton instance.
  Instead of TheRTE 'RTE_IGlobal::Instance() could be used, but TheRTE sounds better...
  Using a global Variable instead of the Instance() function is deadly inside of static
  class constructors...

  The initialization of the RunTime environment must be done while not working in parallel running
  threads. It must be done before any other function of the RunTime is called. The
  RTE_IGlobal::InitializeInstance() is used to do this job. All runtime memory allocation instances
  are created. Therefore parameters are passed from RTE_IGlobal::InitializeInstance() that would
  otherwise not be needed. Inside this function all singletons necessary in the system, that need
  system dependend parameters are initialized. This function can be called as often as wanted, but
  only the first call will have an effect. There is no counterpart for 'Deinitialization' of the
  RunTime!

   */
class RTE_IGlobal
{
public:
  /*!
    Function: Initialize
    Description: Initialize TheRTE singleton instance

    Since this class has no user accessible constructor, this function 
    initializes the one and only instance of the class.
   */
  static void Initialize( );

  /*!
    Function: Instance
    Description: Return TheRTE singleton instance

    Since this class has no user accessible constructor, this function returns the pointer
    to the one and only instance of the class.

    Return value: The"_one_and_only_"RTE
   */
  static RTE_IGlobal & Instance();

  /*!
    Function: CurrentTaskContext
    Description: Get current task context pointer

    If this function is called within a task, the pointer to the task context is returned.

    Return value: Current Task Pointer or NULL if not called within a task
   */
  virtual RTETask_IContext   * CurrentTaskContext()=0;

  /*!
    Function: CurrentThreadContext
    Description: Get current thread context pointer

    If this function is called within a thread, the pointer to the thread context is returned.

    Return value: Current Thread Pointer or NULL if not called within multithreading RTE
   */
  virtual RTEThread_IContext * CurrentThreadContext()=0;

  /*!
    Function: MapTaskContext
    Description: Get current thread context pointer

    If this function is called with a valid TaskId, the pointer to the correspnding task context
    is returned.

    Arguments: TaskId [in] The unique TaskId to map

    Return value: Task Context pointer containing the TaskId or NULL if no matching TaskId found
   */
  virtual RTETask_IContext   * MapTaskContext(RTE_TaskId TaskId)=0;

  /*!
    Function: GetLastErrorMessage
    Description: Get copy of last error message list

    The internal stored message list is only meaningfull, if retrieved after an error
    was reported. You will always retrieve the complete last error message list, including all
    appended messages.
    
    Return value: Last Error Message list set by any error reporting call via SetLastErrorMessage()
   */
  virtual const SAPDBErr_MessageList & GetLastErrorMessage()=0;

  /*!
    Function: SetLastErrorMessage
    Description: Get copy of last error message list

    The internal stored message list is only meaningfull, if retrieved after an error
    was reported. You will always retrieve the last error occured!

    Arguments: LastErrorMessage [in] Message to set or append
               DoAppend [in] Flag to indicate replacement or appending of LastErrorMessage
   */
  virtual void SetLastErrorMessage(const SAPDBErr_MessageList &LastErrorMessage, SAPDB_Bool DoAppend)=0;

}; /* endclass: RTE_IGlobal */

/*!
  endclass:       RTE_IGlobal
*/

 // The global RTE class where it all begins....
#define TheRTE RTE_IGlobal::Instance()

#endif  /* RTE_IGLOBAL_HPP */
