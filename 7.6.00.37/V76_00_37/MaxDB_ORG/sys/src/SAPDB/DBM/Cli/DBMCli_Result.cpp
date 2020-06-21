/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Result.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Result handling
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
#include "DBM/Cli/DBMCli_Result.hpp"

/*    
  =============================================================================
  class: DBMCli_Result
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Result:: DBMCli_Result
  -----------------------------------------------------------------------------
*/
DBMCli_Result :: DBMCli_Result ( )
{
  Clear( );
} 

DBMCli_Result :: DBMCli_Result ( SAPDB_Int nSize )
               : DBMCli_ResultBuf( nSize )
{
  Clear( );
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Result :: ~DBMCli_Result
  -----------------------------------------------------------------------------
*/
DBMCli_Result :: ~DBMCli_Result ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Result :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Result :: Clear ( )
{
  DBMCli_ResultBuf::Clear( );
  ClearError( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Result :: Check
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Result :: Check ( Msg_List & oMsgList )
{
  DBMCli_String sLine;
  Msg_List      tmpMsg;
  //SAPDB_Int     currentPosition(GetPos());

  ClearError( );

  if( GetLine( sLine ) ) {
    if( sLine.GetLength( ) >= 3 ) {
      if( sLine.Left( 3 ) == DBMCLI_VAL_ERR ) {
        while( GetLine( sLine ) ) {
          // as long as there are more lines,
          // make error messages out of them
          SAPDB_Int nPos1 = 0;
          SAPDB_Int nPos2 = 0;
          nPos2 = sLine.Find( ',', nPos1 );
          if( nPos2 >= 0 ) {
            DBMCli_String sValue = sLine.Mid( nPos1, nPos2 - nPos1 );
            
            m_nReturnCode = atoi( sValue );

            nPos1 = nPos2 + 1;
            m_sErrorText = sLine.Mid( nPos1 );
          } else {
            m_nReturnCode = INT_MIN;
            m_sErrorText = sLine.Mid( nPos1 );
          }

          m_sErrorText.Trim( );
          tmpMsg = Msg_List(DBMCLI_COMPID, __FILE__, __LINE__, 
                            Msg_List::Error, m_nReturnCode,
                            m_sErrorText, 0);
          tmpMsg.Overrule(oMsgList);
          oMsgList = tmpMsg;
        }
      }
    }
  }

  //SetPos(currentPosition);
  return ( m_nReturnCode == 0 );
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Result :: ClearError
  -----------------------------------------------------------------------------
*/
void DBMCli_Result :: ClearError ( )
{
  m_nReturnCode = 0;
  m_sErrorValue.Empty( );
  m_sErrorText.Empty( );

  m_nExtReturnCode = 0;
  m_sExtErrorText.Empty( ); 
}
