/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateCommand.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  upgrade system tables
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
#include "DBM/Web/DBMWeb_TemplateCommand.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMCommand.htm"

#define OBJ_COMMAND            "Command"
#define OBJ_CMDSTR             "CmdStr"
#define OBJ_RESULT             "Result"

/*
  =============================================================================
  class:        DBMWeb_TemplateCommand
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateCommand :: DBMWeb_TemplateCommand
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateCommand :: DBMWeb_TemplateCommand ( sapdbwa_WebAgent & wa,
                                                   DBMWeb_TemplCommand_Mode nMode,
                                                   const DBMCli_String & sCommand,
                                                   const DBMCli_String & sResult)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_nMode( nMode ),
                          m_sCommand( sCommand ),
                          m_sResult( sResult )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateCommand :: ~DBMWeb_TemplateCommand
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateCommand :: ~DBMWeb_TemplateCommand (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateCommand :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateCommand :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_COMMAND) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_CMDSTR) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_RESULT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLCOMMAND_MODE_RESULT) ? 1 : 0;
  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateCommand :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateCommand :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  if (szName.Compare(OBJ_CMDSTR) == 0) {
    pReturn = m_sCommand;  
  } else if (szName.Compare(OBJ_RESULT) == 0) {
    pReturn = m_sResult;  
  }
  
  return _Tools_UTF8Ptr(pReturn);
} 

