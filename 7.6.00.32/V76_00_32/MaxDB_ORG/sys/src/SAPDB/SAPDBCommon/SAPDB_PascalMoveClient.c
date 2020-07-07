/*!
  @file    SAPDB_PascalMoveClient.c
  @author  JoergM
  @ingroup Common
  @brief   Pascal coding interface for memory fill and copy functions

  The functions within this file allow memory copy operations with
  and without overlapping memory regions. Additional filling function
  allow to fill ascii or unicode chracters. The interfaces allow a 
  range check.

  The Pascal interface description for type checker is found in vgg101

  This code is not used in combination with 'SAPDB_PascalMoveKernel'.

  It replaces only s10mv,s10fil,g20unifill

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

#include "SAPDBCommon/SAPDB_Types.h"
extern void sqlabort();
#include "geo007_1.h" /* Messages */
#include "geo60.h"    /* MSGD... */

/*!
  @brief Fill given fill byte into given range

  memset replacement with checks for null parameters and range checks.

  In case of problem 'RTE_Crash' is called.

  Replacement for vsp10: s10fil, s10fil1..., s10fil6

  @param destination_upperBound [in] Upper limit of destination
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param fillByte [in] the byte to fill in
  @return none
 */
void SAPDB_PascalForcedFill( SAPDB_Int4 destination_upperBound
                           , SAPDB_Byte *destination
                           , SAPDB_Int4 destination_position
                           , SAPDB_Int4 lengthInBytes
                           , SAPDB_Byte fillByte )
{
    if ( destination_upperBound < (destination_position + lengthInBytes - 1)
      || lengthInBytes < 0
      || destination_position <= 0
      || 0 == destination
       )
    {
        MSGD(( ERR_FILL_PARAMETER
             , destination_upperBound
             , destination
             , destination_position
             , lengthInBytes
             , fillByte ));
        sqlabort();
    }
    else
    {
        memset( destination + destination_position - 1
              , fillByte
              , lengthInBytes );
    }
}

/*!
  @brief Fill given unicode fill bytes into given range

  memset replacement with checks for null parameters and range checks.

  In case of problem 'RTE_Crash' is called.

  Replacement for vgg20: g20unifill

  @param destination_upperBound [in] Upper limit of destination
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param fillBytes [in] the pointer to the unicode character to fill in
  @return none
 */
void SAPDB_PascalForcedUnicodeFill( SAPDB_Int4 destination_upperBound
                                  , SAPDB_Byte *destination
                                  , SAPDB_Int4 destination_position
                                  , SAPDB_Int4 lengthInBytes
                                  , SAPDB_Byte *fillBytes )
{

    if ( destination_upperBound < (destination_position + lengthInBytes - 1)
      || lengthInBytes < 0
      || destination_position <= 0
      || 0 == destination
      || 0 == fillBytes
       )
    {
        MSGD(( ERR_FILL_UNICODE_PARAMETER
             , destination_upperBound
             , destination
             , destination_position
             , lengthInBytes
             , (fillBytes ? fillBytes[0] : 0xFF)
             , (fillBytes ? fillBytes[1] : 0xFF) ));
        sqlabort();
    }
    else
    {
        int iTarget;
        SAPDB_Byte *destinationStart = destination + destination_position - 1;

        memset( destinationStart
              , fillBytes[0]
              , lengthInBytes
              );

        for ( iTarget = 1; iTarget < lengthInBytes; iTarget += 2 )
        {
            destinationStart[iTarget] = fillBytes[1];
        }
    }
}

/*!
  @brief Copy content of non overlapping memory regions

  memcpy replacement with checks for null parameters and range checks.

  In case of problem, RTE_Crash is called.

  Replaces 's10mv, s10mv1..., s10mv29'
  
  @param source_upperBound [in] Upper limit of source
  @param destination_upperBound [in] Upper limit of destination
  @param source [in] start of source area of copy operation
  @param source_position [in] index of first byte in source area to copy
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @return none
 */
void SAPDB_PascalForcedMove( SAPDB_Int4 source_upperBound
                           , SAPDB_Int4 destination_upperBound
                           , SAPDB_Byte *source
                           , SAPDB_Int4 source_position
                           , SAPDB_Byte *destination
                           , SAPDB_Int4 destination_position
                           , SAPDB_Int4 lengthInBytes )
{
    if (
         source_upperBound < (source_position + lengthInBytes - 1) 
      || destination_upperBound < (destination_position + lengthInBytes - 1) 
      || lengthInBytes < 0
      || source_position <= 0
      || destination_position <= 0
      || 0 == source
      || 0 == destination
      || ( ( (destination + (destination_position - 1) + lengthInBytes) > (source + (source_position - 1)) )
        && ( (source + (source_position - 1) + lengthInBytes) > (destination + (destination_position - 1)) ) )
       )
    {
        MSGD(( ERR_MOVE_PARAMETER
             , source_upperBound
             , destination_upperBound
             , source
             , source_position
             , destination
             , destination_position
             , lengthInBytes ));
        sqlabort();
    }
    else
    {
        memcpy( destination + destination_position - 1
              , source + source_position - 1
              , lengthInBytes );
    }
}

/*!
  @brief Copy content of possibly overlapping memory regions

  memmove replacement with checks for null parameters and range checks.

  In case of parameter problems, RTE_Crash is called

  Replacement for vsp10: s10mvr
  
  @param source_upperBound [in] Upper limit of source
  @param destination_upperBound [in] Upper limit of destination
  @param source [in] start of source area of copy operation
  @param source_position [in] index of first byte in source area to copy
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @return none
 */
void SAPDB_PascalForcedOverlappingMove( SAPDB_Int4 source_upperBound
                                      , SAPDB_Int4 destination_upperBound
                                      , SAPDB_Byte *source
                                      , SAPDB_Int4 source_position
                                      , SAPDB_Byte *destination
                                      , SAPDB_Int4 destination_position
                                      , SAPDB_Int4 lengthInBytes )
{
    if (
         source_upperBound < (source_position + lengthInBytes - 1) 
      || destination_upperBound < (destination_position + lengthInBytes - 1) 
      || lengthInBytes < 0
      || source_position <= 0
      || destination_position <= 0
      || 0 == source
      || 0 == destination
       )
    {
        MSGD(( ERR_OVERLAPPING_MOVE_PARAMETER
             , source_upperBound
             , destination_upperBound
             , source
             , source_position
             , destination
             , destination_position
             , lengthInBytes ));
        sqlabort();
    }
    else
    {
        memmove( destination + destination_position - 1
               , source + source_position - 1
               , lengthInBytes );
    }
}
