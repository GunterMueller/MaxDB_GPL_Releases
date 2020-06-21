/*!========================================================================

  @file         RTEUDE_ServerControl.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        User Defined Externsion (UDE) Server handling

                Class implements the kernel UDE server handling
                like starting, stoping, create a connection
                to the server and so on.

  @since        2003-11-06  17:00
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


#ifndef RTEUDE_SERVERCONTROL_HPP
#define RTEUDE_SERVERCONTROL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


class SAPDBErr_MessageList;

class RTEUDE_ServerControl
{
public:
    /*!  @brief   Possible return values */
    enum ConnRet  { NoError = 0,
                    Error                // Common error occured
                  };

    /*!
      @brief     Starts the server executable

      @param     task [in]            - Reference to a task object.
      @param     pExecutable     [in] - contains the absolute path to
                                        the UDE server executable
      @param     messageList    [out] - Error messages

      @return    error return code
     */
    ConnRet    Start ( RTETask_ITask            &task,
                       SAPDB_Char * const        pExecutable,
                       SAPDBErr_MessageList     &messageList );


    /*!
      @brief  Waits for a UDE server connection

      @param     task [in]            - Reference to a task object.

      @return   Object used to handle the connection
   */
    RTETask_ITaskClientConnection * const  AcceptClientConnect(  RTETask_ITask &task  );


    /*!
       @brief          checks if the UDE server is still alive.

       @return         true, if alive
     */

    SAPDB_Bool isAlive () const;


    /// Constructor
    RTEUDE_ServerControl() : m_ClientStarted(false) {}

    /// Destructor
    ~RTEUDE_ServerControl();

private:
#   if defined(_WIN32)
     PROCESS_INFORMATION     m_ProcInfo;
#   else
     pid_t                   m_SonPID;
#   endif 
    SAPDB_Bool               m_ClientStarted;
};


#endif  /* RTEUDE_SERVERCONTROL_HPP */
