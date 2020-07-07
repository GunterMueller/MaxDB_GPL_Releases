/*!***************************************************************************

  module      : RTE_Message.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE
  description : Function to save Kernel Messagelist 

  last changed: 2001-01-11  17:38
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

#   FreeBSD portions added by Kai Mosebach, 
#   For more informations see : http://www.komadev.de/sapdb
#

*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "heo60_2.h"

#include "Messages/Msg_IOutput.hpp"

#if defined NMP || defined FREEBSD
extern "C" void *alloca(size_t);
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if COMPILEMODE_MSP00 >= SLOW_MSP00	
#define COMPLETE_INFO_LINE
#endif

// Message Text limit is used for several reasons.
// dbmgetf reads out knldiag on a line by line base and copies atmost
// 255 character per line. Each knldiag line begins with 44 characters of 
// header information. Two lines on an 80 character display 160 minus 44 character
// header result in 116 character. One character less to prevent automatic wrap
// around leads to 115.
// This value is only needed due to the line oriented output of knldiag.
#define MESSAGE_TEXT_LIMIT 115
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/
#ifndef NO_OLD_DIAG_FILES
static void ShowMessage(SAPDB_UInt1 const     msgMedia,
                        SAPDB_Int4            msgCount, 
                        tsp3_msg_type_Param   msgType, 
                        SAPDB_Int4            msgNo,
                        tsp00_C8            & msgLabel, 
                        const Msg_List      & msgObject);
#endif
/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void RTE_Message(const Msg_List &   msgList,
                 SAPDB_UInt1 const  msgMedia)
{
#ifdef NEW_DIAG_FILES
    Msg_IOutput &diagnosticMessages = Msg_IOutput::GetDiagnosticOutput();
    Msg_List errList;
    Msg_List list = msgList;
    diagnosticMessages.Output( list, errList);
#endif
#ifndef NO_OLD_DIAG_FILES
    if ( !msgList.IsEmpty() )
    {
        const Msg_List * pMsg;
        tsp3_msg_type_Param msgType;
        tsp00_C8 msgLabel;

        const_cast<Msg_List *>(&msgList)->SetOutputIdentification(1);

        switch( msgList.Type() )
        {
        case Msg_List::Error:   msgType = sp3m_error;
            break;
        case Msg_List::Warning: msgType = sp3m_warning;
            break;
        case Msg_List::Info:    msgType = sp3m_info;
            break;
        default:                msgType = sp3m_nil;
            break;
        }
        for ( pMsg = &msgList;
              pMsg != 0;
              pMsg = pMsg->NextMessage() )
        {
            msgLabel.c2p(pMsg->Component());
            ShowMessage(msgMedia, pMsg == &msgList ? 1 : 0, msgType, pMsg->ID(), msgLabel, *pMsg);
        }
    }
#endif
}

/*===========================================================================*
 *  STATIC/INLINE FUNCTION CODE                                              *
 *===========================================================================*/
#ifndef NO_OLD_DIAG_FILES

