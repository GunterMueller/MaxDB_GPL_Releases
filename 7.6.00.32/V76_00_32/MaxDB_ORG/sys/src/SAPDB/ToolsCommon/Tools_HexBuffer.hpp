/*!
  @file           Tools_HexBuffer.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          buffer handling and conversion

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

#ifndef _Tools_HexBuffer_HPP_
#define _Tools_HexBuffer_HPP_

#include <stddef.h>

/*!
  @brief     buffer handling and conversion

             This class stores a buffer in binary and hexadecimal representation
 */
class Tools_HexBuffer {

public:
  /*! @brief constructor 
   */
  Tools_HexBuffer
    ( );

  /*! @brief constructor 
      @param szBuffer [IN] buffer with zero terminated hexadecimal representation
   */
  Tools_HexBuffer
    (const char * szBuffer );

  /*! @brief constructor 
      @param szBuffer [IN] buffer with hexadecimal representation
             nLength  [IN] lenght of the given argument
  */
  Tools_HexBuffer
    (const char * szBuffer,
     size_t       nLength );

  /*! @brief constructor 
      @param pBuffer  [IN] buffer with binary representation
             nLength  [IN] lenght of the given argument.
  */
  Tools_HexBuffer
    (const void * pBuffer,
     size_t       nLength );

  /*! @brief destructor 
   */
  ~Tools_HexBuffer
    ( );

  /*! @brief set new data
      @param szBuffer [IN] buffer with zero terminated hexadecimal representation
   */
  void SetData
    (const char * szBuffer );

  /*! @brief set new data
      @param szBuffer [IN] buffer with hexadecimal representation
             nLength  [IN] lenght of the given argument
  */
  void SetData
    (const char * szBuffer,
     size_t       nLength );

  /*! @brief set new data
      @param pBuffer  [IN] buffer with binary representation
             nLength  [IN] lenght of the given argument.
  */
  void SetData
    (const void * pBuffer,
     size_t       nLength );

  /*! @brief returns pointer to hexadecimal representation
             This pointer will be became invalid when the object will be destroyed
      @return the pointer
   */
  const char * GiveHexBuffer() const {return (m_pHexBuffer==0)?"":m_pHexBuffer;}

  /*! @brief returns size of hexadecimal representation
      @return the size
   */
  size_t       GiveSizeOfHexBuffer() const;

  /*! @brief returns pointer to binary representation
             This pointer will be became invalid when the object will be destroyed
      @return the pointer
   */
  const void * GiveBinBuffer()const {return (m_pBinBuffer==0)?"":m_pBinBuffer;}

  /*! @brief returns size of binary representation
      @return the size
   */
  size_t       GiveSizeOfBinBuffer() const;

  /*! @brief returns whether the object is valid or not
             An invalid object had have a memory allocation error before.
      @return the size
   */
  bool IsValid() const {return ((m_pHexBuffer != 0) && (m_pBinBuffer != 0));}

  private:
    void DeleteData
      ( );

    unsigned char NumericValue
      ( char c );

    char *          m_pHexBuffer;
    void *          m_pBinBuffer;
}; // end class Tools_HexBuffer


#endif /* _Tools_HexBuffer_HPP_ */
