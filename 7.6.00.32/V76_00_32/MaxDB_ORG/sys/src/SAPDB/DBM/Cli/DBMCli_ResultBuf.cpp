/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_ResultBuf.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Result buffer handling.
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
#include "DBM/Cli/DBMCli_ResultBuf.hpp"

/*    
  =============================================================================
  class: DBMCli_ResultBuf
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_ResultBuf:: DBMCli_ResultBuf
  -----------------------------------------------------------------------------
*/
DBMCli_ResultBuf :: DBMCli_ResultBuf ( )
{
  Clear( );
} 

DBMCli_ResultBuf :: DBMCli_ResultBuf ( SAPDB_Int nSize )
                  : DBMCli_String( '\0', nSize )
{
  Clear( );
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_ResultBuf :: ~DBMCli_ResultBuf
  -----------------------------------------------------------------------------
*/
DBMCli_ResultBuf :: ~DBMCli_ResultBuf ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Result :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_ResultBuf :: Clear ( )
{
  Empty( );
  SetPos( 0 );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: GetPos
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_ResultBuf :: GetPos ( ) 
{
  return m_nPos;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: SetPos
  -----------------------------------------------------------------------------
*/
void DBMCli_ResultBuf :: SetPos ( SAPDB_Int nPos ) 
{
  m_nPos = nPos;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: GetPart
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: GetPart ( DBMCli_String & sPart,
                                         SAPDB_Int       nMaxLen )
{
  SAPDB_Int nPos1 = 0;
  SAPDB_Int nLen = 0;

  sPart = "";

  if( m_nPos < GetLength( ) ) {
    if( nMaxLen == -1 ) {
      sPart = Mid( m_nPos );
      m_nPos = GetLength( );
    } else {
      if( m_nPos + nMaxLen < GetLength( ) ) {
        nLen = nMaxLen;
      } else {
        nLen = GetLength( ) - m_nPos;
      }
      sPart = Mid( m_nPos, nLen );
      m_nPos += nLen;
    }
    return true;
  } else {
    return false;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: GetLine
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: GetLine ( DBMCli_String & sLine )
{ 
  SAPDB_Int nPos1 = 0;

  sLine = "";

  if( m_nPos < GetLength( ) ) {
    nPos1 = Find( '\n', m_nPos );
    if( nPos1 >= 0 ) {
      if( nPos1 > 0 ) {
        if( GetAt( nPos1 - 1 ) == '\r' ) {
          sLine = Mid( m_nPos, nPos1 - m_nPos - 1);
          m_nPos = nPos1 + 1;
        } else {
          sLine = Mid( m_nPos, nPos1 - m_nPos );
          m_nPos = nPos1 + 1;
        }
      } else {
        sLine = Mid( m_nPos, nPos1 - m_nPos );
        m_nPos = nPos1 + 1;
      }
    } else {
      sLine = Mid( m_nPos );
      m_nPos = GetLength( );
    }
    return true;
  } else {
    return false;
  }
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: GetFullLine
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: GetFullLine ( DBMCli_String & sLine )
{ 
  SAPDB_Int nPos1 = 0;

  sLine = "";

  if( m_nPos < GetLength( ) ) {
    nPos1 = Find( '\n', m_nPos );
    if( nPos1 >= 0 ) {
      if( nPos1 > 0 ) {
        if( GetAt( nPos1 - 1 ) == '\r' ) {
          sLine = Mid( m_nPos, nPos1 - m_nPos - 1);
          m_nPos = nPos1 + 1;
        } else {
          sLine = Mid( m_nPos, nPos1 - m_nPos );
          m_nPos = nPos1 + 1;
        }
      } else {
        sLine = Mid( m_nPos, nPos1 - m_nPos );
        m_nPos = nPos1 + 1;
      }

      return true;

    } else {

      return false;

    }

  } else {
    return false;
  }
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: GetField
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: GetField (       DBMCli_String & sField, 
                                          const DBMCli_String & sSeperators )
{
  SAPDB_Int nPos1 = 0;

  sField = "";

  if( m_nPos < GetLength( ) ) {
    nPos1 = FindOneOf( sSeperators, m_nPos );
    if( nPos1 >= 0 ) {
      sField = Mid( m_nPos, nPos1 - m_nPos );
      m_nPos = nPos1 + 1;
    } else {
      sField = Mid( m_nPos );
      m_nPos = GetLength( );
    }
    return true;
  } else {
    return false;
  }
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: GetPropVal
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf::GetPropVal ( DBMCli_String & sProperty, 
                                          DBMCli_String & sValue )
{
  return GetPropVal( sProperty, sValue, "=" );
} 

SAPDB_Bool DBMCli_ResultBuf::GetPropVal (       DBMCli_String & sProperty, 
                                                DBMCli_String & sValue,
                                          const DBMCli_String & sSeperators )
{
  DBMCli_String sLine;

  sProperty = "";
  sValue = "";

  if( GetLine(sLine) ) {
    if( !sLine.IsEmpty( ) ) {
      SAPDB_Int nPos1 = sLine.FindOneOf( sSeperators );
      if( nPos1 >= 0 ) {
        sProperty = sLine.Left( nPos1 ); 
        sProperty.Trim(); 
        if( nPos1 + 1 < sLine.GetLength( ) ) {
          sValue = sLine.Mid( nPos1 + 1 ); 
          sValue.Trim( ); 
        }
      } else {
        sProperty = sLine; 
        sProperty.Trim( ); 
      }
    }
    return true;
  } else {
    return false;
  }
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: SkipLine
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: SkipLine ( )
{
  DBMCli_String sLine;

  return GetLine( sLine );
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: SkipField
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: SkipField ( const DBMCli_String & sSeperators )
{
  DBMCli_String sField;

  return GetField( sField, sSeperators );
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: SkipBlanks
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: SkipBlanks ( )
{
  SAPDB_Int nPos1 = 0;
  
  if( m_nPos < GetLength( ) ) {
    SAPDB_Bool bBlank = true;
    while( m_nPos < GetLength( ) && bBlank ) {
      if( GetAt( m_nPos ) == ' ' ) {
        m_nPos++;
      } else {
        bBlank = false;
      }
    }
    return true;
  } else {
    return false;
  }
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_ResultBuf :: EndOfBuffer
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_ResultBuf :: EndOfBuffer ( )
{
  if( m_nPos < GetLength( ) ) {
    return false;
  } else {
    return true;
  }
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

