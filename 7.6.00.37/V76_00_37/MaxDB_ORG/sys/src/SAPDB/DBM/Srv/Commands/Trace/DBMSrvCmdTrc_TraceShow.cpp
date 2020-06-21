/*!
  \file    DBMSrvCmdTrc_TraceShow.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command trace_show

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

#include "SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceShow.hpp"
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp"


//-----------------------------------------------------------------------------
// members of class DBMSrvCmdTrc_TraceShow
//-----------------------------------------------------------------------------
#define COL_NAME_L        RTECONF_MAXNAMELENGTH
#define COL_STATE_L       5
#define COL_LEVEL_L       6

#define COL_NAME_T        "Name"
#define COL_STATE_T       "State"
#define COL_LEVEL_T       "Level"
#define COL_DESC_T        "Description"

#define STR_ON            "ON"
#define STR_OFF           "OFF"
#define STR_NO            "NO"
#define STR_NULL          "0"
#define STR_NOLEVEL       "-"
#define STR_SEPARATOR     ","
#define OPTION_VERBOSE    "VERBOSE"

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdTrc_TraceShow::DBMSrvCmdTrc_TraceShow
//-----------------------------------------------------------------------------
DBMSrvCmdTrc_TraceShow::DBMSrvCmdTrc_TraceShow()
    : DBMSrv_Command( DBMSrv_Command::KeyTraceShow, false, DBMSrvCmdTrc_TraceShow::m_LongHelp ) 
{
} // end DBMSrvCmdTrc_TraceShow::DBMSrvCmdTrc_TraceShow

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdTrc_TraceShow::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmdTrc_TraceShow::m_LongHelp=
"@command trace_show You display which activities of the database instance are currently logged."
                    "All activities that can be logged are displayed under the column name Description." 
                    "The word ON appears in the State column for activities that are actually currently "
                    "being logged." 

    "@preconditions You have the server authorization DBInfoRead or DBFileRead."
    "@syntax trace_show [verbose]"
    "@param  verbose If you specify the option verbose, the system displays an extended description of the database trace."
    "@reply OK<NL>"
           "Name   State     Level            Description<NL>"
           "<name> (ON|OFF)  (-|<level>)      <shortdescription>[,<longdescription>]<NL>"
           "<name> (ON|OFF)  (-|<level>)      <shortdescription>[,<longdescription>]<NL>"
           "..."
           "@replyvalue <name>                 name activity that can be logged"
           "@replyvalue <level>                level of logging"
           "@replyvalue <shortdescription>     short description of activity"
           "@replyvalue <longdescription>      long description of activity";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdTrc_TraceShow::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceShow::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmdTrc_TraceShow oCommand;

  return oCommand.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmdTrc_TraceShow::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceShow::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  DBMSrvPar_Parameters * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  bool                   bInit       = false;

  bool                   bVerbose    = false;

  if (command->oArguments.getCount() == 1) {
    bVerbose = (command->oArguments.getValue(1).ToUpper().Compare(OPTION_VERBOSE) == 0);
  } // end if


  /* init param management for commandline use or tcontrol or rcontrol */
  if (pXParamData == NULL) {
    DBMSrvMsg_Error oParamError;
    pXParamData = DBMSrvPar_Parameters::GetInstance(vcontrol->dbname, true, oParamError);
    if (pXParamData == NULL) {
      nFuncReturn = Reply.startWithMessageList(oParamError);
    } else {
      bInit = true;
    } // end if
  } /* end if */

  if (nFuncReturn == OK_CN00) {
    Reply.startWithOK();

    // generate Header
    Reply.appendStringWithMinWidth(COL_NAME_T, COL_NAME_L);
    Reply.appendTabulator();
    Reply.appendStringWithMinWidth(COL_STATE_T, COL_STATE_L);
    Reply.appendTabulator();
    Reply.appendStringWithMinWidth(COL_LEVEL_T, COL_LEVEL_L);
    Reply.appendTabulator();
    Reply.appendLine(COL_DESC_T);

    DBMSrvPar_ClassIterator oIterator(pXParamData, DBMSrvPar_Parameter::XPClassValueTrace);

    while ((*oIterator) != NULL) {
      // generate record
      Reply.appendStringWithMinWidth((*oIterator)->szParameterName, COL_NAME_L);
      Reply.appendTabulator();

      if ((*oIterator)->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_String) {
        Reply.appendStringWithMinWidth(strcmp((*oIterator)->szActiveValue, STR_NO)==0?STR_OFF:STR_ON, COL_STATE_L);
        Reply.appendTabulator();
        Reply.appendStringWithMinWidth(STR_NOLEVEL, COL_LEVEL_L);
        Reply.appendTabulator();
      } else if ((*oIterator)->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Integer) {
        Reply.appendStringWithMinWidth(strcmp((*oIterator)->szActiveValue, STR_NULL)==0?STR_OFF:STR_ON, COL_STATE_L);
        Reply.appendTabulator();
        Reply.appendStringWithMinWidth((*oIterator)->szActiveValue, COL_LEVEL_L);
        Reply.appendTabulator();
      } else {
        Reply.appendStringWithMinWidth(STR_NOLEVEL, COL_STATE_L);
        Reply.appendTabulator();
        Reply.appendStringWithMinWidth(STR_NOLEVEL, COL_LEVEL_L);
        Reply.appendTabulator();
      } // end if
      if (bVerbose && ((*oIterator)->pHelp != NULL) && ((*oIterator)->pHelp->Line()!=NULL)) {
        Reply.appendLine((*oIterator)->pHelp->Line());
      } else {
        Reply.appendLine(strlen((*oIterator)->szProperties[XPPropClassDisplay])==0?(*oIterator)->szParameterName.asCharp() : (*oIterator)->szProperties[XPPropClassDisplay].asCharp());
      } // end if

      ++oIterator;
    } // end while
  } // end if

  if (bInit) {
    delete pXParamData;
  } // end if

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceShow::run
