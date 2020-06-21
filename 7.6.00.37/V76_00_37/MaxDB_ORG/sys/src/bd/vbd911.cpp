/****************************************************************************

  module      : vbd911.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: recreate/check index for omsKeyedObject Containers
  description : description ...


  version     : 7.4.02.0
  last changed: 2005-03-31  11:32
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright by SAP AG, 1998



    ========== licence begin  GPL
    Copyright (C) 2000 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "hbd911.h"
#include "gbd05.h"   // cbd05_Current
#include "gbd910.h" 
#include "heo56.h"   // vsleep
#include "Logging/Log_History.hpp"  
#include "Logging/Log_Transaction.hpp"  

#include "hbd01.h"
#include "hbd03.h"
#include "hbd20_9.h" // bd20GetDestinationConverterVersion
#include "hbd22.h"
#include "hbd90.h"
#include "hgg06.h"
#include "hkb04.h"
#include "hkb50.h"
#include "hkb51.h"
#include "hkb560.h"
#include "hkb57.h"
#include "hkb67.h"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "FileDirectory/FileDir_OmsKeyPartition.hpp"

#include "RunTime/System/RTESys_MemoryBarrier.h" 
#include "KernelCommon/Kernel_VTrace.hpp"       
#include "liveCache/LVC_AbstractObjKeyIterator.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#ifndef TRANS_CONTEXT_HPP
#   include "Transaction/Trans_Context.hpp"
#endif

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
*  LOCAL FUNCTION DECLARATION                                               *
*===========================================================================*/

/*===========================================================================*
*  EXTERNAL VARIABLES                                                       *
*===========================================================================*/

/*===========================================================================*
*  LOCAL VARIABLES                                                          *
*===========================================================================*/

static RTESync_SpinlockPool *g_SpinLockPool   = NULL;
static RTESync_IRWRegion    *g_IRWLockRegion  = NULL;
static const bool           cDoShareLock      = true;

static bool					bRecreateActive   = false;
static tgg00_BasisError     g_RecreateError;
static FileDir_FileNo       g_RecreateFileNo;
static FileDir_FileNo       g_RecreateKeyFileNo[16];
static tgg00_FileId         g_RecreateKeyFileId[16];

/*===========================================================================*
*  LOCAL FUNCTIONS (CODE)                                                   *
*===========================================================================*/

/*===========================================================================*
*  GLOBAL FUNCTIONS (CODE)                                                  *
*===========================================================================*/

externCpp tgg00_BasisError&
bd911RecreateIndexError()
{
    return g_RecreateError;
}

/*-----------------------------------------------------------------------------------*/

externCpp FileDir_FileNo& 
bd911GetKeyFileNo(tsp00_Int4 ix)
{
    return g_RecreateKeyFileNo[ix];
}

/*-----------------------------------------------------------------------------------*/

externCpp tgg00_FileId& 
bd911GetKeyFileId(tsp00_Int4 ix)
{
    return g_RecreateKeyFileId[ix];
}

/*-----------------------------------------------------------------------------------*/

externCpp void
bd911InitRWRecreateIndex(void)
{
    if ( NULL == g_SpinLockPool) {                    
        g_SpinLockPool = reinterpret_cast<RTESync_SpinlockPool*>(
            ( RTEMem_RteAllocator::Instance())
            .Allocate(sizeof(RTESync_SpinlockPool)) );
        g_SpinLockPool = new(g_SpinLockPool) RTESync_SpinlockPool( UTF8("bd911:RecreateIndexRWLock"),1);
        g_IRWLockRegion = RTESync_CreateRWRegion( 1, *g_SpinLockPool, RTEMem_RteAllocator::Instance());
    }
}
/*-----------------------------------------------------------------------------------*/

externCpp void
bd911EnterRecreateIndex (tsp00_TaskId  Taskid,
                         bool          exclusiveRequest)
{
    g_IRWLockRegion->enter( exclusiveRequest, Taskid );
}

/*-----------------------------------------------------------------------------------*/

externCpp void
bd911LeaveRecreateIndex(tsp00_TaskId   Taskid,
                        bool           exclusiveRequest)
{ 
    g_IRWLockRegion->leave( exclusiveRequest, Taskid);
} 

/*-----------------------------------------------------------------------------------*/

cbd911RecreateIndexLock::~cbd911RecreateIndexLock()
{
    if(m_locked)
        bd911LeaveRecreateIndex (m_Trans.trTaskId_gg00, m_mode);
};

/*-----------------------------------------------------------------------------------*/

bool
cbd911RecreateIndexLock::Lock(bool  bExclusive)
{
    if (Kernel_IAdminInfo::Instance().KernelStateIsOnline())
    {
        m_mode = bExclusive;
        bd911EnterRecreateIndex (m_Trans.trTaskId_gg00, m_mode);
        m_locked = true;
    }
    return m_locked;
};

/*-----------------------------------------------------------------------------------*/

bool 
cbd911RecreateIndexLock::IsNeeded(const FileDir_FileNo& FileNo)
{
    return (bRecreateActive && (FileNo == g_RecreateFileNo));
}; 

/*-----------------------------------------------------------------------------------*/

