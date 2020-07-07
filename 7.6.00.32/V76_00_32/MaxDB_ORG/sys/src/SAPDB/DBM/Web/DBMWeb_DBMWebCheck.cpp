/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebCheck.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  "Check" functions of "the" DBMWeb Class
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Web/DBMWeb_TemplateVerify.hpp"
#include "DBM/Web/DBMWeb_TemplateFiles.hpp"
#include "DBM/Web/DBMWeb_TemplateFile.hpp"
#include "DBM/Web/DBMWeb_TemplateShows.hpp"
#include "DBM/Web/DBMWeb_TemplateShow.hpp"
#include "DBM/Web/DBMWeb_TemplateCommand.hpp"
#include "DBM/Web/DBMWeb_TemplateKernelTrace.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_FILE      "File"
#define PARAM_SHOW      "Show"
#define PARAM_COMMAND   "Command"
#define PARAM_OPTION    "Option"
#define PARAM_MODE      "Mode"
#define PARAM_TIMESTAMP "Timestamp"

#define PARAM_MODE_VAL_OPTIONS  "OPTIONS"
#define PARAM_MODE_VAL_PROTOCOL "PROTOCOL"
#define PARAM_MODE_VAL_FILES    "FILES"
#define PARAM_MODE_VAL_DIAG     "DIAG"
#define PARAM_MODE_VAL_PACK     "PACK"

#define PARAM_ACTION_VAL_ON     "ON"
#define PARAM_ACTION_VAL_OFF    "OFF"
#define PARAM_ACTION_VAL_ALLOFF "ALLOFF"
#define PARAM_ACTION_VAL_MAKE   "MAKE"
#define PARAM_ACTION_VAL_FLUSH  "FLUSH"
#define PARAM_ACTION_VAL_CLEAR  "CLEAR"
#define PARAM_ACTION_VAL_WAIT   "WAIT"


