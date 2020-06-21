/*!
  @file    SAPDB_MemCopyMoveNoCheck.hpp
  @author  JoergM
  @ingroup Common
  @brief   Overlapping and non overlapping memory copy functions

  The functions within this header file allow memory copy operations with
  overlapping.

  A NULL pointer are given for source or destination is definitely handled as an error!!!

  A zero byte length is allowed and not handled as an error but as 'nothing to do'.
  
  Zero byte length would be considered as an errror too, if 'SAPDB_ALLOW_ZERO_LENGTH' 
  will be undefined below. But for now it was decided to tolerate calls with zero byte 
  lengths...

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
*****************************************************************************/

#ifndef SAPDB_MEMCOPYMOVENOCHECK_HPP
#define SAPDB_MEMCOPYMOVENOCHECK_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include <string.h>

/*!
  @brief Fill given bytes into given destination

  Pure 'memset' replacement without checks.

  NOTE: If null pointer given or length less than zero behaviour is undefined!

  If compiled SAPDB_SLOW, an assertion is used to check for parameter error.
  
  @param destination [in] destination of fill operation (must be != 0)
  @param fillByte [in] the byte to fill in
  @param lengthInBytes [in] the number of bytes to transfer (must be >= 0)
  @return given destination pointer
 */
inline void * SAPDB_MemFillNoCheck( void       *destination
                                  , SAPDB_Byte  fillByte
                                  , SAPDB_Long  lengthInBytes)
{
    SAPDBERR_ASSERT_STATE( 0 != destination );

    if ( 0 == lengthInBytes ) return destination;

    SAPDBERR_ASSERT_STATE( 0 < lengthInBytes );

    return memset((SAPDB_Char *)destination, (SAPDB_Char)fillByte, (size_t)lengthInBytes);
}

/*!
  @brief Copy content of non overlapping memory regions

  Pure 'memcpy' replacement without checks.

  NOTE: If null pointer given or length less than zero or memory regions overlap behaviour is undefined!

  If compiled SAPDB_SLOW, an assertion is used to check for parameter error.
  
  @param destination [in] destination of copy operation (must be != 0)
  @param source [in] source of copy operation (must be != 0)
  @param lengthInBytes [in] the number of bytes to transfer (must be >= 0)
  @return given destination pointer
 */
inline void * SAPDB_MemCopyNoCheck( void       *destination
                                  , const void *source
                                  , SAPDB_Long  lengthInBytes)
{
    SAPDBERR_ASSERT_STATE( 0 != destination && 0 != source );

    if ( 0 == lengthInBytes ) return destination;

    SAPDBERR_ASSERT_STATE( 0 < lengthInBytes );

    SAPDBERR_ASSERT_STATE( ( ( ((SAPDB_Byte const *)destination) + lengthInBytes )
                             <= ((SAPDB_Byte const *)source) )
                        || ( ( ((SAPDB_Byte const *)source) + lengthInBytes )
                             <= ((SAPDB_Byte const *)destination) ) );

    return memcpy((SAPDB_Char *)destination, (const SAPDB_Char *)source, (size_t)lengthInBytes);
}

/*!
  @brief Copy content of possible overlapping memory regions

  memmove replacement without checks for non null parameters..

  NOTE: If null pointer given or length less than zero behaviour is undefined!

  If compiled SAPDB_SLOW, an assertion is used to check for parameter error.
  
  @param destination [in] destination of copy operation (must be != 0)
  @param source [in] source of copy operation (must be != 0)
  @param lengthInBytes [in] the number of bytes to transfer (must be >= 0)
  @return given destination pointer
 */
inline void * SAPDB_MemMoveNoCheck( void       *destination
                                  , const void *source
                                  , SAPDB_Long  lengthInBytes)
{
    SAPDBERR_ASSERT_STATE( 0 != destination && 0 != source );

    if ( 0 == lengthInBytes ) return destination;

    SAPDBERR_ASSERT_STATE( 0 < lengthInBytes );

    return memmove((SAPDB_Char *)destination, (const SAPDB_Char *)source, (size_t)lengthInBytes);
}
#endif // SAPDB_MEMCOPYMOVENOCHECK_HPP
