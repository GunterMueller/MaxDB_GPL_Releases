/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateKernelTrace.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  creating list of kernel trace options
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
#include "DBM/Web/DBMWeb_TemplateKernelTrace.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME   "DBMKernelTrace.htm"

#define OBJ_TITLE       "GroupBoxTitle"

#define OBJ_ROWPAIR     "RowPair"

#define OBJ_EVENROW     "EvenRow*"
#define OBJ_ODDROW      "OddRow*"

#define OBJ_TABOPT      "TabOptions"
#define OBJ_TABPROT     "TabProtocol"

#define OBJ_TABLEOPT    "TableOptions"
#define OBJ_TABLEPROT   "TableProtocol"

#define OBJ_OPTION      "Option"
#define OBJ_NAME        "Name"
#define OBJ_DESC        "Description"
#define OBJ_VALUE       "Value"

#define OBJ_BTNON       "BtnOn"
#define OBJ_BTNOFF      "BtnOff"
#define OBJ_BTNALLOFF   "BtnAllOff"
#define OBJ_BTNMAKE     "BtnMake"
#define OBJ_BTNFLUSH    "BtnFlush"
#define OBJ_BTNCLEAR    "BtnClear"

/*
  =============================================================================
  class:        DBMWeb_TemplateKernelTrace
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateKernelTrace :: DBMWeb_TemplateKernelTrace
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateKernelTrace :: DBMWeb_TemplateKernelTrace ( sapdbwa_WebAgent & wa,
                                                           DBMCli_KernelTrace & oKernelTrace, 
                                                           DBMWeb_TemplKnlTrace_Mode nMode)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                             m_oKernelTrace( oKernelTrace ),
                             m_nMode( nMode )
{
  m_nCol = 0;
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateKernelTrace :: ~DBMWeb_TemplateKernelTrace
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateKernelTrace :: ~DBMWeb_TemplateKernelTrace (  )
{
} 


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateKernelTrace :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateKernelTrace :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_TABOPT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS ) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABPROT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_PROTOCOL ) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABLEOPT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS ) ? 1 : 0;
  } else if (szName.Compare(OBJ_TABLEPROT) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_PROTOCOL ) ? 1 : 0;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    switch( m_nMode ) {
      case DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS:
        nReturn = (m_oKernelTrace.KernelTraceOptionArray( ).GetSize( )  / 2) + 
                  (m_oKernelTrace.KernelTraceOptionArray( ).GetSize( ) % 2); 
        break;
      case DBMWEB_TEMPLKNLTRACE_MODE_PROTOCOL:
        nReturn = (m_oKernelTrace.KernelTraceProtOptArray( ).GetSize( )  / 2) + 
                  (m_oKernelTrace.KernelTraceProtOptArray( ).GetSize( ) % 2); 
        break;
    }
  } else if ( (szName.Compare(OBJ_EVENROW) == 0) || 
              (szName.Compare(OBJ_ODDROW) == 0)     ) {
    switch( m_nMode ) {
      case DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS:
        nReturn = (m_nRow < m_oKernelTrace.KernelTraceOptionArray( ).GetSize( )) ? 1 : 0;
        break;
      case DBMWEB_TEMPLKNLTRACE_MODE_PROTOCOL:
        nReturn = (m_nRow < m_oKernelTrace.KernelTraceProtOptArray( ).GetSize( )) ? 1 : 0;
        break;
    }
    m_nRow++;
  } else if (szName.Compare(OBJ_BTNON) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNOFF) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNALLOFF) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNMAKE) == 0) {
    nReturn = (m_nMode == DBMWEB_TEMPLKNLTRACE_MODE_PROTOCOL ) ? 1 : 0;
  } else if (szName.Compare(OBJ_BTNFLUSH) == 0) {
    nReturn = 1;
  } else if (szName.Compare(OBJ_BTNCLEAR) == 0) {
    nReturn = 1;
  }

  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateKernelTrace :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateKernelTrace :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_TITLE) == 0) {
    pReturn = m_oKernelTrace.GetDatabase().DatabaseName();  
  } else {
    switch( m_nMode ) {
      case DBMWEB_TEMPLKNLTRACE_MODE_OPTIONS:
        if (szName.Compare(OBJ_OPTION) == 0) {
          DBMCli_KernelTraceOptionArray & aKernelTrace = m_oKernelTrace.KernelTraceOptionArray( );
          pReturn = aKernelTrace[m_nRow - 1].Name1( );
        } else if (szName.Compare(OBJ_NAME) == 0) {
          DBMCli_KernelTraceOptionArray & aKernelTrace = m_oKernelTrace.KernelTraceOptionArray( );
          pReturn = aKernelTrace[m_nRow - 1].Name3( );
        } else if (szName.Compare(OBJ_DESC) == 0) {
          DBMCli_KernelTraceOptionArray & aKernelTrace = m_oKernelTrace.KernelTraceOptionArray( );
          pReturn = aKernelTrace[m_nRow - 1].Description( );
        } else if (szName.Compare(OBJ_VALUE) == 0) {
          DBMCli_KernelTraceOptionArray & aKernelTrace = m_oKernelTrace.KernelTraceOptionArray( );
          pReturn = aKernelTrace[m_nRow - 1].Value( );
        }
        break;
      case DBMWEB_TEMPLKNLTRACE_MODE_PROTOCOL:
        if (szName.Compare(OBJ_OPTION) == 0) {
          DBMCli_KernelTraceProtOptArray & aProtOpt = m_oKernelTrace.KernelTraceProtOptArray( );
          pReturn = aProtOpt[m_nRow - 1].Option( );
        } else if (szName.Compare(OBJ_DESC) == 0) {
          DBMCli_KernelTraceProtOptArray & aProtOpt = m_oKernelTrace.KernelTraceProtOptArray( );
          pReturn = aProtOpt[m_nRow - 1].Description( );
        }
        break;
    }
  }

  return _Tools_UTF8Ptr(pReturn);
} 

