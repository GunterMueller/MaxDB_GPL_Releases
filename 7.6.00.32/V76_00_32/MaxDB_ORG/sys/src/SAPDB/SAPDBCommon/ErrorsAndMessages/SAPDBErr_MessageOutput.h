/*!***************************************************************************

  module      : SAPDBErr_MessageOutput.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RunTime
  description : C file message output
  
  last changed: 2001-04-03  16:25
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

#ifndef SAPDBERR_MESSAGEOUTPUT_H
#define SAPDBERR_MESSAGEOUTPUT_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"

#include <stdarg.h>

/* individual msgOutputMask argument */
#define SAPDB_WRITE_TO_CONSOLE              0x01
#define SAPDB_WRITE_TO_DIAGFILE             0x02
#define SAPDB_WRITE_TO_EVENTLOG             0x04
#define SAPDB_WRITE_TO_APPL_DIAG            0x08
#define SAPDB_WRITE_TO_MSG_BOX              0x10
#define SAPDB_WRITE_TO_GUI_CONSOLE          0x20
#define SAPDB_WRITE_TO_ERR_DIAGFILE         0x40

/* --- Don't include 'SAPDB_WRITE_TO_MSG_BOX' */
#define SAPDB_WRITE_TO_ALL                SAPDB_WRITE_TO_CONSOLE      | \
                                          SAPDB_WRITE_TO_DIAGFILE     | \
                                          SAPDB_WRITE_TO_EVENTLOG     | \
                                          SAPDB_WRITE_TO_APPL_DIAG    | \
                                          SAPDB_WRITE_TO_GUI_CONSOLE  | \
                                          SAPDB_WRITE_TO_ERR_DIAGFILE

/* predefioned combinations used in privious MSG... macros found in geo60.h */
#define SAPDB_MSG_TO_ALL                  SAPDB_WRITE_TO_ALL
#define SAPDB_MSG_TO_CONSOLE              SAPDB_WRITE_TO_CONSOLE | SAPDB_WRITE_TO_GUI_CONSOLE
#define SAPDB_MSG_TO_DIAGFILE             SAPDB_WRITE_TO_DIAGFILE | SAPDB_WRITE_TO_APPL_DIAG
#define SAPDB_MSG_TO_MESSAGE_BOX          SAPDB_WRITE_TO_MSG_BOX
#define SAPDB_MSG_TO_EVENTLOG             SAPDB_WRITE_TO_EVENTLOG
#define SAPDB_MSG_TO_CONSOLE_AND_EVENTLOG SAPDB_MSG_TO_CONSOLE | SAPDB_MSG_TO_EVENTLOG
#define SAPDB_MSG_TO_CONSOLE_AND_DIAGFILE SAPDB_MSG_TO_CONSOLE | SAPDB_MSG_TO_DIAGFILE

/* msgType argument */
#define SAPDB_MESSAGE_TYPE_INTERNAL_ERROR 0
#define SAPDB_MESSAGE_TYPE_ERROR          1
#define SAPDB_MESSAGE_TYPE_WARNING        2
#define SAPDB_MESSAGE_TYPE_INFO           3

#if defined(__cplusplus)

