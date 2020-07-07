/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_KernelTrace.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  KernelTraceOption Class
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
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_KernelTrace.hpp"
#include "DBM/Cli/DBMCli_DateTime.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define KT_NA1_DEF "DEFAULT"
#define KT_NA1_IND "INDEX"
#define KT_NA1_LOC "LOCK"
#define KT_NA1_LON "LONG"
#define KT_NA1_PAG "PAGES"
#define KT_NA1_STA "ORDER STANDARD"
#define KT_NA1_TAB "TABLE"
#define KT_NA1_OBJ "OBJECT"
#define KT_NA1_ADD "OBJECT ADD"
#define KT_NA1_GET "OBJECT GET"
#define KT_NA1_ALT "OBJECT ALTER"
#define KT_NA1_FRE "OBJECT FREEPAGE"
#define KT_NA1_SEL "SELECT"
#define KT_NA1_INS "INSERT"
#define KT_NA1_UPD "UPDATE"
#define KT_NA1_DEL "DELETE"
#define KT_NA1_ORD "ORDER"
#define KT_NA1_OPT "OPTIMIZE"
#define KT_NA1_TIM "TIME"
#define KT_NA1_CHE "CHECK"
#define KT_NA1_COM "ANALYZE"

#define KT_NA2_DEF "DEFAULT"
#define KT_NA2_IND "INDEX"
#define KT_NA2_LOC "LOCK"
#define KT_NA2_LON "LONG"
#define KT_NA2_PAG "PAGES"
#define KT_NA2_STA "ORDER STNDRD"
#define KT_NA2_TAB "TABLE"
#define KT_NA2_OBJ "OBJECT"
#define KT_NA2_ADD "OBJECT ADD"
#define KT_NA2_GET "OBJECT GET"
#define KT_NA2_ALT "OBJECT ALTER"
#define KT_NA2_FRE "OBJECT FREE"
#define KT_NA2_SEL "SELECT"
#define KT_NA2_INS "INSERT"
#define KT_NA2_UPD "UPDATE"
#define KT_NA2_DEL "DELETE"
#define KT_NA2_ORD "ORDER"
#define KT_NA2_OPT "OPTIMIZE"
#define KT_NA2_TIM "TIME"
#define KT_NA2_CHE "ON"
#define KT_NA2_COM "AK"

#define KT_NA3_DEF "Default"
#define KT_NA3_IND "Index"
#define KT_NA3_LOC "Lock"
#define KT_NA3_LON "Long"
#define KT_NA3_PAG "Pages"
#define KT_NA3_STA "Standard"
#define KT_NA3_TAB "Table"
#define KT_NA3_OBJ "Objects"
#define KT_NA3_ADD "Objects add"
#define KT_NA3_GET "Objects get"
#define KT_NA3_ALT "Objects alter"
#define KT_NA3_FRE "Objects free"
#define KT_NA3_SEL "Select"
#define KT_NA3_INS "Insert"
#define KT_NA3_UPD "Update"
#define KT_NA3_DEL "Delete"
#define KT_NA3_ORD "Order"
#define KT_NA3_OPT "Optimizer"
#define KT_NA3_TIM "Time"
#define KT_NA3_CHE "Check"
#define KT_NA3_COM "Commands"

#define KT_DES_DEF "Includes Index, Lock, Long, Pages, Standard, Table"
#define KT_DES_IND "Interfaces according accesses to indexes"
#define KT_DES_LOC "Lock collisions and their elimination"
#define KT_DES_LON "Accesses to LONG columns"
#define KT_DES_PAG "I/O"
#define KT_DES_STA "Beginning of all parts of the packet interface"
#define KT_DES_TAB "Accesses to primary data"
#define KT_DES_OBJ "Accesses to objects (includes add, get, alter, free)"
#define KT_DES_ADD "Accesses to objects"
#define KT_DES_GET "Accesses to objects"
#define KT_DES_ALT "Accesses to objects"
#define KT_DES_FRE "Accesses to objects"
#define KT_DES_SEL "Complete message buffer of SELECT, GET, FETCH, SELECT ROW and UNION"
#define KT_DES_INS "Complete message buffer of INSERT"
#define KT_DES_UPD "Complete message buffer of UPDATE"
#define KT_DES_DEL "Complete message buffer of DELETE"
#define KT_DES_ORD "Complete order packet"
#define KT_DES_OPT "Optimizer output"
#define KT_DES_TIM "Elapsed time in kernel underneath of the message buffer"
#define KT_DES_CHE "Additional checks on correct state of the regions"
#define KT_DES_COM "All order packets (including internal packets)"

