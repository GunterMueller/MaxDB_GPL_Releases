/*!***************************************************************************

  module      : RTE_Message.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE
  description : Knldiag interface for saving SAPDBErr_MsgList content

  last changed: 2001-01-11  17:35
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef RTE_MESSAGE_HPP
#define RTE_MESSAGE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*
   Declaration:  RTE_MessageType
   Description:  Defines the different output types for the RTE Messages 
*/


#define     MSG_DIAG_EVENT_CONSOLE       6
#define     MSG_DIAG_CONSOLE             7
#define     MSG_DIAG                     8

class Msg_List;

/*!
  Function: RTE_Message
  Description: Write a message list to different output media (kerneldiag by default).
               

  Arguments: msgList [in] The message list to write
             msgMedia[in] Media types for the output 
                          MSG_DIAG_EVENT_CONSOLE :  Output to DIAG, CONSOLE and EVENT
                          MSG_DIAG_CONSOLE       :  Output to DIAG and CONSOLE
                          MSG_DIAG               :  Output to DIAG
 */
void RTE_Message( const Msg_List    &msgList,
                  SAPDB_UInt1 const  msgMedia = MSG_DIAG);

#endif  /* RTE_MESSAGE_HPP */
