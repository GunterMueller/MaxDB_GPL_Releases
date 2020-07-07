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

#ifndef _DBMCLI_STUFF_HPP_
#define _DBMCLI_STUFF_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <new.h>
#include "DBM/Cli/DBMCli_Common.hpp"
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/

#if !defined( BUFFER_BYTE )
typedef unsigned char BUFFER_BYTE;
#endif

/*!
  -----------------------------------------------------------------------------
  Class:        DBMCli_String
  -----------------------------------------------------------------------------
  description:  A string class.
  -----------------------------------------------------------------------------
*/
class DBMCli_String
{
  public:
    /*!  
      Chapter: Constructors/Destruction
    */

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_String
      -------------------------------------------------------------------------
      description:  Constructors 
      prototypes:   6
      -------------------------------------------------------------------------
    */
    DBMCli_String ( );
    DBMCli_String ( const DBMCli_String& stringSrc );
    DBMCli_String ( const Tools_DynamicUTF8String& stringSrc );
    DBMCli_String ( char ch, int nRepeat = 1 );
    DBMCli_String ( const char* lpch, int nLength );
    DBMCli_String ( const unsigned char* lpsz );
    DBMCli_String ( const char* lpsz );

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_String
      -------------------------------------------------------------------------
      description:  Destructor 
      -------------------------------------------------------------------------
    */
    ~DBMCli_String ( );

    /*!  
      EndChapter: Constructors/Destruction
    */

  protected:
    DBMCli_String( int nLen );


  public:

    /*!  
      Chapter: The String as an Array
    */

