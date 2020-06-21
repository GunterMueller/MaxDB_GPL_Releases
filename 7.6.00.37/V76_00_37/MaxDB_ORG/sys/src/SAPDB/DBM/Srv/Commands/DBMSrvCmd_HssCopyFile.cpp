/*!
  \file    DBMSrvCmd_HssCopyFile.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command hss_copyfile

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

#include "hcn42.h"

#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "SAPDB/DBM/Srv/Commands/DBMSrvCmd_HssCopyFile.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_HssCopyFile
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_FileGetFirst   "file_getfirst"
#define DBMSRV_CMD_FileGetNext    "file_getnext"

#define DBMSRV_PARAM_TYPE         "TYPE"

#define DBMSRV_VALUE_BINARY       "BINARY"

#define DBMSRV_CHAR_SPACE         " "
#define DBMSRV_CHAR_ASSIGN        "="

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmd_HssCopyFile::DBMSrvCmd_HssCopyFile
//-----------------------------------------------------------------------------
DBMSrvCmd_HssCopyFile::DBMSrvCmd_HssCopyFile()
    : DBMSrv_Command( DBMSrv_Command::KeyHssCopyFile, false, DBMSrvCmd_HssCopyFile::m_LongHelp ) 
{
} // end DBMSrvCmd_HssCopyFile::DBMSrvCmd_HssCopyFile

//-----------------------------------------------------------------------------
// static member variables
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_HssCopyFile    "hss_copyfile"
#define DBMSRV_PARAM_NODE         "node"
#define DBMSRV_PARAM_NAME         "name"

const char * const DBMSrvCmd_HssCopyFile::m_LongHelp=
"@command "DBMSRV_CMD_HssCopyFile" You copy a database file from a node of a hot standby cluster to the "
                                 "current database instance"
    "@preconditions You do need to log on to the Database Manager to execute this DBM command. "
                   "You have the DBM operator authorization InstallMgm."
                   "@syntax "DBMSRV_CMD_HssCopyFile" "DBMSRV_PARAM_NODE"=<"DBMSRV_PARAM_NODE"> "DBMSRV_PARAM_NAME"=<"DBMSRV_PARAM_NAME">"
    "@param  <"DBMSRV_PARAM_NODE">   You specify the node from which the file should be copied. "
    "@param  <"DBMSRV_PARAM_NAME">   You specify the file key for the file which should be copied. "
    "@reply OK<NL>";

const char* const DBMSrvCmd_HssCopyFile::m_Command   = DBMSRV_CMD_HssCopyFile;
const char* const DBMSrvCmd_HssCopyFile::m_ParamNode = DBMSRV_PARAM_NODE;
const char* const DBMSrvCmd_HssCopyFile::m_ParamName = DBMSRV_PARAM_NAME;

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmd_HssCopyFile::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_HssCopyFile::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmd_HssCopyFile oObject;

  return oObject.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmd_HssCopyFile::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_HssCopyFile::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  Tools_DynamicUTF8String sNode    = command->oArguments.getValue(DBMSRV_PARAM_NODE);
  Tools_DynamicUTF8String sFilekey = command->oArguments.getValue(DBMSRV_PARAM_NAME);

  DBMSrvMsg_Error oMsg;

  Reply.startWithOK();

  // check arguments
  if (sNode.Length() == 0) {
    nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
    Reply.appendLine("-1,missing '"DBMSRV_PARAM_NODE"' value");
  } else if (sFilekey.Length() == 0) {
    nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
    Reply.appendLine("-1,missing '"DBMSRV_PARAM_NAME"' value");
  } // end if

  // get filename
  tsp00_Pathc      szFileName;
  tsp00_Pathc      szBackupName;

  if (nFuncReturn == OK_CN00) {
    tsp00_Pathc      szType;

    szType.rawAssign(sFilekey.CharPtr());
    nFuncReturn = cn42GetFileName(vcontrol->dbname, szType,  szFileName, szBackupName);
    if (nFuncReturn != OK_CN00) {
      Reply.startWithError(nFuncReturn);
    } // end if
  } // end if


  // get the first block from the server
  Tools_DynamicUTF8String  sAnswer;
  Tools_DynamicUTF8String  sCommand;

  if (nFuncReturn == OK_CN00) {
    sCommand.Assign(DBMSRV_CMD_FileGetFirst);
    sCommand.Append(DBMSRV_CHAR_SPACE);
    sCommand.Append(sFilekey);
    sCommand.Append(DBMSRV_CHAR_SPACE);
    sCommand.Append(DBMSRV_PARAM_TYPE);
    sCommand.Append(DBMSRV_CHAR_ASSIGN);
    sCommand.Append(DBMSRV_VALUE_BINARY);

    if (!vcontrol->pHSSNodes->Execute(sNode, sCommand, sAnswer, oMsg)) {
      if ((((int) oMsg.ID()) < -100) && (sAnswer.Length() > 0)) {
        nFuncReturn = oMsg.ID();
        Reply.startWithString(sAnswer.CharPtr());
      } else {
        nFuncReturn = Reply.startWithMessageList(oMsg);
      } // end if
    } // end if
  } // end if

  // now we have the first answer and may open the destination
  FILE * pDestination = NULL;

  if (nFuncReturn == OK_CN00) {
    pDestination = fopen(szFileName, "wb");
    if ((pDestination == NULL) && (strlen(szBackupName) > 0)) {
      pDestination = fopen(szBackupName, "wb");
      if (pDestination != NULL) {
        szFileName = szBackupName;
      } // end if
    } // end if
    if (pDestination == NULL) {
      nFuncReturn = Reply.startWithErrno(ERR_FILE_CN00, errno, 0, szFileName);
    } // end if
  } // end if

  // file is open first answer is available -> save and loop now
  if (nFuncReturn == OK_CN00) {
    DBMCli_Result & oResult = vcontrol->pHSSNodes->GetNode(sNode, oMsg)->GetResult();

    Tools_DynamicUTF8String sHandle;
    Tools_DynamicUTF8String sContinue;
    Tools_DynamicUTF8String sSize;
    const char *            pBuffer;
    const char *            pNewline;
    bool                    bContinue = true;
    
    while (bContinue) {

      // answer maybe binary so we get the original buffer
      pBuffer = (const char *) vcontrol->pHSSNodes->GetNode(sNode, oMsg)->GetResult();
      const DBMSrv_Reply tempReply(pBuffer);

      // skip OK
      // copy handle
      tempReply.getStartAndEndOfLine(1, pBuffer, pNewline);

      sHandle.Assign((Tools_DynamicUTF8String::ConstPointer)pBuffer, (Tools_DynamicUTF8String::ElementCount) (pNewline - pBuffer));
      sHandle.Trim();

      // copy continue/end
      tempReply.getStartAndEndOfNextLine(pNewline, pBuffer, pNewline);

      sContinue.Assign((Tools_DynamicUTF8String::ConstPointer)pBuffer, (Tools_DynamicUTF8String::ElementCount) (pNewline - pBuffer));
      sContinue.Trim();

      // copy size
      tempReply.getStartAndEndOfNextLine(pNewline, pBuffer, pNewline);

      sSize.Assign((Tools_DynamicUTF8String::ConstPointer)pBuffer, (Tools_DynamicUTF8String::ElementCount) (pNewline - pBuffer));
      sSize.Trim();
      sSize     = sSize.SubStr(sSize.Find(' ') + 1).Trim();

      // skip size
      pBuffer  = tempReply.getStartOfNextLine(pNewline);

      // check continue flag
      bContinue = tempReply.startsWithOKAndContinueFlag();

      // write destination file
      if (fwrite(pBuffer, atol(sSize.CharPtr()), 1, pDestination) != 1) {
        nFuncReturn = Reply.startWithErrno(ERR_FILE_CN00, errno, 0, szFileName);
        bContinue = false;
      } // end if

      // build and execute next command
      if (bContinue) {
        sCommand.Assign(DBMSRV_CMD_FileGetNext);
        sCommand.Append(DBMSRV_CHAR_SPACE);
        sCommand.Append(sFilekey);
        sCommand.Append(DBMSRV_CHAR_SPACE);
        sCommand.Append(sHandle);

        if (!vcontrol->pHSSNodes->Execute(sNode, sCommand, sAnswer, oMsg)) {
          if ((((int) oMsg.ID()) < -100) && (sAnswer.Length() > 0)) {
            nFuncReturn = oMsg.ID();
            Reply.startWithString(sAnswer.CharPtr());
            bContinue = false;
          } else {
            nFuncReturn = Reply.startWithMessageList(oMsg);
            bContinue = false;
          } // end if
        } // end if
      } // end if

    } // end while

    // close destination file
    if ((fclose(pDestination) != 0) && (nFuncReturn == OK_CN00)) { 
      nFuncReturn = Reply.startWithErrno(ERR_FILE_CN00, errno, 0, szFileName);
    } // end if

  } // end if

  Reply.startWithOKIfRCIsOk(nFuncReturn);

  return nFuncReturn;
} // end DBMSrvCmd_HssCopyFile::run
