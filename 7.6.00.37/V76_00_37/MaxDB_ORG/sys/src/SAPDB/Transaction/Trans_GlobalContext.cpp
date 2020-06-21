/*
  @file           Trans_GlobalContext.cpp
  @author         FerdiF
  @ingroup        Transaction
  @brief          global transaction context

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "hkb52.h"
#include "Transaction/Trans_GlobalContext.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Trans_GlobalContext* 
Trans_GlobalContext::GetContextByTransindex(tgg00_TransIndex transIndex)
{
	tkb05_TransEntryPtr pCtx = 0;
	k52acc_trans_entry (transIndex, pCtx);
    return reinterpret_cast<Trans_GlobalContext*>(pCtx);
};

const SAPDB_Char* 
Trans_GlobalContext::GetTransactionStartTimeAsTimeStamp(
    RTESys_SQLTimestamp& timeBuffer) const
{
    const SAPDB_UInt8 *pui8 = 
        reinterpret_cast<const SAPDB_UInt8*>(&this->teTransStartTime_kb05);
    return RTESys_BuildSQLTimeStamp( *pui8 * 1000 * 1000, timeBuffer);
};

//--------------------------------------------------------------------------------------
void 
Trans_GlobalContext::SetTransactionStartTime() 
{
    SAPDB_UInt8 *pui8 = reinterpret_cast<SAPDB_UInt8*>(&this->teTransStartTime_kb05);
    *pui8 = RTESys_Time();
};

/*---------------------------------------------------------------------------*/


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
