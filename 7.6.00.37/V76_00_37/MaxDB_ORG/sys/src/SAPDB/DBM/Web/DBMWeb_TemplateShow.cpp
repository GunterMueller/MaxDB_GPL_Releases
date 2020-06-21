/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateShow.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of Shows
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
#include "DBM/Web/DBMWeb_TemplateShow.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMShow.htm"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_DATA               "Data"

/*
  =============================================================================
  class:        DBMWeb_TemplateShow
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateShow :: DBMWeb_TemplateShow
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateShow :: DBMWeb_TemplateShow ( sapdbwa_WebAgent & wa,
                                             DBMCli_Show & oShow )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oShow( oShow )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateShow :: ~DBMWeb_TemplateShow
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateShow :: ~DBMWeb_TemplateShow (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateShow :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateShow :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
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
  private function DBMWeb_TemplateShow :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateShow :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  
  if (szName.Compare(OBJ_TITLE) == 0) {

    pReturn = m_oShow.Name();  

  } else if (szName.Compare(OBJ_DATA) == 0) {

    pReturn = m_sLine;
  
  }

  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateShow :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateShow :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  Msg_List oMsgList;

  if( szName.Compare(OBJ_DATA ) == 0 ) {
    m_sLine.Empty( );
    if( m_oShow.GetPart( m_sLine, -1, oMsgList ) ) {
      bRC = true;
    } else {
      m_oShow.Close( oMsgList ); 
    }
  }

  return bRC;
}
