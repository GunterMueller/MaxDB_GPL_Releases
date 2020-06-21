/*!***************************************************************************

  module:       CnsRead_PascalWrapper.cpp

  responsible : ThomasA

  special area: Consistent Read

  description:  PASCAL interface to consistent read component

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


*****************************************************************************/

#include "hbd999_1.h"
#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "ConsistentRead/CnsRead_CachedLeafPage.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"

void bd999CopyRecord (
    tgg00_RecPtr                         pRec,
    pasbool                              withTransHistoryInfo,
    tsp00_Int4                           destSize,
    tsp00_Addr                           pDest,
    tsp00_Int4            VAR_VALUE_REF  destPos,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    CnsRead_CachedRow* pRow = reinterpret_cast<CnsRead_CachedRow*>(pRec);
    --destPos;
    pRow->Copy ((withTransHistoryInfo != 0), destSize, pDest, destPos, e);
    ++destPos;
}

//-----------------------------------------------------------------------------

void bd999CopyRecordAndTransHistoryInfo (
    tgg00_RecPtr                         pRec,
    tgg00_ObjTransInfo    VAR_VALUE_REF  transHistoryInfo,
    tgg00_Rec             VAR_VALUE_REF  rec,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
       CnsRead_CachedRow* pRow = reinterpret_cast<CnsRead_CachedRow*>(pRec);
       pRow->Copy (*reinterpret_cast<CnsRead_TransHistoryInfo*>(&transHistoryInfo), sizeof(rec), &rec, e);
}

//-----------------------------------------------------------------------------

void bd999CreateTransHistoryInfo (
    tgg00_TransContext    VAR_VALUE_REF  transContext,
    tbd_node              VAR_VALUE_REF  node,
    tgg00_RecPtr                         pRec,
    tgg91_PageRef         VAR_VALUE_REF  beforeRef,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    if (!CnsRead_Manager::IsConsistentEnabled()) 
    {
        e = e_ok;
        return;
    }
    if (!reinterpret_cast<CnsRead_CachedRow*>(pRec)->CreateTransHistoryInfo(transContext, beforeRef))
    {
        e = e_no_more_memory;
    }
    else
    {
        e = e_ok;
    }
}

//-----------------------------------------------------------------------------

void bd999EnableConsistentRead (tsp00_TaskId TaskId)
{
    CnsRead_Manager::EnableConsistentRead(TaskId);
}

//-----------------------------------------------------------------------------

tgg00_ObjTransInfoPtr bd999GetTransHistoryInfo (tgg00_RecPtr pRec)
{
    return reinterpret_cast<tgg00_ObjTransInfoPtr>(
        const_cast<CnsRead_TransHistoryInfo*>(reinterpret_cast<CnsRead_CachedRow*>(pRec)->GetTransHistoryInfo()));
}

//-----------------------------------------------------------------------------

pasbool bd999IsRowDeleted (
    tgg00_RecPtr                         pRec)
{
    return reinterpret_cast<CnsRead_CachedRow*>(pRec)->IsDeleted();
}

//-----------------------------------------------------------------------------

void bd999MarkRowDeleted (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tbd_node              VAR_VALUE_REF  node,
    tgg00_RecPtr                         pRec,
    tgg91_PageRef         VAR_VALUE_REF  beforeRef,
    tgg91_TransNo         VAR_VALUE_REF  updTrans,
    pasbool                              markDeleted)
{
    CnsRead_CachedRow* pRow = reinterpret_cast<CnsRead_CachedRow*>(pRec);
    pRow->Delete(t, beforeRef, updTrans, (markDeleted != 0));
}

//-----------------------------------------------------------------------------

void bd999RemoveTransHistoryInfoFromPage (
    tsp00_TaskId                         TaskId,
    tbd_node              VAR_VALUE_REF  Page)
{
    reinterpret_cast<CnsRead_CachedLeafPage*>(&Page)->RemoveTransHistroyInfo(TaskId);
}

//-----------------------------------------------------------------------------

void bd999ReleaseTransHistoryInfo (
    tsp00_TaskId TaskId,
    tgg00_RecPtr pRec)
{
    reinterpret_cast<CnsRead_CachedRow*>(pRec)->DeleteTransHistoryInfo(TaskId);
}

//-----------------------------------------------------------------------------

void bd999RestoreRecordHeader (
    tsp00_TaskId TaskId,
    tgg00_RecPtr pRec)
{
    const bool releaseTransHistoryInfo = true;
    reinterpret_cast<CnsRead_CachedRow*>(pRec)->RestoreRowHeader(TaskId, !releaseTransHistoryInfo);
}

//-----------------------------------------------------------------------------

void bd999SetMinTrans (
    tgg91_TransNo VAR_VALUE_REF  MinTrans)
{
    CnsRead_Manager::SetMinTrans(MinTrans);
}

//-----------------------------------------------------------------------------

void bd999DelUpdTestQualification (
    tgg00_MessBlock       VAR_VALUE_REF  m,
    pasbool                              first_qual,
    pasbool                              result_wanted,
    pasbool                              check_new_rec,
    tgg00_Rec             VAR_VALUE_REF  rec,
    tsp00_MoveObjPtr                     result_ptr,
    tsp00_Int4                           result_size,
    integer               VAR_VALUE_REF  result_len)
{
    CnsRead_CachedRow* pRow = reinterpret_cast<CnsRead_CachedRow*>(&rec);
    pRow->DelUpdTestQualification (m, first_qual != 0, result_wanted != 0, check_new_rec != 0,
        result_ptr, result_size, result_len);
}

//-----------------------------------------------------------------------------

void bd999UpdateTransHistoryInfo (
    tsp00_TaskId                         TaskId,
    tgg00_RecPtr                         pOldRec,
    tgg00_RecPtr                         pNewRec,
    tgg91_PageRef         VAR_VALUE_REF  beforeRef,
    tgg91_TransNo         VAR_VALUE_REF  updTrans,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    if (!CnsRead_Manager::IsConsistentEnabled()) 
    {
        e = e_ok;
        return;
    }
    CnsRead_CachedRow* pRow    = reinterpret_cast<CnsRead_CachedRow*>(pNewRec);
    CnsRead_CachedRow* pOldRow = reinterpret_cast<CnsRead_CachedRow*>(pOldRec);
    if (!pRow->UpdateTransHistoryInfo (TaskId, *pOldRow, beforeRef, updTrans))
    {
        e = e_no_more_memory;
    }
}
