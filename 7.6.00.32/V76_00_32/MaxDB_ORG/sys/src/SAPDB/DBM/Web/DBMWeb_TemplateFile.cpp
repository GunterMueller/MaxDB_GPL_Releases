/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateFile.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of files
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
#include "DBM/Web/DBMWeb_TemplateFile.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMFile.htm"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_DATA               "Data"

/*
  =============================================================================
  class:        DBMWeb_TemplateFile
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateFile :: DBMWeb_TemplateFile
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateFile :: DBMWeb_TemplateFile ( sapdbwa_WebAgent & wa,
                                             DBMCli_File & oFile )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oFile( oFile )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateFile :: ~DBMWeb_TemplateFile
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateFile :: ~DBMWeb_TemplateFile (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFile :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateFile :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {

    nReturn = 1;
  
  } else if (szName.Compare(OBJ_DATA) == 0) {

    nReturn = -1;
  
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFile :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateFile :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  
  if (szName.Compare(OBJ_TITLE) == 0) {

    pReturn = m_oFile.Name();  

  } else if (szName.Compare(OBJ_DATA) == 0) {

    pReturn = m_sLine;
  
  }

  //return ASCIIToUTF8(pReturn);
  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFile :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateFile :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  Msg_List oMsgList;

  if( szName.Compare(OBJ_DATA ) == 0 ) {
    m_sLine.Empty( );
    if( m_oFile.GetPart( m_sLine, -1, oMsgList ) ) {
      bRC = true;
    } else {
      m_oFile.Close( oMsgList ); 
    }
  }

  return bRC;
}
