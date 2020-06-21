/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateParam.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  parameter
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
#include "DBM/Web/DBMWeb_TemplateParam.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMParam.htm"

#define OBJ_BACK               "BackURL"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_NAME               "Name"
#define OBJ_VALUE              "Value"
#define OBJ_NEWVALUE           "NewValue"

#define OBJ_HELP               "Help"
#define OBJ_EXPLAIN            "Explain"

#define OBJ_REQUEST            "Request"
#define OBJ_MANDATORY          "Mandatory"
#define OBJ_CONSTRAINT         "Constraint"
#define OBJ_UNKNOWN            "Unknown"

#define OBJ_BUTTONOK           "ButtonOK"
#define OBJ_BUTTONNEW          "ButtonNewValue"
#define OBJ_BUTTONCOMP         "ButtonCompValue"
#define OBJ_BUTTONCANCEL       "ButtonCancel"

/*
  =============================================================================
  class:        DBMWeb_TemplateParam
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateParam :: DBMWeb_TemplateParam
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateParam :: DBMWeb_TemplateParam ( sapdbwa_WebAgent             & wa,
                                               DBMCli_Parameter       & oParam,
                                               const DBMCli_ParameterStatus   nStatus,
                                               const DBMCli_String          & sCompValue,
                                               const DBMCli_String          & sBackURL)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_oParam( oParam ),
                            m_nStatus( nStatus ),
                            m_sCompValue( sCompValue ),
                            m_sBackURL( sBackURL )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateParam :: ~DBMWeb_TemplateParam
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateParam :: ~DBMWeb_TemplateParam (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateParam :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateParam :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_BACK) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_NAME) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_VALUE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_NEWVALUE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_HELP) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_EXPLAIN) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_REQUEST) == 0) {
    nReturn = (m_nStatus == DBMCLI_PARAMETERSTAT_REQUEST) ? 1 : 0;
  } else if (szName.Compare(OBJ_MANDATORY) == 0) {
    nReturn = (m_nStatus == DBMCLI_PARAMETERSTAT_MANDATORY) ? 1 : 0;
  } else if (szName.Compare(OBJ_CONSTRAINT) == 0) {
    nReturn = (m_nStatus == DBMCLI_PARAMETERSTAT_CONSTRAINT) ? 1 : 0;
  } else if (szName.Compare(OBJ_UNKNOWN) == 0) {
    nReturn = (m_nStatus == DBMCLI_PARAMETERSTAT_UNKNOWN) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONOK) == 0) {
    nReturn = (m_nStatus == DBMCLI_PARAMETERSTAT_OK         ||
               m_nStatus == DBMCLI_PARAMETERSTAT_MANDATORY  ||
               m_nStatus == DBMCLI_PARAMETERSTAT_CONSTRAINT ||
               m_nStatus == DBMCLI_PARAMETERSTAT_UNKNOWN       ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONNEW) == 0) {
    nReturn = (m_nStatus == DBMCLI_PARAMETERSTAT_REQUEST) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONCOMP) == 0) {
    nReturn = (m_nStatus == DBMCLI_PARAMETERSTAT_REQUEST) ? 1 : 0;
  } else if (szName.Compare(OBJ_BUTTONCANCEL) == 0) {
    nReturn = 1;
  } 

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateParam :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateParam :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_BACK) == 0) {

    pReturn = m_sBackURL;

  } else if (szName.Compare(OBJ_TITLE) == 0) {

    sValue = m_oParam.Name();
    pReturn = sValue;

  } else if(szName.Compare(OBJ_NAME) == 0) {
  
    pReturn = m_oParam.Name( );

  } else if(szName.Compare(OBJ_VALUE) == 0) {

    pReturn = m_oParam.LastKnownGood( );

  } else if(szName.Compare(OBJ_NEWVALUE) == 0) {

    pReturn = m_oParam.Value( );

  } else if(szName.Compare(OBJ_HELP) == 0) {

    pReturn = m_oParam.Help( );

  } else if(szName.Compare(OBJ_EXPLAIN) == 0) {

    pReturn = m_oParam.Explain( );

  } else if (szName.Compare(OBJ_REQUEST) == 0) {

    pReturn = m_sCompValue;

  } else if(szName.Compare(OBJ_BUTTONOK) == 0) {

    sValue = "'" + m_oParam.Name( ) + "'";
    pReturn = sValue;

  } else if (szName.Compare(OBJ_BUTTONNEW) == 0) {

    sValue = "'" + m_oParam.Name( ) + "'";
    pReturn = sValue;

  } else if (szName.Compare(OBJ_BUTTONCOMP) == 0) {

    sValue = "'" + m_oParam.Name( ) + "','" + m_sCompValue + "'";
    pReturn = sValue;

//  } else if (szName.Compare(OBJ_BUTTONCANCEL) == 0) {

//    sValue = "'" + m_oParam.GroupName( ) + "'";
//    sValue = "'" + m_sBackURL + "'";
//    pReturn = sValue;

  }

  return _Tools_UTF8Ptr(pReturn);
} 

