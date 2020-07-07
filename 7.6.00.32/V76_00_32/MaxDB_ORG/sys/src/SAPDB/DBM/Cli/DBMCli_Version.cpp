/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Version.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Version Class
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
#include "DBM/Cli/DBMCli_Version.hpp"

/*    
  =============================================================================
  class: DBMCli_Version
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Version :: DBMCli_Version
  -----------------------------------------------------------------------------
*/
DBMCli_Version :: DBMCli_Version ( )
{
  Clear( );
}

DBMCli_Version :: DBMCli_Version ( const DBMCli_String & sVersion )
{
  SetName( sVersion );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Version :: ~DBMCli_Version
  -----------------------------------------------------------------------------
*/
DBMCli_Version :: ~DBMCli_Version ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Version :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Version :: Clear ( )
{
  m_sVersion.Empty( );

  m_nMajor = 0;
  m_nMinor = 0;
  m_nRevision = 0;
  m_nPatch = 0;
  m_nNumber = 0;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Version :: GetName 
  -----------------------------------------------------------------------------
*/
DBMCli_String & DBMCli_Version :: GetName ( ) 
{ 
  return m_sVersion;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Version :: SetName
  -----------------------------------------------------------------------------
*/
void DBMCli_Version :: SetName ( const DBMCli_String & sName ) 
{ 
  AnalyzeVersion( sName, false );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Version :: SetBuild
  -----------------------------------------------------------------------------
*/
void DBMCli_Version :: SetBuild ( const DBMCli_String & sBuild ) 
{ 
  AnalyzeVersion( sBuild, true );
} // end DBMCli_Version :: SetBuild

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Version :: AnalyzeVersion
  -----------------------------------------------------------------------------
*/
void DBMCli_Version :: AnalyzeVersion ( const DBMCli_String & sVersion,
                                        bool                  bBuild)
{
  SAPDB_Int nPos1;
  SAPDB_Int nPos2;

  DBMCli_String sMajor;
  DBMCli_String sMinor;
  DBMCli_String sRevision; 
  DBMCli_String sPatch;

  Clear( );

  if( !sVersion.IsEmpty( ) ) {
    nPos1 = 0;
    if (bBuild) {
      nPos1 = sVersion.Find( "DBMSERVER ", nPos1 );
      nPos1 = nPos1 + (int) strlen("DBMSERVER ");
    } // end if
    nPos2 = sVersion.Find( '.', nPos1 );
    if( nPos2 > 0 ) {
      sMajor = sVersion.Mid(nPos1, nPos2 - nPos1 );
      nPos1 = nPos2 + 1;
      nPos2 = sVersion.Find( '.', nPos1 );
      if( nPos2 > 0 ) {
        sMinor = sVersion.Mid( nPos1, nPos2 - nPos1 );
        nPos1 = nPos2 + 1;
        nPos2 = sVersion.Find( '.', nPos1 );
        if( nPos2 < 0 ) {
          nPos2 = sVersion.Find( ' ', nPos1 );
        } // end if
        if( nPos2 > 0 ) {
          sRevision = sVersion.Mid( nPos1, nPos2 - nPos1 );
          sPatch = sVersion.Mid( nPos2 + 1 );
          if (bBuild) {
            nPos1 = nPos2 + 1;
            nPos1 = sVersion.Find( "BUILD ", nPos1 );
            nPos1 = nPos1 + (int) strlen("BUILD ");
            nPos2 = sVersion.Find( '-', nPos1 );
            if (nPos2 > 0) {
              sPatch = sVersion.Mid(nPos1, nPos2 - nPos1 );
            } // end if
          } // end if
        } else {
          sRevision = sVersion.Mid( nPos1 );
        }
      } else {
        sMinor= sVersion.Mid( nPos1 );
      }
    } else {
      sMajor = sVersion;
    }
  }

  m_nMajor = atoi( sMajor );
  m_nMinor = atoi( sMinor );
  m_nRevision = atoi( sRevision );
  if (bBuild) {
    m_nPatch = atoi( sPatch );
  } // end if

  m_nNumber = m_nMajor * 0x1000000 + m_nMinor * 0x10000 + m_nRevision * 0x100 + m_nPatch;

  m_sVersion.Format( "%d.%d.%d.%d", m_nMajor, m_nMinor, m_nRevision, m_nPatch );
}