externCpp void
bd911KeyToConsole( void *const pKey, tsp00_Int4 ObjKeyLen)
{
    Kernel_OpError errmsg( csp3_bd_msg, csp3_n_obj ); 
    errmsg << "key=0x";
    for (int i=0; i < ObjKeyLen; i++)
        errmsg << ToStr((SAPDB_UInt1)*((SAPDB_UInt1*)pKey + i), _T_h);
}

/*-----------------------------------------------------------------------------------*/

externCpp void
bd911CheckIndex (tgg00_TransContext   &Trans,
                 FileDir_FileNo       &FileNo)
{
    ROUTINE_DBG_MEO00 ("bd911CheckIndex");

    Kernel_OpInfo( csp3_n_obj ) << "CHECK INDEX FOR CONTAINER '" << FileNo << "'";
    tgg00_BasisError& TrError = Trans.trError_gg00;
    Msg_List&         ErrorList = Trans_Context::GetContext(Trans).ErrorList();

    FileDir_Oms    fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    if (Data_OmsKeyFile != fd_smart.GetFileType())
    {
        Kernel_OpWarning( csp3_bd_msg, csp3_n_obj ) 
            << "bd911CheckIndex - file no keyed object container! Fileno:" << FileNo;
        TrError = e_wrong_class_id;
        return;
    }

    FileDir_OmsKey      fd_key_smart;
    fd_key_smart.Assign(fd_smart);

    tgg00_BasisError    ReturnError = TrError;

    kb04StartConsistentTrans (Trans, false /*not in omsVersion*/);

    tgg00_FileId   ObjFileId;
    fd_key_smart.BuildFileId(ObjFileId);

    tgg92_KernelOid    OId;    
    OId.gg92SetNil();
    tsp00_Int4         ObjKeyLen;    
    cbd05_Current Current (Trans, m_select, ObjFileId);
    {

        cbd910_PrimObjDataNode NptrsPage(Current, fd_key_smart.GetRootPage(), nr_for_read);
        if (e_ok == TrError)
            ObjKeyLen  = NptrsPage.np_ptr()->nd_obj_key_len();

        while ((e_ok == TrError) && NptrsPage.bd910PageIsAvailable()) 
        {   
            const tsp00_Bool bWithObject        = SAPDB_FALSE;
            const bool       ignoreEmptyObjects = SAPDB_TRUE;
            const tsp00_Int4 ExtObjBodySize     = 0;
            void*            pExtObjBody        = NULL;
            tgg91_PageRef    ResultObjVers;
            tsp00_Int4       ObjBodySize        = 0;
            tsp00_Bool       bNextObjIsFound    = SAPDB_FALSE;
            tsp00_Int4       LogReadAccessesPerPage;
            tgg91_PageRef    ObjHistRef;
            bd90NextObjConsistentOnThisPage (Current, fd_smart, OId, ResultObjVers, ObjHistRef, bWithObject, 
                false /* only var hdr*/, ExtObjBodySize, pExtObjBody, NptrsPage, ObjBodySize, 
                bNextObjIsFound, LogReadAccessesPerPage, ErrorList);     
            if (TrError != e_ok)
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                    << "bd91CheckIndex - error:" << TrError
                    << " FileNo:" << FileNo
                    << " bd90NextObjConsistentOnThisPage OId=" 
                    << OId.gg92GetPno() << "." 
                    << OId.gg92GetPos() << " (vers " 
                    << OId.gg92GetFrameVers() << " )";
                break;
            }

            if (bNextObjIsFound) 
            { 
                tgg00_BasisError AuxError;
                tgg92_KernelOid  ResultOid;
                tsp00_Key        key;
                void *const      pKey       = (tsp00_Byte*)NptrsPage.np_ptr() + OId.gg92GetPos()
                    + NptrsPage.np_ptr()->nd_obj_frame_len() - NptrsPage.np_ptr()->nd_obj_key_len();
                memcpy(&key[0], pKey, ObjKeyLen);
                NptrsPage.bd910ReleasePage();
                bd22GetOId (Trans, FileNo, ObjKeyLen, pKey, ResultOid, ErrorList);
                AuxError = TrError;
                TrError = e_ok;
                NptrsPage.bd910AssignToPage(OId.gg92GetPno());
                if ( TrError != e_ok)
                    break;
                else 
                    TrError = AuxError;
                if (TrError != e_ok)
                {
                    Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                        << "bd91CheckIndex - error:" << TrError
                        << " FileNo:" << FileNo
                        << " looking up key for OId=" 
                        << OId.gg92GetPno() << "." 
                        << OId.gg92GetPos() << " (vers " 
                        << OId.gg92GetFrameVers() << " )";
                    bd911KeyToConsole(&key[0], ObjKeyLen);
                    if ( e_ok == ReturnError) 
                        ReturnError = TrError;
                    TrError = e_ok;
                }
                else if ( ! OId.gg92IsEqIgnoreFrameVers(ResultOid))
                {
                    TrError = e_key_not_allowed;
                    Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                        << "bd91CheckIndex - error:" << TrError
                        << " FileNo:" << FileNo
                        << " key mismatch for OId=" 
                        << OId.gg92GetPno() << "." 
                        << OId.gg92GetPos() << " (vers " 
                        << OId.gg92GetFrameVers() << " )"
                        << " found OId="
                        << ResultOid.gg92GetPno() << "." 
                        << ResultOid.gg92GetPos() << " (vers " 
                        << ResultOid.gg92GetFrameVers() << " )";
                    bd911KeyToConsole(&key[0], ObjKeyLen);
                    if ( e_ok == ReturnError) 
                        ReturnError = TrError;
                    TrError = e_ok;
                }
            }
            else
            {
                /* reset OId and  goto next page */
                OId.gg92SetNil();
                NptrsPage.bd910MoveToNextPage();
            }

            if ((TrError == e_ok) && Current.curr_trans->trRteCommPtr_gg00->to_cancel)
            {
                Current.curr_trans->trRteCommPtr_gg00->to_cancel = SAPDB_FALSE;
                TrError = e_cancelled;
                break;
            }

        }
    }

    if ( e_ok == ReturnError) 
        ReturnError = TrError;
    TrError = e_ok;
    kb04EndConsistentTrans (Trans);
    if (e_ok == TrError)
    {
        tsp00_Key     StartKey;
        tsp00_Key     EndKey;
        memset(&StartKey[0],0x00,sizeof(tsp00_Key));
        memset(&EndKey[0],0xff,sizeof(tsp00_Key));
        void *p = NULL;

        kb04CreateKeyIterator (Trans, ObjFileId, ObjKeyLen, &StartKey[0], &StartKey[0], &EndKey[0], 
            p, true /*ascending*/);
        if (NULL != p)
        {
            LVC_AbstractObjKeyIterator &KeyIterator = *reinterpret_cast<LVC_AbstractObjKeyIterator*>(p);

            cbd05_Current  Current (Trans, m_select, ObjFileId);          
            bool bInclusive = true;
            while (!KeyIterator.IsEmpty() && (e_ok == TrError))
            {
                tgg92_KernelOid ResultOId[100];
                tsp00_Int4      noOfOid = 100;
                tsp00_Int4      KeyBufLen = sizeof(StartKey);
                {
                    cbd911RecreateIndexLock lock(Trans);
                    lock.Lock(false /*non exclusive */);
                    KeyIterator.GetNextOIdsWithKey( NULL, &ResultOId[0], noOfOid, &StartKey[0], 
                        KeyBufLen, bInclusive, ErrorList); 
                }
                bInclusive = false;

                int i;
                for (i=0; i<noOfOid; ++i)
                {
                    unsigned char* pResultKey = &StartKey[0];
                    pResultKey = pResultKey + i*KeyBufLen;
                    cbd910_PrimObjDataNode   NptrsPage (Current, ResultOId[i].gg92GetPno(), nr_for_read);
                    if(TrError != e_ok) 
                    {
                        NptrsPage.bd910ReleasePage();
                        tgg92_KernelOid Result2OId;
                        bd22GetOId (Trans, FileNo, ObjKeyLen, pResultKey, Result2OId, ErrorList);
                        if (TrError != e_ok)
                            TrError = e_ok;
                        else if ( ResultOId[i].gg92IsEqIgnoreFrameVers(Result2OId))
                        {
                            TrError = e_key_not_allowed;
                            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                << "bd91CheckIndex - error:" << TrError
                                << " FileNo:" << FileNo
                                << " dangling OId=" 
                                << ResultOId[i].gg92GetPno() << "." 
                                << ResultOId[i].gg92GetPos() << " (vers " 
                                << ResultOId[i].gg92GetFrameVers() << " )"
                                << " found";
                            bd911KeyToConsole(pResultKey, ObjKeyLen);
                            if ( e_ok == ReturnError) 
                                ReturnError = TrError;
                            TrError = e_ok;
                        }                   
                    }
                    else
                    {
                        tbd900_ObjFrame* pObjFrame = NptrsPage.bd910ObjFramePtr (ResultOId[i].gg92GetPos());
                        void *const      pKey       = (tsp00_Byte*)NptrsPage.np_ptr() + ResultOId[i].gg92GetPos()
                            + NptrsPage.np_ptr()->nd_obj_frame_len() - NptrsPage.np_ptr()->nd_obj_key_len();
                        memcpy(&StartKey[0], pKey, ObjKeyLen);

                        switch (pObjFrame->objHeader_bd900.ohdState_gg92)
                        {
                        case obsFree_egg00:
                            {
                                NptrsPage.bd910ReleasePage();
                                tgg92_KernelOid Result2OId ;
                                bd22GetOId (Trans, FileNo, ObjKeyLen, pResultKey, Result2OId, ErrorList);
                                if (TrError != e_ok)
                                    TrError = e_ok;
                                else if ( ResultOId[i].gg92IsEqIgnoreFrameVers(Result2OId))
                                {
                                    TrError = e_key_not_allowed;
                                    Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                        << "bd91CheckIndex - error:" << TrError
                                        << " FileNo:" << FileNo
                                        << " dangling OId=" 
                                        << ResultOId[i].gg92GetPno() << "." 
                                        << ResultOId[i].gg92GetPos() << " (vers " 
                                        << ResultOId[i].gg92GetFrameVers() << " )"
                                        << " found";
                                    bd911KeyToConsole(pKey, ObjKeyLen);
                                    if ( e_ok == ReturnError) 
                                        ReturnError = TrError;
                                    TrError = e_ok;
                                }                   
                            }
                            break;
                        case obsKeyReserved_egg00:
                        case obsFreeAfterEot_egg00:
                        case obsOccupied_egg00:
                        case obsReserved_egg00:
                            if (pResultKey != NULL)
                            {
                                if ( 0 != memcmp(pResultKey,pKey,NptrsPage.np_ptr()->nd_obj_key_len()))
                                {
                                    NptrsPage.bd910ReleasePage();
                                    tgg92_KernelOid Result2OId;
                                    bd22GetOId (Trans, FileNo, ObjKeyLen, pResultKey, Result2OId, ErrorList);
                                    if (TrError != e_ok)
                                    {
                                        TrError = e_ok;
                                    }
                                    else if ( ResultOId[i].gg92IsEqIgnoreFrameVers(Result2OId))
                                    {
                                        TrError = e_key_not_allowed;
                                        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                            << "bd91CheckIndex - error:" << TrError
                                            << " FileNo:" << FileNo
                                            << " key mismatch OId=" 
                                            << ResultOId[i].gg92GetPno() << "." 
                                            << ResultOId[i].gg92GetPos() << " (vers " 
                                            << ResultOId[i].gg92GetFrameVers() << " )";
                                        bd911KeyToConsole(pKey, ObjKeyLen);
                                        bd911KeyToConsole(pResultKey, ObjKeyLen);
                                        if ( e_ok == ReturnError) 
                                            ReturnError = TrError;
                                        TrError = e_ok;
                                    }                   
                                }
                            }
                            break;
                        }                    
                    }
                }
            }
            if (e_ok == ReturnError) 
                ReturnError = TrError;
            TrError = e_ok; /* make shure the KeyIterator is destroyed */
            kb04DestroyKeyIterator(Trans, p);
        }
    };
    if ( e_ok != ReturnError) 
    {
        TrError = ReturnError;
    }
};

