/*!
  @file    SAPDB_MemCopyMove.cpp
  @author  JoergM
  @ingroup Common
  @brief   Overlapping and non overlapping memory copy functions with checks

  The functions within this header file allow memory copy operations with
  overlapping 

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

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove_Messages.hpp"

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
                    , SAPDBErr_MessageList &errList )
{
    if ( 0 != destination )
    {
        if ( 0 == lengthInBytes ) return destination;

        if ( 0 < lengthInBytes )
        {
            return SAPDB_MemFillNoCheck(destination, fillByte, lengthInBytes);
        }
    }

    errList = SAPDBErr_MessageList( component
                                  , fileName
                                  , lineNumber
                                  , SAPDB_MEMFILL_PARAMETER_INVALID
                                  , SAPDB_ToString((const void *)destination)
                                  , SAPDB_ToString(lengthInBytes) );
    return 0;
}

/*!
  @brief Copy content of non overlapping memory regions

  memcpy replacement with checks for null parameters and overlap.
  
  If compiled in 'slowknl' an assertion code varifies non null parameters.
  
  NOTE: If null pointer given or region overlaps, error is returned

  @param destination [in] destination of copy operation (must be != 0)
  @param source [in] source of copy operation (must be != 0)
  @param lengthInBytes [in] the number of bytes to transfer (may be 0)
  @param component [in] the name of the calling component
  @param fileName [in] the name of the calling module
  @param lineNumber [in] the line number in the calling module
  @param errList [out] filled with error, if parameter check found null pointer
  @return destination pointer or 0 if error occured (errList filled)
 */
void * SAPDB_MemCopy( void                 *destination
                    , const void           *source
                    , SAPDB_Long            lengthInBytes
                    , SAPDB_Char  const    *component
                    , SAPDB_Char  const    *fileName
                    , SAPDB_UInt4 const     lineNumber
                    , SAPDBErr_MessageList &errList)
{
    if ( 0 != destination 
      && 0 != source )
    {
        if ( 0 == lengthInBytes ) return destination;

        if ( 0 < lengthInBytes )
        {
            if ( ( ( ((SAPDB_Byte const *)destination) + lengthInBytes )
                   <= ((SAPDB_Byte const *)source) )
              || ( ( ((SAPDB_Byte const *)source) + lengthInBytes )
                   <= ((SAPDB_Byte const *)destination) ) )
            {
                return SAPDB_MemCopyNoCheck(destination, source, lengthInBytes);
            }
            else
            {
                errList = SAPDBErr_MessageList( component
                                              , fileName
                                              , lineNumber
                                              , SAPDB_MEMCPY_OVERLAPPING
                                              , SAPDB_ToString((const void *)source)
                                              , SAPDB_ToString((const void *)(((const SAPDB_Byte *)source) + lengthInBytes))
                                              , SAPDB_ToString((const void *)destination)
                                              , SAPDB_ToString((const void *)(((const SAPDB_Byte *)destination) + lengthInBytes)) );
                return 0;
            }
        }
    }

    errList = SAPDBErr_MessageList( component
                                  , fileName
                                  , lineNumber
                                  , SAPDB_MEMCPY_PARAMETER_INVALID
                                  , SAPDB_ToString((const void *)source)
                                  , SAPDB_ToString((const void *)destination)
                                  , SAPDB_ToString(lengthInBytes) );
    return 0;
}

/*!
  @brief Copy content of possible overlapping memory regions

  memmove replacement with check for non null parameters.
  
  NOTE: If null pointer given or 0 bytes to copy, error is returned

  @param destination [in] destination of copy operation (must be != 0)
  @param source [in] source of copy operation (must be != 0)
  @param lengthInBytes [in] the number of bytes to transfer (may be 0)
  @param component [in] the name of the calling component
  @param fileName [in] the name of the calling module
  @param lineNumber [in] the line number in the calling module
  @param errList [out] filled with error, if parameter check found null pointer
  @return destination pointer or 0 if error occured (errList filled)
 */
void * SAPDB_MemMove( void                 *destination
                    , const void           *source
                    , SAPDB_Long            lengthInBytes
                    , SAPDB_Char  const    *component
                    , SAPDB_Char  const    *fileName
                    , SAPDB_UInt4 const     lineNumber
                    , SAPDBErr_MessageList &errList)
{
    if ( 0 != destination
      && 0 != source )
    {
        if ( 0 == lengthInBytes ) return destination;

        if ( 0 < lengthInBytes )
        {
            return SAPDB_MemMoveNoCheck(destination, source, lengthInBytes);
        }
    }

    errList = SAPDBErr_MessageList( component
                                  , fileName
                                  , lineNumber
                                  , SAPDB_MEMMOVE_PARAMETER_INVALID
                                  , SAPDB_ToString((const void *)source)
                                  , SAPDB_ToString((const void *)destination)
                                  , SAPDB_ToString(lengthInBytes) );
    return 0;
}
