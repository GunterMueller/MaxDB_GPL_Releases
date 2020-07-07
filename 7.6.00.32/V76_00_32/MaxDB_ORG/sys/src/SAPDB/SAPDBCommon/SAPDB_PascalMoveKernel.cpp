/*!
  @file    SAPDB_PascalMoveKernel.cpp
  @author  JoergM
  @ingroup Common
  @brief   Pascal coding interface for memory fill and copy functions

  The functions within this file allow memory copy operations with
  and without overlapping memory regions. Additional filling function
  allow to fill ascii or unicode chracters. The interfaces allow a 
  range check.

  A variant is realized by a precompiler define:
  -DSAPDB_DIRECT_KNLDIAG_OUTPUT allow in case of error to have direct knldiag output

  The error list in the SQLMan_Context() is filled too.

  The Pascal interface description for type checker is found in vgg101

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
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "SQLManager/SQLMan_Context.hpp"

/*!
  @brief check if given error code is already set

  If an error code is already set, add this information to the SAPDBErr_MessageList given.

  @param errorCode [in] the error code
  @param errList [out] the error list to set in case errorCode is not e_ok
  @return none
 */
void SAPDB_CheckBasisErrorSet( tgg00_BasisError errorCode
                             , SAPDB_Char const *component
                             , SAPDB_Char const *moduleName
                             , SAPDB_Int4        moduleInternIndex
                             , SAPDBErr_MessageList &errList )
{
    if ( e_ok != errorCode )
    {
        errList = SAPDBErr_MessageList( component
                                      , moduleName
                                      , moduleInternIndex
                                      , SAPDB_MEMCPY_PASCAL_ERROR_ALREADY_SET
                                      , SAPDB_ToString(errorCode) );
    }
}

/*!
  @brief handle message list produced in case of error

  If called, the error list is dumped into knldiag first and then replacing
  the message list attached to the SQLMan_Context.

  @param errList [out] the error list to set in case errorCode is not e_ok
  @return none
 */
void SAPDB_HandleMoveErrorMessageList(SAPDBErr_MessageList &errList)
{
#  ifdef SAPDB_DIRECT_KNLDIAG_OUTPUT
    RTE_Message(errList);
#  endif

    SQLMan_Context *sqlContext = SQLMan_Context::GetContext();

    if ( sqlContext )
    {
        SAPDBErr_MessageList externalErrList( errList.Component()
                                             , __FILE__
                                             , __LINE__
                                             , SAPDB_MEMCPY_EXTERNAL_MOVE_ERROR
                                             , errList.FileName()
                                             , SAPDB_ToString(errList.LineNumber()) );
        externalErrList.AppendNewMessage( errList );
        sqlContext->GetErrorList() = externalErrList;
    }
}

/*!
  @brief Fill given fill byte into given range

  memset replacement with checks for null parameters and range checks.

  Replacement for vgg10c: g10fil, g10fil1..., g10fil5

  @param moduleIdentifier [in] The calling pascal module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param destination_upperBound [in] Upper limit of destination
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param fillByte [in] the byte to fill in
  @param errorCode [out] filled with error code 'e_move_error' if error
  @return none
 */
extern "C" void SAPDB_PascalFill( tsp00_C6 & moduleIdentifier
                                , SAPDB_Int4 moduleInternIndex
                                , SAPDB_Int4 destination_upperBound
                                , SAPDB_Byte *destination
                                , SAPDB_Int4 destination_position
                                , SAPDB_Int4 lengthInBytes
                                , SAPDB_Byte fillByte
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
            tsp00_C6c moduleName;

            moduleName.p2c(moduleIdentifier);

            SAPDB_CheckBasisErrorSet(errorCode, "FILLCODE", moduleName.asCharp(), moduleInternIndex, errList);

            errList.AppendNewMessage(SAPDBErr_MessageList( "FILLCODE"
                                             , moduleName.asCharp()
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
            SAPDB_MemFillNoCheck( destination + destination_position - 1
                                , fillByte
                                , lengthInBytes );
        }
    }
}

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
extern "C" void SAPDB_PascalForcedFill( SAPDB_Int4 destination_upperBound
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
        RTE_Crash( SAPDBErr_MessageList( "FILLCODE"
                                         , __FILE__
                                         , __LINE__
                                         , SAPDB_MEMCPY_PASCAL_FILL
                                         , SAPDB_ToString(destination_upperBound)
                                         , SAPDB_ToString((const void *)destination)
                                         , SAPDB_ToString(destination_position)
                                         , SAPDB_ToString(lengthInBytes)
                                         , SAPDB_ToString(fillByte) ) );
    }
    else
    {
        SAPDB_MemFillNoCheck( destination + destination_position - 1
                            , fillByte
                            , lengthInBytes );
    }
}

