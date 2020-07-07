/*!**************************************************************************

    module      : CnsRead_Manager.cpp

    -------------------------------------------------------------------------

    author      : ThomasA
    responsible : ThomasA

    special area: coniststent read
    description : defines a class providing support for constsistent read in the sql environment

    last changed: 2004-07-15

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


#include "ConsistentRead/CnsRead_Manager.hpp"
#include "hgg01_3.h"
#include "hbd01.h"
#include "hbd02.h"
#include "hgg04.h"
#include "hbd998.h"
#include "hkb50.h"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "ConsistentRead/CnsRead_Messages.hpp"
#include "ConsistentRead/CnsRead_TransHistoryTab.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

class CnsRead_ManagerImplementation
{
public :
    CnsRead_ManagerImplementation(tsp00_TaskId TaskId);

    inline tgg91_TransNo GetMinTrans();

    inline SAPDB_UInt8 GetHistorySavedInFileCount() const;

    inline SAPDB_UInt8 GetHistorySavedInPageCount() const;

    inline void IncHistorySavedInPageCount();

    inline bool ReadTransHistoryInfo (tsp00_TaskId TaskId, tgg00_Rec& Rec, Msg_List& MsgList);

    inline void SetMinTrans (const tgg91_TransNo& MinTrans);

    inline bool WriteTransHistoryInfo(tsp00_TaskId TaskId, const tgg00_Rec& Rec, Msg_List& MsgList);
private :
    class AuxilliaryTrans : public tgg00_TransContext 
    {
    public :
        AuxilliaryTrans(tsp00_TaskId TaskId);
    private :
        tsp00_RteCommRec m_rteCommRec;
    };
    tgg00_TransContext BuildAuxTrans( tsp00_TaskId TaskId );
    tgg00_FileId          m_transHistoryPagingFile;
    RTESync_NamedSpinlock m_minTransSync;
    tgg91_TransNo         m_minTrans;
    SAPDB_UInt8           m_savedInFileCount;
    SAPDB_UInt8           m_savedInPageCount;
};

//-----------------------------------------------------------------------------

CnsRead_ManagerImplementation::AuxilliaryTrans::AuxilliaryTrans( tsp00_TaskId TaskId )
{
    memset (this, 0, sizeof(*this));
    trTaskId_gg00 = TaskId;
    trSessionId_gg00.becomes( cgg_nil_session );
    trTransId_gg00.becomes( cgg_nil_trans );
    trWriteTransId_gg00.becomes( cgg_nil_trans );
    trSubtransId_gg00      = cgg_zero_subtrans;
    trRteCommPtr_gg00      = &m_rteCommRec;
}

//-----------------------------------------------------------------------------

CnsRead_ManagerImplementation::CnsRead_ManagerImplementation( tsp00_TaskId TaskId )
: m_minTransSync((SAPDB_UTF8*) "Cns_Read")
, m_savedInFileCount(0)
, m_savedInPageCount(0)
{
}

//-----------------------------------------------------------------------------

inline tgg91_TransNo CnsRead_ManagerImplementation::GetMinTrans() 
{
    RTESync_LockedScope lock(m_minTransSync);
    return m_minTrans;
}

//-----------------------------------------------------------------------------

inline SAPDB_UInt8 CnsRead_ManagerImplementation::GetHistorySavedInFileCount() const
{
    return m_savedInFileCount;
}

//-----------------------------------------------------------------------------

inline SAPDB_UInt8 CnsRead_ManagerImplementation::GetHistorySavedInPageCount() const
{
    return m_savedInPageCount;
}

//-----------------------------------------------------------------------------

void CnsRead_ManagerImplementation::IncHistorySavedInPageCount()
{
    ++m_savedInPageCount;
}

//-----------------------------------------------------------------------------

inline bool CnsRead_ManagerImplementation::ReadTransHistoryInfo (tsp00_TaskId TaskId, tgg00_Rec& Rec, Msg_List& MsgList)
{
    AuxilliaryTrans auxTransContext(TaskId);
    b02get_record (auxTransContext, m_transHistoryPagingFile, Rec.recKey_gg00(), Rec);
    if (auxTransContext.trError_gg00 != e_ok)
    {
//       ErrorList.AppendNewMessage (
//           SAPDBErr_MessageList ("CnsRead", __CONTEXT__, CNSREAD_TRANSHISTORY_NOT_READABLE, 
//            ToStr(*reinterpret_cast<tsp00_PageNo*>(&Rec.recBody_gg00())), ToStr(auxTransContext.trError_gg00)));
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------

inline void CnsRead_ManagerImplementation::SetMinTrans (const tgg91_TransNo& MinTrans)
{
    RTESync_LockedScope lock(m_minTransSync);
    m_minTrans = MinTrans;
}

//-----------------------------------------------------------------------------

inline bool CnsRead_ManagerImplementation::WriteTransHistoryInfo(tsp00_TaskId TaskId, const tgg00_Rec& Rec, Msg_List& MsgList)
{
    AuxilliaryTrans auxTransContext(TaskId);
    b02add_record (auxTransContext, m_transHistoryPagingFile, Rec);
    if (e_duplicate_key == auxTransContext.trError_gg00)
    {
        b02repl_record (auxTransContext, m_transHistoryPagingFile, Rec);
    }
    if (e_ok != auxTransContext.trError_gg00)
    {
//        ErrorList.AppendNewMessage (
//            SAPDBErr_MessageList ("CnsRead", __CONTEXT__, CNSREAD_TRANSHISTORY_NOT_WRITABLE, 
//            ToStr(*reinterpret_cast<const tsp00_PageNo*>(&Rec.recBody_gg00())), ToStr(auxTransContext.trError_gg00)));
        return false;
    }
    ++m_savedInFileCount;
    return true;
}

//-----------------------------------------------------------------------------

bool CnsRead_Manager::m_consistentReadEnabled          = false;
CnsRead_ManagerImplementation* CnsRead_Manager::m_impl = 0;

//-----------------------------------------------------------------------------

void CnsRead_Manager::BuildInvKey (const tgg00_Lkey& SourceKey, tgg00_Lkey& Key, const tgg91_TransNo&  UpdateTrans)
{
    int keyLen = SourceKey.keyLen_gg00();
    SAPDB_MemCopyNoCheck (&Key.keyVal_gg00()[0], &SourceKey.keyVal_gg00()[0], keyLen);
    for (int ix = 0; ix < sizeof(tgg91_TransNo); ++ix)
    {
        Key.keyVal_gg00()[keyLen + ix] = 255 - UpdateTrans[ix];
        
    }
    Key.keyLen_gg00() = keyLen + sizeof(UpdateTrans);
}

//-----------------------------------------------------------------------------

void CnsRead_Manager::EnableConsistentRead(tsp00_TaskId TaskId)
{
    if (IsConsistentRead_Configurated())
    {
        m_consistentReadEnabled  = true;
        g01ConsistentReadEnabled = true;
        if (0 == m_impl)
        {
            m_impl = new CnsRead_ManagerImplementation(TaskId); 
        }
    }
}

//-----------------------------------------------------------------------------

tgg91_TransNo CnsRead_Manager::GetMinTrans()
{
    if (m_impl)
    {
        return m_impl->GetMinTrans();
    }
    else
    {
        tgg91_TransNo transNo;
        transNo.gg90SetDummy(); // TODO
        return transNo;
    }
}

//-----------------------------------------------------------------------------

SAPDB_UInt8 CnsRead_Manager::GetHistorySavedInFileCount()
{
    return (m_impl) ? m_impl->GetHistorySavedInFileCount() : 0;
}

//-----------------------------------------------------------------------------

SAPDB_UInt8 CnsRead_Manager::GetHistorySavedInPageCount()
{
    return (m_impl) ? m_impl->GetHistorySavedInPageCount() : 0;
}

//-----------------------------------------------------------------------------

int CnsRead_Manager::IsIndexKeyConsistent (tgg00_TransContext& Trans, SAPDB_Byte* pKey, int KeyLength)
{
    if (CnsReadNone == Trans.trConsistReadInfo_gg00)
    {
        return KeyLength - sizeof(tgg91_TransNo);
    }
    tsp00_C6 transNo;
    KeyLength -= sizeof(tgg91_TransNo);
    for (int ix = 0; ix < sizeof(tgg91_TransNo); ++ix)
    {
        transNo[ix] = 255 - pKey[KeyLength+ix];

    }
    tsp00_Bool bConsistent;  
    tsp00_Bool bCancelled;
    kb50ObjConsistentCheck (Trans, *reinterpret_cast<tgg91_TransNo*>(&transNo[0]), bConsistent, bCancelled);
    return (bConsistent) ? KeyLength : 0;
}

//-----------------------------------------------------------------------------

void CnsRead_Manager::IncHistorySavedInPageCount()
{

}

//-----------------------------------------------------------------------------

bool CnsRead_Manager::ReadTransHistoryInfo (tsp00_TaskId TaskId, tgg00_Rec& Rec, Msg_List& MsgList)
{
    return (m_impl) ? m_impl->ReadTransHistoryInfo (TaskId, Rec, MsgList) : false;
}

//-----------------------------------------------------------------------------

void CnsRead_Manager::SetMinTrans (const tgg91_TransNo& MinTrans)
{
    if ( m_impl )
        m_impl->SetMinTrans(MinTrans);
}

//-----------------------------------------------------------------------------

bool CnsRead_Manager::WriteTransHistoryInfo(tsp00_TaskId TaskId, const tgg00_Rec& Rec, Msg_List& MsgList)
{
    return m_impl->WriteTransHistoryInfo(TaskId, Rec, MsgList);
}
