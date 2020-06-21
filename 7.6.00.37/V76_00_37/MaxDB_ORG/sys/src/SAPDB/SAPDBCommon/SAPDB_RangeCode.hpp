/*!
  @file    SAPDB_RangeCode.hpp
  @author  JoergM
  @ingroup Common
  @brief   C++ (Pascal) coding interface for memory fill and copy functions

  The functions within this file allow memory copy operations with
  and without overlapping memory regions. Additional filling function
  allow to fill ascii or unicode chracters. The interfaces allow a 
  range and overlap check.

  This code is not to be used in liboms...

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

#ifndef SAPDB_RANGECODE_HPP
#define SAPDB_RANGECODE_HPP

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove_Messages.hpp"
#include "gsp00.h"
#include "ggg00.h"

/*!
  @brief check if given basis error code is already set

  If an error code is already set, add this information to the SAPDBErr_MessageList given.

  @param errorCode [in] the error code
  @param component [in] the component name
  @param moduleIdentifier [in] The calling module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param errList [out] the error list to set in case errorCode is not e_ok
  @return none
 */
void SAPDB_CheckBasisErrorSet( tgg00_BasisError errorCode
                             , SAPDB_Char const *component
                             , SAPDB_Char const *moduleIdentifier
                             , SAPDB_Int4        moduleInternIndex
                             , SAPDBErr_MessageList &errList );

/*!
  @brief handle message list produced in case of move/fill error

  If called, the error list is dumped into knldiag first and then replacing
  the message list attached to the SQLMan_Context.

  @param errList [out] the error list to set in case errorCode is not e_ok
  @return none
 */
void SAPDB_HandleMoveErrorMessageList(SAPDBErr_MessageList &errList);

/*!
  @brief Fill given fill byte into given range

  memset replacement with checks for null parameters and range checks.

  Replacement for hgg10.h : g10fil

  @param moduleIdentifier [in] The calling module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param destination_upperBound [in] Upper limit of destination
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param fillByte [in] the byte to fill in
  @param errorCode [out] filled with error code 'e_move_error' if error
  @return none
 */
inline void SAPDB_RangeFill( SAPDB_Char const *moduleIdentifier
                           , SAPDB_Int4 moduleInternIndex
                           , SAPDB_Int4 destination_upperBound
                           , void      *destination
                           , SAPDB_Int4 destination_position
                           , SAPDB_Int4 lengthInBytes
                           , SAPDB_Char fillByte
                           , tgg00_BasisError &errorCode )
{
    if ( e_move_error != errorCode )
    {
        if ( destination_upperBound < (destination_position + lengthInBytes - 1)
          || lengthInBytes < 0
          || destination_position <= 0
          || 0 == destination
           )
        {
            SAPDBErr_MessageList errList;

            SAPDB_CheckBasisErrorSet(errorCode, "FILLCODE", moduleIdentifier, moduleInternIndex, errList);

            errList.AppendNewMessage( SAPDBErr_MessageList( "FILLCODE"
                                                          , moduleIdentifier
                                                          , moduleInternIndex
                                                          , SAPDB_MEMCPY_PASCAL_FILL
                                                          , SAPDB_ToString(destination_upperBound)
                                                          , SAPDB_ToString((const void *)destination)
                                                          , SAPDB_ToString(destination_position)
                                                          , SAPDB_ToString(lengthInBytes)
                                                          , SAPDB_ToString(fillByte) ) );
            SAPDB_HandleMoveErrorMessageList(errList);

            errorCode = e_move_error;
        }
        else
        {
            SAPDB_MemFillNoCheck( reinterpret_cast<unsigned char*>(destination) + destination_position - 1
                                , (SAPDB_Byte)fillByte
                                , lengthInBytes );
        }
    }
}

/*!
  @brief Fill given fill byte into given range

  memset replacement with checks for null parameters and range checks.

  Replacement for hgg10.h: g10filuni

  @param moduleIdentifier [in] The calling module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param destination_upperBound [in] Upper limit of destination
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param fillBytes [in] the pointer to the unicode character to fill in
  @param errorCode [out] filled with error code 'e_move_error' if error
  @return none
 */
