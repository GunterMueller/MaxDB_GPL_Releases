/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateDatabases.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of Databases
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
#include "DBM/Web/DBMWeb_TemplateDatabases.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME          "DBMDatabases.htm"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_ROWPAIR            "RowPair"

#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define OBJ_SERVER             "Server"
#define OBJ_NAME               "Name"
#define OBJ_VER                "Version"
#define OBJ_PATH               "Path"
#define OBJ_KERNEL             "Kernel"

/*
  =============================================================================
  class:        DBMWeb_TemplateDatabases
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateDatabases :: DBMWeb_TemplateDatabases
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateDatabases :: DBMWeb_TemplateDatabases ( sapdbwa_WebAgent & wa,
                                                       DBMCli_Node & oNode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oNode( oNode )
{
  m_nCol = 0;
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateDatabases :: ~DBMWeb_TemplateDatabases
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateDatabases :: ~DBMWeb_TemplateDatabases (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateDatabases :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateDatabases :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_TITLE) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    nReturn = (m_oNode.EnumDatabaseArray().GetSize()  / 2) + 
              (m_oNode.EnumDatabaseArray().GetSize() % 2); 
  } else if ( ( ISOBJ( OBJ_EVENROW ) ) || 
              ( ISOBJ( OBJ_ODDROW ) )     ) {
    nReturn = (m_nRow < m_oNode.EnumDatabaseArray().GetSize()) ? 1 : 0;
    m_nRow++;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateDatabases :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateDatabases :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  DBMCli_String sValue;
  
  if( ISOBJ( OBJ_TITLE ) ) {
    sValue = m_oNode.ServerName();
    if (sValue.IsEmpty()) {
      sValue = "(local)";
    }
    pReturn = sValue; 
  } else if( ISOBJ( OBJ_SERVER ) ) {
    sValue = m_oNode.ServerName();
    pReturn = sValue;
  } else if( ISOBJ( OBJ_NAME ) ) {
    DBMCli_EnumDatabaseArray & aDatabase = m_oNode.EnumDatabaseArray( );
    sValue = aDatabase[m_nRow - 1].Name( );
    pReturn = sValue;
  } else if( ISOBJ( OBJ_VER ) ) {
    DBMCli_EnumDatabaseArray & aDatabase = m_oNode.EnumDatabaseArray( );
    sValue = aDatabase[m_nRow - 1].Version( ).GetName( );
    pReturn = sValue;
  } else if( ISOBJ( OBJ_PATH ) ) {
    DBMCli_EnumDatabaseArray & aDatabase = m_oNode.EnumDatabaseArray( );
    pReturn = aDatabase[m_nRow - 1].InstRoot( );
  } else if( ISOBJ( OBJ_KERNEL ) ) {
    DBMCli_EnumDatabaseArray & aDatabase = m_oNode.EnumDatabaseArray( );
    pReturn = aDatabase[m_nRow - 1].Kernel( );
  }
  return _Tools_UTF8Ptr(pReturn);
} 

