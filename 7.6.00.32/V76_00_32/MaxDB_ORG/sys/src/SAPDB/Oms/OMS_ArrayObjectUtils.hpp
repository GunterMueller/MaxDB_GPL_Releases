/*!
* \file    OMS_ArrayObjectUtils.hpp
* \author  ReinerSi
* \brief   Utilities for array objects
*/

/*

========== licence begin  GPL
Copyright (c) 2003-2005 SAP AG

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

#ifndef OMS_ARRAY_UTILS_HPP
#define OMS_ARRAY_UTILS_HPP

#include "Oms/OMS_Handle.hpp"

/*!
* Class containing utility methods
*/
class AFX_EXT_CLASS OmsArrayUtils
{
public: 
  static char getHexCode( char val ); 
  static char* convertToHex( char* buf, OmsTypeInt8 val ); 


  /*! 
   * Utility function, that writes a int8 value to a file in hex format
   * 
   * \param h OmsHandle
   * \param fh file handle
   * \param val value to write
   */
  static void writeInt8( OmsHandle& h, OmsTypeInt4 fh, const OmsTypeInt8 val );

  static void tmemcpy( void* tgt, const void* src, OmsTypeUInt4 cnt ); 
  static void tmemset( void* tgt, OmsTypeByte val, OmsTypeUInt4 cnt ); 

  static void tstrcat(  char* tgt, OmsTypeUInt4 len, const char* s );
};

//==========================================================================

/*!
* Template for abstract enumerator
*/
template< class T >
class OmsEnumerator 
{
public: 

  virtual const T& operator() () = 0; 
  virtual void operator++() = 0; 
  virtual operator bool() = 0; 
  virtual void reset() = 0; 

  virtual ~OmsEnumerator() { }
};

/*! 
* Template for abstract appender 
*/
template< class T >
class OmsAppender
{
public: 

  virtual void append( T& elem ) = 0; 
  virtual ~OmsAppender() { }
};


/*!
* Enumerator for array based data storage. 
*/
template< class T >
class OmsArrayEnumerator : public OmsEnumerator< T >
{
public: 

  OmsArrayEnumerator( const T* data, const OmsTypeUInt4 size )
    : m_data( data )
    , m_size( size )
    , m_count( 0 )
  { }

  const T& operator() () 
  {
    return *m_data; 
  }

  void operator++() 
  {
    ++m_data; 
    ++m_count; 
  }

  operator bool() 
  {
    return ( m_count < m_size ); 
  }

  void reset() 
  {
    m_data -= m_count; 
    m_count = 0; 
  }

private: 
  const T* m_data; 
  const OmsTypeUInt4 m_size; 
  mutable OmsTypeUInt4 m_count;
};

/*!
 * Appender for array based data storage
 */
template< class T >
class OmsArrayAppender : public OmsAppender< T >
{
public: 

  OmsArrayAppender( T* data, const OmsTypeUInt4 size )
    : m_data( data )
    , m_size( size )
  { }

  void append( T& elem )
  { 
    *m_data = elem; 
    ++m_data; 
  }

private: 
  T* m_data; 
  const OmsTypeUInt4 m_size; 
};

#endif // #ifndef OMS_ARRAY_UTILS_HPP
