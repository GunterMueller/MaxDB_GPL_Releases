/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateParams.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  parameters
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
#include "DBM/Web/DBMWeb_TemplateParams.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME       "DBMParams.htm"

#define OBJ_TITLE           "GroupBoxTitle"

#define OBJ_REFRESHBUTTON   "RefreshButton"

#define OBJ_TABGEN          "TabGeneral"
#define OBJ_TABEXT          "TabExtended"
#define OBJ_TABSUP          "TabSupport"

#define OBJ_TABLEHEADER     "TableHeader"

#define OBJ_ROWPAIR         "RowPair"

#define OBJ_EVENROW         "EvenRow*"
#define OBJ_ODDROW          "OddRow*"

#define OBJ_NAME            "Name"
#define OBJ_CHANGE          "Change"
#define OBJ_VALUE           "Value"
#define OBJ_NEWVALUE        "NewValue"
#define OBJ_DESC            "Description"

/*
  =============================================================================
  class:        DBMWeb_TemplateParams
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateParams :: DBMWeb_TemplateParams
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateParams :: DBMWeb_TemplateParams ( sapdbwa_WebAgent & wa,
                                                 DBMCli_Parameters & oParams,
                                                 DBMCli_ParameterGroup nGroup)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oParams( oParams ),
                            m_nGroup( nGroup )
{
  m_nParam = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateParams :: ~DBMWeb_TemplateParams
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateParams :: ~DBMWeb_TemplateParams (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateParams :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateParams :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_REFRESHBUTTON) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_TABGEN) == 0) {
    nReturn = (m_nGroup == DBMCLI_PARAMGROUP_GENERAL) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABEXT) == 0) {
    nReturn = (m_nGroup == DBMCLI_PARAMGROUP_EXTENDED) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABSUP) == 0) {
    nReturn = (m_nGroup == DBMCLI_PARAMGROUP_SUPPORT) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABLEHEADER) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    nReturn = (m_nParam < m_oParams.ParameterArray( ).GetSize( )) ? -1 : 0; 
  } else if ( (szName.Compare(OBJ_EVENROW) == 0) || 
              (szName.Compare(OBJ_ODDROW) == 0)     ) {
    nReturn = (FindNextParam( ) == true) ? 1 : 0;
  }
  
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateParams :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateParams :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  sValue.Empty( );
  
  if (szName.Compare(OBJ_TITLE) == 0) {
    sValue = m_oParams.GetDatabase().DatabaseName();  
  } else if (szName.Compare(OBJ_REFRESHBUTTON) == 0) {
    sValue = m_oParams.GroupNameFromGroup(m_nGroup);
  } else if (szName.Compare(OBJ_TABLEHEADER) == 0) {
    switch( m_nGroup ) {
      case DBMCLI_PARAMGROUP_GENERAL: 
        sValue = "General ";
        break;
      case DBMCLI_PARAMGROUP_EXTENDED: 
        sValue = "Extended ";
        break;
      case DBMCLI_PARAMGROUP_SUPPORT: 
        sValue = "Support ";
        break;
      default:
        sValue = "???";
        break;
    }
    sValue += "Parameters";
  } else if (szName.Compare(OBJ_NAME) == 0) {
    DBMCli_ParameterArray & aParams = m_oParams.ParameterArray( );
    sValue = aParams[m_nParam].Name( );
  } else if (szName.Compare(OBJ_CHANGE) == 0) {
    DBMCli_ParameterArray & aParams = m_oParams.ParameterArray( );
    if (aParams[m_nParam].Change( )) {
      sValue = "1";
    } else {
      sValue = "0";
    }
  } else if (szName.Compare(OBJ_VALUE) == 0) {
    DBMCli_ParameterArray & aParams = m_oParams.ParameterArray( );

    sValue = aParams[m_nParam].LastKnownGood( );

    if( sValue.IsEmpty() )
      sValue = "&nbsp";

  } else if (szName.Compare(OBJ_NEWVALUE) == 0) {
    DBMCli_ParameterArray & aParams = m_oParams.ParameterArray( );

    sValue.Empty( );

    if( aParams[m_nParam].Value( ) != aParams[m_nParam].LastKnownGood() ) {
      sValue = aParams[m_nParam].Value( );
    }
    
    if( sValue.IsEmpty() )
      sValue = "&nbsp";

  } else if (szName.Compare(OBJ_DESC) == 0) {
    DBMCli_ParameterArray & aParams = m_oParams.ParameterArray( );
    
    sValue = aParams[m_nParam].Help( );

    if( sValue.IsEmpty() )
      sValue = "&nbsp";

    m_nParam++;

  }

  pReturn = sValue;

  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFile :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateParams :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  
  DBMCli_ParameterArray & aParams = m_oParams.ParameterArray( );

  if( szName.Compare(OBJ_ROWPAIR ) == 0 ) {

    bRC = ( m_nParam < aParams.GetSize( ) );
  }

  return bRC;
}

bool DBMWeb_TemplateParams :: FindNextParam ( )
{
  bool bFound = false;

  DBMCli_ParameterArray & aParams = m_oParams.ParameterArray( );

  while( ( m_nParam < aParams.GetSize( ) ) && ( bFound == false ) ) {
    if( aParams[m_nParam].Group( ) == m_nGroup ) {
      bFound = true;
    } else {
      m_nParam++;
    }
  }

  return bFound;
}