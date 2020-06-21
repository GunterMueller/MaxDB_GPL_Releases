/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Stuff.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Stuff
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
#include <stdarg.h>

#include "hsp77.h"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "SAPDBCommon/SAPDB_string.h"

/*    
  =============================================================================
  class: DBMCli_String
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  Construction/Destruction
  -----------------------------------------------------------------------------
*/

// default constructor: construct an empty string
DBMCli_String::DBMCli_String( )
{
  NewString( );
}

// copy constructor
DBMCli_String::DBMCli_String( const DBMCli_String& stringSrc )
{
  NewString( stringSrc.m_nLen );
  SAPDB_memcpy( m_pszString, stringSrc.m_pszString, m_nLen );
}

// constructor: create a string of length nRepeat and fill it with ch
DBMCli_String::DBMCli_String( char ch, int nRepeat )
{
  NewString( nRepeat );
  memset( m_pszString, ch, nRepeat );
//  for ( int i = 0; i < nRepeat; i++ )
//    m_pszString[ i ] = ch;
}

// constructor: create a string with the c-string lpch, copy only first
// nLength characters
DBMCli_String::DBMCli_String( const char* lpch, int nLength )
{
  NewString( nLength );
  SAPDB_memcpy( m_pszString, lpch, nLength );
}

// constructor: create a string with the c-string lpsz
DBMCli_String::DBMCli_String( const unsigned char* lpsz )
{
  NewString( (int) strlen( ( const char* ) lpsz ) );
  SAPDB_memcpy( m_pszString, lpsz, m_nLen );
}

// constructor: create a string with the c-string lpsz
DBMCli_String::DBMCli_String( const char* lpsz )
{
  NewString( (int) strlen( lpsz ) );
  SAPDB_memcpy( m_pszString, lpsz, m_nLen );
}

// constructor: create a string with the Tools_DynamicUTF8String
DBMCli_String::DBMCli_String( const Tools_DynamicUTF8String& stringSrc )
{
  NewString( stringSrc.Length() );
  SAPDB_memcpy( m_pszString, stringSrc.Data(), m_nLen );
}

// destructor: free the character buffer
DBMCli_String::~DBMCli_String( )
{
  delete [ ] m_pszString;
}

// protected constructor: create an uninitialized string
DBMCli_String::DBMCli_String( int nLen )
{
  NewString( nLen );
}

/*
  -----------------------------------------------------------------------------
  Assignement/Concatenation
  -----------------------------------------------------------------------------
*/

// assignement operator
const DBMCli_String& DBMCli_String::operator=( const DBMCli_String& rOther )
{
  ReallocString( rOther.m_nLen );
  SAPDB_memcpy( m_pszString, rOther.m_pszString, m_nLen );

  return *this;
}

const DBMCli_String& DBMCli_String::operator=( const Tools_DynamicUTF8String& rOther )
{
  ReallocString( rOther.Length() );
  SAPDB_memcpy( m_pszString, rOther.Data(), m_nLen );

  return *this;
}


const DBMCli_String& DBMCli_String::operator=( char ch )
{
  ReallocString( 1 );
  *m_pszString = ch;
  
  return *this;
}

const DBMCli_String& DBMCli_String::operator=( const unsigned char* lpsz )
{
  int nLen = (int) strlen( ( const char* ) lpsz );
  ReallocString( nLen );
  SAPDB_memcpy( m_pszString, lpsz, nLen );

  return *this;
}

const DBMCli_String& DBMCli_String::operator=( const char* lpsz )
{
  int nLen = (int) strlen( lpsz );
  ReallocString( nLen );
  SAPDB_memcpy( m_pszString, lpsz, nLen );

  return *this;
}

// friend function: concatenate string1 and string2
DBMCli_String operator+( const DBMCli_String& string1, const DBMCli_String& string2 )
{
  DBMCli_String result( string1.m_nLen + string2.m_nLen );
  SAPDB_memcpy( result.m_pszString, string1.m_pszString, string1.m_nLen );
  SAPDB_memcpy( result.m_pszString + string1.m_nLen, string2.m_pszString, string2.m_nLen );

  return result;
}

// friend function: concatenate string and ch
DBMCli_String operator+( const DBMCli_String& string, char ch )
{
  DBMCli_String result( string.m_nLen + 1 );
  SAPDB_memcpy( result.m_pszString, string.m_pszString, string.m_nLen );
  result.m_pszString[ result.m_nLen - 1 ] = ch;
  
  return result;
}

// friend function: concatenate ch and string
DBMCli_String operator+( char ch, const DBMCli_String& string )
{
  DBMCli_String result( string.m_nLen + 1 );
  *result.m_pszString = ch;
  SAPDB_memcpy( result.m_pszString + 1, string.m_pszString, string.m_nLen );
  
  return result;
}