static void ShowLongline(const SAPDB_Char *line, 
                         SAPDB_Int4 len,
                         const SAPDB_Char *intro,
                         SAPDB_UInt1 const      msgMedia,
                         tsp3_msg_type_Param    msgType, 
                         SAPDB_Int4             msgNo,
                         tsp00_C8             & msgLabel)
{
    SAPDB_Int4 introLen = SAPDB_strlen(intro);
    SAPDB_Int4 maxLen;
    SAPDB_Char msgText[MESSAGE_TEXT_LIMIT+1];
    const SAPDB_Char *tmpName = line;
    SAPDB_Int4  tmpLen = (len == 0 ? SAPDB_strlen(line) : len);

    if ( introLen > 10 ) 
    {
        intro = "";
        introLen = 0;
    }

    maxLen = (sizeof(msgText) - 1) - introLen;

    while ( tmpLen > maxLen )
    {
        SAPDB_memset(msgText,' ',sizeof(msgText));
        if ( introLen > 0 )
        {
            SAPDB_memcpy(msgText, intro, introLen);
        }
        SAPDB_memcpy(&msgText[introLen], tmpName, maxLen);
        msgText[introLen+maxLen] = 0;

        eo60k_Message((tsp3_priority_Param)msgMedia, msgType, msgNo, msgLabel.asCharp(), msgText );
        tmpName  += maxLen;
        tmpLen   -= maxLen;
    }

    SAPDB_memset(msgText,' ',sizeof(msgText));
    if ( introLen > 0 )
    {
        SAPDB_memcpy(msgText, intro, introLen);
    }
    SAPDB_memcpy(&msgText[introLen], tmpName, tmpLen);
    msgText[introLen+tmpLen] = 0;
    eo60k_Message((tsp3_priority_Param)msgMedia, msgType, msgNo, msgLabel.asCharp(), msgText );
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Function: FindFilename
  Description: Return the filename without directories
  ---------------------------------------------------------------------------*/
static const SAPDB_Char *FindFilename(const SAPDB_Char *fileName)
{
    const SAPDB_Char *lastChar = fileName;
    const SAPDB_Char *lastEntry = fileName;

    while ( 0 != *lastChar )
    {
#if defined(_WIN32)
        if ( *lastChar == '\\' || *lastChar == '/' )
        {
            do
            {
                ++lastChar;
            } while ( *lastChar == '\\' || *lastChar == '/' );

            if ( *lastChar ) lastEntry = lastChar; 
        }
#else
        if ( *lastChar == '/' )
        {
            do
            {
                ++lastChar;
            } while ( *lastChar == '/' );

            if ( *lastChar ) lastEntry = lastChar; 
        }
#endif
        ++lastChar;
    }

    return lastEntry;
}

/*---------------------------------------------------------------------------
  Function: ShowFileLine
  Description: Show the message __FILE__ and __LINE__ parameter
  ---------------------------------------------------------------------------*/
static void ShowFileLine(SAPDB_UInt1 const      msgMedia,
                         tsp3_msg_type_Param    msgType, 
                         SAPDB_Int4             msgNo,
                         tsp00_C8             & msgLabel, 
                         const SAPDB_Char     * intro,
                         const Msg_List & msgObject)
{
    SAPDB_Char *tmpName;
    SAPDB_Int4 len;
    SAPDB_ToStringClass numberString(msgObject.LineNumber());

    const SAPDB_Char *fileName = FindFilename(msgObject.FileName());

    len = SAPDB_strlen(fileName) 
        + SAPDB_strlen(":") 
        + SAPDB_strlen((SAPDB_Char *)numberString);
    tmpName = (SAPDB_Char *)alloca( len+1 );

    SAPDB_strcpy(tmpName, fileName);
    SAPDB_strcat(tmpName, ":");
    SAPDB_strcat(tmpName, (SAPDB_Char *)numberString);

    ShowLongline(tmpName, len, intro, msgMedia, msgType, msgNo, msgLabel);
}

/*---------------------------------------------------------------------------
  Function: ShowMesgPara
  Description: Show the message parameter DateTime,Component,Type,ID separated by blank
  ---------------------------------------------------------------------------*/
static void ShowMesgPara(SAPDB_UInt1 const      msgMedia,
                         tsp3_msg_type_Param    msgType, 
                         SAPDB_Int4             msgNo,
                         tsp00_C8             & msgLabel, 
                         const SAPDB_Char     * intro,
                         const Msg_List & msgObject)
{
    SAPDB_Char *tmpName;
    SAPDB_Int4 len;
    SAPDB_ToStringClass idString(msgObject.ID());
    Msg_List::ShortTimeBuffer timeBuffer;
    len = SAPDB_strlen(msgObject.DateTime(timeBuffer)) + SAPDB_strlen(" ")
        + SAPDB_strlen(msgObject.Component()) + SAPDB_strlen(" ")
        + SAPDB_strlen(msgObject.TypeAsString()) + SAPDB_strlen(" ")
        + SAPDB_strlen((SAPDB_Char *)idString);
    tmpName = (SAPDB_Char *)alloca( len+1 );

    SAPDB_strcpy(tmpName, &timeBuffer[0]);
    SAPDB_strcat(tmpName, " ");
    SAPDB_strcat(tmpName, msgObject.Component());
    SAPDB_strcat(tmpName, " ");
    SAPDB_strcat(tmpName, msgObject.TypeAsString());
    SAPDB_strcat(tmpName, " ");
    SAPDB_strcat(tmpName, (SAPDB_Char *)idString);
    ShowLongline(tmpName, len, intro, msgMedia, msgType, msgNo, msgLabel);
}

/*---------------------------------------------------------------------------
  Function: ShowMesgText
  Description: Show the message text + optional system return code
  ---------------------------------------------------------------------------*/
static void ShowMesgText(SAPDB_UInt1 const      msgMedia,
                         tsp3_msg_type_Param    msgType, 
                         SAPDB_Int4             msgNo,
                         tsp00_C8             & msgLabel, 
                         const SAPDB_Char     * intro,
                         const Msg_List & msgObject)
{
    SAPDB_UInt4 neededSize = 0;
    SAPDB_UInt4 realSize = 0;
    SAPDB_Char *tmpMessage;
    (void)msgObject.MessageWithInsertedArguments( 0,
                                        0,
                                        neededSize,
                                        true );
    tmpMessage = (SAPDB_Char *)alloca( neededSize );
    (void)msgObject.MessageWithInsertedArguments( neededSize,
                                        tmpMessage,
                                        realSize,
                                        true );

    ShowLongline(tmpMessage, realSize-1, intro, msgMedia, msgType, msgNo, msgLabel);
}

/*---------------------------------------------------------------------------*/

static void ShowMessage(SAPDB_UInt1 const      msgMedia,
                        SAPDB_Int4             msgCount, 
                        tsp3_msg_type_Param    msgType, 
                        SAPDB_Int4             msgNo,
                        tsp00_C8             & msgLabel, 
                        const Msg_List       & msgObject)
{
    if ( msgCount == 1 )
    {
#ifdef COMPLETE_INFO_LINE
        ShowFileLine(msgMedia, msgType, msgNo, msgLabel, "", msgObject);
        ShowMesgPara(msgMedia, msgType, msgNo, msgLabel, "", msgObject);
#endif
        ShowMesgText(msgMedia, msgType, msgNo, msgLabel, "", msgObject);
    }
    else
    {
#ifdef COMPLETE_INFO_LINE
        ShowFileLine(msgMedia, msgType, msgNo, msgLabel, " + ", msgObject);
        ShowMesgPara(msgMedia, msgType, msgNo, msgLabel, " + ", msgObject);
#endif
        ShowMesgText(msgMedia, msgType, msgNo, msgLabel, " +   ", msgObject);
    }
}
#endif
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
