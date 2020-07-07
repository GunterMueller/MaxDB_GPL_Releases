/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateParmed.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Parmed
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
#include "DBM/Web/DBMWeb_TemplateParmed.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMParmed.htm"

#define OBJ_BACK               "BackURL"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_PARNAME            "ParallelName"
#define OBJ_DEVTYPE            "DeviceType"
#define OBJ_BAKTYPE            "BackupType"
#define OBJ_OVER               "Overwrite"
#define OBJ_AUTOL              "Autoloader"

#define OBJ_MEDIUM             "OneMedium"
#define OBJ_HEADER             "MediumHeader"

#define OBJ_NAME               "Name"
#define OBJ_LOCATION           "Location"
#define OBJ_SIZE               "Size"
#define OBJ_OSCMD              "OSCommand"

#define OBJ_BUTTONOKADD        "ButtonOKAdd"
#define OBJ_BUTTONOKPUT        "ButtonOKPut"
#define OBJ_BUTTONCANCEL       "ButtonCancel"
#define OBJ_BUTTONDELETE       "ButtonDelete"
/*
  =============================================================================
  class:        DBMWeb_TemplateParmed
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateParmed :: DBMWeb_TemplateParmed
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateParmed :: DBMWeb_TemplateParmed ( sapdbwa_WebAgent & wa,
                                                 DBMCli_MediumParallel & oParMed,
                                                 DBMWeb_TemplParmed_Mode nMode,
                                                 const DBMCli_String & sBackURL)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oParMed( oParMed ),
                            m_nMode( nMode ),
                            m_sBackURL( sBackURL )
{
  m_nMedium = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateParmed :: ~DBMWeb_TemplateParmed
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateParmed :: ~DBMWeb_TemplateParmed (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateParmed :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateParmed :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_BACK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_PARNAME) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_DEVTYPE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BAKTYPE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_OVER) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_AUTOL) == 0) {
    nReturn = 1;

  } else if (szName.Compare(OBJ_MEDIUM) == 0) {
    nReturn = m_oParMed.MediumArray( ).GetSize( );
  } else if (szName.Compare(OBJ_HEADER) == 0) {
    nReturn = 1;
    m_nMedium++;

  } else if (szName.Compare(OBJ_NAME) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_LOCATION) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_SIZE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_OSCMD) == 0) {
    nReturn = 1;

  } else if (szName.Compare(OBJ_BUTTONOKADD) == 0) {
    nReturn = ( m_nMode == DBMWEB_TEMPLPARMED_NEW ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONOKPUT) == 0) {
    nReturn = ( m_nMode == DBMWEB_TEMPLPARMED_EDIT ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONCANCEL) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BUTTONDELETE) == 0) {
    nReturn = ( m_nMode == DBMWEB_TEMPLPARMED_EDIT ) ? 1 : 0;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateParmed :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateParmed :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_BACK) == 0) {
    pReturn = m_sBackURL;
  } else if (szName.Compare(OBJ_TITLE) == 0) {
    if( m_oParMed.Name( ).IsEmpty( ) ) {
      sValue = "New Parallel Medium";
    } else {
      sValue = m_oParMed.Name();
    }
    pReturn = sValue;
  } else if(szName.Compare(OBJ_PARNAME) == 0) {
    pReturn = m_oParMed.Name( );
  } else if(szName.Compare(OBJ_DEVTYPE) == 0) {
    pReturn = m_oParMed.DeviceType( );
  } else if(szName.Compare(OBJ_BAKTYPE) == 0) {
    pReturn = m_oParMed.BackupType( );
  } else if(szName.Compare(OBJ_OVER) == 0) {
    pReturn = m_oParMed.Overwrite( );
  } else if(szName.Compare(OBJ_AUTOL) == 0) {
    pReturn = m_oParMed.Autoloader( );

  } else if(szName.Compare(OBJ_HEADER) == 0) {
    DBMCli_MediumArray & aMedium = m_oParMed.MediumArray( );

    sValue.Format("%d. ", m_nMedium);

    if( !m_oParMed.Name( ).IsEmpty( ) && 
        !aMedium[m_nMedium - 1].Name( ).IsEmpty( ) ) {
      sValue += m_oParMed.Name( );
      sValue += "/" ;
      sValue += aMedium[m_nMedium - 1].Name( );
    }
    pReturn = sValue;

  } else if(szName.Compare(OBJ_NAME) == 0) {
    DBMCli_MediumArray & aMedium = m_oParMed.MediumArray( );
    pReturn = aMedium[m_nMedium - 1].Name( );
  } else if(szName.Compare(OBJ_LOCATION) == 0) {
    DBMCli_MediumArray & aMedium = m_oParMed.MediumArray( );
    pReturn = aMedium[m_nMedium - 1].Location( );
  } else if(szName.Compare(OBJ_SIZE) == 0) {
    DBMCli_MediumArray & aMedium = m_oParMed.MediumArray( );
    pReturn = aMedium[m_nMedium - 1].Pages( );
  } else if(szName.Compare(OBJ_OSCMD) == 0) {
    DBMCli_MediumArray & aMedium = m_oParMed.MediumArray( );
    pReturn = aMedium[m_nMedium - 1].OSCommand( );
  } 

  return _Tools_UTF8Ptr(pReturn);
} 