/*-----------------------------------------------------------------------------------*/

class SingleUseControl
{
public:
    SingleUseControl(FileDir_FileNo  &FileNo)
        : m_active(false)
        , m_fileno(FileNo)
    {
        Kernel_OpInfo( csp3_n_obj ) 
            << "start: REPLACE INDEX FOR CONTAINER '" << FileNo << "'";
    };

    ~SingleUseControl() {
        SetInactive();
        Kernel_OpInfo( csp3_n_obj ) 
            << "end: REPLACE INDEX FOR CONTAINER '" << m_fileno << "'";
    };
    bool SetActive(tgg00_TransContext &Trans) 
    {
        if (! bRecreateActive)
        {
            bRecreateActive = true;
            m_active = true;
        }
        return m_active;
    }
    void SetInactive() 
    {
        if (m_active)
        {
            int i;
            for (i = 0; i<16; i++)
            {
                g_RecreateKeyFileNo[i].Invalidate();
                memset(&g_RecreateKeyFileId[i],0,sizeof(tgg00_FileId));
            }
            g_RecreateFileNo.Invalidate();
            bRecreateActive = false;
        }
    }
private:
    FileDir_FileNo  m_fileno;
    bool		    m_active;
};

/*-----------------------------------------------------------------------------------*/

externCpp void
bd911RecreateIndex (tgg00_TransContext   &Trans,
                    FileDir_FileNo       &FileNo )
{
    ROUTINE_DBG_MEO00 ("bd91RecreateIndex");

    SingleUseControl callOnlyOnce(FileNo); 

    tgg00_BasisError&   TrError = Trans.trError_gg00;
    Msg_List&           ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    FileDir_IDirectory& IFDC  = FileDir_IDirectory::SessionInstance(Trans);

    FileDir_Oms    fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    if (Data_OmsKeyFile != fd_smart.GetFileType())
    {
        Kernel_OpWarning( csp3_bd_msg, csp3_n_obj ) 
            << "bd911RecreateIndex - file no keyed object container! Fileno:" << FileNo;
        TrError = e_wrong_class_id;
        return;
    }

    FileDir_OmsKey      fd_key_smart;
    fd_key_smart.Assign(fd_smart);

    tgg00_FileId   ObjFileId;
    fd_key_smart.BuildFileId(ObjFileId);

    tgg92_KernelOid     OId;    
    OId.gg92SetNil();
    tsp00_Int4          ObjFrameLen;
    int                 AlignedObjFrameLen;  
    tsp00_Int4          ObjKeyLen;
    tgg00_ObjPagePos    ObjPos;
    tsp00_Int4          ObjBodySize;

    cbd05_Current Current (Trans, m_select, ObjFileId);
    {
        cbd910_PrimObjDataNode  NptrsPage(Current, fd_key_smart.GetRootPage() , nr_for_read);
        if (TrError != e_ok) return; 
        ObjFrameLen        = NptrsPage.np_ptr()->nd_obj_frame_len();        
        AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);  
        ObjKeyLen          = NptrsPage.np_ptr()->nd_obj_key_len();
        ObjBodySize        = ObjFrameLen - sizeof(tgg92_ObjHeader);
    }

    int ix;
    int NumObjKeyFiles = fd_key_smart.GetKeyPartitionCount();
    {
        cbd911RecreateIndexLock lock(Trans);
        lock.Lock(true /*exclusive */);

        if (!callOnlyOnce.SetActive(Trans))
        {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj ) 
                << "bd91RecreateIndex - Recreate already active for some container!";
            TrError = e_recreate_index_failed;
            return;
        }

        for ( ix=0; ix < NumObjKeyFiles ; ix++ )
        {
            tgg00_ObjFileType FileType;
            FileType.becomes(oftUnknownType_egg00);

            bd911GetKeyFileNo(ix) = IFDC.GetNewFileNo();
            IFDC.BuildNewOmsFileId( bd911GetKeyFileNo(ix), FileType, ix, bd911GetKeyFileId(ix));
            bd03CreateOmsKeyPartitionFile( Trans, ErrorList, bd911GetKeyFileId(ix), bd911GetKeyFileNo(ix),
                FileNo, ix + 1, SAPDB_FALSE /*addFileDir*/ );

            if (e_ok != Trans.trError_gg00)
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                    << "bd91RecreateIndex - file create error:" << Trans.trError_gg00
                    << " FileNo:" << FileNo
                    << " KeyIndex:" << ix;
                return;
            }
        }
        g_RecreateFileNo = FileNo;
        RTESys_WriteMemoryBarrier();
    }

    tgg91_TransNo WriteTransNo;
    k57new_write_trans(Trans.trTaskId_gg00 ,WriteTransNo);
    k51WaitForAllOlderActiveTransToStop(Trans);

    bool  bTryAgain = true;
    while (bTryAgain 
        && (bd911RecreateIndexError() == e_ok)
        && (e_ok == TrError))
    {
        bTryAgain = false;
        tgg00_ObjPagePos ObjPosBeg = OId.gg92GetPos();
        tsp00_PageNo     actPage   = OId.gg92GetPno();
        if (NIL_PAGE_NO_GG00 == actPage) 
            actPage = fd_key_smart.GetRootPage();
        {
            cbd910_PrimObjDataNode  NptrsPage(Current, actPage, nr_for_update);
            if (e_ok != Trans.trError_gg00)
                return;
            do 
            {
                if ( NptrsPage.np_ptr()->nd_max_obj_cnt() > NptrsPage.np_ptr()->nd_free_obj_cnt())
                {
                    if ( ObjPosBeg == 0 )
                        ObjPosBeg = NptrsPage.np_ptr()->nd_first_obj();

                    for ( ObjPos = ObjPosBeg;
                        ((ObjPos + ObjFrameLen) <= BODY_END_BD00) && (e_ok == TrError);
                        ObjPos += AlignedObjFrameLen )
                    { 
                        OId.gg92SetPno (NptrsPage.np_ptr()->nd_id());
                        OId.gg92SetPos (ObjPos);                   
                        tbd900_ObjFrame* pObjFrame = NptrsPage.bd910ObjFramePtr (ObjPos);
                        OId.gg92SetFrameVers (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);

                        switch (pObjFrame->objHeader_bd900.ohdState_gg92)
                        {
                        case obsKeyReserved_egg00:
                            if (WriteTransNo > 
                                pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrLockTransId_gg00 )
                                do 
                                {
                                    tgg91_PageRef WantedObjVers = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;
                                    tsp00_Uint2 AuxReqTimeout = Current.curr_trans->trObjReqTimeout_gg00; //save
                                    Current.curr_trans->trObjReqTimeout_gg00 = 9000;   
                                    kb67LockObj (*Current.curr_trans, Current.curr_tree_id, OId, WantedObjVers,
                                        pObjFrame->objHeader_bd900.ohdState_gg92,
                                        pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
                                    Current.curr_trans->trObjReqTimeout_gg00 = AuxReqTimeout;   
                                    if (e_ok == Trans.trError_gg00)
                                    {
                                        Trans.trError_gg00 = e_recreate_index_failed;
                                        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                            << "bd91RecreateIndex - error:" << TrError
                                            << " FileNo:" << FileNo
                                            << " OId=" 
                                            << OId.gg92GetPno() << "." 
                                            << OId.gg92GetPos() << " (vers " 
                                            << OId.gg92GetFrameVers() << " )"
                                            << " obsKeyReserved could be locked";
                                    }
                                }
                                while ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsKeyReserved_egg00) 
                                    &&
                                    (e_ok == Trans.trError_gg00));

                                break;
                        case obsFreeAfterEot_egg00:
                            {
                                bool           bSkipOid            = false;
                                tgg91_PageRef  CurrEntryRef;
                                tsp00_Uint4    BodyLen             = ObjBodySize;
                                tgg00_ObjFrameVers ResultFrameVers = NIL_OBJ_FRAME_VERS_GG92;
                                Log_ActionType ActionType          = Log_DeleteObject;
                                tgg91_TransNo  ConsistentView;
                                tgg91_TransNo  PrevUpdTrans;
                                tgg91_PageRef  NextPageRef;
                                CurrEntryRef = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;

                                if ( NIL_PAGE_NO_GG00 == CurrEntryRef.gg91RefPno())
                                {
                                    Kernel_OpWarning( csp3_bd_msg, csp3_n_obj ) 
                                        << "bd91RecreateIndex - obsFreeAfterEot without history found"
                                        << " FileNo:" << FileNo
                                        << " OId=" 
                                        << OId.gg92GetPno() << "." 
                                        << OId.gg92GetPos() << " (vers " 
                                        << OId.gg92GetFrameVers() << " )";
                                    Trans.trError_gg00 = e_obj_history_not_found;
                                }
                                else
                                {
                                    Log_History::GetBeforeImage (Trans,
                                        OId,
                                        CurrEntryRef,
                                        0,              /* ObjBodySize */
                                        NULL,           /* pBody */
                                        false,          /* bOnlyExistenceCheck */
                                        BodyLen, 
                                        ResultFrameVers,
                                        ActionType,
                                        ConsistentView,
                                        PrevUpdTrans,
                                        NextPageRef,
                                        ErrorList);

                                    if  ((e_ok != Trans.trError_gg00) || (ActionType != Log_DeleteObject ))
                                    {
                                        Kernel_OpWarning( csp3_bd_msg, csp3_n_obj ) 
                                            << "bd91RecreateIndex - obsFreeAfterEot error:" << TrError
                                            << " FileNo:" << FileNo
                                            << " OId=" 
                                            << OId.gg92GetPno() << "." 
                                            << OId.gg92GetPos() << " (vers " 
                                            << OId.gg92GetFrameVers() << " )"
                                            << " ActionType:" << ActionType;
                                        Trans.trError_gg00 = e_recreate_index_failed;
                                        bSkipOid = true;
                                    } 
                                    else if (PrevUpdTrans != pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00)
                                        bSkipOid = false;
                                    else
                                    {
                                        while (e_ok == Trans.trError_gg00) 
                                        {
                                            CurrEntryRef = NextPageRef;
                                            if ( NIL_PAGE_NO_GG00 == CurrEntryRef.gg91RefPno())
                                                Trans.trError_gg00 = e_obj_history_not_found;
                                            else
                                            {
                                                Log_History::GetBeforeImage (Trans,
                                                    OId,
                                                    CurrEntryRef,
                                                    0,              /* ObjBodySize */
                                                    NULL,           /* pBody */
                                                    false,          /* bOnlyExistenceCheck */
                                                    BodyLen, 
                                                    ResultFrameVers,
                                                    ActionType,
                                                    ConsistentView,
                                                    PrevUpdTrans,
                                                    NextPageRef,
                                                    ErrorList);
                                            }
                                            if  (e_ok == Trans.trError_gg00)
                                            {
                                                if ((ActionType == Log_NewObject )
                                                    && (PrevUpdTrans.gg90IsNil()))
                                                {
                                                    bSkipOid = true;
                                                    break;
                                                }
                                                else if (PrevUpdTrans != pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00)
                                                {
                                                    bSkipOid = false;
                                                    break;
                                                }
                                            }
                                            if ((e_ok != Trans.trError_gg00) 
                                                && (e_obj_history_not_found != Trans.trError_gg00))
                                            {
                                                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                                                    << " recreateindex history access error:" << Trans.trError_gg00
                                                    << " OID=" << OId.gg92GetPno() << "." 
                                                    << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")";
                                            }
                                        }
                                    }
                                }

                                if (e_obj_history_not_found == Trans.trError_gg00)
                                {
                                    bSkipOid = true;
                                    Trans.trError_gg00 = e_ok;
                                }

                                if (e_ok != Trans.trError_gg00) 
                                    break;

                                if (!bSkipOid)                                
                                {
                                    tsp00_Int4    ObjKeyFileIndex;
                                    tbd_nodeptr  &pPage = NptrsPage.np_ptr();
                                    void *const   pKey  = (tsp00_Byte*) pPage + ObjPos + ObjFrameLen - ObjKeyLen;
                                    ObjKeyFileIndex = (NumObjKeyFiles == 1 ? 0 :bd22ObjKeyFileIndex (ObjKeyLen, pKey, NumObjKeyFiles)); 
                                    tgg00_FileId&  ObjKeyFileId = bd911GetKeyFileId(ObjKeyFileIndex);

                                    tsp00_Int4       FoundOIdLen;
                                    tgg92_KernelOid  FoundOId;
                                    FoundOId.gg92SetNil();

                                    bd03OmsAddInv (Trans, ObjKeyFileId,
                                        tsp00_BytePtr (pKey), ObjKeyLen, tsp00_BytePtr(&OId), sizeof(OId),
                                        sizeof(FoundOId), tsp00_BytePtr(&FoundOId), FoundOIdLen);
                                    if (g01vtrace.vtrOmsUpd_gg00)
                                    {
                                        Kernel_VTrace vtrace;
                                        vtrace << __FILE__ << ":" << __LINE__ << 
                                            " recreateindex ADD freeaftereot OID=" << OId.gg92GetPno() << "." 
                                            << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ") KeyLen:"
                                            << ObjKeyLen << " Key=";
                                        for (int i=0; i < ObjKeyLen; i++)
                                            vtrace << ToStr((SAPDB_UInt1)*((SAPDB_UInt1*)pKey + i), _T_h);
                                        vtrace << " error:" << TrError
                                            << " ActionType" << ActionType
                                            << " CurrEntyRef:" << ( NIL_PAGE_NO_GG00 == CurrEntryRef.gg91RefPno());
                                        vtrace << FlushLine;
                                        vtrace << "Object found via key OID=" 
                                            << FoundOId.gg92GetPno() << "." 
                                            << FoundOId.gg92GetPos() << " (vers " 
                                            << FoundOId.gg92GetFrameVers() << " )";
                                    }
                                    switch (TrError)
                                    {
                                    case e_ok:
                                        break;
                                    case e_duplicate_invkey:
                                    case e_duplicate_key   : 
                                        if ( OId.gg92IsEqIgnoreFrameVers(FoundOId))
                                            TrError = e_ok; 
                                        else
                                        {
                                            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                                << "bd91RecreateIndex - error:" << TrError
                                                << " FileNo:" << FileNo
                                                << " OId=" 
                                                << OId.gg92GetPno() << "." 
                                                << OId.gg92GetPos() << " (vers " 
                                                << OId.gg92GetFrameVers() << " )"
                                                << " and OId=" 
                                                << FoundOId.gg92GetPno() << "." 
                                                << FoundOId.gg92GetPos() << " (vers " 
                                                << FoundOId.gg92GetFrameVers() << " )"
                                                << " have same key";
                                            bd911KeyToConsole(pKey, ObjKeyLen);
                                        }
                                        break;
                                    default:
                                        {
                                            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                                << "bd91RecreateIndex - error:" << TrError
                                                << " FileNo:" << FileNo
                                                << " OId=" 
                                                << OId.gg92GetPno() << "." 
                                                << OId.gg92GetPos() << " (vers " 
                                                << OId.gg92GetFrameVers() << " )";
                                            bd911KeyToConsole(pKey, ObjKeyLen);
                                        }
                                    }
                                }
                                else
                                {
                                    tbd_nodeptr  &pPage = NptrsPage.np_ptr();
                                    void *const   pKey  = (tsp00_Byte*) pPage + ObjPos + ObjFrameLen - ObjKeyLen;
                                    if (g01vtrace.vtrOmsUpd_gg00)
                                    {
                                        Kernel_VTrace vtrace;
                                        vtrace << " recreateindex SKIP freeaftereot OID=" << OId.gg92GetPno() << "." 
                                            << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ") KeyLen:"
                                            << ObjKeyLen << " Key=";
                                        for (int i=0; i < ObjKeyLen; i++)
                                            vtrace << ToStr((SAPDB_UInt1)*((SAPDB_UInt1*)pKey + i), _T_h);
                                        vtrace << " error:" << TrError
                                            << " ActionType:" << ActionType;
                                    }
                                }
                            }
                            break;
                        case obsOccupied_egg00:
                            {
                                tsp00_Int4    ObjKeyFileIndex;
                                tbd_nodeptr  &pPage = NptrsPage.np_ptr();
                                void *const   pKey  = (tsp00_Byte*) pPage + ObjPos + ObjFrameLen - ObjKeyLen;
                                ObjKeyFileIndex = (NumObjKeyFiles == 1 ? 0 :bd22ObjKeyFileIndex (ObjKeyLen, pKey, NumObjKeyFiles)); 
                                tgg00_FileId&  ObjKeyFileId = bd911GetKeyFileId(ObjKeyFileIndex);

                                tsp00_Int4       FoundOIdLen;
                                tgg92_KernelOid  FoundOId;
                                FoundOId.gg92SetNil();

                                bd03OmsAddInv (Trans, ObjKeyFileId,
                                    tsp00_BytePtr (pKey), ObjKeyLen, tsp00_BytePtr(&OId), sizeof(OId),
                                    sizeof(FoundOId), tsp00_BytePtr(&FoundOId), FoundOIdLen);

                                if (g01vtrace.vtrOmsUpd_gg00)
                                {
                                    Kernel_VTrace vtrace;
                                    vtrace << " recreateindex ADD occupied OID=" << OId.gg92GetPno() << "." 
                                        << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ") KeyLen:"
                                        << ObjKeyLen << " Key=";
                                    for (int i=0; i < ObjKeyLen; i++)
                                        vtrace << ToStr((SAPDB_UInt1)*((SAPDB_UInt1*)pKey + i), _T_h);
                                    vtrace << " error:" << TrError;
                                    vtrace << FlushLine;
                                    vtrace << "Object found via key OID=" 
                                        << FoundOId.gg92GetPno() << "." 
                                        << FoundOId.gg92GetPos() << " (vers " 
                                        << FoundOId.gg92GetFrameVers() << " )";
                                }                                    
                                switch (TrError)
                                {
                                case e_ok:
                                    break;
                                case e_duplicate_invkey:
                                case e_duplicate_key   : 
                                    if ( OId.gg92IsEqIgnoreFrameVers(FoundOId))
                                        TrError = e_ok; 
                                    else
                                    {
                                        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                            << "bd91RecreateIndex - error:" << TrError
                                            << " FileNo:" << FileNo
                                            << " OId=" 
                                            << OId.gg92GetPno() << "." 
                                            << OId.gg92GetPos() << " (vers " 
                                            << OId.gg92GetFrameVers() << " )"
                                            << " and OId=" 
                                            << FoundOId.gg92GetPno() << "." 
                                            << FoundOId.gg92GetPos() << " (vers " 
                                            << FoundOId.gg92GetFrameVers() << " )"
                                            << " have same key";
                                        bd911KeyToConsole(pKey, ObjKeyLen);
                                    }
                                    break;
                                default:
                                    {
                                        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                            << "bd91RecreateIndex - error:" << TrError
                                            << " FileNo:" << FileNo
                                            << " OId=" 
                                            << OId.gg92GetPno() << "." 
                                            << OId.gg92GetPos() << " (vers " 
                                            << OId.gg92GetFrameVers() << " )";
                                        bd911KeyToConsole(pKey, ObjKeyLen);
                                    }
                                }
                            }
                            break;
                        case obsFree_egg00:
                            break;
                        case obsReserved_egg00:
                            {
                                TrError = e_wrong_object_state;
                                Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                                    << "bd91RecreateIndex - error:" << TrError
                                    << " FileNo:" << FileNo
                                    << " OId=" 
                                    << OId.gg92GetPno() << "." 
                                    << OId.gg92GetPos() << " (vers " 
                                    << OId.gg92GetFrameVers() << " )";
                            }
                        }                      
                    }
                } 
                if (e_ok == TrError)
                {
                    ObjPosBeg = 0;
                    NptrsPage.bd910MoveToNextPage();
                }
            }  
            while ((e_ok == TrError) 
                && (bd911RecreateIndexError() == e_ok )
                && NptrsPage.bd910PageIsAvailable());  
        }
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
    }

    {
        cbd911RecreateIndexLock lock(Trans);
        lock.Lock(true /*exclusive */);
        if (( bd911RecreateIndexError() == e_ok ) &&  (e_ok == TrError))
        {
            if ((e_ok == TrError) && ( bd911RecreateIndexError() == e_ok ))
            {
                cbd910_PrimObjDataNode  NptrsPage(Current, fd_key_smart.GetRootPage() , nr_for_update);
                if (e_ok == TrError)
                    TrError = bd911RecreateIndexError();
                if (e_ok == TrError)
                {
                    Converter_Version convVersion = 
                        bd20GetDestinationConverterVersion( Trans.trTaskId_gg00, NptrsPage.np_cbptr());

                    for ( ix=0; ix < NumObjKeyFiles ; ix++ )
                    {
                        if ( e_ok == TrError )
                        {
                            FileDir_ReturnCode      fd_rc;
                            FileDir_OmsKeyPartition fd_keypart_smart;
                            FileDir_OmsKeyPartition fd_keypartnew_smart;
                            FileDir_FileNo          KeyFileNo  = fd_key_smart.GetKeyPartition(ix + 1);
                            fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, KeyFileNo, fd_keypart_smart, ErrorList);
                            if (FileDir_Okay != fd_rc) {
                                bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
                                return;
                            }     
                            tgg00_FileId  ObjKeyFileIdOld;
                            fd_keypart_smart.BuildFileId(ObjKeyFileIdOld);

                            fd_rc = IFDC.AddOmsKeyPartitionFile( 
                                Trans.trTaskId_gg00, bd911GetKeyFileNo(ix), FileNo, convVersion, 
                                ix+1, bd911GetKeyFileId(ix).fileRoot_gg00(), ErrorList, &fd_keypartnew_smart );
                            if( FileDir_Okay == fd_rc )
                            {
                                fd_keypartnew_smart.SetLeafPageCount( 1 );

                                tgg91_TransNo DummyTransNo;               /* *** TODO *** */
                                gg06SetNilTrans (DummyTransNo);           /* *** TODO *** */
                                fd_keypart_smart.MarkFileDeleted(DummyTransNo);
                                bd03ReleaseInvTree(Trans, ObjKeyFileIdOld);
                                fd_rc = IFDC.DelFile(Trans.trTaskId_gg00, KeyFileNo, ErrorList);
                                if (FileDir_Okay != fd_rc) 
                                {
                                    if ( fd_rc == FileDir_FileNoNotFound ) {
                                        Trans.trError_gg00 = e_file_not_found;
                                    } else 
                                        bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
                                }
                            }
                            else
                            {
                                TrError = e_wrong_obj_file_id;
                            }
                        }
                        else
                            bd03ReleaseInvTree(Trans, bd911GetKeyFileId(ix));
                    }
                }
            }
        }

        if ( bd911RecreateIndexError() != e_ok )
            if (e_ok == TrError) 
                TrError = bd911RecreateIndexError();

        if (e_ok != TrError)
        {
            tgg00_BasisError auxError = TrError;
            TrError = e_ok;
            for ( ix=0; ix < NumObjKeyFiles ; ix++ )
            {
                bd03ReleaseInvTree(Trans, bd911GetKeyFileId(ix));
                if (e_ok != Trans.trError_gg00)
                {
                    Kernel_OpWarning( csp3_bd_msg, csp3_n_obj ) 
                        << "bd91RecreateIndex - file destroy error:" << Trans.trError_gg00
                        << " FileNo:" << FileNo
                        << " KeyIndex:" << ix;
                    TrError = e_ok;
                }
            }
            TrError = auxError;
        }
        callOnlyOnce.SetInactive();
        RTESys_WriteMemoryBarrier();
    }
    if (e_ok == TrError)
    {
        Trans_Context& t = Trans_Context::GetContext(Trans);
        kb560StartSavepointAndWait (t, mm_index);
    }
};

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/