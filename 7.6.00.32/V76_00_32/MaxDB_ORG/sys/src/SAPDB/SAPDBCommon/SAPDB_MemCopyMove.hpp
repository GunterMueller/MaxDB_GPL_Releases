/*!
  @file    SAPDB_MemCopyMove.hpp
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

#ifndef SAPDB_MEMCOPYMOVE_HPP
#define SAPDB_MEMCOPYMOVE_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include <string.h>

/*! SAPDB_ALLOW_ZERO_LENGTH defined to allow zero length memcpy/memset/memmove. To disable uncomment this definition */
#define SAPDB_ALLOW_ZERO_LENGTH

/*!
  @brief Fill given bytes into given destination

  memset replacement with checks for destination null pointer and negativ length

  NOTE: If null pointer given or length less than zero error error is returned

  @param destination [in] destination of fill operation (must be != 0)
  @param fillByte [in] the byte to fill in
  @param lengthInBytes [in] the number of bytes to transfer (must be >= 0)
  @param component [in] the name of the calling component
  @param fileName [in] the name of the calling module
  @param lineNumber [in] the line number in the calling module
  @param errList [out] filled with error, if parameter check found null pointer
  @return destination pointer or 0 if error occured (errList filled)
 */
void * SAPDB_MemFill( void       *destination
                    , SAPDB_Byte  fillByte
                    , SAPDB_Long  lengthInBytes
                    , SAPDB_Char  const    *component
                    , SAPDB_Char  const    *fileName
                    , SAPDB_UInt4 const     lineNumber
                    , SAPDBErr_MessageList &errList );

/*!
  @brief Copy content of non overlapping memory regions

  memcpy replacement with checks for null parameters and overlap.
  
  If compiled in 'slowknl' an assertion code varifies non null parameters.
  
  NOTE: If null pointer given or length less than zero error or region overlaps, error is returned

  @param destination [in] destination of copy operation (must be != 0)
  @param source [in] source of copy operation (must be != 0)
  @param lengthInBytes [in] the number of bytes to transfer (must be >= 0)
  @param component [in] the name of the calling component
  @param fileName [in] the name of the calling module
  @param lineNumber [in] the line number in the calling module
  @param errList [out] filled with error, if parameter check found null pointer
  @return destination pointer or 0 if error occured (errList filled)
 */
void * SAPDB_MemCopy( void       *destination
                    , const void *source
                    , SAPDB_Long  lengthInBytes
                    , SAPDB_Char  const    *component
                    , SAPDB_Char  const    *fileName
                    , SAPDB_UInt4 const     lineNumber
                    , SAPDBErr_MessageList &errList );

/*!
  @brief Copy content of possible overlapping memory regions

  memmove replacement with check for non null parameters.
  
  NOTE: If null pointer given or length less than zero error is returned

  @param destination [in] destination of copy operation (must be != 0)
  @param source [in] source of copy operation (must be != 0)
  @param lengthInBytes [in] the number of bytes to transfer (must be >= 0)
  @param component [in] the name of the calling component
  @param fileName [in] the name of the calling module
  @param lineNumber [in] the line number in the calling module
  @param errList [out] filled with error, if parameter check found null pointer
  @return destination pointer or 0 if error occured (errList filled)
 */
void * SAPDB_MemMove( void       *destination
                    , const void *source
                    , SAPDB_Long  lengthInBytes
                    , SAPDB_Char  const    *component
                    , SAPDB_Char  const    *fileName
                    , SAPDB_UInt4 const     lineNumber
                    , SAPDBErr_MessageList &errList );

#endif // SAPDB_MEMCOPYMOVE_HPP
