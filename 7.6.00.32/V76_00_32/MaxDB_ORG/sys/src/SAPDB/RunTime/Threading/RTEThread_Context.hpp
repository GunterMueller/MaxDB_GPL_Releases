/*!========================================================================

  @file         RTEThread_Context.hpp
  @ingroup      Runtime
  @author       JoergM

  @brief        Common class used to access thread context

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
#ifndef RTETHREAD_CONTEXT_HPP
#define RTETHREAD_CONTEXT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Threading/RTEThread_IContext.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

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

class RTEThread_Context : public RTEThread_IContext
{
public:
  RTEThread_Context();
  ~RTEThread_Context();

  virtual RTETask_IContext * CurrentTaskContext();

  //! Thread Local Storage Interface
  /*!
     @brief create thread local storage key
     @param Key [out] key created
     @param Destructor [in] callback for destruction
     @return true if created, false else
    */
  static SAPDB_Bool CreateTLSKey( RTE_TLSKey &Key, void (*Destructor)(void *) );
  /*!
     @brief delete thread local storage key
     @param Key [in] key to delete
     @return true if deleted, false else
    */
  static SAPDB_Bool DeleteTLSKey( RTE_TLSKey Key );
  /*!
     @brief get thread local storage pointer
     @param Key [in] key to use
     @return thread local storage pointer referenced by key
   */
  static void *     GetTLS(       RTE_TLSKey Key );
  /*!
     @brief set thread local storage pointer
     @param Key [in] key to use
     @param Value [in] new value to set
     @return true if value set, false if key invalid
   */
  static SAPDB_Bool SetTLS(       RTE_TLSKey Key, void *Value );

  /*!
     @brief    Get copy of last error message list


  The internal stored message list is only meaningfull, if retrieved after an error
  was reported. You will always retrieve the complete last error message list, including all
  appended messages.
     @return Last Error Message list set by any error reporting call via SetLastErrorMessage()

   */

  virtual const SAPDBErr_MessageList & GetLastErrorMessage();

  /*!
     @brief    Get copy of last error message list


  The internal stored message list is only meaningfull, if retrieved after an error
  was reported. You will always retrieve the last error occured!
     @param          LastErrorMessage [in] Message to set or append
     @param          DoAppend [in] Flag to indicate replacement or appending of LastErrorMessage

   */

  virtual void SetLastErrorMessage(const SAPDBErr_MessageList &LastErrorMessage, SAPDB_Bool DoAppend);

private:
  RTETask_IContext * m_CurrentTaskContext;
  SAPDBErr_MessageList m_LastErrorMessage;
};

#endif  /* RTETHREAD_CONTEXT_HPP */