#define KT_DEF_VAL "0"
#define KT_OFF_VAL "1"
#define KT_ON_VAL  "2"

#define DESC_VTRACE "VTRACE"
#define DESC_CHECK  "VTRACE CHECK"

#define UTL_CMD_DIAGVTRACE      "DIAGNOSE VTRACE"
#define UTL_CMD_DIAGVTRACEON    "ON"
#define UTL_CMD_DIAGVTRACEOFF   "OFF"
#define UTL_CMD_DIAGVTRACECLEAR "CLEAR"
#define UTL_CMD_DIAGVTRACEFLUSH "VTRACE"

/*    
  =============================================================================
  class: DBMCli_KernelTraceOption
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_KernelTraceOption :: DBMCli_KernelTraceOption
  -----------------------------------------------------------------------------
*/
DBMCli_KernelTraceOption :: DBMCli_KernelTraceOption ( )
{
} 

DBMCli_KernelTraceOption :: DBMCli_KernelTraceOption ( const DBMCli_String & sName1, 
                                                       const DBMCli_String & sName2,
                                                       const DBMCli_String & sName3,
                                                       const DBMCli_String & sDescription,
                                                       const DBMCli_String & sValue )
                          : m_sName1( sName1),
                            m_sName2( sName2),
                            m_sName3( sName3),
                            m_sDescription( sDescription ),
                            m_sValue( sValue )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_KernelTraceOption :: ~DBMCli_KernelTraceOption
  -----------------------------------------------------------------------------
*/
DBMCli_KernelTraceOption :: ~DBMCli_KernelTraceOption ( )
{
} 
 
/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_KernelTraceOption :: ~DBMCli_KernelTraceOption
  -----------------------------------------------------------------------------
*/

const DBMCli_String & DBMCli_KernelTraceOption :: Name ( SAPDB_Int nNum )
{
  switch( nNum ) {
    case 1: return m_sName1;
    case 2: return m_sName2;
    case 3: return m_sName3;
    default: return m_sName1;
  }
}

/*    
  =============================================================================
  class: DBMCli_KernelTraceProtOpt
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_KernelTraceProtOpt :: DBMCli_KernelTraceProtOpt
  -----------------------------------------------------------------------------
*/
DBMCli_KernelTraceProtOpt :: DBMCli_KernelTraceProtOpt ( )
{
} 

DBMCli_KernelTraceProtOpt :: DBMCli_KernelTraceProtOpt ( const DBMCli_String & sOption, 
                                                         const DBMCli_String & sDescription )
                          : m_sOption( sOption),
                            m_sDescription( sDescription )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_KernelTraceProtOpt :: ~DBMCli_KernelTraceProtOpt
  -----------------------------------------------------------------------------
*/
DBMCli_KernelTraceProtOpt :: ~DBMCli_KernelTraceProtOpt ( )
{
} 

/*    
  =============================================================================
  class: DBMCli_KernelTrace
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_KernelTrace :: DBMCli_KernelTrace
  -----------------------------------------------------------------------------
*/
DBMCli_KernelTrace :: DBMCli_KernelTrace ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_KernelTrace :: ~DBMCli_KernelTrace
  -----------------------------------------------------------------------------