    /*!
      -------------------------------------------------------------------------
      function:     GetLength
      -------------------------------------------------------------------------
      description:  Returns the number of characters in a string object. 

      return value: A count of the bytes in the string.
      -------------------------------------------------------------------------
    */
    int GetLength( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     IsEmpty
      -------------------------------------------------------------------------
      description:  Tests whether a string object contains no characters.
      return value: Nonzero if the string object has 0 length; otherwise 0.
      -------------------------------------------------------------------------
    */
    bool IsEmpty( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     Empty
      -------------------------------------------------------------------------
      description:  Forces a string to have 0 length. 
      -------------------------------------------------------------------------
    */
    void Empty( );

    /*!
      -------------------------------------------------------------------------
      function:     GetAt
      -------------------------------------------------------------------------
      description:  Returns the character at a given position.
      arguments:    nIndex [IN] - Zero-based index of the character in the 
                                  string object. 
      return value: A char containing the character at the specified position 
                    in the string.
      -------------------------------------------------------------------------
    */
    char GetAt( int nIndex ) const;

    /*!
      -------------------------------------------------------------------------
      function:     operator []
      -------------------------------------------------------------------------
      description:  Returns the character at a given position.
                    Operator substitution for GetAt.
      arguments:    nIndex [IN] - Zero-based index of the character in the 
                                  string object. 
      return value: A char containing the character at the specified position 
                    in the string.
      -------------------------------------------------------------------------
    */
    char operator[]( int nIndex ) const;

    /*!
      -------------------------------------------------------------------------
      function:     SetAt
      -------------------------------------------------------------------------
      description:  Sets a character at a given position.
      arguments:    nIndex  [IN]  - Zero-based index of the character in the 
                                    string object. 
                    ch      [IN]  - The character to insert.
      -------------------------------------------------------------------------
    */
    void SetAt( int nIndex, char ch );

    /*!
      -------------------------------------------------------------------------
      function:     operator const char*
      -------------------------------------------------------------------------
      description:  Directly accesses characters stored in a string object as 
                    a C-style string.
      return value: A character pointer to the string’s data.
      -------------------------------------------------------------------------
    */
    operator const char       *( ) const;
    
    /*!
        @brief cast to Tools_DynamicUTF8String

        @return new instance of a Tools_DynamicUTF8String
    */
    operator const Tools_DynamicUTF8String () const;
    
    /*!
      -------------------------------------------------------------------------
      function:     operator const void*
      -------------------------------------------------------------------------
      description:  Directly accesses characters stored in a string object as 
                    a C-style string.
      return value: A character pointer to the string’s data.
      -------------------------------------------------------------------------
    */
    operator const void*( ) const;

    /*!  
      EndChapter: The String as an Array
    */

    /*!  
      Chapter: Assignment/Concatenation
    */

    /*!
      -------------------------------------------------------------------------
      function:     operator =
      -------------------------------------------------------------------------
      description:  Assigns a new value to a string object.
      prototypes:   4
      -------------------------------------------------------------------------
    */
    const DBMCli_String& operator=( const DBMCli_String& stringSrc );
    const DBMCli_String& operator=( const Tools_DynamicUTF8String& stringSrc );
    const DBMCli_String& operator=( char ch );
    const DBMCli_String& operator=( const unsigned char* lpsz );
    const DBMCli_String& operator=( const char* lpsz );

    /*!
      -------------------------------------------------------------------------
      function:     operator +
      -------------------------------------------------------------------------
      description:  Concatenates two strings and returns a new string.
      prototypes:   5
      -------------------------------------------------------------------------
    */
    friend DBMCli_String operator+( const DBMCli_String& string1, const DBMCli_String& string2 );
    friend DBMCli_String operator+( const DBMCli_String& string, char ch );
    friend DBMCli_String operator+( char ch, const DBMCli_String& string );
    friend DBMCli_String operator+( const DBMCli_String& string, const char* lpsz );
    friend DBMCli_String operator+( const char* lpsz, const DBMCli_String& string );

    /*!
      -------------------------------------------------------------------------
      function:     operator +=
      -------------------------------------------------------------------------
      description:  Concatenates a new string to the end of an existing string. 
      prototypes:   3
      -------------------------------------------------------------------------
    */
    const DBMCli_String& operator+=( const DBMCli_String& string ); 
    const DBMCli_String& operator+=( char ch );
    const DBMCli_String& operator+=( const char* lpsz );
    /*!  
      EndChapter: Assignment/Concatenation
    */

    /*!  
      Chapter: Comparision
    */
    /*!
      -------------------------------------------------------------------------
      function:     operator == <, etc. 
      -------------------------------------------------------------------------
      description:  Comparison Operators. 
      prototypes:   18
      -------------------------------------------------------------------------
    */
    friend bool operator==( const DBMCli_String& s1, const DBMCli_String& s2 );
    friend bool operator==( const DBMCli_String& s1, const char* s2 );
    friend bool operator==( const char* s1, const DBMCli_String& s2 );

    friend bool operator!=( const DBMCli_String& s1, const DBMCli_String& s2 );
    friend bool operator!=( const DBMCli_String& s1, const char* s2 );
    friend bool operator!=( const char* s1, const DBMCli_String& s2);
  
    friend bool operator<( const DBMCli_String& s1, const DBMCli_String& s2 );
    friend bool operator<( const DBMCli_String& s1, const char* s2 );
    friend bool operator<( const char* s1, const DBMCli_String& s2 );
  
    friend bool operator>( const DBMCli_String& s1, const DBMCli_String& s2 );
    friend bool operator>( const DBMCli_String& s1, const char* s2 );
    friend bool operator>( const char* s1, const DBMCli_String& s2 );
  
    friend bool operator<=( const DBMCli_String& s1, const DBMCli_String& s2 );
    friend bool operator<=( const DBMCli_String& s1, const char* s2 );
    friend bool operator<=( const char* s1, const DBMCli_String& s2 );
  
    friend bool operator>=( const DBMCli_String& s1, const DBMCli_String& s2 );
    friend bool operator>=( const DBMCli_String& s1, const char* s2 );
    friend bool operator>=( const char* s1, const DBMCli_String& s2 );

    /*!
      -------------------------------------------------------------------------
      function:     Compare
      -------------------------------------------------------------------------
      description:  Compares two strings (case sensitive).
      -------------------------------------------------------------------------
    */
    int Compare( const char* lpsz ) const; 

    /*!
      -------------------------------------------------------------------------
      function:     CompareNoCase
      -------------------------------------------------------------------------
      description:  Compares two strings (case insensitive).
      -------------------------------------------------------------------------
    */
    int CompareNoCase( const char* lpsz ) const;

    /*!
      -------------------------------------------------------------------------
      function:     Collate
      -------------------------------------------------------------------------
      description:  Compares two strings (case sensitive, uses locale-specific 
                    information).
      -------------------------------------------------------------------------
    */
    int Collate( const char* lpsz ) const;

    /*!  
      EndChapter: Comparision
    */

    /*!  
      Chapter: Extraction
    */
    /*!
      -------------------------------------------------------------------------
      function:     Mid
      -------------------------------------------------------------------------
      description:  Mid Extracts the middle part of a string. 
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_String Mid( int nFirst ) const;
    DBMCli_String Mid( int nFirst, int nCount ) const;

    /*!
      -------------------------------------------------------------------------
      function:     Left
      -------------------------------------------------------------------------
      description:  Extracts the left part of a string.
      -------------------------------------------------------------------------
    */
    DBMCli_String Left( int nCount ) const;

    /*!
      -------------------------------------------------------------------------
      function:     Right
      -------------------------------------------------------------------------
      description:  Extracts the right part of a string.
      -------------------------------------------------------------------------
    */
    DBMCli_String Right( int nCount ) const;

    /*!
      -------------------------------------------------------------------------
      function:     SpanIncluding  
      -------------------------------------------------------------------------
      description:  Extracts a substring that contains only the characters 
                    in a set.
      -------------------------------------------------------------------------
    */
    DBMCli_String SpanIncluding( const char* lpszCharSet ) const;

    /*!
      -------------------------------------------------------------------------
      function:     SpanExcluding
      -------------------------------------------------------------------------
      description:  Extracts a substring that contains only the characters 
                    not in a set.
      -------------------------------------------------------------------------
    */
    DBMCli_String SpanExcluding( const char* lpszCharSet ) const;

    /*!  
      EndChapter: Extraction
    */

    /*!  
      Chapter: Other conversion
    */
    /*!
      -------------------------------------------------------------------------
      function:     MakeUpper
      -------------------------------------------------------------------------
      description:  Converts all the characters in this string to uppercase 
                    characters.
      -------------------------------------------------------------------------
    */
    void MakeUpper( );

    /*!
      -------------------------------------------------------------------------
      function:     MakeLower
      -------------------------------------------------------------------------
      description:  Converts all the characters in this string to lowercase 
                    characters.
      -------------------------------------------------------------------------
    */
    void MakeLower( );

    /*!
      -------------------------------------------------------------------------
      function:     MakeReverse
      -------------------------------------------------------------------------
      description:  Reverses the characters in this string.
      -------------------------------------------------------------------------
    */
    void MakeReverse( );

    /*!
      -------------------------------------------------------------------------
      function:     Format
      -------------------------------------------------------------------------
      description:  Format the string as sprintf does.
      -------------------------------------------------------------------------
    */
    void Format( const char* lpszFormat, ... );

    /*!
      -------------------------------------------------------------------------
      function:     TrimLeft
      -------------------------------------------------------------------------
      description:  Trim leading whitespace characters from the string.
      -------------------------------------------------------------------------
    */
    void TrimLeft( );

    /*!
      -------------------------------------------------------------------------
      function:     TrimRight
      -------------------------------------------------------------------------
      description:  Trim trailing whitespace characters from the string.
      -------------------------------------------------------------------------
    */
    void TrimRight( );

    /*!
      -------------------------------------------------------------------------
      function:     Trim
      -------------------------------------------------------------------------
      description:  Trim leading and trailing whitespace characters from the 
                    string.
      -------------------------------------------------------------------------
    */
    void Trim( );

    /*!
      -------------------------------------------------------------------------
      function:     TrimLeft
      -------------------------------------------------------------------------
      description:  Trim leading specified characters from the string.
      -------------------------------------------------------------------------
    */
    void TrimLeft(char cTrim);

    /*!
      -------------------------------------------------------------------------
      function:     TrimRight
      -------------------------------------------------------------------------
      description:  Trim trailing specified characters from the string.
      -------------------------------------------------------------------------
    */
    void TrimRight(char cTrim);

    /*!
      -------------------------------------------------------------------------
      function:     Trim
      -------------------------------------------------------------------------
      description:  Trim leading and trailing specified characters from the 
                    string.
      -------------------------------------------------------------------------
    */
    void Trim(char cTrim);

    /*!  
      EndChapter: Other conversion
    */

    /*!  
      Chapter: Searching
    */

    /*!
      -------------------------------------------------------------------------
      function:     Find
      -------------------------------------------------------------------------
      description:  Finds a character or substring inside a larger string.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    int Find( char ch, int nStart = 0 ) const;
    int Find( const char* lpszSub, int nStart = 0 ) const;

    /*!
      -------------------------------------------------------------------------
      function:     ReverseFind
      -------------------------------------------------------------------------
      description:  Finds a character inside a larger string; starts from 
                    the end.
      -------------------------------------------------------------------------
    */
    int ReverseFind( char ch ) const;

    /*!
      -------------------------------------------------------------------------
      function:     FindOneOf
      -------------------------------------------------------------------------
      description:  Finds the first matching character from a set.
      -------------------------------------------------------------------------
    */
    int FindOneOf( const char* lpszCharSet ) const;
    int FindOneOf( const char* lpszCharSet, int nStart ) const;

    /*!  
      EndChapter: Searching
    */

    /*!  
      Chapter: Buffer Access
    */

    /*!
      -------------------------------------------------------------------------
      function:     GetBuffer
      -------------------------------------------------------------------------
      description:  Returns a pointer to the characters in the string. 
      -------------------------------------------------------------------------
    */
    char* GetBuffer( int nMinBufLength );

    /*!
      -------------------------------------------------------------------------
      function:     GetBufferSetLength
      -------------------------------------------------------------------------
      description:  Returns a pointer to the characters in the string, 
                    truncating to the specified length.
      -------------------------------------------------------------------------
    */
    char* GetBufferSetLength( int nMinBufLength );

    /*!
      -------------------------------------------------------------------------
      function:     ReleaseBuffer
      -------------------------------------------------------------------------
      description:  Releases control of the buffer returned by GetBuffer. 
      -------------------------------------------------------------------------
    */
    void ReleaseBuffer( int nNewLength = -1 );

    /*!
      -------------------------------------------------------------------------
      function:     FreeExtra
      -------------------------------------------------------------------------
      description:  Removes any overhead of this string object by freeing any 
                    extra memory previously allocated to the string. 
      -------------------------------------------------------------------------
    */
    void FreeExtra( );

    /*!  
      EndChapter: Buffer Access
    */

  private:
    void NewString( int nLen = 0 );
    void ReallocString( int nLen );
    void ResizeString( int nLen );
    void ForceResizeString( int nLen );

  private:
    int m_nLen;
    int m_nAlloc;
    char* m_pszString;
};

/*
  -----------------------------------------------------------------------------
  The String as an Array
  -----------------------------------------------------------------------------
*/
inline int DBMCli_String::GetLength( ) const
{
  return m_nLen;
}

inline bool DBMCli_String::IsEmpty( ) const
{
  return m_nLen == 0;
}

inline void DBMCli_String::Empty( )
{
  ReallocString( 0 );
}

inline char DBMCli_String::GetAt( int nIndex ) const
{
  ASSERT( nIndex >= 0 && nIndex < m_nLen );
  return m_pszString[ nIndex ];
}

inline char DBMCli_String::operator[]( int nIndex ) const
{
  ASSERT( nIndex >= 0 && nIndex < m_nLen );
  return GetAt( nIndex );
}

inline void DBMCli_String::SetAt( int nIndex, char ch )
{
  ASSERT( nIndex >= 0 && nIndex < m_nLen );
  m_pszString[ nIndex ] = ch;
}

inline DBMCli_String::operator const char*( ) const
{
  return m_pszString;
}
inline DBMCli_String::operator const Tools_DynamicUTF8String () const
{
  return Tools_DynamicUTF8String(m_pszString);
}
inline DBMCli_String::operator const void*( ) const
{
  return (const unsigned char*) m_pszString;
}

/*
  -----------------------------------------------------------------------------
  Other conversions
  -----------------------------------------------------------------------------
*/
inline void DBMCli_String::MakeUpper( )
{
#ifdef _WIN32
  strupr( m_pszString );
#else
  for ( int i = 0; i < m_nLen; i++ )
    m_pszString[ i ] = toupper( ( int ) m_pszString[ i ] );
#endif
}

inline void DBMCli_String::MakeLower( )
{
#ifdef _WIN32
  strlwr( m_pszString );
#else
  for ( int i = 0; i < m_nLen; i++ )
    m_pszString[ i ] = tolower( ( int ) m_pszString[ i ] );
#endif
}

inline void DBMCli_String::MakeReverse( )
{
#ifdef _WIN32
  strrev( m_pszString );
#else
  char chTemp;

  for ( int i = 0; i < m_nLen / 2; i++ )
  {
    chTemp = m_pszString[ i ];
    m_pszString[ i ] = m_pszString[ m_nLen - 1 - i ];
    m_pszString[ m_nLen - i - 1 ] = chTemp;
  }
#endif
}

/*
  -----------------------------------------------------------------------------
  Comparison
  -----------------------------------------------------------------------------
*/
inline int DBMCli_String::Compare( const char* lpsz ) const
{
  return strcmp( m_pszString, lpsz );
}

inline int DBMCli_String::CompareNoCase( const char* lpsz ) const
{
#ifdef _WIN32
  return stricmp( m_pszString, lpsz );
#else
  return strcasecmp( m_pszString, lpsz );
#endif
}

inline int DBMCli_String::Collate( const char* lpsz ) const
{
  return strcoll( m_pszString, lpsz );
}

inline bool operator==( const DBMCli_String& s1, const DBMCli_String& s2 )
  {  return s1.Compare( s2 ) == 0; }
inline bool operator==( const DBMCli_String& s1, const char* s2 )
  { return s1.Compare( s2 ) == 0; }
inline bool operator==( const char* s1, const DBMCli_String& s2 )
  {  return s2.Compare( s1 ) == 0; }
inline bool operator!=( const DBMCli_String& s1, const DBMCli_String& s2 )
  {  return s1.Compare( s2 ) != 0; }
inline bool operator!=( const DBMCli_String& s1, const char* s2 )
  {  return s1.Compare( s2 ) != 0; }
inline bool operator!=(const char* s1, const DBMCli_String& s2)
  { return s2.Compare( s1 ) != 0; }
inline bool operator<( const DBMCli_String& s1, const DBMCli_String& s2 )
  { return s1.Compare( s2 ) < 0; }
inline bool operator<( const DBMCli_String& s1, const char* s2 )
  { return s1.Compare( s2 ) < 0; }
inline bool operator<( const char* s1, const DBMCli_String& s2 )
  { return s2.Compare( s1 ) > 0; }
inline bool operator>( const DBMCli_String& s1, const DBMCli_String& s2 )
  { return s1.Compare( s2 ) > 0; }
inline bool operator>( const DBMCli_String& s1, const char* s2 )
  { return s1.Compare( s2 ) > 0; }
inline bool operator>( const char* s1, const DBMCli_String& s2 )
  { return s2.Compare( s1 ) < 0; }
inline bool operator<=( const DBMCli_String& s1, const DBMCli_String& s2 )
  { return s1.Compare( s2 ) <= 0; }
inline bool operator<=( const DBMCli_String& s1, const char* s2 )
  { return s1.Compare( s2 ) <= 0; }
inline bool operator<=( const char* s1, const DBMCli_String& s2 )
  { return s2.Compare( s1 ) >= 0; }
inline bool operator>=( const DBMCli_String& s1, const DBMCli_String& s2 )
  { return s1.Compare( s2 ) >= 0; }
inline bool operator>=( const DBMCli_String& s1, const char* s2 )
  { return s1.Compare( s2 ) >= 0; }
inline bool operator>=( const char* s1, const DBMCli_String& s2 )
  { return s2.Compare( s1 ) <= 0; }

/*
  -----------------------------------------------------------------------------
  Extraction
  -----------------------------------------------------------------------------
*/
inline DBMCli_String DBMCli_String::Mid( int nFirst ) const
{
  ASSERT( nFirst >= 0 && nFirst < m_nLen || ( nFirst == 0 && m_nLen == 0 ) );

  return DBMCli_String( m_pszString + nFirst );
}

inline DBMCli_String DBMCli_String::Mid( int nFirst, int nCount ) const
{
  ASSERT( nFirst >= 0 && nFirst < m_nLen );
  ASSERT( nFirst + nCount <= m_nLen );

  return DBMCli_String( m_pszString + nFirst, nCount );
}

inline DBMCli_String DBMCli_String::Left( int nCount ) const
{
  ASSERT( nCount >= 0 && nCount <= m_nLen );

  return DBMCli_String( m_pszString, nCount );
}

inline DBMCli_String DBMCli_String::Right( int nCount ) const
{
  ASSERT( nCount >= 0 && nCount < m_nLen );

  return DBMCli_String( m_pszString + m_nLen - nCount, nCount );
}

inline DBMCli_String DBMCli_String::SpanIncluding( const char* lpszCharSet ) const
{
  return Left( (int) strspn( m_pszString, lpszCharSet) );
}

inline DBMCli_String DBMCli_String::SpanExcluding( const char* lpszCharSet ) const
{
  return Left( (int) strcspn( m_pszString, lpszCharSet ) );
}

/*
  -----------------------------------------------------------------------------
  Buffer Access
  -----------------------------------------------------------------------------
*/
inline void DBMCli_String::FreeExtra( )
{
  ForceResizeString( m_nLen );
}

/*! EndClass: DBMCli_String */


/*!
  -----------------------------------------------------------------------------
  Class:        DBMCli_Buffer
  -----------------------------------------------------------------------------
  description:  A buffer class.
  -----------------------------------------------------------------------------
*/

template< class TYPE >
class DBMCli_Buffer
{
  public:
    // Construction / Destruction
    DBMCli_Buffer( );
    DBMCli_Buffer( int nSize );
    DBMCli_Buffer( const DBMCli_Buffer& rOther );
    ~DBMCli_Buffer( );
  
  // Operators
    DBMCli_Buffer& operator=( const DBMCli_Buffer& rOther );
    TYPE& operator[ ]( int nIndex );
    TYPE operator[ ]( int nIndex ) const;
    operator const TYPE*( ) const; 
    operator TYPE*( ); 

  // Attributes
    int Size( ) const;

  // Resizing
    void Alloc( int nSize );
    void Realloc( int nSize );
    void Resize( int nSize );
    void ForceResize( int nSize );

  private:
    TYPE* m_pData;   // the actual array of data
    int m_nSize;     // # of elements
};


/*
#if !defined(SAPDBSTL) && !defined(__PLACEMENT_NEW_INLINE)

  // special new operator that does nothing: needed to call constructors
  inline void* operator new( size_t s, void* p )
  {
    return p;
  }

#endif
*/

// call the destructors. For intrinsics this inline function will 
// expand to nothing.
template< class TYPE >
inline void ConstructElements( TYPE* pElements, int nCount )
{
  ASSERT( nCount >= 0 );

  // call the constructor(s)
  for ( ; nCount--; pElements++ )
    new( ( void* ) pElements ) TYPE;
}

// call the destructors. For intrinsics this inline function will
// expand to nothing.
template< class TYPE >
inline void DestructElements( TYPE* pElements, int nCount )
{
  ASSERT( nCount >= 0 );

  // call the destructor(s)
  for ( ; nCount--; pElements++ )
    pElements->~TYPE( );
}

// Copy nCount elements from pSrc to pDest
template< class TYPE >
inline void CopyElements( TYPE* pDest, const TYPE* pSrc, int nCount )
{
  // Default is element-copy using assignment
  while ( nCount-- )
    *pDest++ = *pSrc++;
}

// Copy nCount elements from pSrc to pDest
template< class TYPE >
inline void CopyElements( TYPE* pDest, const TYPE& rSrc, int nCount )
{
  while ( nCount-- )
    *pDest++ = rSrc;
}

template< class TYPE >
inline void CopyConstructElements( TYPE* pDest, const TYPE* pSrc, int nCount )
{
  // Construct elements with copy constructor
  while ( nCount-- )
    new( ( void* ) pDest++ ) TYPE( *pSrc++ );
}

template< class TYPE >
inline void CopyConstructElements( TYPE* pDest, const TYPE& rSrc, int nCount )
{
  // Construct elements with copy constructor
  while ( nCount-- )
    new( ( void* ) pDest++ ) TYPE( rSrc );
}

template< class TYPE >
DBMCli_Buffer< TYPE >::DBMCli_Buffer( )
{
  m_nSize = 0;
  m_pData = NULL;
}

template< class TYPE >
inline DBMCli_Buffer< TYPE >::DBMCli_Buffer( int nSize )
{
  m_nSize = nSize;
  m_pData = ( TYPE* ) new BUFFER_BYTE[ nSize * sizeof( TYPE ) ];
  
  ConstructElements( m_pData, nSize );
}

template< class TYPE >
inline DBMCli_Buffer< TYPE >::DBMCli_Buffer( const DBMCli_Buffer< TYPE >& rOther )
{
  m_nSize = rOther.m_nSize;
  m_pData = ( TYPE* ) new BUFFER_BYTE[ m_nSize * sizeof( TYPE ) ];
  
  CopyConstructElements( m_pData, rOther.m_pData, rOther.m_nSize );
}

template< class TYPE >
DBMCli_Buffer< TYPE >::~DBMCli_Buffer( )
{
  DestructElements( m_pData, m_nSize );

  delete [ ] ( BUFFER_BYTE* ) m_pData;
}

template< class TYPE >
DBMCli_Buffer< TYPE >& DBMCli_Buffer< TYPE >::operator=( const DBMCli_Buffer< TYPE >& rOther )
{
  ASSERT( this != &rOther );   // No self assignement

  Resize( rOther.m_nSize );
  CopyElements( m_pData, rOther.m_pData, rOther.m_nSize );

  return *this;
}

template< class TYPE >
inline int DBMCli_Buffer< TYPE >::Size( ) const
{
  return m_nSize;
}

template< class TYPE >
inline TYPE& DBMCli_Buffer< TYPE >::operator[]( int nIndex )
{
  ASSERT( nIndex >= 0 && nIndex <= m_nSize );

  return m_pData[ nIndex ];
}

template< class TYPE >
inline TYPE DBMCli_Buffer< TYPE >::operator[]( int nIndex ) const
{
  ASSERT( nIndex >= 0 && nIndex <= m_nSize );

  return m_pData[ nIndex ];
}

template< class TYPE >
inline DBMCli_Buffer< TYPE >::operator const TYPE*( ) const
{
  return m_pData;
}

template< class TYPE >
inline DBMCli_Buffer< TYPE >::operator TYPE*( )
{
  return m_pData;
}

// Allocate a buffer of nLen elements. Works only
// for buffers constructed with the default constructor.
template< class TYPE >
void DBMCli_Buffer< TYPE >::Alloc( int nSize )
{
  ASSERT( m_nSize == 0 && m_pData == NULL );

  m_pData = ( TYPE* ) new BUFFER_BYTE[ nSize * sizeof( TYPE ) ];
  ConstructElements( m_pData, nSize );
  
  m_nSize = nSize;
}

// Resize the buffer to at least nLen elements.
// In case of reallocation the buffer content will be lost.
// Realloc is faster than Resize for instrinsics and trivial
// objects but slower for non-trivial objects like CString's.
template< class TYPE >
void DBMCli_Buffer< TYPE >::Realloc( int nSize )
{
  if ( nSize > m_nSize )
  {
    DestructElements( m_pData, m_nSize );
    delete [ ] ( BUFFER_BYTE* ) m_pData;

    m_pData = ( TYPE* ) new BUFFER_BYTE[ nSize * sizeof( TYPE ) ];
    ConstructElements( m_pData, nSize );
  }
  m_nSize = nSize;
}

// Resize the buffer to at least nSize elements. The buffer
// content will be preserved in any case.
// Resize is slower than Realloc for intrinsics and
// trivial objects but faster for non-trivial objects
// like CString's.
template< class TYPE >
void DBMCli_Buffer< TYPE >::Resize( int nSize )
{
  if ( nSize > m_nSize )
  {
    TYPE* pNewData = ( TYPE* ) new BUFFER_BYTE[ nSize * sizeof( TYPE ) ];

    // construct new elements
    ConstructElements( pNewData + m_nSize, nSize - m_nSize );
    
    // this will eventually make a shallow copy, but this ok since
    // the old buffer will be deleted
    SAPDB_memcpy( pNewData, m_pData, sizeof( TYPE ) * m_nSize );

    // delete old buffer (no destructors called)
    delete [ ] ( BUFFER_BYTE* ) m_pData;

    m_pData = pNewData;
  }

  m_nSize = nSize;
}

// Same as Resize with the exception that
// the buffer will be exact nSize big.
template< class TYPE >
void DBMCli_Buffer< TYPE >::ForceResize( int nSize )
{
  if ( nSize > m_nSize )
  {
    TYPE* pNewData = ( TYPE* ) new BUFFER_BYTE[ nSize * sizeof( TYPE ) ];

    // construct new elements
    ConstructElements( pNewData + m_nSize, nSize - m_nSize );
    
    // this will eventually make a shallow copy, but this ok since
    // the old buffer will be deleted
    SAPDB_memcpy( pNewData, m_pData, sizeof( TYPE ) * m_nSize );

    // delete old buffer (no destructors called)
    delete [ ] ( BUFFER_BYTE* ) m_pData;

    m_pData = pNewData;
    m_nSize = nSize;
  }
  else if ( nSize < m_nSize )
  {
    TYPE* pNewData = ( TYPE* ) new BUFFER_BYTE[ nSize * sizeof( TYPE ) ];
    
    // this will eventually make a shallow copy, but this ok since
    // the old buffer will be deleted
    SAPDB_memcpy( pNewData, m_pData, sizeof( TYPE ) * nSize );

    // destruct unnecessary elements
    DestructElements( m_pData + nSize, m_nSize - nSize );
    
    // delete old buffer (no destructors called)
    delete [ ] ( BUFFER_BYTE* ) m_pData;

    m_pData = pNewData;
    m_nSize = nSize;
  }  

}

/*
// Better versions for intrinsics

inline void CopyElements( char* pDest, const char* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( char ) ); }
inline void CopyElements( short* pDest, const short* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( short ) ); }
inline void CopyElements( int* pDest, const int* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( int ) ); }
inline void CopyElements( long* pDest, const long* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( long ) ); }
inline void CopyElements( float* pDest, const float* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( float ) ); }
inline void CopyElements( double* pDest, const double* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( double ) ); }
inline void CopyConstructElements( char* pDest, const char* pSrc, int nCount )
  {  SAPDB_memcpy( pDest, pSrc, nCount * sizeof( char ) ); }
inline void CopyConstructElements( short* pDest, const short* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( short ) ); }
inline void CopyConstructElements( int* pDest, const int* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( int ) ); }
inline void CopyConstructElements( long* pDest, const long* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( long ) ); }
inline void CopyConstructElements( float* pDest, const float* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( float ) ); }
inline void CopyConstructElements( double* pDest, const double* pSrc, int nCount )
  { SAPDB_memcpy( pDest, pSrc, nCount * sizeof( double ) ); }

inline DBMCli_Buffer< char >& DBMCli_Buffer< char >::operator=( const DBMCli_Buffer< char >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( char ) );

  return *this;
}

inline DBMCli_Buffer< short >& DBMCli_Buffer< short >::operator=( const DBMCli_Buffer< short >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( char ) );

  return *this;
}

inline DBMCli_Buffer< int >& DBMCli_Buffer< int >::operator=( const DBMCli_Buffer< int >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( int ) );

  return *this;
}

inline DBMCli_Buffer< long >& DBMCli_Buffer< long >::operator=( const DBMCli_Buffer< long >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( long ) );

  return *this;
}

inline DBMCli_Buffer< float >& DBMCli_Buffer< float >::operator=( const DBMCli_Buffer< float >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( float ) );

  return *this;
}

inline DBMCli_Buffer< double >& DBMCli_Buffer< double >::operator=( const DBMCli_Buffer< double >& rOther )
{
  Realloc( rOther.m_nSize );
  SAPDB_memcpy( m_pData, rOther.m_pData, rOther.m_nSize * sizeof( double ) );

  return *this;
}
*/

/*! EndClass: DBMCli_Buffer */


/*!
  -----------------------------------------------------------------------------
  Class:        DBMCli_Array
  -----------------------------------------------------------------------------
  description:  A array class.
  -----------------------------------------------------------------------------
*/
template< class TYPE, class ARG_TYPE >
class DBMCli_Array 
{
  public:
    /*!  
      Chapter: Constructors
    */
    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Array
      -------------------------------------------------------------------------
      description:  Constructors 
      prototypes: 3
      -------------------------------------------------------------------------
    */
    DBMCli_Array( int nGrowBy = 8 );
    DBMCli_Array( const DBMCli_Array& rOther ); 
    DBMCli_Array& operator=( const DBMCli_Array& rOther ); 
    /*!  
      EndChapter: Constructors
    */

    /*!  
      Chapter: Attributes
    */

    /*!
      -------------------------------------------------------------------------
      function:     IsEmpty
      -------------------------------------------------------------------------
      description:  Tests whether a this array contains no elements.
      -------------------------------------------------------------------------
    */
    bool IsEmpty( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     GetSize
      -------------------------------------------------------------------------
      description:  Gets the number of elements in this array.
      -------------------------------------------------------------------------
    */
    int GetSize( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     GetUpperBound
      -------------------------------------------------------------------------
      description:  Returns the largest valid index.
      -------------------------------------------------------------------------
    */
    int GetUpperBound( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     SetSize
      -------------------------------------------------------------------------
      description:  Sets the number of elements to be contained in this array.
      -------------------------------------------------------------------------
    */
    void SetSize( int nNewSize, int nGrowBy = -1 );

    /*!  
      EndChapter: Attributes
    */

    /*!  
      Chapter: Operations
    */
    
    /*!
      -------------------------------------------------------------------------
      function:     FreeExtra
      -------------------------------------------------------------------------
      description:  Frees all unused memory above the current upper bound.
      -------------------------------------------------------------------------
    */
    void FreeExtra( );

    /*!
      -------------------------------------------------------------------------
      function:     RemoveAll
      -------------------------------------------------------------------------
      description:  Removes all the elements from this array.
      -------------------------------------------------------------------------
    */
    void RemoveAll( );

    /*!  
      EndChapter: Operations
    */
  
    /*!  
      Chapter: Element Access
    */

    /*!
      -------------------------------------------------------------------------
      function:     GetAt
      -------------------------------------------------------------------------
      description:  Returns the value at a given index.
      -------------------------------------------------------------------------
    */
    TYPE GetAt( int nIndex ) const;

    /*!
      -------------------------------------------------------------------------
      function:     SetAt
      -------------------------------------------------------------------------
      description:  Sets the value for a given index; array not allowed to grow.
      -------------------------------------------------------------------------
    */
    void SetAt( int nIndex, ARG_TYPE newElement );

    /*!
      -------------------------------------------------------------------------
      function:     ElementAt
      -------------------------------------------------------------------------
      description:  Returns a temporary reference to the element pointer within
                    the array.
      -------------------------------------------------------------------------
    */
    TYPE& ElementAt( int nIndex );

    /*!
      -------------------------------------------------------------------------
      function:     GetData
      -------------------------------------------------------------------------
      description:  Allows access to elements in the array. Can be NULL.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    const TYPE* GetData( ) const;
    TYPE* GetData( );

    /*!  
      EndChapter: Element Access
    */

    /*!  
      Chapter: Growing the array
    */

    /*!
      -------------------------------------------------------------------------
      function:     SetAtGrow
      -------------------------------------------------------------------------
      description:  Sets the value for a given index; grows the array if 
                    necessary.
      -------------------------------------------------------------------------
    */
    void SetAtGrow( int nIndex, ARG_TYPE newElement );

    /*!
      -------------------------------------------------------------------------
      function:     Add
      -------------------------------------------------------------------------
      description:  Adds an element to the end of the array; grows the array 
                    if necessary.
      -------------------------------------------------------------------------
    */
    int Add( ARG_TYPE newElement );

    /*!
      -------------------------------------------------------------------------
      function:     Append
      -------------------------------------------------------------------------
      description:  Appends another array to the array; grows the array 
                    if necessary.
      -------------------------------------------------------------------------
    */
    int Append( const DBMCli_Array& src );

    /*!
      -------------------------------------------------------------------------
      function:     Copy
      -------------------------------------------------------------------------
      description:  Copies another array to the array; grows the array 
                    if necessary.
      -------------------------------------------------------------------------
    */
    void Copy( const DBMCli_Array& src );

    /*!  
      EndChapter: Growing the array
    */

    /*!  
      Chapter: Insertion/Removal
    */

    /*!
      -------------------------------------------------------------------------
      function:     InsertAt
      -------------------------------------------------------------------------
      description:  Inserts an element (or all the elements in another array) 
                    at a specified index.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    void InsertAt( int nIndex, ARG_TYPE newElement, int nCount = 1 );

    void InsertAt( int nStartIndex, DBMCli_Array* pNewArray );

    /*!
      -------------------------------------------------------------------------
      function:     RemoveAt
      -------------------------------------------------------------------------
      description:  Removes an element at a specific index.
      -------------------------------------------------------------------------
    */
    void RemoveAt( int nIndex, int nCount = 1 );

    /*!  
      EndChapter: Insertion/Removal
    */

    /*!  
      Chapter: Operators
    */

    /*!
      -------------------------------------------------------------------------
      function:     operator[]
      -------------------------------------------------------------------------
      description:  Sets or gets the element at the specified index.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    TYPE operator[]( int nIndex ) const;

    TYPE& operator[]( int nIndex );

    /*!  
      EndChapter: Operators
    */

  private:
    DBMCli_Buffer< TYPE > m_data;   // the actual data
    int m_nSize;                    // # of elements (upperBound - 1)
    int m_nGrowBy;                  // grow amount
};

// Construction

template< class TYPE, class ARG_TYPE >
inline DBMCli_Array< TYPE, ARG_TYPE >::DBMCli_Array( int nGrowBy ) :
  m_data( nGrowBy )
{
  m_nGrowBy = nGrowBy;
  m_nSize = 0;
}

template< class TYPE, class ARG_TYPE >
inline DBMCli_Array< TYPE, ARG_TYPE >::DBMCli_Array( const DBMCli_Array< TYPE, ARG_TYPE >& rOther ) :
  m_data( rOther.m_data )
{
  m_nGrowBy = rOther.m_nGrowBy;
  m_nSize = rOther.m_nSize;
}

template< class TYPE, class ARG_TYPE >
inline DBMCli_Array< TYPE, ARG_TYPE >& DBMCli_Array< TYPE, ARG_TYPE >::operator=( const DBMCli_Array< TYPE, ARG_TYPE >&
  rOther )
{
  m_data = rOther.m_data;
  m_nSize = rOther.m_nSize;
  m_nGrowBy = rOther.m_nGrowBy;

  return *this;
}

// Attributes

template< class TYPE, class ARG_TYPE >
inline bool DBMCli_Array< TYPE, ARG_TYPE >::IsEmpty( ) const
{
  return m_nSize == 0;
}

template< class TYPE, class ARG_TYPE >
inline int DBMCli_Array< TYPE, ARG_TYPE >::GetSize( ) const
{
  return m_nSize;
}

template< class TYPE, class ARG_TYPE >
inline int DBMCli_Array< TYPE, ARG_TYPE >::GetUpperBound( ) const
{
  return m_nSize - 1;
}

template< class TYPE, class ARG_TYPE >
void DBMCli_Array< TYPE, ARG_TYPE >::SetSize( int nNewSize, int nGrowBy )
{
  m_data.ForceResize( nNewSize );
  m_nSize = nNewSize;

  if ( nGrowBy > 0 )
    m_nGrowBy = nGrowBy;
}

// Operations

template< class TYPE, class ARG_TYPE >
inline void DBMCli_Array< TYPE, ARG_TYPE >::FreeExtra( )
{
  m_data.ForceResize( m_nSize );
}

template< class TYPE, class ARG_TYPE >
inline void DBMCli_Array< TYPE, ARG_TYPE >::RemoveAll( )
{
  m_data.ForceResize( 0 );
  m_nSize = 0;
}

// Element Access

template< class TYPE, class ARG_TYPE >
inline TYPE DBMCli_Array< TYPE, ARG_TYPE >::GetAt( int nIndex ) const
{
  ASSERT( nIndex >= 0 && nIndex < m_nSize );

  return m_data[ nIndex ];
}

template< class TYPE, class ARG_TYPE >
inline void DBMCli_Array< TYPE, ARG_TYPE >::SetAt( int nIndex, ARG_TYPE newElement )
{
  ASSERT( nIndex >= 0 && nIndex < m_nSize );

  m_data[ nIndex ] = newElement;
}

template< class TYPE, class ARG_TYPE >
inline TYPE& DBMCli_Array< TYPE, ARG_TYPE >::ElementAt( int nIndex )
{
  return ( ( TYPE* ) m_data )[ nIndex ];
}

template< class TYPE, class ARG_TYPE >
inline const TYPE* DBMCli_Array< TYPE, ARG_TYPE >::GetData( ) const
{
  return ( const TYPE* ) m_data;
}

template< class TYPE, class ARG_TYPE >
inline TYPE* DBMCli_Array< TYPE, ARG_TYPE >::GetData( )
{
  return ( TYPE* ) m_data;
}

// Growing the array

template< class TYPE, class ARG_TYPE >
void DBMCli_Array< TYPE, ARG_TYPE >::SetAtGrow( int nIndex, ARG_TYPE newElement )
{
  ASSERT( nIndex >= 0 );

  if ( nIndex > m_data.Size( ) - 1 )
    m_data.Resize( nIndex + m_nGrowBy );
  if ( nIndex > m_nSize - 1 )
    m_nSize = nIndex + 1;

  m_data[ nIndex ] = newElement;
}

template< class TYPE, class ARG_TYPE >
int DBMCli_Array< TYPE, ARG_TYPE >::Add( ARG_TYPE newElement )
{
  if ( m_nSize == m_data.Size( ) )
    m_data.Resize( m_nSize + m_nGrowBy );
  m_data[ m_nSize ] = newElement;

  return m_nSize++;
}

template< class TYPE, class ARG_TYPE >
int DBMCli_Array< TYPE, ARG_TYPE >::Append( const DBMCli_Array< TYPE, ARG_TYPE >& src )
{
  if ( m_nSize + src.m_nSize > m_data.Size( ) )
    m_data.Resize( m_nSize + src.m_nSize + m_nGrowBy );
  
  CopyElements( ( TYPE* ) m_data, ( const TYPE* ) src.m_data, src.m_nSize );
  m_nSize += src.m_nSize;

  return m_nSize;
}

template< class TYPE, class ARG_TYPE >
void DBMCli_Array< TYPE, ARG_TYPE >::Copy( const DBMCli_Array< TYPE, ARG_TYPE >& src )
{
  m_nSize = src.m_nSize;
  m_data = src.m_data;
  
//   return m_nSize;
}

// Insertion / Removal

template< class TYPE, class ARG_TYPE >
void DBMCli_Array< TYPE, ARG_TYPE >::InsertAt( int nIndex, ARG_TYPE newElement, int nCount  )
{
  ASSERT( nIndex >= 0 && nCount >= 0 );

  if ( m_nSize + nCount > m_data.Size( ) )
    m_data.Resize( m_nSize + nCount + m_nGrowBy );

  if ( nIndex < m_nSize )
  {
    // Destroy elements before overwriting them
    DestructElements( ( TYPE* ) m_data + m_nSize, nCount );

    // We can make shallow copies, because the old elements will be overwritten.
    // The memory blocks may overlap so use SAPDB_memmove!
    SAPDB_memmove( ( TYPE* ) m_data + nIndex + nCount, ( TYPE* ) m_data + nIndex, ( m_nSize - nIndex ) * sizeof( TYPE ) );  

    // Copyconstruct the new elements
    CopyConstructElements( ( TYPE* ) m_data + nIndex, newElement, nCount );
  }
  else
    CopyElements( ( TYPE* ) m_data, newElement, nCount );
  
  m_nSize += nCount;
}

template< class TYPE, class ARG_TYPE >
void DBMCli_Array< TYPE, ARG_TYPE >::InsertAt( int nStartIndex, DBMCli_Array* pNewArray )
{
  ASSERT( this != pNewArray );   // No self insertion
  ASSERT( nStartIndex >= 0 );

  if ( m_nSize + pNewArray->m_nSize > m_data.Size( ) )
    m_data.Resize( m_nSize + pNewArray->m_nSize + m_nGrowBy );

  if ( nStartIndex < m_nSize )
  {
    // Destroy elements before overwriting them
    DestructElements( ( TYPE* ) m_data + m_nSize, pNewArray->m_nSize );

    // We can make shallow copies, because the old elements will be overwritten.
    // The memory blocks may overlap so use SAPDB_memmove!
    SAPDB_memmove( ( TYPE* ) m_data + nStartIndex + pNewArray->m_nSize, ( TYPE* ) m_data + nStartIndex, ( m_nSize - nStartIndex ) * sizeof( TYPE ) );  

    // Copyconstruct the new elements
    CopyConstructElements( ( TYPE* ) m_data + nStartIndex, ( TYPE* ) pNewArray->m_data, pNewArray->m_nSize );
  }
  else
    CopyElements( ( TYPE* ) m_data + nStartIndex, ( TYPE* ) pNewArray->m_data, pNewArray->m_nSize );

  m_nSize += pNewArray->m_nSize;
}

template< class TYPE, class ARG_TYPE >
void DBMCli_Array< TYPE, ARG_TYPE >::RemoveAt( int nIndex, int nCount )
{
  ASSERT( nIndex >= 0 && nIndex < m_nSize );
  ASSERT( nCount > 0 );

  DestructElements( ( TYPE* ) m_data + nIndex, nCount );

  // We can make shallow copies, because the old elements will no longer be used.
  // The memory blocks may overlap so use SAPDB_memmove!
  SAPDB_memmove( ( TYPE* ) m_data + nIndex, ( TYPE* ) m_data + nIndex + nCount, ( m_nSize - nIndex - 1 ) * sizeof( TYPE ) );

  // Reinit unused elements
  ConstructElements( ( TYPE* ) m_data + m_nSize - nCount, nCount );

  m_nSize--;
}

// Operators

template< class TYPE, class ARG_TYPE >
inline TYPE DBMCli_Array< TYPE, ARG_TYPE >::operator[]( int nIndex ) const
{
  ASSERT( nIndex >= 0 && nIndex < m_nSize );

  return m_data[ nIndex ];
}

template< class TYPE, class ARG_TYPE >
inline TYPE& DBMCli_Array< TYPE, ARG_TYPE >::operator[]( int nIndex )
{
  ASSERT( nIndex >= 0 && nIndex < m_nSize );

  return m_data[ nIndex ];
}

/*! EndClass: DBMCli_Array */

#endif // _DBMCLI_STUFF_HPP_

