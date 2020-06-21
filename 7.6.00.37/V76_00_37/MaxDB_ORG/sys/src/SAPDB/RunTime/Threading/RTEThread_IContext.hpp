/*!========================================================================

  @file         RTEThread_IContext.hpp
  @ingroup      Runtime
  @author       JoergM

  @brief        Common interface class used to access thread context

  @since        2005-01-10  14:03
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

  \endif
 ============================================================================*/
#ifndef RTETHREAD_ICONTEXT_HPP
#define RTETHREAD_ICONTEXT_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

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

class RTETask_IContext;
class SAPDBErr_MessageList;

class RTEThread_IContext 
{
public:
  /*!
    @brief get current context of thread
   */
  virtual RTETask_IContext * CurrentTaskContext()=0;

  // Thread Local Storage Interface
  static SAPDB_Bool CreateTLSKey( RTE_TLSKey &Key, void (*Destructor)(void *) );
  static SAPDB_Bool DeleteTLSKey( RTE_TLSKey Key );
  static void *     GetTLS(       RTE_TLSKey Key );
  static SAPDB_Bool SetTLS(       RTE_TLSKey Key, void *Value );
  /*!
     @description    Get copy of last error message list


  The internal stored message list is only meaningfull, if retrieved after an error
  was reported. You will always retrieve the complete last error message list, including all
  appended messages.
     @return value   Last Error Message list set by any error reporting call via SetLastErrorMessage()

   */

  virtual const SAPDBErr_MessageList & GetLastErrorMessage() = 0;

  /*!
     @description    Get copy of last error message list


  The internal stored message list is only meaningfull, if retrieved after an error
  was reported. You will always retrieve the last error occured!
     @param          LastErrorMessage [in] Message to set or append
     @param          DoAppend [in] Flag to indicate replacement or appending of LastErrorMessage

   */

  virtual void SetLastErrorMessage(const SAPDBErr_MessageList &LastErrorMessage, SAPDB_Bool DoAppend) = 0;
};

#endif  /* RTETHREAD_ICONTEXT_HPP */