*/
DBMCli_KernelTrace :: ~DBMCli_KernelTrace ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_KernelTrace :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_KernelTrace :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  m_aKernelTraceOption.RemoveAll( );
  m_aKernelTraceProtOpt.RemoveAll( );

  AddOption( KT_NA1_DEF, KT_NA2_DEF, KT_NA3_DEF, KT_DES_DEF, KT_DEF_VAL );
  AddOption( KT_NA1_IND, KT_NA2_IND, KT_NA3_IND, KT_DES_IND, KT_DEF_VAL );
  AddOption( KT_NA1_LOC, KT_NA2_LOC, KT_NA3_LOC, KT_DES_LOC, KT_DEF_VAL );
  AddOption( KT_NA1_LON, KT_NA2_LON, KT_NA3_LON, KT_DES_LON, KT_DEF_VAL );
  AddOption( KT_NA1_PAG, KT_NA2_PAG, KT_NA3_PAG, KT_DES_PAG, KT_DEF_VAL );
  AddOption( KT_NA1_STA, KT_NA2_STA, KT_NA3_STA, KT_DES_STA, KT_DEF_VAL );
  AddOption( KT_NA1_TAB, KT_NA2_TAB, KT_NA3_TAB, KT_DES_TAB, KT_DEF_VAL );
  AddOption( KT_NA1_OBJ, KT_NA2_OBJ, KT_NA3_OBJ, KT_DES_OBJ, KT_DEF_VAL );
  AddOption( KT_NA1_ADD, KT_NA2_ADD, KT_NA3_ADD, KT_DES_ADD, KT_DEF_VAL );
  AddOption( KT_NA1_GET, KT_NA2_GET, KT_NA3_GET, KT_DES_GET, KT_DEF_VAL );
  AddOption( KT_NA1_ALT, KT_NA2_ALT, KT_NA3_ALT, KT_DES_ALT, KT_DEF_VAL );
  AddOption( KT_NA1_FRE, KT_NA2_FRE, KT_NA3_FRE, KT_DES_FRE, KT_DEF_VAL );
  AddOption( KT_NA1_SEL, KT_NA2_SEL, KT_NA3_SEL, KT_DES_SEL, KT_DEF_VAL );
  AddOption( KT_NA1_INS, KT_NA2_INS, KT_NA3_INS, KT_DES_INS, KT_DEF_VAL );
  AddOption( KT_NA1_UPD, KT_NA2_UPD, KT_NA3_UPD, KT_DES_UPD, KT_DEF_VAL );
  AddOption( KT_NA1_DEL, KT_NA2_DEL, KT_NA3_DEL, KT_DES_DEL, KT_DEF_VAL );
  AddOption( KT_NA1_ORD, KT_NA2_ORD, KT_NA3_ORD, KT_DES_ORD, KT_DEF_VAL );
  AddOption( KT_NA1_OPT, KT_NA2_OPT, KT_NA3_OPT, KT_DES_OPT, KT_DEF_VAL );
  AddOption( KT_NA1_TIM, KT_NA2_TIM, KT_NA3_TIM, KT_DES_TIM, KT_DEF_VAL );
  AddOption( KT_NA1_CHE, KT_NA2_CHE, KT_NA3_CHE, KT_DES_CHE, KT_DEF_VAL );
  AddOption( KT_NA1_COM, KT_NA2_COM, KT_NA3_COM, KT_DES_COM, KT_DEF_VAL );

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {
    if( oDB.SQLConnect( oMsgList ) ) {

      DBMCli_String sCmd;

      sCmd = "SELECT";
      sCmd += " DESCRIPTION, VALUE";
      sCmd += " FROM SYSDD.SERVER_DB_STATE";
      sCmd += " WHERE UPPER(DESCRIPTION) LIKE 'VTRACE%'";
      sCmd += " OR UPPER(DESCRIPTION) = 'MONITORING'";

      if( oDB.SQLExecute( sCmd, oMsgList ) ) {
        
        SetAllOptions( KT_OFF_VAL );

        DBMCli_String sDesc;
        DBMCli_String sVal;

        SAPDB_Bool bOK = true;

        bOK = oResult.GetLine( sDesc );
        while( bOK ) {
          bOK = oResult.GetPropVal( sDesc, sVal, ";" );
          if( bOK ) {
            sDesc = sDesc.Mid( 1, sDesc.GetLength( ) - 2 );
            sDesc.MakeUpper( );
            sVal = sVal.Mid( 1, sVal.GetLength( ) - 2 );
            sVal.MakeUpper( );

            if( sDesc == DESC_VTRACE ) {
              SetOption( sVal, KT_ON_VAL );
            } else if ( sDesc == DESC_CHECK ) {
              SetOption( KT_NA2_CHE, KT_ON_VAL );
            }

          }
        }

        bRC = true;
      }

      oDB.SQLRelease( oMsgList );

    }

  } else {
    bRC = true;
  }

  if( bRC ) {
    bRC = false;
    
    DBMCli_String sCmd( DBMCLI_CMD_TRACE_PROTOPT );

    if( oDB.Execute( sCmd, oMsgList ) ) {

      DBMCli_String sOpt;
      DBMCli_String sDesc;

      SAPDB_Bool bOK = true;

      bOK = oResult.GetLine( sOpt );
      while( bOK ) {
        bOK = oResult.GetPropVal( sOpt, sDesc, "\t" );
        if( bOK ) {
          sOpt.Trim( );
          sDesc.Trim( );

          DBMCli_KernelTraceProtOpt oKernelTraceProtOpt( sOpt, sDesc );
          m_aKernelTraceProtOpt.Add( oKernelTraceProtOpt );
        }
      }

      bRC = true;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_KernelTrace :: OptionsOn
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_KernelTrace :: OptionsOn ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( ( oDB.GetState().Value() == DBMCLI_DBSTATE_COLD ) ||
      ( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM )    ) {

    DBMCli_String sCmd( UTL_CMD_DIAGVTRACE );

    sCmd += " ";
    sCmd += OptionSpecList( UTL_CMD_DIAGVTRACEON );

    if( oDB.UTLConnect( oMsgList ) ) {
      if( oDB.UTLExecute( sCmd, oMsgList ) ) {
        bRC = Refresh( oMsgList );
      }
      oDB.UTLRelease( oMsgList );
    }
  } else {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_KernelTrace :: OptionsOff
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_KernelTrace :: OptionsOff ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( ( oDB.GetState().Value() == DBMCLI_DBSTATE_COLD ) ||
      ( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM )    ) {

    DBMCli_String sCmd( UTL_CMD_DIAGVTRACE );

    sCmd += " ";
    sCmd += OptionSpecList( UTL_CMD_DIAGVTRACEOFF );
  
    if( oDB.UTLConnect( oMsgList ) ) {
      if( oDB.UTLExecute( sCmd, oMsgList ) ) {
        bRC = Refresh( oMsgList );
      }
      oDB.UTLRelease( oMsgList );
    }
  } else {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_KernelTrace :: Make
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_KernelTrace :: Make ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_TRACE_PROT );

  sCmd += " ";
  sCmd += ProtOptSpecList( );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_KernelTrace :: Flush
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_KernelTrace :: Flush ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( ( oDB.GetState().Value() == DBMCLI_DBSTATE_COLD ) ||
      ( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM )    ) {

    if( oDB.NodeInfo().Version().GetNumber() < 0x7040000 ) {
		  DBMCli_String sCmd( UTL_CMD_DIAGVTRACEFLUSH );

      if( oDB.GetState().Value() == DBMCLI_DBSTATE_COLD ) {
        if( oDB.UTLConnect( oMsgList ) ) {
          if( oDB.UTLExecute( sCmd, oMsgList ) ) {
            bRC = Refresh( oMsgList );
          }
          oDB.UTLRelease( oMsgList );
        }
      } else {
        if( oDB.SQLConnect( oMsgList ) ) {
          if( oDB.SQLExecute( sCmd, oMsgList ) ) {
            bRC = Refresh( oMsgList );
          }
          oDB.SQLRelease( oMsgList );
        }
      }
    } else {
      bRC = oDB.Execute( DBMCli_String( DBMCLI_CMD_TRACE_FLUSH ), oMsgList );
    }
  } else {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_KernelTrace :: Clear
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_KernelTrace :: Clear ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( ( oDB.GetState().Value() == DBMCLI_DBSTATE_COLD ) ||
      ( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM )    ) {

    if( oDB.NodeInfo().Version().GetNumber() < 0x7040000 ) {
      DBMCli_String sCmd( UTL_CMD_DIAGVTRACE );

      sCmd += " ";
      sCmd += UTL_CMD_DIAGVTRACECLEAR;
    
      if( oDB.UTLConnect( oMsgList ) ) {
        if( oDB.UTLExecute( sCmd, oMsgList ) ) {
          bRC = Refresh( oMsgList );
        }
        oDB.UTLRelease( oMsgList );
      }
    } else {
      bRC = oDB.Execute( DBMCli_String( DBMCLI_CMD_TRACE_CLEAR ), oMsgList );
    }
  } else {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_KernelTrace :: IndexByOption
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_KernelTrace :: IndexByName ( const DBMCli_String & sName,
                                              SAPDB_Int             nNum )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nOption = 0;

  while( nOption < m_aKernelTraceOption.GetSize( ) && !bFound ) {
    if( m_aKernelTraceOption[nOption].Name( nNum ) == sName ) {
      bFound = true;
    } else {
      nOption++;
    }
  }

  if( bFound ) {
    return nOption;
  } else {
    return -1;
  }
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_KernelTrace :: AddOption
  -----------------------------------------------------------------------------
*/
void DBMCli_KernelTrace :: AddOption ( const DBMCli_String & sName1, 
                                       const DBMCli_String & sName2,
                                       const DBMCli_String & sName3,
                                       const DBMCli_String & sDescription,
                                       const DBMCli_String & sValue )
{
  DBMCli_KernelTraceOption oKernelTraceOption( sName1 , sName2, sName3, sDescription, sValue );
  m_aKernelTraceOption.Add( oKernelTraceOption );
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_KernelTrace :: SetAllOptions
  -----------------------------------------------------------------------------
*/
void DBMCli_KernelTrace :: SetAllOptions ( const DBMCli_String & sValue )
{
  SAPDB_Int nIndex = 0;

  for( nIndex = 0; nIndex < m_aKernelTraceOption.GetSize( ); nIndex++) {
    m_aKernelTraceOption[nIndex].SetValue( sValue );
  }
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_KernelTrace :: SetOption
  -----------------------------------------------------------------------------
*/
void DBMCli_KernelTrace :: SetOption ( const DBMCli_String & sName, 
                                       const DBMCli_String & sValue )
{
  SAPDB_Int nIndex = IndexByName( sName, 2 );  

  if( nIndex >= 0 ) {
    m_aKernelTraceOption[nIndex].SetValue( sValue );
  }
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_KernelTrace :: OptionSpecList
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_KernelTrace :: OptionSpecList(const DBMCli_String & sAction)
{
    SAPDB_Int nIndex;
    DBMCli_String sSpec;

    sSpec = "";

    for( nIndex = 0; nIndex < m_aKernelTraceOption.GetSize( ); nIndex++) {
      if( m_aKernelTraceOption[nIndex].Selected( ) ) {
        sSpec += m_aKernelTraceOption[nIndex].Name1( );
        sSpec += " ";
      }
    }

    if( sSpec.GetLength( ) == 0 ) {
      sSpec += KT_NA1_DEF;
      sSpec += " ";
    }
    
    sSpec += sAction;

    return sSpec;
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_KernelTrace :: ProtOptSpecList
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_KernelTrace :: ProtOptSpecList( )
{
    SAPDB_Int nIndex;
    DBMCli_String sSpec;

    sSpec = "";

    for( nIndex = 0; nIndex < m_aKernelTraceProtOpt.GetSize( ); nIndex++) {
      if( m_aKernelTraceProtOpt[nIndex].Selected( ) ) {
        sSpec += m_aKernelTraceProtOpt[nIndex].Option( );
      }
    }

    return sSpec;
}
