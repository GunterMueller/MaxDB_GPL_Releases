/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateWizMenu.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Wizard Menu
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
#include "DBM/Web/DBMWeb_TemplateWizMenu.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME   "DBMWizMenu.htm"

#define OBJ_STEP        "Step"

#define OBJ_STATE       "State*"
#define OBJ_STATE1      "State1"
#define OBJ_STATE2      "State2"
#define OBJ_STATE3      "State3"
#define OBJ_STATE4      "State4"
#define OBJ_STATE5      "State5"
#define OBJ_STATE6      "State6"
#define OBJ_STATE7      "State7"
#define OBJ_STATE8      "State8"

#define STATE_VAL_DEFAULT "0"
#define STATE_VAL_WORK    "1"
#define STATE_VAL_READY   "2"
#define STATE_VAL_ERROR   "3"

#define OBJ_BTNPREV     "BtnPrev"
#define OBJ_BTNNEXT     "BtnNext"

/*
  =============================================================================
  class:        DBMWeb_TemplateWizMenu
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateWizMenu :: DBMWeb_TemplateWizMenu
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateWizMenu :: DBMWeb_TemplateWizMenu ( sapdbwa_WebAgent & wa,
                                                   SAPDB_Int nStep,
                                                   SAPDB_Int nState )
                        : Tools_Template( wa, _Tools_UTF8Ptr(TEMPLATE_NAME) ),
                          m_nStep( nStep ),
                          m_nState( nState )


{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateWizMenu :: ~DBMWeb_TemplateWizMenu
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateWizMenu :: ~DBMWeb_TemplateWizMenu (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateWizMenu :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateWizMenu :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_STEP) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_STATE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNPREV) == 0) {
    nReturn = ( (m_nStep > 1) && (m_nStep < 9) ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNNEXT) == 0) {
    nReturn = (m_nStep < 8) ? 1 : 0;
  }

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateWizMenu :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateWizMenu :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  if (szName.Compare(OBJ_STEP) == 0) {
    m_sValue.Format("%d", m_nStep);
    pReturn = m_sValue;  
  } else if (szName.Compare(OBJ_STATE1) == 0) {
    GetStateValue( 1 );
  } else if (szName.Compare(OBJ_STATE2) == 0) {
    GetStateValue( 2 );
  } else if (szName.Compare(OBJ_STATE3) == 0) {
    GetStateValue( 3 );
  } else if (szName.Compare(OBJ_STATE4) == 0) {
    GetStateValue( 4 );
  } else if (szName.Compare(OBJ_STATE5) == 0) {
    GetStateValue( 5 );
  } else if (szName.Compare(OBJ_STATE6) == 0) {
    GetStateValue( 6 );
  } else if (szName.Compare(OBJ_STATE7) == 0) {
    GetStateValue( 7 );
  } else if (szName.Compare(OBJ_STATE8) == 0) {
    GetStateValue( 8 );
  }

  pReturn = m_sValue;

  return _Tools_UTF8Ptr(pReturn);
} 

void DBMWeb_TemplateWizMenu :: GetStateValue( SAPDB_Int nState )
{
  if( m_nStep == nState ) {
    if( m_nState == 3 ) {
      m_sValue = STATE_VAL_ERROR;
    } else {
      m_sValue = STATE_VAL_WORK;
    }
  } else {
    if( m_nStep > nState ) {
      m_sValue = STATE_VAL_READY;
    } else {
      m_sValue = STATE_VAL_DEFAULT;
    }
  }
}