extern "C" {

#endif

/*============================================================================*
 * TYPE DEFINITION  
 *============================================================================*/

/*!
  Decleration: SAPDBErr_MessageOutputFunctionPointer
  Description: Function pointer used as parameter for message output setup

  The functions RTE_SetMessageOutputCall and RTE_GetMessageOutputCall are used to
  setup the default message output functions used inside runtime functions in case
  of diagnostic messages.

  The function arguments are

  msgOutputMask (logical or of SAPDB_WRITE_TO... defines above)
  msgNumber     a message identication number
  msgType       (one of SAPDB_MESSAGE_TYPE... defines above)
  msgLabel      a (normally maximum 8 character long) component label
  msgFormat     a printf style message output format string
  msgArguments  a variable argument list
 */
typedef void (*SAPDBErr_MessageOutputCallPointer) ( SAPDB_Int4,         /* msgOutputMask */
                                                    SAPDB_Int4,         /* msgNumber */
                                                    SAPDB_Int4,         /* msgType */
                                                    SAPDB_Char const *, /* msgLabel */
                                                    SAPDB_Char const *, /* msgFormat */
                                                    va_list             /* msgArguments */ );
           
/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

/*!
    Function:    SAPDBErr_MessageOutput
    Description: Do a message output

    
    Arguments: msgOutputMask [in] 0 to specify default output logical ored combination of SAPDB_WRITE_...
               msgNumber     [in] a message identification number
               msgType       [in] one of SAPDB_MESSAGE_TYPE_... defines
               msgLabel      [in] a normally restricted to 8 character length component label
               msgFormat     [in] a printf style message output format string following by variable arguments
 */
void SAPDBErr_MessageOutput( SAPDB_Int4         msgOutputMask,
                             SAPDB_Int4         msgNumber, 
                             SAPDB_Int4         msgType, 
                             SAPDB_Char const * msgLabel, 
                             SAPDB_Char const * msgFormat, ... );

/*!
    Function:    SAPDBErr_SetMessageOutputCall
    Description: Set default message output function

    This call is normally used during initialization of runtime (sqlinit or kernel start).
    It allows to override the default output function used for those runtime functions,
    that have no other chance to inform about serious problems. The message output implementation 
    is calling the 'ownMessageOutput' in the following code

    Code:

    ...include <stdio.h>
    ...include <stdarg.h>
 
   static void DefaultMessageOutputCall( SAPDB_Int4         msgOutputMask, ---[ ignored for default output to stdout...
                                         SAPDB_Int4         msgNumber,
                                         SAPDB_Int4         msgType,
                                         SAPDB_Char const * msgLabel,
                                         SAPDB_Char const * msgFormat,
                                         va_list            msgArguments )
    {
        static char * msgTypeText[] = { "ERR", "ERR", "WNG", "   " } ;

        printf("%3s %-6d %-8s ", 
                msgTypeText[ msgType > 3 ? 0 : msgType < 0 ? 0 : msgType ],
                msgNumber,
                msgLabel );

        vprintf  ( msgFormat, msgArguments );

        printf("\n");
    }

    void SAPDBErr_MessageOutputCall( SAPDB_Int4 msgOutputMask,
                                     SAPDB_Int4 msgNumber, 
                                     SAPDB_Int4 msgType, 
                                     SAPDB_Char const * msgLabel, 
                                     SAPDB_Char const * format, ... )
    {
        va_list args ;

        va_start ( args, format );
        if ( messageOutputCall )
        {
            (*messageOutputCall) ( msgOutputMask, msgNumber, msgType, msgLabel, format, args );
        }
        else
        {
            DefaultMessageOutputCall( ( msgOutputMask, msgNumber, msgType, msgLabel, format, args );
        }
        va_end   ( args );
    }

    The 'own' message output function used to implement 'NT RunTime behaviour is acting according
    to the old implementation found in vos60.c and the MSG... macro definitions found in geo60.h

    void os60MessageOutputCall( SAPDB_Int4         msgOutputMask,
                                SAPDB_Int4         msgNumber, 
                                SAPDB_Int4         msgType, 
                                SAPDB_Char const * msgLabel, 
                                SAPDB_Char const * msgFormat,
                                va_list            msgArguments )
    {
        SAPDB_UInt4 outputRequestMask;

        outputRequestMask = msgOutputMask;

        switch ( outputRequestMask )
        {
            case SAPDB_MSG_TO_ALL:                 ----[ MSGALL = All possible outputs
                outputRequestMask &= ~(SAPDB_WRITE_TO_MSG_BOX); ---[ Suppress message box output
                switch ( msgType )
                {
                    case SAPDB_MESSAGE_TYPE_INFO:
                    case SAPDB_MESSAGE_TYPE_WARNING:
                        outputRequestMask &= ~(SAPDB_WRITE_TO_ERR_DIAGFILE);
                        break;
                    default:
                        break;
                }
                break;

            case SAPDB_MSG_TO_CONSOLE_AND_DIAGFILE: ----[ MSGCD = All possible consoles and diagnostic files
            case SAPDB_MSG_TO_CONSOLE:              ----[ MSGC  = All possible consoles
            case SAPDB_MSG_TO_DIAGFILE:             ----[ MSGD  = All possible diagnostic files
                switch( msgType )
                {
                    case SAPDB_MESSAGE_TYPE_ERROR:
                    case SAPDB_MESSAGE_TYPE_INTERNAL_ERROR:
                        outputRequestMask |= (SAPDB_WRITE_TO_ERR_DIAGFILE);
                        break;
                    default:
                        break;
                }
                break;

            case SAPDB_MSG_TO_MESSAGE_BOX:          ----[ MSGCD = Message to message box
            case SAPDB_MSG_TO_EVENTLOG:             ----[ MSGEL = Message to NT Event Log
            case SAPDB_MSG_TO_CONSOLE_AND_EVENTLOG: ----[ MSGCEL = Message to all possible consoles and NT Event Log
            default:
                break;
        }

        sql60_deliver_msg ( outputRequestMask, msgNumber, msgType, msgLabel, msgFormat, msgArguments );
    }

  Arguments:   newMessageOutput [in] Function to be used for message output
  Return value: previous function used for message output (0 if none defined)
 */
SAPDBErr_MessageOutputCallPointer SAPDBErr_SetMessageOutputCall ( SAPDBErr_MessageOutputCallPointer newMessageOutputCall );

/*!
    Function:    SAPDBErr_GetMessageOutputCall
    Description: Set default message output function
    
    Return value: current function used for message output (0 if none defined)
 */
SAPDBErr_MessageOutputCallPointer SAPDBErr_GetMessageOutputCall ( );

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#if defined(__cplusplus)

} /* extern "C" */

#endif

#endif /* SAPDBERR_MESSAGEOUTPUT_H */
