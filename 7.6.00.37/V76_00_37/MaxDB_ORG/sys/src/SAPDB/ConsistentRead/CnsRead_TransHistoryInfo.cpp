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
#include "ConsistentRead/CnsRead_TransHistoryInfo.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"

CnsRead_TransHistoryInfo::CnsRead_TransHistoryInfo()
{
    otrBeforeRef_gg00.gg91SetNilRef();
    otrLockTransIndex_gg00 = cgg_nil_transindex;
    otrLockTransId_gg00.gg90SetNil();
    otrUpdTransId_gg00.gg90SetNil();
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryInfo::CnsRead_TransHistoryInfo(const tgg00_TransContext& currentTrans)
{
    otrBeforeRef_gg00.gg91SetNilRef();
    otrLockTransIndex_gg00 = currentTrans.trIndex_gg00;
    otrLockTransId_gg00    = currentTrans.trTransId_gg00;
    otrUpdTransId_gg00     = currentTrans.trWriteTransId_gg00;
}

//-----------------------------------------------------------------------------

bool CnsRead_TransHistoryInfo::IsRequired() const
{

    if (!CnsRead_Manager::IsConsistentEnabled())
    {
        return false;
    }
    tgg91_TransNo minTrans = CnsRead_Manager::GetMinTrans();
    if (minTrans.gg90IsNil())
    {
        return true; // TODO change behavior of min trans ??
    }
    return (otrUpdTransId_gg00 >= minTrans);
}

//-----------------------------------------------------------------------------

void CnsRead_TransHistoryInfo::SetBeforeRef (const tgg91_PageRef& beforeRef)
{
    otrBeforeRef_gg00 = beforeRef;
}