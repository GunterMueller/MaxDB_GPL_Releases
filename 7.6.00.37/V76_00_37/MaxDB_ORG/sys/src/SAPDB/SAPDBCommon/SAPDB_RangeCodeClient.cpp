/*!
  @file    SAPDB_RangeCodeClient.cpp
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

#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "RunTime/RTE_Crash.hpp"

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
    RTE_Crash(errList);
}