/*!
  @brief Fill given unicode fill bytes into given range

  memset replacement with checks for null parameters and range checks.

  Replacement for vgg10c: g10filuni, g10filuni1..., g10filuni3

  @param moduleIdentifier [in] The calling pascal module
  @param moduleInternIndex [in] The internal index of the move call in that module
  @param destination_upperBound [in] Upper limit of destination
  @param destination [in] start of destination area of copy operation
  @param destination_position [in] index of first byte in destination area to overwrite
  @param lengthInBytes [in] the number of bytes to transfer
  @param fillBytes [in] the pointer to the unicode character to fill in
  @param errorCode [out] filled with error code 'e_move_error' if error
  @return none
 */
extern "C" void SAPDB_PascalUnicodeFill( tsp00_C6 & moduleIdentifier
                                       , SAPDB_Int4 moduleInternIndex
                                       , SAPDB_Int4 destination_upperBound
                                       , SAPDB_Byte *destination
                                       , SAPDB_Int4 destination_position
                                       , SAPDB_Int4 lengthInBytes
                                       , SAPDB_Byte *fillBytes
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
            tsp00_C6c moduleName;
            SAPDBErr_MessageList errList;

            moduleName.p2c(moduleIdentifier);

            SAPDB_CheckBasisErrorSet(errorCode, "FILUCODE", moduleName.asCharp(), moduleInternIndex, errList);
            SAPDBErr_MessageList tmpErrList;

            tmpErrList = SAPDBErr_MessageList( "FILUCODE"
                                             , moduleName.asCharp()
                                             , moduleInternIndex
                                             , SAPDB_MEMCPY_PASCAL_UNICODE_FILL
                                             , SAPDB_ToString(destination_upperBound)
                                             , SAPDB_ToString((const void *)destination)
                                             , SAPDB_ToString(destination_position)
                                             , SAPDB_ToString(lengthInBytes)
                                             , (fillBytes ? (SAPDB_Char const *)SAPDB_ToString((SAPDB_UInt1)fillBytes[0], _T_x) : "null fillbytes")
                                             , (fillBytes ? (SAPDB_Char const *)SAPDB_ToString((SAPDB_UInt1)fillBytes[1], _T_x) : "") );
            errList.AppendNewMessage( tmpErrList );
            SAPDB_HandleMoveErrorMessageList(errList);

            errorCode = e_move_error;
        }
        else
        {
            SAPDB_Byte *destinationStart = destination + destination_position - 1;
            SAPDB_MemFillNoCheck( destinationStart
                                , fillBytes[0]
                                , lengthInBytes
                                );

            SAPDB_Byte secondFillByte = fillBytes[1];
            for ( int iTarget = 1; iTarget < lengthInBytes; iTarget += 2 )
            {
                destinationStart[iTarget] = secondFillByte;
            }
        }
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
extern "C" void SAPDB_PascalForcedUnicodeFill( SAPDB_Int4 destination_upperBound
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
        RTE_Crash( SAPDBErr_MessageList( "FILUCODE"
                                       , __FILE__
                                       , __LINE__
                                       , SAPDB_MEMCPY_PASCAL_UNICODE_FILL
                                       , SAPDB_ToString(destination_upperBound)
                                       , SAPDB_ToString((const void *)destination)
                                       , SAPDB_ToString(destination_position)
                                       , SAPDB_ToString(lengthInBytes)
                                       , (fillBytes ? (SAPDB_Char const *)SAPDB_ToString((SAPDB_UInt1)fillBytes[0], _T_x) : "null fillbytes")
                                       , (fillBytes ? (SAPDB_Char const *)SAPDB_ToString((SAPDB_UInt1)fillBytes[1], _T_x) : "") ) );
    }
    else
    {
        SAPDB_Byte *destinationStart = destination + destination_position - 1;
        SAPDB_MemFillNoCheck( destinationStart
                            , fillBytes[0]
                            , lengthInBytes
                            );

        SAPDB_Byte secondFillByte = fillBytes[1];
        for ( int iTarget = 1; iTarget < lengthInBytes; iTarget += 2 )
        {
            destinationStart[iTarget] = secondFillByte;
        }
    }
}

