/*

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

*/
#include "ggg00.h"
#include "DataAccess/Data_RecordHeaderAccess.h"
#include "ConsistentRead/CnsRead_CachedRow.hpp"

int Data_DeRefKeyLen(tgg00_Rec* pRec)
{
    return reinterpret_cast<CnsRead_CachedRow*>(pRec)->GetKeyLen();
}

int Data_DeRefRecLen(tgg00_Rec* pRec)
{
    return reinterpret_cast<CnsRead_CachedRow*>(pRec)->GetRecLen();
}

inline static bool IsSwizzledPointer(void* pRec)
{
    return (((unsigned char*) pRec)[HI_BYTE] & 0x80) != 0;
}

int Data_GetRecLen(void* pRec)
{
    return (IsSwizzledPointer(pRec)) ? Data_DeRefRecLen((tgg00_Rec*) pRec) : ((tgg00_Rec*) pRec)->recLen_gg00();
}

int Data_GetKeyLen(void* pRec)
{
    return (IsSwizzledPointer(pRec)) ? Data_DeRefKeyLen((tgg00_Rec*) pRec) : ((tgg00_Rec*) pRec)->recKeyLen_gg00();
}

int Data_GetVarcolOffset(void* pRec)
{
    return ((tgg00_Rec*) pRec)->recVarcolOffset_gg00();
}

int Data_GetVarcolCount(void* pRec)
{
    return ((tgg00_Rec*) pRec)->recVarcolCnt_gg00();
}