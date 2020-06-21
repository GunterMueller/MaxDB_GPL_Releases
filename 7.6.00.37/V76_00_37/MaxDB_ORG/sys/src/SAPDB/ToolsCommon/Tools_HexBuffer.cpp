/*!
  @file           Tools_HexBuffer.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          buffer handling and conversion - Implementation

\if EMIT_LICENCE

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



\endif
*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>

#include "SAPDB/ToolsCommon/Tools_HexBuffer.hpp"

/*! -----------------------------------------------------------------------------
    Constructor 
    -----------------------------------------------------------------------------
 */
Tools_HexBuffer :: Tools_HexBuffer
  ( )
  :m_pHexBuffer(0)
  ,m_pBinBuffer(0)
{
  SetData("");
} // end Tools_HexBuffer :: Tools_HexBuffer

/*! -----------------------------------------------------------------------------
    Constructor 
    -----------------------------------------------------------------------------
 */
Tools_HexBuffer :: Tools_HexBuffer
  ( const char * szBuffer )
  :m_pHexBuffer(0)
  ,m_pBinBuffer(0)
{
  SetData(szBuffer);
} // end Tools_HexBuffer :: Tools_HexBuffer

/*! -----------------------------------------------------------------------------
    Constructor 
    -----------------------------------------------------------------------------
 */
Tools_HexBuffer :: Tools_HexBuffer
  ( const char * szBuffer,
    size_t       nLength )
  :m_pHexBuffer(0)
  ,m_pBinBuffer(0)
{
  SetData(szBuffer, nLength);
} // end Tools_HexBuffer :: Tools_HexBuffer

/*! -----------------------------------------------------------------------------
    Constructor 
    -----------------------------------------------------------------------------
 */
Tools_HexBuffer :: Tools_HexBuffer
  ( const void * pBuffer,
    size_t       nLength )
  :m_pHexBuffer(0)
  ,m_pBinBuffer(0)
{
  SetData(pBuffer, nLength);
} // end Tools_HexBuffer :: Tools_HexBuffer

/*! -----------------------------------------------------------------------------
    Destructor 
    -----------------------------------------------------------------------------
 */
Tools_HexBuffer :: ~Tools_HexBuffer
  ( )
{
  DeleteData();
} // end Tools_HexBuffer :: ~Tools_HexBuffer

/*! -----------------------------------------------------------------------------
    public member GiveSizeOfHexBuffer
    -----------------------------------------------------------------------------
 */
size_t Tools_HexBuffer :: GiveSizeOfHexBuffer
  ( ) const 
{
  return strlen(GiveHexBuffer());
} // Tools_HexBuffer :: GiveSizeOfHexBuffer

/*! -----------------------------------------------------------------------------
    public member GiveSizeOfBinBuffer
    -----------------------------------------------------------------------------
 */
size_t Tools_HexBuffer :: GiveSizeOfBinBuffer
  ( ) const 
{
  return (strlen(GiveHexBuffer())/2);
} // Tools_HexBuffer :: GiveSizeOfBinBuffer

/*! -----------------------------------------------------------------------------
    public member SetData
    -----------------------------------------------------------------------------
 */
void Tools_HexBuffer :: SetData
  ( const char * szBuffer )
{
  SetData(szBuffer, strlen(szBuffer));
} // end Tools_HexBuffer :: SetData

/*! -----------------------------------------------------------------------------
    public member SetData
    -----------------------------------------------------------------------------
 */
void Tools_HexBuffer :: SetData
  ( const char * szBuffer,
    size_t       nLength )
{
  DeleteData();

  // create and set hexadecimal buffer
  m_pHexBuffer = new char[nLength+1];
  if (m_pHexBuffer != 0) {
    strncpy(m_pHexBuffer, szBuffer, nLength);
    m_pHexBuffer[nLength] = 0;
  } else {
    return;
  } // end if

  // create and set binary buffer
  m_pBinBuffer = new char[(nLength/2)+1];
  if (m_pBinBuffer != 0) {
    size_t nIndex = 0;
    for (nIndex = 0; nIndex < nLength/2; nIndex++) {
      ((unsigned char*)m_pBinBuffer)[nIndex] = NumericValue(m_pHexBuffer[nIndex*2])*16 + NumericValue(m_pHexBuffer[(nIndex*2)+1]);
    } // end if
  } else {
    delete [] m_pHexBuffer;
    return;
  } // end if

} // end Tools_HexBuffer :: SetData

/*! -----------------------------------------------------------------------------
    public member SetData
    -----------------------------------------------------------------------------
 */
void Tools_HexBuffer :: SetData
  ( const void * pBuffer,
    size_t       nLength )
{
  DeleteData();

  // create and set binary buffer
  m_pBinBuffer = new char[nLength+1];
  if (m_pBinBuffer != 0) {
    memcpy(m_pBinBuffer, pBuffer, nLength);
  } else {
    return;
  } // end if

  // create and set hexadecimal buffer
  m_pHexBuffer = new char[(nLength*2)+1];
  if (m_pHexBuffer != 0) {
    size_t nIndex = 0;
    for (nIndex = 0; nIndex < nLength; nIndex++) {
      sprintf(&m_pHexBuffer[nIndex*2],"%.2X", (unsigned int) ((unsigned char *)m_pBinBuffer)[nIndex]);
    } // end if
    m_pHexBuffer[nIndex*2] = 0;
  } else {
    delete [](char *)m_pBinBuffer;
    return;
  } // end if
  
} // end Tools_HexBuffer :: SetData

/*! -----------------------------------------------------------------------------
    private member NumericValue
    -----------------------------------------------------------------------------
 */
unsigned char Tools_HexBuffer :: NumericValue
  ( char c ) 
{
  switch (c) {
    case '0':case '1':case '2':case '3':case '4':
    case '5':case '6':case '7':case '8':case '9':
      return c - 48;
      break;
    case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
      return c - 55;
      break;
    case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
      return c - 87;
      break;
  } // end switch
  return 0;
} // end Tools_HexBuffer :: NumericValue

/*! -----------------------------------------------------------------------------
    private member DeleteData
    -----------------------------------------------------------------------------
 */
void Tools_HexBuffer :: DeleteData
  ( ) 
{
  if (m_pHexBuffer != 0)
    delete []m_pHexBuffer;
  if (m_pBinBuffer != 0)
    delete [](char *)m_pBinBuffer;
} // end Tools_HexBuffer :: DeleteData