// friend function: concatenate string and lpsz
DBMCli_String operator+( const DBMCli_String& string, const char* lpsz )
{
  int nLen = (int) strlen( lpsz );
  DBMCli_String result( string.m_nLen + nLen );
  SAPDB_memcpy( result.m_pszString, string.m_pszString, string.m_nLen );
  SAPDB_memcpy( result.m_pszString + string.m_nLen, lpsz, nLen );

  return result;
}

// friend function: concatenate lpsz and string
DBMCli_String operator+( const char* lpsz, const DBMCli_String& string )
{
  int nLen = (int) strlen( lpsz );
  DBMCli_String result( nLen + string.m_nLen );
  SAPDB_memcpy( result.m_pszString, lpsz, nLen );
  SAPDB_memcpy( result.m_pszString + nLen, string.m_pszString, string.m_nLen );

  return result;
}

const DBMCli_String& DBMCli_String::operator+=( const DBMCli_String& string )
{
  ResizeString( m_nLen + string.m_nLen );   // ResizeString will update m_nLen
  SAPDB_memcpy( m_pszString + m_nLen - string.m_nLen,
          string.m_pszString, string.m_nLen );
  
  return *this;
}

const DBMCli_String& DBMCli_String::operator+=( char ch )
{
  ResizeString( m_nLen + 1 );
  m_pszString[ m_nLen - 1 ] = ch;

  return *this;
}

const DBMCli_String& DBMCli_String::operator+=( const char* lpsz )
{
  int nLen = (int) strlen( lpsz );
  ResizeString( m_nLen + nLen );   // ResizeString will update m_nLen
  SAPDB_memcpy( m_pszString + m_nLen - nLen, lpsz, nLen );
  
  return *this;
}

/*
  -----------------------------------------------------------------------------
  Other Conversions
  -----------------------------------------------------------------------------
*/
void DBMCli_String::Format( const char* lpszFormat, ... )
{
  // Let's hope the output will be no more than 1024 chars
  ReallocString( 5010 );
  
  va_list argList;
  va_start( argList, lpszFormat );
//  m_nLen = vsprintf( m_pszString, lpszFormat, argList );
  m_nLen = sp77vsprintf( m_pszString, 5000, lpszFormat, argList );
  va_end( argList );
  m_nLen = (m_nLen < 0) ? 0 : m_nLen;

//  ASSERT( m_nLen >= 0 && m_nLen <= 1024 );
  
  FreeExtra( );
}

void DBMCli_String::TrimLeft( )
{
  char* pszFirst = m_pszString;

  while ( *pszFirst == ' ' || *pszFirst == '\t' || *pszFirst == '\n' )
    pszFirst++;

  m_nLen -= (int) (pszFirst - m_pszString);
  // The memory blocks overlap, we have to use SAPDB_memmove
  SAPDB_memmove( m_pszString, pszFirst, m_nLen + 1 );
}

void DBMCli_String::TrimRight( )
{
  char* pszLast = m_pszString + m_nLen - 1;

  while ( *pszLast == ' ' || *pszLast == '\t' || *pszLast == '\n' )
    pszLast--;

  m_nLen = (int) (pszLast - m_pszString + 1);
  m_pszString[ m_nLen ] = '\0';
}

void DBMCli_String::Trim( )
{
  TrimLeft( );
  TrimRight( );
}

void DBMCli_String::TrimLeft(char cTrim)
{
  char* pszFirst = m_pszString;

  while ( *pszFirst == cTrim )
    pszFirst++;

  m_nLen -= (int) (pszFirst - m_pszString);
  // The memory blocks overlap, we have to use SAPDB_memmove
  SAPDB_memmove( m_pszString, pszFirst, m_nLen + 1 );
}

void DBMCli_String::TrimRight(char cTrim)
{
  char* pszLast = m_pszString + m_nLen - 1;

  while ( *pszLast == cTrim )
    pszLast--;

  m_nLen = (int) (pszLast - m_pszString + 1);
  m_pszString[ m_nLen ] = '\0';
}

void DBMCli_String::Trim(char cTrim)
{
  TrimLeft(cTrim);
  TrimRight(cTrim);
}

/*
  -----------------------------------------------------------------------------
  Buffer Access
  -----------------------------------------------------------------------------
*/
// Resize the character buffer so that it is at least nMinBufLength long and
// return a pointer to it for low level access. The buffer must be released
// with ReleaseBuffer before the next DBMCli_String operation is beeing executed.
char* DBMCli_String::GetBuffer( int nMinBufLength )
{
  ResizeString( nMinBufLength );

  return m_pszString;
}

// Same as GetBuffer with the exception that the buffer will be exact
// nLength long.
char* DBMCli_String::GetBufferSetLength( int nLength )
{
  ForceResizeString( nLength );

  return m_pszString;
}