inline void SAPDB_RangeUnicodeFill( SAPDB_Char const *moduleIdentifier
                                  , SAPDB_Int4 moduleInternIndex
                                  , SAPDB_Int4 destination_upperBound
                                  , void       *destination
                                  , SAPDB_Int4 destination_position
                                  , SAPDB_Int4 lengthInBytes
                                  , const SAPDB_Char *fillBytes
                                  , tgg00_BasisError &errorCode )
{
    if ( e_move_error != errorCode )
    {
        if ( destination_upperBound < (destination_position + lengthInBytes - 1)
          || lengthInBytes < 0
          || destination_position <= 0
          || 0 == destination
          || 0 == fillBytes
           )
        {
            SAPDBErr_MessageList errList;

            SAPDB_CheckBasisErrorSet(errorCode, "FILUCODE", moduleIdentifier, moduleInternIndex, errList);

            SAPDBErr_MessageList tmpErrList = SAPDBErr_MessageList( "FILUCODE"
                                             , moduleIdentifier
                                             , moduleInternIndex
                                             , SAPDB_MEMCPY_PASCAL_UNICODE_FILL
                                             , SAPDB_ToString(destination_upperBound)
                                             , SAPDB_ToString((const void *)destination)
                                             , SAPDB_ToString(destination_position)
                                             , SAPDB_ToString(lengthInBytes)
                                             , (fillBytes ? (SAPDB_Char const *)SAPDB_ToString(fillBytes[0]) : "null fillbytes")
                                             , (fillBytes ? (SAPDB_Char const *)SAPDB_ToString(fillBytes[1]) : "") );
            errList.AppendNewMessage( tmpErrList );
            SAPDB_HandleMoveErrorMessageList(errList);

            errorCode = e_move_error;
        }
        else
        {
            SAPDB_Byte *destinationStart = reinterpret_cast<SAPDB_Byte *>(destination) + destination_position - 1;
            SAPDB_MemFillNoCheck( destinationStart
                                , (SAPDB_Byte)fillBytes[0]
                                , lengthInBytes
                                );

            SAPDB_Byte secondFillByte = (SAPDB_Byte)fillBytes[1];
            for ( int iTarget = 1; iTarget < lengthInBytes; iTarget += 2 )
            {
                destinationStart[iTarget] = secondFillByte;
            }
        }
    }
}

/*!
  @brief Copy content of possibly overlapping memory ranges

  memcpy replacement with checks for null parameters and range checks.

  Replaces hgg10:g10mv
  
  NOTE: If null pointer given, 0 bytes to copy or region overlaps, error is returned

  @param moduleIdentifier [in] The calling module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param source_upperBound [in] Upper limit of source
  @param destination_upperBound [in] Upper limit of destination
  @param source [in] start of source area of copy operation
  @param source_position [in] index of first byte in source area to copy
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param errorCode [out] filled with error code 'e_move_error' if error
  @return none
 */
inline void SAPDB_RangeMove( SAPDB_Char const *moduleIdentifier
                           , SAPDB_Int4 moduleInternIndex
                           , SAPDB_Int4 source_upperBound
                           , SAPDB_Int4 destination_upperBound
                           , const void *source
                           , SAPDB_Int4 source_position
                           , void      *destination
                           , SAPDB_Int4 destination_position
                           , SAPDB_Int4 lengthInBytes
                           , tgg00_BasisError &errorCode )
{
    if ( e_move_error != errorCode )
    {
        if (
             source_upperBound < (source_position + lengthInBytes - 1) 
          || destination_upperBound < (destination_position + lengthInBytes - 1) 
          || lengthInBytes < 0
          || source_position <= 0
          || destination_position <= 0
          || 0 == source
          || 0 == destination
          || ( ( (reinterpret_cast<const SAPDB_Byte *>(destination) + (destination_position - 1) + lengthInBytes) > (reinterpret_cast<const SAPDB_Byte *>(source) + (source_position - 1)) )
            && ( (reinterpret_cast<const SAPDB_Byte *>(source) + (source_position - 1) + lengthInBytes) > (reinterpret_cast<const SAPDB_Byte *>(destination) + (destination_position - 1)) ) )
           )
        {
            SAPDBErr_MessageList errList;

            SAPDB_CheckBasisErrorSet(errorCode, "MOVECODE", moduleIdentifier, moduleInternIndex, errList);

            errList.AppendNewMessage( SAPDBErr_MessageList( "MOVECODE"
                                             , moduleIdentifier
                                             , moduleInternIndex
                                             , SAPDB_MEMCPY_PASCAL_MOVE
                                             , SAPDB_ToString(source_upperBound)
                                             , SAPDB_ToString(destination_upperBound)
                                             , SAPDB_ToString(source)
                                             , SAPDB_ToString(source_position)
                                             , SAPDB_ToString((const void *)destination)
                                             , SAPDB_ToString(destination_position)
                                             , SAPDB_ToString(lengthInBytes) ) );
            SAPDB_HandleMoveErrorMessageList(errList);

            errorCode = e_move_error;
        }
        else
        {
            SAPDB_MemCopyNoCheck( reinterpret_cast<SAPDB_Byte *>(destination) + destination_position - 1
                                , reinterpret_cast<const SAPDB_Byte *>(source) + source_position - 1
                                , lengthInBytes );
        }
    }
}