/*!
  @brief Copy content of possibly overlapping memory regions

  memcpy replacement with checks for null parameters and range checks.

  Replaces 'g10mv, g10mv1..., g10mv29'
  
  NOTE: If null pointer given, 0 bytes to copy or region overlaps, error is returned

  @param moduleIdentifier [in] The calling pascal module
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
extern "C" void SAPDB_PascalMove( tsp00_C6 & moduleIdentifier
                                , SAPDB_Int4 moduleInternIndex
                                , SAPDB_Int4 source_upperBound
                                , SAPDB_Int4 destination_upperBound
                                , SAPDB_Byte *source
                                , SAPDB_Int4 source_position
                                , SAPDB_Byte *destination
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
          || ( ( (destination + (destination_position - 1) + lengthInBytes) > (source + (source_position - 1)) )
            && ( (source + (source_position - 1) + lengthInBytes) > (destination + (destination_position - 1)) ) )
           )
        {
            SAPDBErr_MessageList errList;
            tsp00_C6c moduleName;

            moduleName.p2c(moduleIdentifier);

            SAPDB_CheckBasisErrorSet(errorCode, "MOVECODE", moduleName.asCharp(), moduleInternIndex, errList);

            errList.AppendNewMessage( SAPDBErr_MessageList( "MOVECODE"
                                             , moduleName.asCharp()
                                             , moduleInternIndex
                                             , SAPDB_MEMCPY_PASCAL_MOVE
                                             , SAPDB_ToString(source_upperBound)
                                             , SAPDB_ToString(destination_upperBound)
                                             , SAPDB_ToString((const void *)source)
                                             , SAPDB_ToString(source_position)
                                             , SAPDB_ToString((const void *)destination)
                                             , SAPDB_ToString(destination_position)
                                             , SAPDB_ToString(lengthInBytes) ) );
            SAPDB_HandleMoveErrorMessageList(errList);

            errorCode = e_move_error;
        }
        else
        {
            SAPDB_MemCopyNoCheck( destination + destination_position - 1
                                , source + source_position - 1
                                , lengthInBytes );
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
extern "C" void SAPDB_PascalForcedMove( SAPDB_Int4 source_upperBound
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
        RTE_Crash( SAPDBErr_MessageList( "MOVECODE"
                                        , __FILE__
                                        , __LINE__
                                        , SAPDB_MEMCPY_PASCAL_MOVE
                                        , SAPDB_ToString(source_upperBound)
                                        , SAPDB_ToString(destination_upperBound)
                                        , SAPDB_ToString((const void *)source)
                                        , SAPDB_ToString(source_position)
                                        , SAPDB_ToString((const void *)destination)
                                        , SAPDB_ToString(destination_position)
                                        , SAPDB_ToString(lengthInBytes) ) );
    }
    else
    {
        SAPDB_MemCopyNoCheck( destination + destination_position - 1
                            , source + source_position - 1
                            , lengthInBytes );
    }
}

/*!
  @brief Copy content of possibly overlapping memory regions

  memmove replacement with checks for null parameters and range checks.

  Replacement for vgg10c: g10mvb, g10mvr
  
  NOTE: If null pointer given, 0 bytes to copy or region overlaps, error is returned

  @param moduleIdentifier [in] The calling pascal module
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
extern "C" void SAPDB_PascalOverlappingMove( tsp00_C6 & moduleIdentifier
                                           , SAPDB_Int4 moduleInternIndex
                                           , SAPDB_Int4 source_upperBound
                                           , SAPDB_Int4 destination_upperBound
                                           , SAPDB_Byte *source
                                           , SAPDB_Int4 source_position
                                           , SAPDB_Byte *destination
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
            tsp00_C6c moduleName;

            moduleName.p2c(moduleIdentifier);

            SAPDB_CheckBasisErrorSet(errorCode, "OVERLAP ", moduleName.asCharp(), moduleInternIndex, errList);

            errList.AppendNewMessage(SAPDBErr_MessageList( "OVERLAP "
                                             , moduleName.asCharp()
                                             , moduleInternIndex
                                             , SAPDB_MEMCPY_PASCAL_MOVE
                                             , SAPDB_ToString(source_upperBound)
                                             , SAPDB_ToString(destination_upperBound)
                                             , SAPDB_ToString((const void *)source)
                                             , SAPDB_ToString(source_position)
                                             , SAPDB_ToString((const void *)destination)
                                             , SAPDB_ToString(destination_position)
                                             , SAPDB_ToString(lengthInBytes) ) );
            SAPDB_HandleMoveErrorMessageList(errList);

            errorCode = e_move_error;
        }
        else
        {
            SAPDB_MemMoveNoCheck( destination + destination_position - 1
                                , source + source_position - 1
                                , lengthInBytes );
        }
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
extern "C" void SAPDB_PascalForcedOverlappingMove( SAPDB_Int4 source_upperBound
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
        RTE_Crash( SAPDBErr_MessageList( "OVERLAP "
                                       , __FILE__
                                       , __LINE__
                                       , SAPDB_MEMCPY_PASCAL_MOVE
                                       , SAPDB_ToString(source_upperBound)
                                       , SAPDB_ToString(destination_upperBound)
                                       , SAPDB_ToString((const void *)source)
                                       , SAPDB_ToString(source_position)
                                       , SAPDB_ToString((const void *)destination)
                                       , SAPDB_ToString(destination_position)
                                       , SAPDB_ToString(lengthInBytes) ) );
    }
    else
    {
        SAPDB_MemMoveNoCheck( destination + destination_position - 1
                            , source + source_position - 1
                            , lengthInBytes );
    }
}
