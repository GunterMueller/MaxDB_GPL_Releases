/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateShows.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  a class for creating info tables in SAP MiniApps style
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
#include "DBM/Web/DBMWeb_TemplateShows.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME   "DBMShows.htm"

#define OBJ_TITLE       "GroupBoxTitle"

#define OBJ_ROWPAIR     "RowPair"

#define OBJ_EVENROW     "EvenRow*"
#define OBJ_ODDROW      "OddRow*"

#define OBJ_NAME        "Name"
#define OBJ_DESC        "Description"

/*
  =============================================================================
  class:        DBMWeb_TemplateShows
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateShows :: DBMWeb_TemplateShows
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateShows :: DBMWeb_TemplateShows ( sapdbwa_WebAgent & wa,
                                               DBMCli_Shows & oShows )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oShows( oShows )
{
  m_nCol = 0;
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateShows :: ~DBMWeb_TemplateShows
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateShows :: ~DBMWeb_TemplateShows (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateShows :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateShows :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    nReturn = (m_oShows.ShowArray( ).GetSize( )  / 2) + 
              (m_oShows.ShowArray( ).GetSize( ) % 2); 
  } else if ( (szName.Compare(OBJ_EVENROW) == 0) || 
              (szName.Compare(OBJ_ODDROW) == 0)     ) {
    nReturn = (m_nRow < m_oShows.ShowArray( ).GetSize( )) ? 1 : 0;
    m_nRow++;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateShows :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateShows :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_TITLE) == 0) {
    pReturn = m_oShows.GetDatabase().DatabaseName();  
  } else if (szName.Compare(OBJ_NAME) == 0) {
    DBMCli_ShowArray & aShows = m_oShows.ShowArray( );
    pReturn = aShows[m_nRow - 1].Name( );
  } else if (szName.Compare(OBJ_DESC) == 0) {
    DBMCli_ShowArray & aShows = m_oShows.ShowArray( );
    pReturn = aShows[m_nRow - 1].Description( );
  }

  return _Tools_UTF8Ptr(pReturn);
} 