/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: checkDB
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: checkDB ( sapdbwa_WebAgent    & wa,
                                      sapdbwa_HttpRequest & request,
                                      sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    DBMWeb_TemplateVerify oTemplate( wa, DBMWEB_TEMPLVERIFY_MODE_START );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if ( sAction == PARAM_ACTION_VAL_WAIT ) {

    DBMWeb_TemplateVerify oTemplate( wa, DBMWEB_TEMPLVERIFY_MODE_WAIT );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if ( sAction == PARAM_ACTION_VAL_START ) {

    if( m_Database->Verify( oMsgList ) ) {

      DBMWeb_TemplateVerify oTemplate( wa, DBMWEB_TEMPLVERIFY_MODE_RESULT );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: checkFiles
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: checkFiles ( sapdbwa_WebAgent    & wa,
                                         sapdbwa_HttpRequest & request,
                                         sapdbwa_HttpReply   & reply )
{
  Msg_List oMsgList;

  DBMCli_String sAction = PARAM_ACTION_VAL_VIEW;
  DBMCli_String sMode   = PARAM_MODE_VAL_FILES;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_MODE,   request, sMode);

  if ( sAction == PARAM_ACTION_VAL_VIEW) {
    DBMCli_Files       & oFiles       = m_Database->GetFiles( );
    DBMCli_Diagnosises & oDiagnosises = m_Database->GetDiagnosises( );

    if (sMode == PARAM_MODE_VAL_FILES) {

      if( oFiles.Refresh( oMsgList ) ) {
        DBMWeb_TemplateFiles oTemplate( wa, DBMWeb_TemplateFiles::MODE_FILES, oFiles, oDiagnosises );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      } else {
        sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
      } // end if

    } else if (sMode == PARAM_MODE_VAL_DIAG) {

      DBMCli_String sTimestamp;
      GetParameterValue(PARAM_TIMESTAMP, request, sTimestamp);

      if( oDiagnosises.Refresh( oMsgList ) ) {
        if (sTimestamp.GetLength() > 0) {
          SAPDB_Int2 nIndex;
          for (nIndex = 0; nIndex < oDiagnosises.DiagnosisArray().GetSize(); ++nIndex) {
            if (oDiagnosises.DiagnosisArray( )[nIndex].GetTimestamp().Get(DBMCLI_DT_NUM_DATETIME_FMT) == sTimestamp) {
              break;
            } // end if
          } // end if
          if (nIndex < oDiagnosises.DiagnosisArray().GetSize()) {
            if ( oDiagnosises.DiagnosisArray( )[nIndex].Refresh(oMsgList) ) {
              DBMWeb_TemplateFiles oTemplate( wa, DBMWeb_TemplateFiles::MODE_DIAGLIST, oFiles, oDiagnosises, nIndex);
              oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
            } else {
              sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
            } // end if
          } else {
            DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (invalid request)!");
            oMsgBox.writePage(Tools_TemplateWriterWA(reply));
          } // end if

        } else {
          DBMWeb_TemplateFiles oTemplate( wa,  DBMWeb_TemplateFiles::MODE_DIAG, oFiles, oDiagnosises );
          oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
        } // end if

      } else {
        sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
      } // end if

    } else if (sMode == PARAM_MODE_VAL_PACK) {

      if( oDiagnosises.Refresh( oMsgList ) ) {
        DBMWeb_TemplateFiles oTemplate( wa,  DBMWeb_TemplateFiles::MODE_PACK, oFiles, oDiagnosises );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      } else {
        sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
      } // end if

    } else {
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (invalid request)!");
      oMsgBox.writePage(Tools_TemplateWriterWA(reply));
    } // end if

  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (invalid request)!");
    oMsgBox.writePage(Tools_TemplateWriterWA(reply));
  } // end if



  return SAPDB_TRUE;
}  // end DBMWeb_DBMWeb :: checkFiles

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: checkFile
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: checkFile ( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  DBMCli_String sName; 

  GetParameterValue(PARAM_FILE, request, sName);

  Msg_List oMsgList;
  DBMCli_File & oFile = m_Database->GetFile( );

  if( oFile.Open( sName, oMsgList ) ) {

    DBMWeb_TemplateFile oTemplate( wa, oFile );

    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: checkServer
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: checkServer ( sapdbwa_WebAgent    & wa,
                                          sapdbwa_HttpRequest & request,
                                          sapdbwa_HttpReply   & reply )
{
  Msg_List oMsgList;
  DBMCli_Shows & oShows = m_Database->GetShows( );

  if( oShows.Refresh( oMsgList ) ) {

    DBMWeb_TemplateShows oTemplate( wa, oShows );

    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: checkSrvShow
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: checkSrvShow ( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  DBMCli_String sName; 

  GetParameterValue(PARAM_SHOW, request, sName);

  Msg_List oMsgList;
  DBMCli_Show & oShow = m_Database->GetShow( );

  if( oShow.Open( sName, oMsgList ) ) {

    DBMWeb_TemplateShow oTemplate( wa, oShow );

    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: checkCommand
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: checkCommand ( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sCommand;

  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;

  if( sAction == PARAM_ACTION_VAL_VIEW ) {

      sCommand.Empty( );

      DBMWeb_TemplateCommand oTemplate( wa, DBMWEB_TEMPLCOMMAND_MODE_COMMAND, sCommand, "" );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if ( sAction == PARAM_ACTION_VAL_EXECUTE ) {

    GetParameterValue(PARAM_COMMAND, request, sCommand);

    if( m_Database->Command( sCommand, oMsgList ) ) {

      DBMWeb_TemplateCommand oTemplate( wa, DBMWEB_TEMPLCOMMAND_MODE_RESULT, sCommand, m_Database->GetResult( ) );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

    } else {

      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: checkKernelTrace
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: checkKernelTrace ( sapdbwa_WebAgent    & wa,
                                               sapdbwa_HttpRequest & request,
                                               sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool bOK = SAPDB_FALSE;
  DBMCli_String sAction;
  DBMCli_String sMode( PARAM_MODE_VAL_OPTIONS );
  DBMWeb_TemplKnlTrace_Mode nMode = DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_MODE, request, sMode);

  if( sMode == PARAM_MODE_VAL_OPTIONS ) {
    nMode = DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS;
  } else if( sMode == PARAM_MODE_VAL_PROTOCOL ) {
    nMode = DBMWEB_TEMPLKNLTRACE_MODE_PROTOCOL;
  } else {
    nMode = DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS;
  }

  Msg_List oMsgList;
  DBMCli_KernelTrace & oKernelTrace = m_Database->GetKernelTrace( );

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {
    bOK = oKernelTrace.Refresh( oMsgList );
  
  } else if( sAction == PARAM_ACTION_VAL_VIEW ) {
    bOK = SAPDB_TRUE;

  } else if( ( sAction == PARAM_ACTION_VAL_ON )  || 
             ( sAction == PARAM_ACTION_VAL_OFF )    ) {

    SAPDB_Int nItem = 0;
    SAPDB_Int nOption = 0;
    DBMCli_String sOption;

    // select checked options
    for (nItem = 0; nItem  < oKernelTrace.KernelTraceOptionArray().GetSize(); ++nItem ) {
      GetParameterValueByIndex(PARAM_OPTION, nOption, request, sOption);
      if (sOption == oKernelTrace.KernelTraceOptionArray()[nItem].Name1()) {
        oKernelTrace.KernelTraceOptionArray()[nItem].SetSelected(SAPDB_TRUE);
        ++nOption;
      } else {
        oKernelTrace.KernelTraceOptionArray()[nItem].SetSelected(SAPDB_FALSE);
      } 
      sOption.Empty( );
    } 

    if( sAction == PARAM_ACTION_VAL_ON ) {
      bOK = oKernelTrace.OptionsOn( oMsgList );
    } else if ( sAction == PARAM_ACTION_VAL_OFF ) {
      bOK = oKernelTrace.OptionsOff( oMsgList );
    }

  } else if ( sAction == PARAM_ACTION_VAL_ALLOFF ) {
    SAPDB_Int nItem = 0;

    // select all options
    for (nItem = 0; nItem  < oKernelTrace.KernelTraceOptionArray().GetSize(); ++nItem ) {
      oKernelTrace.KernelTraceOptionArray()[nItem].SetSelected(SAPDB_TRUE);
    } 

    bOK = oKernelTrace.OptionsOff( oMsgList );

  } else if ( sAction == PARAM_ACTION_VAL_MAKE ) {

    SAPDB_Int nItem = 0;
    SAPDB_Int nOption = 0;
    DBMCli_String sOption;

    // select checked protocol options
    for (nItem = 0; nItem  < oKernelTrace.KernelTraceProtOptArray().GetSize(); ++nItem ) {
      GetParameterValueByIndex(PARAM_OPTION, nOption, request, sOption);
      if (sOption == oKernelTrace.KernelTraceProtOptArray()[nItem].Option()) {
        oKernelTrace.KernelTraceProtOptArray()[nItem].SetSelected(SAPDB_TRUE);
        ++nOption;
      } else {
        oKernelTrace.KernelTraceProtOptArray()[nItem].SetSelected(SAPDB_FALSE);
      } 
      sOption.Empty( );
    } 

    bOK = oKernelTrace.Make( oMsgList );

  } else if ( sAction == PARAM_ACTION_VAL_FLUSH ) {
    bOK = oKernelTrace.Flush( oMsgList );
  } else if ( sAction == PARAM_ACTION_VAL_CLEAR ) {
    bOK = oKernelTrace.Clear( oMsgList );
  }

  if( bOK ) {
    DBMWeb_TemplateKernelTrace oTemplate( wa, oKernelTrace, nMode );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  } else {
    sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
  }

  return SAPDB_TRUE;
} 