// Release the buffer previously locked with GetBuffer
void DBMCli_String::ReleaseBuffer( int nNewLength )
{
  if ( nNewLength < 0 )
    m_nLen = (int) strlen( m_pszString );
  else
  {
    m_nLen = nNewLength;
    m_pszString[ m_nLen ] = '\0';
  }
}

/*
  -----------------------------------------------------------------------------
  Searching
  -----------------------------------------------------------------------------
*/
int DBMCli_String::Find( char ch, int nStart ) const
{
  char* pchFound = strchr( m_pszString + nStart, ch );

  if ( pchFound )
    return (int) (pchFound - m_pszString);
  else
    return -1;
}

int DBMCli_String::Find( const char* lpszSub, int nStart ) const
{
  char* pszFound = strstr( m_pszString + nStart, lpszSub );

  if ( pszFound )
    return (int) (pszFound - m_pszString);
  else
    return -1;
}

int DBMCli_String::ReverseFind( char ch ) const
{
  char* pchFound = strrchr( m_pszString, ch );

  if ( pchFound )
    return (int) (pchFound - m_pszString);
  else
    return -1;
}

int DBMCli_String::FindOneOf( const char* lpszCharSet ) const
{
  char* pszFound = strpbrk( m_pszString, lpszCharSet );

  if ( pszFound )
    return (int) (pszFound - m_pszString);
  else
    return -1;
}

int DBMCli_String::FindOneOf( const char* lpszCharSet, int nStart ) const
{
  char* pszFound = strpbrk( m_pszString + nStart, lpszCharSet );

  if ( pszFound )
    return (int) (pszFound - m_pszString);
  else
    return -1;
}

/*
  -----------------------------------------------------------------------------
  Private Functions
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_String::NewString
  -----------------------------------------------------------------------------
*/
void DBMCli_String::NewString( int nLen )
{
  m_nLen = nLen;
  m_nAlloc = nLen;
  m_pszString = new char[ nLen + 1 ];
  m_pszString[ nLen ] = '\0';
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_String::ReallocString
  -----------------------------------------------------------------------------
*/
// Resize the buffer to at least nLen chars.
// In case of reallocation the buffer content will be lost.
void DBMCli_String::ReallocString( int nLen )
{
  if ( nLen > m_nAlloc )
  {
    delete [ ] m_pszString;
    NewString( nLen );
  }
  else
  {
    m_pszString[ nLen ] = '\0';
    m_nLen = nLen;
  }
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_String::ResizeString
  -----------------------------------------------------------------------------
*/
// resizes the character buffer to at least nLen
// characters, the content of the buffer will be preserved.
void DBMCli_String::ResizeString( int nLen )
{
  if ( nLen > m_nAlloc )
  {
    char* pNewString = new char[ nLen + 1 ];
    SAPDB_memcpy( pNewString, m_pszString, m_nAlloc );
    delete [ ] m_pszString;
    m_pszString = pNewString;
    m_nAlloc = nLen;
  }

  m_nLen = nLen;
  m_pszString[ nLen ] = '\0';
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_String::ForceResizeString
  -----------------------------------------------------------------------------
*/
// Same as ResizeString with the exception that
// the buffer will be exact nLen long.
void DBMCli_String::ForceResizeString( int nLen )
{
  if ( nLen != m_nAlloc )
  {
    m_nAlloc = nLen;
    char* pNewString = new char[ nLen + 1 ];
    SAPDB_memcpy( pNewString, m_pszString, nLen );
    delete [ ] m_pszString;
    m_pszString = pNewString;
  }
  
  m_nLen = nLen;
  m_pszString[ nLen ] = '\0';
}

/*    
  =============================================================================
  class: DBMCli_Buffer
  =============================================================================
*/

// Better versions for intrinsics

template<> 
DBMCli_Buffer< char >& DBMCli_Buffer< char >::operator=( const DBMCli_Buffer< char >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( char ) );

  return *this;
}

template<> 
DBMCli_Buffer< short >& DBMCli_Buffer< short >::operator=( const DBMCli_Buffer< short >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( char ) );

  return *this;
}

template<> 
DBMCli_Buffer< int >& DBMCli_Buffer< int >::operator=( const DBMCli_Buffer< int >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( int ) );

  return *this;
}

template<> 
DBMCli_Buffer< long >& DBMCli_Buffer< long >::operator=( const DBMCli_Buffer< long >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( long ) );

  return *this;
}

template<> 
DBMCli_Buffer< float >& DBMCli_Buffer< float >::operator=( const DBMCli_Buffer< float >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( float ) );

  return *this;
}

template<> 
DBMCli_Buffer< double >& DBMCli_Buffer< double >::operator=( const DBMCli_Buffer< double >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( double ) );

  return *this;
}