/*!
  @brief Copy content of possibly overlapping memory ranges

  memcpy replacement with checks for null parameters and range checks.

  Replaces hgg10:g10mv1
  
  NOTE: If null pointer given, 0 bytes to copy or region overlaps, error is returned

  @param moduleIdentifier [in] The calling module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param source_upperBound [in] Upper limit of source
  @param destination_upperBound [in] Upper limit of destination
  @param source [in] start of source area of copy operation
  @param source_position [in] index of first byte in source area to copy
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param errorCode [out] filled with error code 'e_move_error' if error
  @return none
 */
inline void SAPDB_RangeMove1( SAPDB_Char const *moduleIdentifier
                            , SAPDB_Int4 moduleInternIndex
                            , SAPDB_Int4 source_upperBound
                            , SAPDB_Int4 destination_upperBound
                            , const void *source
                            , SAPDB_Int4 source_position
                            , void      *destination
                            , SAPDB_Int4 destination_position
                            , SAPDB_Int4 lengthInBytes
                            , tgg00_BasisError &errorCode )
{
    SAPDB_RangeMove( moduleIdentifier
                   , moduleInternIndex
                   , source_upperBound
                   , destination_upperBound
                   , source
                   , source_position
                   , destination
                   , destination_position
                   , lengthInBytes
                   , errorCode );
}

/*!
  @brief Copy content of possibly overlapping memory regions

  memmove replacement with checks for null parameters and range checks.

  Replacement for hgg10.h: g10mvb
  
  NOTE: If null pointer given, 0 bytes to copy or region overlaps, error is returned

  @param moduleIdentifier [in] The calling module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param source_upperBound [in] Upper limit of source
  @param destination_upperBound [in] Upper limit of destination
  @param source [in] start of source area of copy operation
  @param source_position [in] index of first byte in source area to copy
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param errorCode [out] filled with error code 'e_move_error' if error
  @return none
 */
inline void SAPDB_RangeOverlappingMove( SAPDB_Char const * moduleIdentifier
                                      , SAPDB_Int4 moduleInternIndex
                                      , SAPDB_Int4 source_upperBound
                                      , SAPDB_Int4 destination_upperBound
                                      , const void *source
                                      , SAPDB_Int4 source_position
                                      , void       *destination
                                      , SAPDB_Int4 destination_position
                                      , SAPDB_Int4 lengthInBytes
                                      , tgg00_BasisError &errorCode )
{
    if ( e_move_error != errorCode )
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
            SAPDBErr_MessageList errList;

            SAPDB_CheckBasisErrorSet(errorCode, "MOVECODE", moduleIdentifier, moduleInternIndex, errList);

            errList.AppendNewMessage(SAPDBErr_MessageList( "MOVECODE"
                                             , moduleIdentifier
                                             , moduleInternIndex
                                             , SAPDB_MEMCPY_PASCAL_MOVE
                                             , SAPDB_ToString(source_upperBound)
                                             , SAPDB_ToString(destination_upperBound)
                                             , SAPDB_ToString(source)
                                             , SAPDB_ToString(source_position)
                                             , SAPDB_ToString((const void *)destination)
                                             , SAPDB_ToString(destination_position)
                                             , SAPDB_ToString(lengthInBytes) ) );
            SAPDB_HandleMoveErrorMessageList(errList);

            errorCode = e_move_error;
        }
        else
        {
            SAPDB_MemMoveNoCheck( reinterpret_cast<SAPDB_Byte *>(destination) + destination_position - 1
                                , reinterpret_cast<const SAPDB_Byte *>(source) + source_position - 1
                                , lengthInBytes );
        }
    }
}

#endif /* SAPDB_RANGECODE_HPP */
