/*!
  \file    DBMSrvCmdTrc_TraceOn.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command trace_on

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
#include <stdlib.h>

#include "SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOn.hpp"
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdTrc_TraceOn
//-----------------------------------------------------------------------------
#define MAX_TOKENLEN       100
#define PREFIX_TRACE       "TRACE_"
#define PREFIX_CHECK       "CHECK_"
#define LEVEL_OFF          "0"
#define LEVEL_DEFAULT      "5"
#define VALUE_OFF          "NO"
#define VALUE_ON           "YES"

const char * const DBMSrvCmdTrc_TraceOn::OPTION_ALL = "ALL";

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdTrc_TraceOn::DBMSrvCmdTrc_TraceOn
//-----------------------------------------------------------------------------
DBMSrvCmdTrc_TraceOn::DBMSrvCmdTrc_TraceOn()
    : DBMSrv_Command( DBMSrv_Command::KeyTraceOn, false, DBMSrvCmdTrc_TraceOn::m_LongHelp ) 
{
} // end DBMSrvCmdTrc_TraceOn::DBMSrvCmdTrc_TraceOn

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdTrc_TraceOn::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmdTrc_TraceOn::m_LongHelp=
"@command trace_on You activate the database trace with this command. In this "
                  "way, selected database kernel activities are logged. "
                  "You specify which activities are to be logged using the "
                  "option activity_list. Separate multiple activities by spaces."
                  "The special option ALL activates the logging of all activities."
                  "Some activities allow you to set a logging level between 1 and 9. "
                  "This is optional. The Database Manager CLI default is Level 5."
"@preconditions You have the server authorization DBInfoRead or DBFileRead."
"@syntax trace_on <activity_list> | ALL [<level>]"
"@param  <activity_list> List of activities which are to be logged."
"@reply OK";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdTrc_TraceOn::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceOn::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply Reply(replyData, replyLen, replyLenMax);
  DBMSrvCmdTrc_TraceOn oCommand;

  return oCommand.run(vcontrol, command, Reply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmdTrc_TraceOn::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceOn::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  Reply.startWithOK();
  
  if (command->oArguments.getValue(1).ToUpper().Compare(DBMSrvCmdTrc_TraceOn::OPTION_ALL) == 0) {
    int nLevel = atoi(LEVEL_DEFAULT);

    if (command->oArguments.getCount() == 2) {
      Tools_DynamicUTF8String szToken;
      szToken = command->oArguments.getValue(2);
      if ( (szToken.Length() == 1  ) &&
           (szToken.At(0)    >= '1') &&
           (szToken.At(0)    <= '9')    ) {
          nLevel = atoi(szToken.CharPtr());
      } else {
        nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
      } // end if
    } else if (command->oArguments.getCount() > 2) {
      nFuncReturn = Reply.startWithError(ERR_TOOMANYPARAM_CN00);
    } // end if

    if (nFuncReturn == OK_CN00) {
      nFuncReturn = DBMSrvCmdTrc_TraceOn::SwitchAll(vcontrol, nLevel, true, Reply);
    } // end if
  } else {
    nFuncReturn = DBMSrvCmdTrc_TraceOn::Switch(vcontrol, command->args, true, Reply);
  } // end if

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceOn::run

/*
  -----------------------------------------------------------------------------
  function:     DBMSrvCmdTrc_TraceOn::SwitchAll
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvCmdTrc_TraceOn::SwitchAll
      ( VControlDataT * vcontrol,
        int             nLevel,
        bool            bOn,
        DBMSrv_Reply  & Reply )
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  DBMSrvPar_Parameters  * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  bool                    bInit       = false;
  Tools_DynamicUTF8String szTokens;
  DBMSrvMsg_Error         oParamError;

  // init param management 
  if (pXParamData == NULL) {
    pXParamData = DBMSrvPar_Parameters::GetInstance(vcontrol->dbname, true, oParamError);
    if (pXParamData == NULL) {
      nFuncReturn = Reply.startWithMessageList(oParamError);
    } else {
      bInit = true;
    } // end if
  } /* end if */

  // iterate trough trace parameters
  if (nFuncReturn == OK_CN00) {
    Tools_DynamicUTF8String szLevel;
    szLevel.ConvertFromInt(nLevel);

    DBMSrvPar_ClassIterator oIterator(pXParamData, DBMSrvPar_Parameter::XPClassValueTrace);
    while ((*oIterator) != NULL) {
      szTokens.Append((*oIterator)->szParameterName.asCharp());
      szTokens.Append(" ");
      if (bOn && ((*oIterator)->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Integer)) {
        szTokens.Append(szLevel);
        szTokens.Append(" ");
      } // end if
      ++oIterator;
    } // end while
  } // end if

  if (bInit) {
    delete pXParamData;
  } // end if

  if (nFuncReturn == OK_CN00) {
    szTokens.Trim();
    nFuncReturn = Switch(vcontrol, szTokens.CharPtr(), bOn, Reply);
  } // end if

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceOn::SwitchAll

