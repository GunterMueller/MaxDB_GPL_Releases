/*!
  \file    DBMSrvCmdTrc_TraceProtOpt.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command trace_protopt

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
*/

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceProtOpt.hpp"


//-----------------------------------------------------------------------------
// members of class DBMSrvCmdTrc_TraceProtOpt
//-----------------------------------------------------------------------------

#define TRACE_PROTOPT \
  {{"name"  , "option"                      },  \
   {"a"     , "Order Interface (AK)"        },  \
   {"b"     , "Record Interface (BD)"       },  \
   {"k"     , "Show Message Block (KB)"     },  \
   {"m"     , "Message Block"               },  \
   {"n"     , "net (distribution)"          },  \
   {"s"     , "Strategy"                    },  \
   {"t"     , "Time"                        },  \
   {"x"     , "Switch Output (Slow Kernel)" },  \
   {NULL , NULL                          }}

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdTrc_TraceProtOpt::DBMSrvCmdTrc_TraceProtOpt
//-----------------------------------------------------------------------------
DBMSrvCmdTrc_TraceProtOpt::DBMSrvCmdTrc_TraceProtOpt()
    : DBMSrv_Command( DBMSrv_Command::KeyTraceProtOpt, false, DBMSrvCmdTrc_TraceProtOpt::m_LongHelp ) 
{
} // end DBMSrvCmdTrc_TraceProtOpt::DBMSrvCmdTrc_TraceProtOpt

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdTrc_TraceProtOpt::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmdTrc_TraceProtOpt::m_LongHelp=
"@command trace_protopt Use this DBM command for the database trace function "
                       "only when told to do so by Support. The database trace "
                       "is not required for normal database operation.<NL><NL>"
                       "You display which command options are permitted for "
                       "creating the text version of the current database trace "
                       "to restrict the trace output of the internal database "
                       "components. You receive a list with the names and description "
                       "of these command options."
"@preconditions You have the server authorization DBInfoRead or DBFileRead."
"@syntax trace_protopt"
"@reply OK";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdTrc_TraceProtOpt::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceProtOpt::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmdTrc_TraceProtOpt oCommand;

  return oCommand.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmdTrc_TraceProtOpt::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceProtOpt::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error nFuncReturn = OK_CN00; 
  int         nIndex      = 0;
  struct  {
    const char * szOption;
    const char * szName;
  } sTraceProtOpt[] = TRACE_PROTOPT;

  // init OK Answer
  Reply.startWithOK();

  while (sTraceProtOpt[nIndex].szOption != NULL) {
    Reply.appendString(sTraceProtOpt[nIndex].szOption);
    Reply.appendTabulator();
    Reply.appendLine(sTraceProtOpt[nIndex].szName);

    nIndex++;
  } // end while

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceProtOpt::run
