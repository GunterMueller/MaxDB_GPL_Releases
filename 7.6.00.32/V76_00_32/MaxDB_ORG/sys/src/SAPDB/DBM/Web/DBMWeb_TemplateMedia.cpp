/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateMedia.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Media
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
#include "DBM/Web/DBMWeb_TemplateMedia.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMMedia.htm"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_TABSNG             "TabSingle"
#define OBJ_TABPAR             "TabParallel"

#define OBJ_TABLESNG           "TableSingle"
#define OBJ_TABLEPAR           "TableParallel"

#define OBJ_ROWPAIR            "RowPair"

#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define OBJ_NAME               "Name"
#define OBJ_LOC                "Location"
#define OBJ_DEVTYPE            "DeviceType"
#define OBJ_BAKTYPE            "BackupType"
#define OBJ_MOD                "Modified"
#define OBJ_SIZE               "Size"
#define OBJ_OVW                "Overwrite"

#define OBJ_TOOLBAR            "Toolbar"
#define OBJ_BTNADDSNG          "ButtonAddSingle"
#define OBJ_BTNADDPAR          "ButtonAddParallel"


/*
  =============================================================================
  class:        DBMWeb_TemplateMedia
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateMedia:: DBMWeb_TemplateMedia
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMedia :: DBMWeb_TemplateMedia ( sapdbwa_WebAgent & wa,
                                               DBMCli_Media & oMedia,
                                               DBMWeb_TemplMedia_Mode nMode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oMedia( oMedia ),
                            m_nMode( nMode )
{
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateMedia :: ~DBMWeb_TemplateMedia
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateMedia :: ~DBMWeb_TemplateMedia (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMedia :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateMedia :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {

    nReturn = 1;

  } else if (szName.Compare(OBJ_TABSNG) == 0) {

    nReturn = (m_nMode == DBMWEB_TEMPLMEDIA_SINGLE) ? 1 : 0;

  } else if (szName.Compare(OBJ_TABPAR) == 0) {

    nReturn = (m_nMode == DBMWEB_TEMPLMEDIA_PARALLEL) ? 1 : 0;

  } else if (szName.Compare(OBJ_TABLESNG) == 0) {

    nReturn = (m_nMode == DBMWEB_TEMPLMEDIA_SINGLE) ? 1 : 0;

  } else if (szName.Compare(OBJ_TABLEPAR) == 0) {

    nReturn = (m_nMode == DBMWEB_TEMPLMEDIA_PARALLEL) ? 1 : 0;

  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {

    if (m_nMode == DBMWEB_TEMPLMEDIA_SINGLE) {
      nReturn = (m_oMedia.MediumArray( ).GetSize( )  / 2) + 
                (m_oMedia.MediumArray( ).GetSize( ) % 2); 
    } else {
      nReturn = (m_oMedia.MediumParallelArray( ).GetSize( )  / 2) + 
                (m_oMedia.MediumParallelArray( ).GetSize( ) % 2); 
    }

  } else if ( (szName.Compare(OBJ_EVENROW) == 0) || 
              (szName.Compare(OBJ_ODDROW) == 0)     ) {

    if (m_nMode == DBMWEB_TEMPLMEDIA_SINGLE) {
      nReturn = (m_nRow < m_oMedia.MediumArray( ).GetSize( )) ? 1 : 0;
    } else {
      nReturn = (m_nRow < m_oMedia.MediumParallelArray( ).GetSize( )) ? 1 : 0;
    }
    m_nRow++;

  } else if (szName.Compare(OBJ_TOOLBAR) == 0) {

    nReturn = 1;

  } else if (szName.Compare(OBJ_BTNADDSNG) == 0) {

    nReturn = (m_nMode == DBMWEB_TEMPLMEDIA_SINGLE) ? 1 : 0;

  } else if (szName.Compare(OBJ_BTNADDPAR) == 0) {

    nReturn = (m_nMode == DBMWEB_TEMPLMEDIA_PARALLEL) ? 1 : 0;

  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateMedia :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateMedia :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_TITLE) == 0) {
    pReturn = m_oMedia.GetDatabase().DatabaseName();  
  } else {
    if (m_nMode == DBMWEB_TEMPLMEDIA_SINGLE) {
      DBMCli_MediumArray & aMedium = m_oMedia.MediumArray( );
      if (szName.Compare(OBJ_NAME) == 0) {
        pReturn = aMedium[m_nRow - 1].Name( );
      } else if (szName.Compare(OBJ_LOC) == 0) {
        pReturn = aMedium[m_nRow - 1].Location( );
      } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
        pReturn = aMedium[m_nRow - 1].DeviceType( );
      } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
        pReturn = aMedium[m_nRow - 1].BackupType( );
      } else if (szName.Compare(OBJ_MOD) == 0) {
        pReturn = aMedium[m_nRow - 1].Modified( );
      } else if (szName.Compare(OBJ_SIZE) == 0) {
        if(aMedium[m_nRow - 1].Pages( ) == "0") {
          sValue = sValue = "&nbsp;";
        } else {
          sValue = aMedium[m_nRow - 1].Pages( );
        }
        pReturn = sValue;
      } else if (szName.Compare(OBJ_OVW) == 0) {
        pReturn = aMedium[m_nRow - 1].Overwrite( );
      }
    } else {
      DBMCli_MediumParallelArray & aMediumParallel = m_oMedia.MediumParallelArray( );
      if (szName.Compare(OBJ_NAME) == 0) {
        pReturn = aMediumParallel[m_nRow - 1].Name( );;
      } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
        pReturn = aMediumParallel[m_nRow - 1].DeviceType( );
      } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
        pReturn = aMediumParallel[m_nRow - 1].BackupType( );;
      } else if (szName.Compare(OBJ_MOD) == 0) {
        pReturn = aMediumParallel[m_nRow - 1].Modified( );
      } else if (szName.Compare(OBJ_OVW) == 0) {
        pReturn = aMediumParallel[m_nRow - 1].Overwrite( );
      }
    }
  }

  return _Tools_UTF8Ptr(pReturn);
} 