/*
  -----------------------------------------------------------------------------
  function:     DBMSrvCmdTrc_TraceOn::Switch
  -----------------------------------------------------------------------------
 */
tcn00_Error DBMSrvCmdTrc_TraceOn::Switch
      ( VControlDataT * vcontrol,
        const char    * szTokens,
        bool            bOn,
        DBMSrv_Reply  & Reply )
{
  tcn00_Error             nFuncReturn  = OK_CN00;
  Tools_Arguments         oArguments;
  DBMSrvPar_Parameters  * pXParamData = (DBMSrvPar_Parameters *) vcontrol->pXParamSection;
  bool                    bInit       = false;
  DBMSrvMsg_Error         oParamError;

  oArguments.setArguments(szTokens);

  // init param management 
  if (pXParamData == NULL) {
    pXParamData = DBMSrvPar_Parameters::GetInstance(vcontrol->dbname, true, oParamError);
    if (pXParamData == NULL) {
      nFuncReturn = Reply.startWithMessageList(oParamError);
    } else {
      bInit = true;
    } // end if
  } /* end if */

  // iterate through tokens
  if (nFuncReturn == OK_CN00) {
    int                     nMaxTokens = oArguments.getCount();
    int                     nToken = 1;
    Tools_DynamicUTF8String szToken;
    Tools_DynamicUTF8String szValue;
    DBMSrvPar_Parameter   * pParameter = NULL;

    while ((nFuncReturn == OK_CN00) && (nToken <= nMaxTokens)) {
      // extrakt token
      szToken = oArguments.getValue(nToken);

      // find parameter
      pParameter = pXParamData->Get(szToken.ToUpper().CharPtr());
      if (pParameter == NULL) {
        // compatibilty fallback
        if ( (szToken.Compare(_Tools_UTF8Ptr(PREFIX_TRACE), (Tools_DynamicUTF8String::ElementCount) strlen(PREFIX_TRACE)) != 0) &&
             (szToken.Compare(_Tools_UTF8Ptr(PREFIX_CHECK), (Tools_DynamicUTF8String::ElementCount) strlen(PREFIX_CHECK)) != 0)    ) {
          szToken.Insert(0, PREFIX_TRACE);
          pParameter = pXParamData->Get(szToken.CharPtr());
        } // end if
      } // end if
      if (pParameter == NULL) {
        nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
      } else {
        // generate value
        if (pParameter->xpRecord.xpType == DBMSrvPar_RTEConfParameter::t_Integer) {
          szValue = bOn ? LEVEL_DEFAULT : LEVEL_OFF;
          if (bOn) {
            szToken = oArguments.getValue(nToken+1);
            if ( (szToken.Length() == 1  ) &&
                 (szToken.At(0)    >= '1') &&
                 (szToken.At(0)    <= '9')    ) {
              szValue = szToken;
              ++nToken;
            } // end if
          } // end if
        } else {
          szValue = bOn ? VALUE_ON : VALUE_OFF;
        } // end if

        // set value
        if (!pXParamData->SetValueWithCheck(pParameter->szParameterName.asCharp(), 
                                            szValue.CharPtr(),
                                            true,
                                            DBMSrvPar_Parameter::XPWriteModeAll,
                                            oParamError)) {
          nFuncReturn = Reply.startWithMessageList(oParamError);
        } // end if

      } // end if

      ++nToken;
    } // end while
  } // end if

  // check parameters
  if (nFuncReturn == OK_CN00) {
    if (!pXParamData->CheckAll(oParamError)) {
      nFuncReturn = Reply.startWithMessageList(oParamError);
    } // end if
  } // end if

  // write parameters
  if (nFuncReturn == OK_CN00) {
    if (!pXParamData->Commit(oParamError)) {
      nFuncReturn = Reply.startWithMessageList(oParamError);
    } // end if
  } // end if

  if (bInit) {
    delete pXParamData;
  } // end if

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceOn::Switch

