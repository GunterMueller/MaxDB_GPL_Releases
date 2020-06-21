/*!***************************************************************************

  module      : SAPDBErr_MessageOutput.c

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RunTime
  description : C Message Output routine for UNIX and NT
  
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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"
#include <stdio.h>

/*==========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*
  Setup only during sqlinit() or kernel/control_server main
 */
static SAPDBErr_MessageOutputCallPointer messageOutputCall;

static void DefaultMessageOutput( SAPDB_Int4         msgOutputMask, /* ignored for default output to stdout... */
                                  SAPDB_Int4         msgNumber,
                                  SAPDB_Int4         msgType,
                                  SAPDB_Char const * msgLabel,
                                  SAPDB_Char const * msgFormat,
                                  va_list            msgArguments );

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

SAPDBErr_MessageOutputCallPointer SAPDBErr_SetMessageOutputCall( SAPDBErr_MessageOutputCallPointer newMessageOutputCall)
{
    SAPDBErr_MessageOutputCallPointer oldMessageOutputCall = messageOutputCall;
    messageOutputCall = newMessageOutputCall;
    return oldMessageOutputCall;
}

/*---------------------------------------------------------------------------*/

SAPDBErr_MessageOutputCallPointer SAPDBErr_GetMessageOutputCall( )
{
    return messageOutputCall;
}

/*---------------------------------------------------------------------------*/

void SAPDBErr_MessageOutput( SAPDB_Int4 msgOutputMask,
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
        DefaultMessageOutput( msgOutputMask, msgNumber, msgType, msgLabel, format, args );
    }
    va_end   ( args );
}

/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/

static void DefaultMessageOutput( SAPDB_Int4         msgOutputMask, /* ignored for default output to stdout... */
                                  SAPDB_Int4         msgNumber,
                                  SAPDB_Int4         msgType,
                                  SAPDB_Char const * msgLabel,
                                  SAPDB_Char const * msgFormat,
                                  va_list            msgArguments )
{
    /* msgType == 0 was former 'runtime internal error IER', but this differentiation is not wanted anymore */
    static char * msgTypeText[] = { "ERR", "ERR", "WNG", "   " } ;

    printf("%3s %-6d %-8s ", 
            msgTypeText[ msgType > 3 ? 0 : msgType < 0 ? 0 : msgType ],
            msgNumber,
            msgLabel );

    vprintf  ( msgFormat, msgArguments );

    printf("\n");
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
