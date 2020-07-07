/****************************************************************************

  module      : vbd03.cpp

  -------------------------------------------------------------------------

  responsible : UweH

  special area: BD : filesysteminterface_3
  description : Index-Handling Interface
 

  last changed: 1999-09-14  10:20
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

//   Contents of include files
#include "ggg01.h" // OMS errors
#include "gbd300.h"
#include "gbd500.h"
#include "gbd600.h"
#include "ggg11.h"   // PASCAL: GG_vtrace_types
#include "hbd01.h"   // PASCAL: filesysteminterface_1
#include "hbd01_1.h" // b01niltree_id
#include "hbd03.h"   // Interface Definition to this module
#include "hbd13.h"   // PASCAL: nodehandling
#include "hbd20_9.h" // bd20GetDestinationConverterVersion
#include "hbd30.h"   // PASCAL: treehandling
#include "hbd73.h"   // PASCAL: Ref_statistic
#include "hbd300.h"  // SetRootCheck
#include "hbd400.h"  // Inverted Lists Add/Delete
#include "hbd401.h"  // Inverted Lists Accesses And Statistics
#include "hbd402.h"  // Inverted Lists Selects
#include "hbd403.h"  // Inverted Lists Merge
#include "hbd404.h"  // Aggregate functions on inverted lists
#include "hbd405.h"  // Inverted Lists Consistency Check (Diagnose Check Index)
#include "hbd998.h"  // bd998ArePagesclustered, bd998SetPagesClustered
#include "hgg04.h"   // g04inv_tfn
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "hsp30.h"   // PASCAL: Buffer handling and comparison routines
#include "hgg01_3.h" // PASCAL: g01vtrace

#include "Converter/Converter_Version.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_OmsKeyPartition.hpp"

#include "KernelCommon/Kernel_VTrace.hpp"      
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"  
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "Messages/Msg_List.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Trace/Trace_Entry.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Data.h"
#include "DataAccess/Data_Types.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define TRUE                 1
#define ASCENDING_KEY_ORDER  true
#define DESCENDING_KEY_ORDER !ASCENDING_KEY_ORDER
// LOOK INTO VBD300.CPP TOO
// #define TEST_STATISTICS true

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/ 

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/* static + inline cannot be declared forward on SUN's */

static inline void
bd03_OptimizeKeyPointers (tgg00_SelectFieldsParam &SelectParam,
                          tgg00_BdInvSet          &InvRangeSet,
                          tsp00_KeyPtr            &pStartSecKey,
                          tsp00_Int2              &StartSecKeyLen,
                          tsp00_KeyPtr            &pStopSecKey,
                          tsp00_Int4              &StopSecKeyLen,
                          tsp00_KeyPtr            &pStartPrimKey,
                          tsp00_Int4              &StartPrimKeyLen,
                          tsp00_KeyPtr            &pStopPrimKey,
                          tsp00_Int4              &StopPrimKeyLen,
                          tsp00_KeyPtr            &pFirstPrimKey,
                          tsp00_Int2              &FirstPrimKeyLen)
{
    /* prepare key pointers for later usage in iterators */

    if (mm_direct == SelectParam.sfp_bd_mess2_type())
    {
        pStopSecKey    = pStartSecKey;
        StopSecKeyLen  = StartSecKeyLen;
        pStopPrimKey   = pFirstPrimKey;
        StopPrimKeyLen = FirstPrimKeyLen;
    }
    else
    {
        if (InvRangeSet.includes (secondary_stop))
        {
            tsp00_LcompResult Lresult;

            s30cmp (pStartSecKey, POS_OFF_DIFF_BD00, StartSecKeyLen,
                    pStopSecKey, POS_OFF_DIFF_BD00, StopSecKeyLen, Lresult);

            if (l_equal == Lresult)
                pStopSecKey   = pStartSecKey;
        }
        else
            pStopSecKey = NULL;

        if (InvRangeSet.includes (primary_stop))
        {
            tsp00_LcompResult Lresult;

            s30cmp (pStartPrimKey, POS_OFF_DIFF_BD00, StartPrimKeyLen,
                    pStopPrimKey, POS_OFF_DIFF_BD00, StopPrimKeyLen, Lresult);

            if (l_equal == Lresult)
                pStopPrimKey   = pStartPrimKey;
        }
        else
            pStopPrimKey = NULL;

        if ( ! InvRangeSet.includes (primary_start))
        {
            pStartPrimKey   = NULL;
            StartPrimKeyLen = 0;
        }
    }
}

/*---------------------------------------------------------------------------*/

static inline void
bd03_LKeyVtrace (tgg00_TransContext    &Trans,
                 tgg00_Lkey            &Key1,
                 tgg00_Lkey            &Key2,
                 tgg00_VtraceType_Enum  TraceObject)
{
    tgg00_BasisError     AuxError = e_ok;
    tsp00_Int2           OutKeyLen;
    tgg11_BdTwoKeysTrace KeyTrace;

    KeyTrace.bdtkTrType_gg11.becomes(bdtrTwoKeys_egg11);
    KeyTrace.bdtkKeyLen_gg11 [0] = Key1.keyLen_gg00();
    KeyTrace.bdtkKeyLen_gg11 [1] = Key2.keyLen_gg00();

    OutKeyLen =  (Key1.keyLen_gg00() > sizeof (tgg00_VtraceKey)) ?
                 tsp00_Int2(sizeof (tgg00_VtraceKey)) : Key1.keyLen_gg00();

    SAPDB_RangeMove( __FILE__, 1,
           PERM_KEY_MXSP00, sizeof (KeyTrace.bdtkKey_gg11 [0]),
           Key1.keyVal_gg00(),        POS_OFF_DIFF_BD00,
           KeyTrace.bdtkKey_gg11 [0], POS_OFF_DIFF_BD00,
           OutKeyLen, AuxError);

    AuxError = e_ok; // ignore move error

    OutKeyLen =  (Key2.keyLen_gg00() > sizeof (tgg00_VtraceKey)) ?
                 tsp00_Int2(sizeof (tgg00_VtraceKey)) : Key2.keyLen_gg00();

    SAPDB_RangeMove( __FILE__, 2,
           PERM_KEY_MXSP00, sizeof (KeyTrace.bdtkKey_gg11 [1]),
           Key2.keyVal_gg00(),        POS_OFF_DIFF_BD00,
           KeyTrace.bdtkKey_gg11 [1], POS_OFF_DIFF_BD00,
           OutKeyLen, AuxError);

    Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject),
                       REINTERPRET_CAST( tsp00_BytePtr, &KeyTrace ), sizeof (KeyTrace) );
}

/*---------------------------------------------------------------------------*/

static inline void
bd03_MultiLKeyVtrace (
    tgg00_TransContext    &Trans,
    tgg00_Lkey            &Key1,
    tgg00_Lkey            &Key2,
    tgg00_Lkey            &Key3,
    tgg00_Lkey            &Key4,
    tsp00_Bool            bStopPrimKeyExists,
    tgg00_VtraceType_Enum  TraceObject = tgg00_VtraceType::fromConst(b03merge))
{
    tgg00_BasisError    AuxError = e_ok;
    tsp00_Int2          OutKeyLen;
    tgg11_BdMergeTrace  MergeTrace;

    {
        // Insert start keys (sec and prim key)
        MergeTrace.bdmTrType_gg11.becomes(bdtrMerge_egg11);
        MergeTrace.bdmIsStartKey_gg11 = true;
        MergeTrace.bdmIsPrimStop_gg11 = false;

        MergeTrace.bdmKeyLen_gg11 [0] = Key1.keyLen_gg00();
        MergeTrace.bdmKeyLen_gg11 [1] = Key2.keyLen_gg00();


        OutKeyLen =  (Key1.keyLen_gg00() > sizeof (tgg00_VtraceKey)) ?
                     tsp00_Int2(sizeof (tgg00_VtraceKey)) : Key1.keyLen_gg00();

        SAPDB_RangeMove( __FILE__, 3,
               PERM_KEY_MXSP00, sizeof (MergeTrace.bdmKey_gg11[0]),
               Key1.keyVal_gg00(),        POS_OFF_DIFF_BD00,
               MergeTrace.bdmKey_gg11[0], POS_OFF_DIFF_BD00,
               OutKeyLen, AuxError);

        AuxError  = e_ok; // ignore move error

        OutKeyLen =  (Key2.keyLen_gg00() > sizeof (tgg00_VtraceKey)) ?
                     tsp00_Int2(sizeof (tgg00_VtraceKey)) : Key2.keyLen_gg00();

        SAPDB_RangeMove( __FILE__, 4,
               PERM_KEY_MXSP00, sizeof (MergeTrace.bdmKey_gg11[1]),
               Key2.keyVal_gg00(),        POS_OFF_DIFF_BD00,
               MergeTrace.bdmKey_gg11[1], POS_OFF_DIFF_BD00,
               OutKeyLen, AuxError);

        AuxError = e_ok; // ignore move error

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject),
                           REINTERPRET_CAST( tsp00_BytePtr, &MergeTrace ), sizeof (MergeTrace) );
    }
    {
        // Insert stop keys (sec and prim key)
        MergeTrace.bdmTrType_gg11.becomes(bdtrMerge_egg11);
        MergeTrace.bdmIsStartKey_gg11 = false;
        MergeTrace.bdmIsPrimStop_gg11 = bStopPrimKeyExists;

        MergeTrace.bdmKeyLen_gg11 [0] = Key3.keyLen_gg00();

        OutKeyLen =  (Key3.keyLen_gg00() > sizeof (tgg00_VtraceKey)) ?
                     tsp00_Int2(sizeof (tgg00_VtraceKey)) : Key3.keyLen_gg00();

        SAPDB_RangeMove( __FILE__, 5,
               PERM_KEY_MXSP00, sizeof (MergeTrace.bdmKey_gg11[0]),
               Key3.keyVal_gg00(),        POS_OFF_DIFF_BD00,
               MergeTrace.bdmKey_gg11[0], POS_OFF_DIFF_BD00,
               OutKeyLen, AuxError);

        if (bStopPrimKeyExists)
        {
            MergeTrace.bdmKeyLen_gg11 [1] = Key4.keyLen_gg00();
            AuxError                      = e_ok;

            SAPDB_RangeMove( __FILE__, 6,
                   PERM_KEY_MXSP00, sizeof (MergeTrace.bdmKey_gg11[1]),
                   Key4.keyVal_gg00(),        POS_OFF_DIFF_BD00,
                   MergeTrace.bdmKey_gg11[1], POS_OFF_DIFF_BD00,
                   OutKeyLen, AuxError);

        };
        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject),
                           REINTERPRET_CAST( tsp00_BytePtr, &MergeTrace ), sizeof (MergeTrace) );
    }
}

/*---------------------------------------------------------------------------*/

static void
bd03_WriteFileIdsAndInputKeysIntoVtrace (
    tgg00_TwoFileIds        &FileIds,
    tgg00_TwoKeys           &StartKeyPair,
    tgg00_TwoKeys           &StopKeyPair,
    tgg00_Lkey              &StartKey,
    tgg00_BdInvSet          &InvRangeSet,
    tgg00_SelectFieldsParam &SelectParam)
{
    ROUTINE_DBG_MEO00 ("bd03_WriteFileIdsAndInputKeysIntoVtrace");

    /* this function is solely active for the slow kernel */

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 

    /* output of task */
    t01sname  (bi, "bd select   ");

    if ((mm_last == SelectParam.sfp_bd_mess2_type()) ||
            (mm_prev == SelectParam.sfp_bd_mess2_type()))
        t01sname (bi, "bd pqual inv");
    else
        t01sname (bi, "bd nqual inv");

    /* ouput of working files */
    t01treeid (bi, "treeid      ", FileIds.file_id);
    t01treeid (bi, "treeid inv  ", FileIds.inv_id);
    t01int4   (bi, "=> inv_only:", SelectParam.sfp_bd_inv_only());

    /* output of StartKeys */
    t01sname (bi, "prim key    ");
    t01lkey  (bi, StartKeyPair.reckey);

    if (!FileIds.inv_id.fileType_gg00().includes(ftsTemp_egg00))
    {
        t01sname (bi, "sec key     ");
        t01lkey  (bi, StartKeyPair.listkey);
    }

    if (InvRangeSet.includes(primary_start))
    {
        t01sname (bi, "prim start k");
        t01lkey  (bi, StartKey);
    }

    /* output of StopKeys */
    if  (InvRangeSet.includes(primary_stop))
    {
        t01sname (bi, "prim stop k ");
        t01lkey  (bi, StopKeyPair.reckey);
    }

    if  (InvRangeSet.includes(secondary_stop))
    {
        t01sname (bi, "sec stop key");
        t01lkey  (bi, StopKeyPair.listkey);
    }
#   endif
}

/*---------------------------------------------------------------------------*/


static SAPDB_Bool bd03_GetInvPageAndRecordCount(
    tgg00_TransContext&    trans,
    const tgg00_Surrogate& indexSurr,
    FileDir_Index&         indexEntry,
    SAPDB_Int4&            leafPageCount,
    SAPDB_Int4&            secKeyCount)
{
    ROUTINE_DBG_MEO00 ("bd03_GetInvPageAndRecordCount");

    trans.trError_gg00 = e_ok;

    FileDir_IDirectory &fdir    = FileDir_IDirectory::SessionInstance(trans);
    Msg_List           &msgList = Trans_Context::GetContext(trans).ErrorList();
    FileDir_ReturnCode  fDirRc  =
        fdir.GetIndexFile(trans.trTaskId_gg00,
                          FileDir_FileNo( indexSurr ),
                          indexEntry,
                          msgList);
    if ( FileDir_Okay == fDirRc )
    {
        if ( indexEntry.AreCountersInitialized() &&
             (indexEntry.GetFileState() == Data_FileOk) )
        {
            const SAPDB_UInt8 indexEntryCount = indexEntry.GetEntryCount(trans.trTaskId_gg00);

            if ( indexEntryCount <= SAPDB_MAX_INT4 )
                secKeyCount = static_cast<SAPDB_Int4>(indexEntryCount);
            else
                secKeyCount = SAPDB_MAX_INT4;

            leafPageCount = indexEntry.GetLeafPageCount(trans.trTaskId_gg00);

            return true;
        }
    }
    else
    {
        switch ( fDirRc )
        {
            case FileDir_FileNoNotFound:
                trans.trError_gg00 = e_file_not_found;
                break;
            default:
                trans.trError_gg00 = e_system_error;
        }
    }
    return false;
}

/*-----------------------------------------------------------------------------------*/

static void 
bd03_CreateIndexFile(
     tgg00_TransContext    &trans,
     tgg00_FileId          &invFileId )
{
    ROUTINE_DBG_MEO00 ("bd03_CreateIndexFile");

    tbd_node_ptrs       pRoot;
    tbd_current_tree    invCurrent;
    
    pRoot.np_ptr()   = NULL;
    pRoot.np_cbptr() = NULL;
    
    bd30BuildCurrent( trans, invFileId, invCurrent );
    
    b13new_root( pRoot, invCurrent );
    
    if( e_ok == trans.trError_gg00 )
    {
        invFileId.fileRoot_gg00() = pRoot.np_ptr()->nd_id();
        bd300SetRootCheck( invFileId );
    }
    if( NULL != pRoot.np_ptr()){
        b13w_release_node( pRoot, invCurrent ); 
    }
}

/*---------------------------------------------------------------------------*/

static void bd03_AddIndexToFileDirectory ( cbd300_InvCurrent        &invCurrent,
                                           tgg00_FileId             &tableFileId,
                                           const tsp00_Int2          indexNo,
                                           const tgg00_IndexCounter &indexCounter,
                                           bool                      forCreateIndex,
                                           bool                      bPagesClustered )
{
    ROUTINE_DBG_MEO00 ("bd03_AddIndexToFileDirectory");

    Trans_Context           &trans   = Trans_Context::GetContext(*invCurrent.curr_trans);
    tgg00_BasisError        &trError = invCurrent.curr_trans->trError_gg00;
    FileDir_IDirectory      &fdir    = FileDir_IDirectory::SessionInstance(*invCurrent.curr_trans);
    RTE_TaskId               taskid  = trans.Task().ID();
    
    const FileDir_FileNo     tableFileNo ( tableFileId.fileTabId_gg00());
    const FileDir_FileNo     invFileNo   ( invCurrent.curr_tree_id.fileTabId_gg00());
    const Converter_Version  converterVersion = bd20GetDestinationConverterVersion( taskid, invCurrent.currRootNptrs_bd00.np_cbptr() );

    FileDir_ReturnCode       retCode = FileDir_Okay;
    FileDir_Index            indexFileEntry;

    if ( forCreateIndex )
    {
        retCode = fdir.AddIndexFile( taskid,
                                     invFileNo, tableFileNo, converterVersion, indexNo,
                                     invCurrent.curr_tree_id.fileRoot_gg00(),
                                     invCurrent.curr_tree_id.fileType_gg00(),
                                     invCurrent.curr_tree_id.fileVersion_gg00(),
                                     trans.ErrorList(),
                                     &indexFileEntry );
                                     
        if( FileDir_Okay == retCode )
        {
            indexFileEntry.SetPagesClustered ( taskid, bPagesClustered );
            indexFileEntry.SetLeafPageCount  ( invCurrent.currLeafPageCountDelta_bd00  );
            indexFileEntry.SetIndexPageCount ( invCurrent.currIndexPageCountDelta_bd00 );
            indexFileEntry.SetEntryCount     ( indexCounter.idc_sec_keycount           );
            invCurrent.currLeafPageCountDelta_bd00  = 0;
            invCurrent.currIndexPageCountDelta_bd00 = 0;
        }
    }

    switch( retCode )
    {
	case FileDir_Okay:
        break;
    case FileDir_IndexExists:
    case FileDir_FileNoExists:
        trError = e_duplicate_filename;
        break;
    case FileDir_OutOfMemory:
        trError = e_no_more_memory;
        break;
    case FileDir_FileTypeMismatch:
        trError = e_invalid_filetype;
        break;
    default:
        trError = e_file_not_found;
        break;
    }
}

/*---------------------------------------------------------------------------*/
static void bd03_CheckPageAndEntryCount (cbd300_InvCurrent        &InvCurrent,
                                         const tgg00_IndexCounter &counter)
{
    ROUTINE_DBG_MEO00 ("bd03_CheckPageAndEntryCount");

    Trans_Context   &trans = Trans_Context::GetContext(*InvCurrent.curr_trans);
    FileDir_Generic  fdirEntry;
    Msg_List         errlist;
    
    // this method is also called during 'check data'; therefore the
    // index might be marked deleted -> use GcGetFile to access file
    // directory entry
    if( (FileDir_Okay != FileDir_IDirectory::SessionInstance(trans.OldTrans()).
        GcGetFile( trans.Task().ID(),
                   InvCurrent.curr_tree_id.fileTabId_gg00(),
                   fdirEntry,
                   errlist ))
        || ((fdirEntry.GetFileType() != Data_IndexFile) &&
            (fdirEntry.GetFileType() != Data_OmsKeyPartitionFile)))
    {
        RTE_Message (errlist);
        return;
    }
    bool error = false;
    if ( fdirEntry.GetLeafPageCount(trans.Task().ID()) != InvCurrent.currLeafPageCountDelta_bd00 )
    {
        errlist.Overrule( Msg_List( Msg_List::Warning, SDBMSG_DATA_FDIR_COUNTER_MISMATCH,
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__TITLE,           "Index Leaf Page Count"),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__FDIR_PAGE_COUNT, SAPDB_ToString(fdirEntry.GetLeafPageCount(trans.Task().ID()))),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__COUNTED_PAGES,   SAPDB_ToString(InvCurrent.currLeafPageCountDelta_bd00)),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__PAGENO,          SAPDB_ToString(InvCurrent.bd300GetRootId())) ) );
        fdirEntry.SetLeafPageCount( InvCurrent.currLeafPageCountDelta_bd00 );
        InvCurrent.currLeafPageCountDelta_bd00 = 0;
        error = true;
    }
    if ( fdirEntry.GetIndexPageCount(trans.Task().ID()) != InvCurrent.currIndexPageCountDelta_bd00 )
    {
        errlist.Overrule( Msg_List( Msg_List::Warning, SDBMSG_DATA_FDIR_COUNTER_MISMATCH,
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__TITLE,           "Index Index Page Count"),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__FDIR_PAGE_COUNT, SAPDB_ToString(fdirEntry.GetIndexPageCount(trans.Task().ID()))),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__COUNTED_PAGES,   SAPDB_ToString(InvCurrent.currIndexPageCountDelta_bd00)),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__PAGENO,          SAPDB_ToString(InvCurrent.bd300GetRootId())) ) );
        fdirEntry.SetIndexPageCount( InvCurrent.currIndexPageCountDelta_bd00 );
        InvCurrent.currIndexPageCountDelta_bd00 = 0;
        error = true;
    }
    if ( fdirEntry.GetEntryCount(trans.Task().ID()) != counter.idc_sec_keycount )
    {
        errlist.Overrule( Msg_List( Msg_List::Warning, SDBMSG_DATA_FDIR_COUNTER_MISMATCH,
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__TITLE,           "SecKey Count"),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__FDIR_PAGE_COUNT, SAPDB_ToString(fdirEntry.GetEntryCount(trans.Task().ID()))),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__COUNTED_PAGES,   SAPDB_ToString(counter.idc_sec_keycount)),
                                    Msg_Arg(SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__PAGENO,          SAPDB_ToString(InvCurrent.bd300GetRootId())) ) );
        fdirEntry.SetEntryCount( counter.idc_sec_keycount );
        error = true;
    }
#ifdef TEST_STATISTICS
    if ( error )
#else
    if ( error && g01glob.treecheck )
#endif
	{
        RTE_Message(errlist);
	}
}
/*---------------------------------------------------------------------------*/
#ifdef TEST_STATISTICS
static void bd03_CheckFiledirectoryStatistics (tgg00_TransContext      &Trans,
                                               tgg00_FileId            &FileId)
{
    ROUTINE_DBG_MEO00 ("bd03_CheckFiledirectoryStatistics");

    Trans_Context      &trans = Trans_Context::GetContext(Trans);
    FileDir_IDirectory &fdir  = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_Index       fdirIndexEntry;

    if ( FileDir_Okay != fdir.GetIndexFile(trans.Task().ID(),
                                           FileDir_FileNo(FileId.fileTabId_gg00()),
                                           fdirIndexEntry,
                                           trans.ErrorList()) )
    {
        // ignore errors because temp index files look like tab's
        trans.ErrorList().ClearMessageList();
        return;
    }
    SAPDB_UInt8 fdirLeafPageCount  = fdirIndexEntry.GetLeafPageCount(trans.Task().ID());
    SAPDB_UInt8 fdirIndexPageCount = fdirIndexEntry.GetIndexPageCount(trans.Task().ID());
    SAPDB_UInt8 fdirEntryCount     = fdirIndexEntry.GetEntryCount(trans.Task().ID());

    tgg00_SampleInfo StatInfo;
    bool             abort = false;
    /*StatInfo.leafnodes    = 0;
    StatInfo.sec_key_cnt  = 0;
    StatInfo.prim_key_cnt = 0;
    StatInfo.nodes        = 0;*/

    bd03InvStatistics (Trans, FileId, 0 /*bWithSelectivity*/, StatInfo);

    if ( Trans.trError_gg00 != e_ok )
    {
        Kernel_VTrace() << "bd03_CheckFiledirectoryStatistics: Trans.trError_gg00: " << Trans.trError_gg00;
        abort = true; 
    }
    
    if ( fdirLeafPageCount != StatInfo.leafnodes )
    {
        Kernel_VTrace() << "bd03_CheckFiledirectoryStatistics: fdirLeafPageCount: " << fdirLeafPageCount
                        << ", StatInfo.leafnodes: " << StatInfo.leafnodes;
        abort = true;
    }

    if ( fdirEntryCount != StatInfo.sec_key_cnt )
    {
        Kernel_VTrace() << "bd03_CheckFiledirectoryStatistics: fdirEntryCount: " << fdirEntryCount
                        << ", StatInfo.sec_key_cnt: " << StatInfo.sec_key_cnt;
        abort = true;
    }
    if ( abort )
        Kernel_IAdminRestartShutdown::Instance().Offline (e_not_implemented, true, true);
}
#endif
/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  * 
 *===========================================================================*/

externC void
b03add_inv (tgg00_TransContext &Trans,
            tgg00_FileId       &FileId,
            tgg00_Lkey         &SecKey,
            tgg00_Lkey         &PrimKey)
{
    ROUTINE_DBG_MEO00 ("b03add_inv");

    tgg00_BasisError  &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace  RootTrace;
    bool               DummySecKeyFound;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03_LKeyVtrace (Trans, SecKey, PrimKey, b03add);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd add inv  ");
    t01treeid (bi, "FileId      ", FileId);
    t01sname  (bi, "SecKey:     ");
    t01lkey   (bi, SecKey);
    t01sname  (bi, "PrimKey:    ");
    t01lkey   (bi, PrimKey);
#   endif
   
    if (FileId.fileType_gg00().includes (ftsTemp_egg00 ))
        b73cmd_count (icadd_inv);
    else
        b73cmd_count (iadd_inv);

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        if ( (SecKey.keyLen_gg00()  > PERM_KEY_MXSP00) ||
                (SecKey.keyLen_gg00()  < 0              ) ||
                (PrimKey.keyLen_gg00() > PERM_KEY_MXSP00) ||
                (PrimKey.keyLen_gg00() < 0              )  )
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            #ifdef TEST_STATISTICS
            if (e_ok == TrError)
                bd03_CheckFiledirectoryStatistics (Trans, FileId);
            #endif        
            TrError = e_ok;

            FileId.fileBdUse_gg00().addElement(bd_write_acc);

            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_insert),
                                          tgg00_VtraceType::fromConst(b03add),
                                          IGNORE_INV_ERROR_BD300);
            if( e_ok == TrError )
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                bd400AddToInvTree (InvCurrent,
                                   tsp00_KeyPtr(&SecKey.keyVal_gg00()), SecKey.keyLen_gg00(),
                                   tsp00_KeyPtr(&PrimKey.keyVal_gg00()), PrimKey.keyLen_gg00(),
                                   DummySecKeyFound);
                FileId = InvCurrent.curr_tree_id;

                if (e_file_not_found == TrError)
                    TrError = e_inv_not_found;
            }
            FileId.fileBdUse_gg00().delElement(bd_write_acc);
        }
        #ifdef TEST_STATISTICS
        if (e_ok == TrError)
            bd03_CheckFiledirectoryStatistics (Trans, FileId);
        #endif        
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "add inv err ", TrError);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03add),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/
/* CRS 1104319 Alexander Kley 25/10/1999                                     */

externC void
bd03AggrFuncOnIndex (tgg00_TransContext      &Trans,
                     tgg00_FileId            &InvFileId,
                     tgg00_FileId            &AuxFileId,
                     tgg00_TwoKeys           &StartKeyPair,
                     tgg00_TwoKeys           &StopKeyPair,
                     tgg00_AggrDescrRec      &AggrFieldDescr,
                     tgg00_SelectFieldsParam &SelectParam,
                     tgg00_StackDesc         &StackDesc,
                     tsp00_Buf               &AuxBuffer,
                     tsp00_Buf               &ResultBuffer,
                     tsp00_Bool               bOneListOnly,
                     tsp00_Bool               bWarnOfNullValues)
{
    ROUTINE_DBG_MEO00 ("bd03AggrFuncOnIndex");

    /* this function calculates aggregate function (sum,count,avg,min,max) */
    /* on indexes and puts the result into the ResultBuffer                */

    tgg00_BasisError  &TrError               = Trans.trError_gg00;
    bool               bDistinctAccessExists = false;
    tgg11_BdRootTrace  RootTrace;
    tsp00_LcompResult  CompResult;

    /* vtrace output */
    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03_LKeyVtrace (Trans, StartKeyPair.listkey, StartKeyPair.reckey, b03aggr);
    }

    /* slow vtrace output */
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname    (bi, "bd aggr func");
    t01treeid   (bi, "InvFileId      ", InvFileId);
    t01treeid   (bi, "AuxFileId   ", AuxFileId);
    if   (AggrFieldDescr.agg_opt_infos.includes(prim_keys))
        t01sname    (bi, "incl prim k ");
    else
        t01sname    (bi, "excl prim k ");
    t01sname    (bi, "sec start k ");
    t01lkey     (bi, StartKeyPair.listkey);
    t01sname    (bi, "prim start k");
    t01lkey     (bi, StartKeyPair.reckey);
    if  (StopKeyPair.listkey.keyLen_gg00() > 0)
    {
        t01sname (bi, "sec stop k  ");
        t01lkey  (bi, StopKeyPair.listkey);
    }
    if  (StopKeyPair.reckey.keyLen_gg00() >0 )
    {
        t01sname (bi, "prim stop k ");
        t01lkey  (bi, StopKeyPair.reckey);
    }

    t01bool  (bi, "one list onl", bOneListOnly);
#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        /* check input keys */
        if  ((StartKeyPair.listkey.keyLen_gg00() > PERM_KEY_MXSP00) ||
                (StartKeyPair.listkey.keyLen_gg00() < 0              )  ||
                (StartKeyPair.reckey.keyLen_gg00()  > PERM_KEY_MXSP00)  ||
                (StartKeyPair.reckey.keyLen_gg00()  < 0              )  ||
                (StopKeyPair.listkey.keyLen_gg00()  > PERM_KEY_MXSP00)  ||
                (StopKeyPair.listkey.keyLen_gg00()  < 0              )  ||
                (StopKeyPair.reckey.keyLen_gg00()   > PERM_KEY_MXSP00)  ||
                (StopKeyPair.reckey.keyLen_gg00()   < 0              ))
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            tsp00_KeyPtr  pStartSecKey     = tsp00_KeyPtr (&StartKeyPair.listkey.keyVal_gg00());
            tsp00_Int2   &StartSecKeyLen   = StartKeyPair.listkey.keyLen_gg00();
            tsp00_KeyPtr  pStopSecKey      = tsp00_KeyPtr (&StopKeyPair.listkey.keyVal_gg00());
            tsp00_Int2    StopSecKeyLen    = StopKeyPair.listkey.keyLen_gg00();
            tsp00_KeyPtr  pStartPrimKey    = tsp00_KeyPtr (&StartKeyPair.reckey.keyVal_gg00());
            tsp00_Int2    StartPrimKeyLen  = StartKeyPair.reckey.keyLen_gg00();
            tsp00_KeyPtr  pStopPrimKey     = tsp00_KeyPtr (&StopKeyPair.reckey.keyVal_gg00());
            tsp00_Int2    StopPrimKeyLen   = StopKeyPair.reckey.keyLen_gg00();

            /* prepare key pointers for later usage in iterators */
            if ( 0 == StopPrimKeyLen)
                pStopPrimKey = NULL;
            else
            {
                s30cmp (pStartPrimKey, POS_OFF_DIFF_BD00, StartPrimKeyLen,
                        pStopPrimKey, POS_OFF_DIFF_BD00, StopPrimKeyLen, CompResult);
                if (l_equal == CompResult) pStopPrimKey = pStartPrimKey;
            }

            if (0 == StopSecKeyLen)
                pStopSecKey = NULL;
            else
            {
                s30cmp (pStartSecKey, POS_OFF_DIFF_BD00, StartSecKeyLen,
                        pStopSecKey, POS_OFF_DIFF_BD00, StopSecKeyLen, CompResult);
                if (l_equal == CompResult) pStopSecKey = pStartSecKey;
            }

            /* contruct handles to access files */
            cbd300_InvCurrent InvCurrent (Trans, InvFileId,
                                          tbd_node_request::fromConst (nr_for_read),
                                          tgg00_MessType::fromConst (m_select),
                                          tgg00_VtraceType::fromConst (b03aggr),
                                          !IGNORE_INV_ERROR_BD300);

            if (e_ok == TrError && !bOneListOnly )
            {
                /* find if distinct values are requested for any column of the result buffer */
                for (int i=0; i < AggrFieldDescr.agg_cnt; ++i)
                {
                    tgg00_StackOpFunc & StackOpFunc = AggrFieldDescr.agg_field_descr [i].field_function;

                    bDistinctAccessExists =
                        ((InvCurrent.curr_tree_id.fileTfn_gg00() != tfnOmsInv_egg00)   &&
                         ((op_f_dis_avg  == StackOpFunc)                           ||
                          (op_f_dis_count == StackOpFunc)                          ||
                          (op_f_dis_sum   == StackOpFunc)));

                    if (bDistinctAccessExists) break;
                }
            }

            tbd_current_tree AuxCurrent;

            if( e_ok == TrError && bDistinctAccessExists && !bOneListOnly)
            {
                AuxFileId.fileBdUse_gg00().addElement(bd_write_acc);

                bd30GetTree (Trans, AuxFileId, AuxCurrent, m_nil,
                    LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00);
            }

            if  (e_ok == TrError)
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                bd404AggrFuncOnIndex(InvCurrent, AuxCurrent,
                                     pStartSecKey, StartSecKeyLen, pStopSecKey, StopSecKeyLen,
                                     pStartPrimKey, StartPrimKeyLen, pStopPrimKey, StopPrimKeyLen, AggrFieldDescr,
                                     SelectParam, StackDesc, * (reinterpret_cast <tgg00_RecPtr> (&AuxBuffer)),
                                     ResultBuffer, bDistinctAccessExists,
                                     (TRUE == bOneListOnly), (TRUE == bWarnOfNullValues));

                if (e_ok == TrError)
                {
                    InvFileId = InvCurrent.curr_tree_id;
                    if (bDistinctAccessExists)
                        AuxFileId = AuxCurrent.curr_tree_id;
                }
            }
            if( bDistinctAccessExists && !bOneListOnly)
            {
                bd30ReleaseTree (AuxCurrent);

                AuxFileId.fileBdUse_gg00().delElement(bd_write_acc);
            }
            /* interpret error */
            if (e_file_not_found == TrError)
                TrError = e_inv_not_found;
        }
    }

    /* vtrace output*/
    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes (bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry(  tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03aggr),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }

    /* slow vtrace output */
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    {
        tsp00_Int4 ResultTraceLen = 0;
        for (int i=0; i<AggrFieldDescr.agg_cnt; ++i)
        {
            const tgg00_FieldDescrRec & FieldDescRec = AggrFieldDescr.agg_field_descr [i];

            t01int4 (bi, "descr no    ", i);
            t01int4 (bi, "field count ", FieldDescRec.field_count);
            t01int4 (bi, "res out pos ", FieldDescRec.result_out_pos);
            t01int4 (bi, "res out len ", FieldDescRec.result_out_len);
            if (ResultTraceLen < FieldDescRec.result_out_pos + FieldDescRec.result_out_len - 1)
                ResultTraceLen = FieldDescRec.result_out_pos + FieldDescRec.result_out_len - 1;
        }
        t01buf1        (bi, ResultBuffer, 1, ResultTraceLen);
        t01basis_error (bi, "agg func err", TrError);
    }
#   endif   
}


/*---------------------------------------------------------------------------*/

externC void
bd03DiagnoseIndex (tgg00_TransContext &Trans,
                   tgg00_FileId       &PrimFileId,
                   tgg00_FileId       &InvFileId,
                   tgg00_FileId       &TempFileId,
                   tgg00_Rec          &RecBuffer,
                   tsp00_Int4         &PrimKeyDuplicates,
                   tsp00_Int4         &PrimKeyMissing)
{
    ROUTINE_DBG_MEO00 ("bd03DiagnoseIndex");


    tgg00_BasisError  &TrError = Trans.trError_gg00;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01name   (bi, "BD CheckIndex     ");
    t01treeid (bi, "PrimFileId  ", PrimFileId);
    t01treeid (bi, "InvFileId   ", InvFileId );
    t01int4   (bi, "TempRoot    ", TempFileId.fileRoot_gg00());
#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        TrError = e_ok;

        tbd_current_tree PrimCurrent;

        bd30GetTree (Trans, PrimFileId, PrimCurrent, m_select,
               ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00);

        if (e_ok == TrError)
        {
            InvFileId.fileBdUse_gg00().addElement(bd_write_acc);

            if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
            {
                Trace_CommonEntry traceEntry ( tgg00_Debug::fromConst(ta), tgg00_VtraceType::fromConst(td_none), 
                                               reinterpret_cast< const tsp00_Byte*> ( "bd03DiagnoseIndex" ), 17);
            }

            cbd300_InvCurrent InvCurrent (Trans, InvFileId,
                                          tbd_node_request::fromConst (nr_for_update),
                                          tgg00_MessType::fromConst (m_insert),
                                          tgg00_VtraceType::fromConst (b03check),
                                          !IGNORE_INV_ERROR_BD300);
            if (e_ok == TrError)
            {
                tbd_current_tree TempCurrent;

                TempFileId.fileBdUse_gg00().addElement(bd_write_acc);

                bd30GetTree (Trans, TempFileId, TempCurrent, m_nil,
                      LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00);

                if ( e_ok == TrError )
                {
                    bd405DiagnoseIndex (InvCurrent, PrimCurrent, TempCurrent,
                                        RecBuffer, PrimKeyDuplicates, PrimKeyMissing);
                }
                bd30ReleaseTree (TempCurrent);

                TempFileId.fileBdUse_gg00().delElement(bd_write_acc);

                InvFileId  = InvCurrent.curr_tree_id;
                PrimFileId = PrimCurrent.curr_tree_id;
            }
            InvFileId.fileBdUse_gg00().delElement(bd_write_acc);
        }

        bd30ReleaseTree (PrimCurrent);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01int4        (bi, "PrimRoot    ", PrimFileId.fileRoot_gg00());
    t01int4        (bi, "InvRoot     ", InvFileId.fileRoot_gg00());
    t01int4        (bi, "Duplicates  ", PrimKeyDuplicates);
    t01int4        (bi, "Missing     ", PrimKeyMissing);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        tgg11_BdRootTrace  RootTrace;

        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11 = TrError;
        RootTrace.bdrRoot_gg11  = InvFileId.fileRoot_gg00();

        Trace_CommonEntry traceEntry(  tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03check),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd03BuildIndex(
    tgg00_TransContext          &trans,
    const tgg00_IndexMode_Param indexMode,
    tgg00_FileId                &primFileId,
    tgg00_FileId                &invFileId,
    tgg00_StackDesc             &stackDesc,
    const tsp00_Int2            indexNo,
    tgg00_IndexCounter          &indexCounter )
{
    ROUTINE_DBG_MEO00( "bd03BuildIndex" );

    tgg00_BasisError  &trError = trans.trError_gg00;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01name  ( bi, "BD CreateIndex    ");
    t01treeid( bi, "PrimFileId  ", primFileId );
    t01treeid( bi, "InvFileId   ", invFileId  );
#   endif

    if( b01downfilesystem )
    {
        trError = e_shutdown;
        return;
    }

    SAPDBERR_ASSERT_STATE( restart_index != indexMode );
      
    if( e_ok == trError)
    {
        if( create_index == indexMode ) // PTS 1105022 TS 1999-12-21
            bd03_CreateIndexFile( trans, invFileId ); 

        const bool bPagesClustered = e_ok == trError
                ? (bd998ArePagesClustered(trans, primFileId.fileTabId_gg00())==0?false:true)
                : false;

        if( e_ok == trError )
        {
            invFileId.fileBdUse_gg00().addElement( bd_write_acc );
            
            cbd300_InvCurrent invCurrent( trans,
                                          invFileId,
                                          tbd_node_request::fromConst( nr_for_update ),
                                          tgg00_MessType::fromConst( m_create_index_parallel ),
                                          tgg00_VtraceType::fromConst( b03create ),
                                          ! IGNORE_INV_ERROR_BD300);

            if ( bPagesClustered )
                invCurrent.currExtendedFileTypeSet_bd00.addElement( eftsClustered_egg00 );

            if( e_ok == trError ) // PTS 1134104 TS 2005-02-22
                 bd13WriteExclusiveLockedRoot (trans, invFileId, invCurrent.currRootNptrs_bd00);

            if ( test_index != indexMode )
                invCurrent.currLeafPageCountDelta_bd00 = 1;
            
            if( e_ok == trError )
            {
                if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
                {
                    Trace_CommonEntry traceEntry ( tgg00_Debug::fromConst(ta), tgg00_VtraceType::fromConst(td_none), 
                                                   reinterpret_cast< const tsp00_Byte*> ( "b03create_index" ), 15);
                }
                bd405CreateIndex( invCurrent, primFileId, stackDesc,
                                  tgg00_IndexMode::fromConst( indexMode ), indexCounter);
                
                if(( e_ok != trError ) && ( e_file_not_accessible != trError ))
                {
                    const tgg00_BasisError auxError = trError;
                    trError = e_ok;
                    if ( test_index == indexMode )  // PTS 1140528 FF
                        invCurrent.bd300SetFileNotAccessible( false );
                    else
                        bd400DropInvTree( invCurrent );
                    trError = auxError; // Ignore DropInvTree errors
                }
            }
            invFileId.fileBdUse_gg00().delElement( bd_write_acc );

            if ( e_ok == trError && create_index == indexMode )
            {
                invFileId  = invCurrent.curr_tree_id;
                
                // re-lock the tree with synchronization with savepoint
                const tsp00_Int4 auxLeafPageCounter  = invCurrent.currLeafPageCountDelta_bd00;
                const tsp00_Int4 auxIndexPageCounter = invCurrent.currIndexPageCountDelta_bd00;
                invCurrent.bd300UnlockTree();
                
                const tgg00_MessType savedMessageType(invCurrent.curr_action);
                
                invCurrent.curr_action.becomes(m_create_file);
                invCurrent.bd300LockTreeExclusive();
  
                invCurrent.currLeafPageCountDelta_bd00  = auxLeafPageCounter;
                invCurrent.currIndexPageCountDelta_bd00 = auxIndexPageCounter; 
                bd03_AddIndexToFileDirectory( invCurrent, primFileId, indexNo,
                                              indexCounter, create_index == indexMode,
                                              bPagesClustered );

                invCurrent.curr_action = savedMessageType;
            }
        }
        #ifdef TEST_STATISTICS
        if (e_ok == trError)
            bd03_CheckFiledirectoryStatistics (trans, invFileId);
        #endif        
    }
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01p2int4( bi, "PrimKeyCnt  ", indexCounter.idc_prim_keycount,
                   "NullCnt     ", indexCounter.idc_nullcount );
    t01p2int4( bi, "SecKeyCnt   ", indexCounter.idc_sec_keycount,
                   "PrimIndexCnt", indexCounter.idc_prim_indexnodes );
    t01p2int4( bi, "PrimLeafNode", indexCounter.idc_prim_leafnodes,
                   "SecLeafNodes", indexCounter.idc_sec_leafnodes );
#   endif

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
    {
        tgg11_BdRootTrace  rootTrace;

        rootTrace.bdrTrType_gg11.becomes( bdtrRoot_egg11 );
        rootTrace.bdrError_gg11 = trError;
        rootTrace.bdrRoot_gg11  = invFileId.fileRoot_gg00();

        Trace_CommonEntry traceEntry(  tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03create),
                           REINTERPRET_CAST( tsp00_BytePtr, &rootTrace ), sizeof (rootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
b03del_inv (tgg00_TransContext      &Trans,
            tgg00_FileId            &FileId,
            tgg00_Lkey              &SecKey,
            tgg00_Lkey              &PrimKey,
            tbd00_DelInvReason_Param callReason)
{
    ROUTINE_DBG_MEO00 ("b03del_inv");

    const bool UNLOCK = true;

    tgg00_BasisError  &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace  RootTrace;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03_LKeyVtrace (Trans, SecKey, PrimKey, b03del);
    }
   

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd del inv  ");
    t01treeid (bi, "FileId      ", FileId);
    t01sname  (bi, "SecKey:     ");
    t01lkey   (bi, SecKey);
    t01sname  (bi, "PrimKey:    ");
    t01lkey   (bi, PrimKey);
#   endif

    if (FileId.fileType_gg00().includes (ftsTemp_egg00 ))
        b73cmd_count (icdel_inv);
    else
        b73cmd_count (idel_inv);

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        if ( (SecKey.keyLen_gg00()  > PERM_KEY_MXSP00) ||
                (SecKey.keyLen_gg00()  < 0              ) ||
                (PrimKey.keyLen_gg00() > PERM_KEY_MXSP00) ||
                (PrimKey.keyLen_gg00() < 0              )  )
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            #ifdef TEST_STATISTICS
            if (e_ok == TrError)
                bd03_CheckFiledirectoryStatistics (Trans, FileId);
            #endif        

            TrError = e_ok;

            FileId.fileBdUse_gg00().addElement(bd_write_acc);

            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_delete),
                                          tgg00_VtraceType::fromConst(b03del),
                                          IGNORE_INV_ERROR_BD300);

            if (e_ok == TrError)
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                bd400DelFromInvTree (InvCurrent,
                                     tsp00_KeyPtr(&SecKey.keyVal_gg00()), SecKey.keyLen_gg00(),
                                     tsp00_KeyPtr(&PrimKey.keyVal_gg00()), PrimKey.keyLen_gg00(),
                                     UNLOCK, callReason);

                FileId = InvCurrent.curr_tree_id;

                if (e_file_not_found == TrError)
                    TrError = e_inv_not_found;
            }

            FileId.fileBdUse_gg00().delElement(bd_write_acc);
        }
        #ifdef TEST_STATISTICS
        if (e_ok == TrError)
            bd03_CheckFiledirectoryStatistics (Trans, FileId);
        #endif
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "del inv err ", TrError);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry(  tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03del),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
b03calculate_page_count (tgg00_TransContext &Trans,
                         tgg00_FileId       &FileId,
                         tgg00_Lkey         &StartKey,
                         tgg00_Lkey         &StopKey,
                         tsp00_Bool          bCountRecords,
                         tsp00_Int4         &PageCnt,
                         tsp00_Int4         &AllPageCnt,
                         tsp00_Int4         &RecordCnt)
{
    ROUTINE_DBG_MEO00 ("b03calculate_page_count");

    /*
    This procedure operates on the B* tree that contains an inversion
    It enters in the parameter PageCount the number of pages that store
    the inversion lists with the list keys in the area
    between StartKey and StopKey. 
    */

    tgg00_BasisError        &TrError = Trans.trError_gg00;
    TrError = e_ok;
    tgg11_BdEvalPagesTrace   EvalPageCntTrace;

    if  (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        EvalPageCntTrace.bdeRootTrace_gg11.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03_LKeyVtrace (Trans, StartKey, StopKey, b03eval);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd eval pcnt");
    t01treeid (bi, "treeid      ", FileId);
    t01sname  (bi, "StartKey:   ");
    t01lkey   (bi, StartKey);
    t01sname  (bi, "StopKey:    ");
    t01lkey   (bi, StopKey);
    t01bool   (bi, "count recs  ", bCountRecords);

    /* check if file contains really an inversion */
    if ( ! g04inv_tfn( FileId.fileTfn_gg00() ))
    {
        TrError = e_invalid_filetype;
        return;
    }

#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        if ((StartKey.keyLen_gg00() > PERM_KEY_MXSP00)  ||
                (StartKey.keyLen_gg00() < 0              )  ||
                (StopKey.keyLen_gg00()  > PERM_KEY_MXSP00)  ||
                (StopKey.keyLen_gg00()  < 0              ))
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_select),
                                          tgg00_VtraceType::fromConst(b03eval),
                                          ! IGNORE_INV_ERROR_BD300);
            if (e_ok == TrError)
            {
                /* determine pointers to start and stop keys */
                tsp00_KeyPtr  pStartKey     = tsp00_KeyPtr (&StartKey.keyVal_gg00());
                tsp00_KeyPtr  pStopKey      = tsp00_KeyPtr (&StopKey.keyVal_gg00());
                tsp00_Int2   &StartKeyLen   = StartKey.keyLen_gg00();
                tsp00_Int2   &StopKeyLen    = StopKey.keyLen_gg00();

                tsp00_LcompResult Lresult;
                s30cmp (pStartKey, POS_OFF_DIFF_BD00, StartKeyLen, pStopKey, POS_OFF_DIFF_BD00, StopKeyLen, Lresult);
                if (l_equal == Lresult) pStopKey = pStartKey;

                bd401CalculatePageCount (InvCurrent, pStartKey,  StartKeyLen, pStopKey, StopKeyLen,
                                         (bCountRecords == TRUE), PageCnt, AllPageCnt, RecordCnt);

                FileId = InvCurrent.curr_tree_id;
            }

            if (e_file_not_found == TrError)
                TrError = e_inv_not_found;
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01p2int4      (bi, "page count  ", PageCnt, " page cnt", AllPageCnt);
    t01int4        (bi, "record count", RecordCnt);
    t01basis_error (bi, "eval pc err ", TrError);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        EvalPageCntTrace.bdeRootTrace_gg11.bdrRoot_gg11    = FileId.fileRoot_gg00();
        EvalPageCntTrace.bdeRootTrace_gg11.bdrTrType_gg11.becomes(bdtrEvalPages_egg11);
        EvalPageCntTrace.bdeRootTrace_gg11.bdrError_gg11   = TrError;
        EvalPageCntTrace.bdePageCnt_gg11                   = PageCnt;
        EvalPageCntTrace.bdeRecordCnt_gg11                 = RecordCnt;
        EvalPageCntTrace.bdeAllPageCnt_gg11                = AllPageCnt;

        Trace_CommonEntry traceEntry(  tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03eval),
                           REINTERPRET_CAST( tsp00_BytePtr, &EvalPageCntTrace ), sizeof (EvalPageCntTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
b03get_inv (tgg00_TransContext &Trans,
            tgg00_FileId       &FileId,
            tgg00_Lkey         &SecKey,
            tgg00_Lkey         &StartPrimKey,
            tgg00_Lkey         &StopPrimKey,
            tgg00_Lkey         &CurrPrimKey,
            tgg00_BdInvSet     &InvRangeSet,
            tsp00_Bool          CountOnly,
            unsigned char       AuxWantedLock,
            tgg00_KeylistBuf   &KeyListBuf,
            tsp00_Int4         &NumPrimKeys,
            tsp00_Int4         &ListLen)
{
    ROUTINE_DBG_MEO00 ("b03get_inv");

    /*
    A search is carried out in inversion FileId for the list with
    the list key entered in SecKey.  If the list is present, those of
    its record keys that are greater than the key entered in StartPrimKey
    and which are smaller or equal than the key StopPrimKey considered that 
    InvRangeSet includes(primary_stop) are sorted and arranged in ascending 
    order and transferred to buffer KeyListBuf.  If there are more keys to 
    be transferred than the buffer can hold, the buffer is filled to capacity 
    with (complete) record keys and 'e_buffer_limit' is sent to trError_gg00.
    The keys that have been transferred are concatenated with length
    bytes to form a string that is stored in b, is left-justified
    (i.e. begins in position 1) and whose length is displayed in ListLen.

    Buffer KeyListBuf with list of record keys:
      
    <- - - - - - ListLen bytes  - - - ->
    +--+----+--+------+--+--+--+-------+---------+
    |l1| k1 |l2| k2   |l3|k3|l4|  k4   |/////////|
    +--+----+--+------+--+--+--+-------+---------+

    li = length of the Key ki 
            
    Long inversion lists can be completely scanned via a series of
    'b03get_inv' calls whereby the lowest record key is entered in 
    StartPrimKey during the first call and, for all subsequent calls,
    the highest record key that has been provided up until that 
    time is entered in StartPrimKey. */


    tgg00_SelectFieldsParam   *pSelectFields     = NULL; //  Holger
    tgg00_StackDesc           *pStackDescription = NULL; //  Holger

    tgg00_BasisError   &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace   RootTrace;
    tgg00_LockReqMode   WantedLock;

    WantedLock = tgg00_LockReqMode::fromInt(AuxWantedLock);

    if  (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03_LKeyVtrace (Trans, SecKey, StartPrimKey, b03get);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd get inv  ");
    t01treeid (bi, "FileId      ", FileId);
    t01lkey (bi, SecKey);
    t01lkey (bi, StartPrimKey);
    if  (InvRangeSet.includes(primary_stop)) t01lkey (bi, StopPrimKey);
#   endif

    if (FileId.fileType_gg00().includes (ftsTemp_egg00))
        b73cmd_count (icget_inv);
    else
        b73cmd_count (iget_inv);

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }

    if (e_ok == TrError)
    {
        if (((SecKey.keyLen_gg00()       > PERM_KEY_MXSP00)      ||
                (SecKey.keyLen_gg00()       < 0              )   ||
                (StartPrimKey.keyLen_gg00() > PERM_KEY_MXSP00)   ||
                (StartPrimKey.keyLen_gg00() < 0              ))  ||
                (InvRangeSet.includes(primary_stop)              &&
                 ((StopPrimKey.keyLen_gg00() > PERM_KEY_MXSP00)  ||
                  (StopPrimKey.keyLen_gg00()  < 0            ))))
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            ListLen = 0;
            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_select),
                                          tgg00_VtraceType::fromConst(b03get),
                                          ! IGNORE_INV_ERROR_BD300);
            if (e_ok == TrError)
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                /* determine pointers to start and stop keys */
                tsp00_KeyPtr  pStartPrimKey   = tsp00_KeyPtr (&StartPrimKey.keyVal_gg00());
                tsp00_KeyPtr  pStopPrimKey    = tsp00_KeyPtr (&StopPrimKey.keyVal_gg00());
                tsp00_KeyPtr  pCurrPrimKey    = tsp00_KeyPtr (&CurrPrimKey.keyVal_gg00());
                tsp00_Int2   &StartPrimKeyLen = StartPrimKey.keyLen_gg00();
                tsp00_Int2   &StopPrimKeyLen  = StopPrimKey.keyLen_gg00();
                tsp00_Int2   &CurrPrimKeyLen  = CurrPrimKey.keyLen_gg00();


                if (InvRangeSet.includes (primary_stop))
                {
                    tsp00_LcompResult Lresult;

                    s30cmp (pStartPrimKey, POS_OFF_DIFF_BD00, StartPrimKeyLen,
                            pStopPrimKey, POS_OFF_DIFF_BD00, StopPrimKeyLen, Lresult);

                    if (l_equal == Lresult)
                        pStopPrimKey   = pStartPrimKey;
                }
                else
                    pStopPrimKey = NULL;

                bd401GetFromInvTree (InvCurrent,
                                     tsp00_KeyPtr(&SecKey.keyVal_gg00()), SecKey.keyLen_gg00(),
                                     pStartPrimKey, StartPrimKeyLen, pStopPrimKey, StopPrimKeyLen,
                                     pCurrPrimKey, CurrPrimKeyLen, InvRangeSet.includes (incl_first),
                                     TRUE == CountOnly, pSelectFields, pStackDescription, WantedLock,
                                     KeyListBuf, NumPrimKeys, ListLen);

                FileId = InvCurrent.curr_tree_id;

                if (e_buffer_limit == TrError)
                    InvRangeSet.addElement (incl_first);
            }
            if (e_file_not_found == TrError)
                TrError = e_inv_not_found;
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if (ListLen > 0)
    {
        t01buf  (bi, KeyListBuf, 1, ListLen);
    }
    t01int4 (bi, "ListLen     ", ListLen);
    t01int4 (bi, "NumPrimKeys ", NumPrimKeys);
    t01basis_error (bi, "get inv err ", Trans.trError_gg00);
#   endif

    if  (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = Trans.trError_gg00;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03get),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd03InvStatistics (tgg00_TransContext &Trans,
                   tgg00_FileId       &FileId,
                   tsp00_Bool          bWithSelectivity,
                   tgg00_SampleInfo   &StatInfo)
{
    ROUTINE_DBG_MEO00 ("bd03InvStatistics");

    tgg00_BasisError  &TrError = Trans.trError_gg00;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd Statistic");
    t01treeid (bi, "FileId      ", FileId);
    t01bool   (bi, "WithSelectiv", bWithSelectivity);
#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        TrError = e_ok;

        cbd300_InvCurrent InvCurrent (Trans, FileId,
                                      tbd_node_request::fromConst(nr_for_read),
                                      tgg00_MessType::fromConst(m_select),
                                      tgg00_VtraceType::fromConst(b03get),
                                      ! IGNORE_INV_ERROR_BD300);
            
        if (e_ok == TrError)
        {
            const SAPDB_Bool cWithSelectivity = true;
            bd401InvStatistics (InvCurrent, cWithSelectivity, StatInfo);
        }
    }
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "Statistic er", TrError);
#   endif
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03CountClusteredPages (
    tgg00_TransContext    &  Trans,
    tgg00_FileId          &  FileId,
    tsp00_Int4            &  LeafPageCount,
    tsp00_Int4            &  ClusteredBlockCount)
{
    ROUTINE_DBG_MEO00 ("bd03CountClusteredPages");
    tgg00_BasisError &trError = Trans.trError_gg00;
    
    LeafPageCount       = 0;
    ClusteredBlockCount = 0;
    tsp00_Int4 TotalClusteredPages = 0;
            
    if( b01downfilesystem )
        trError = e_shutdown;
    else
    {
        trError = e_ok;
        
        cbd300_InvCurrent invCurrent( Trans, FileId,
                                      tbd_node_request::fromConst( nr_for_read ),
                                      tgg00_MessType::fromConst(m_select), 
                                      tgg00_VtraceType::fromConst(b03eval),
                                      IGNORE_INV_ERROR_BD300 );

        if( e_ok == trError )
        {
            bd401CountClusteredPages( invCurrent, LeafPageCount, ClusteredBlockCount );
            
            if ( LeafPageCount == 0 )
                LeafPageCount = 1;
                
            if ( ClusteredBlockCount == 0 )
                ClusteredBlockCount = 1;
        }
    }
   
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error( bi, "bd03CountClu", trError );
#   endif
}

/*---------------------------------------------------------------------------*/

externC void
bd03KeyVtrace (
    tgg00_TransContext    &Trans,
    tsp00_KeyPtr           pKey1,
    tsp00_Int4             KeyLen1,
    tsp00_KeyPtr           pKey2,
    tsp00_Int4             KeyLen2,
    tgg00_VtraceType_Enum  TraceObject)
{
    tgg00_BasisError        AuxError = e_ok;
    /* */
    tsp00_Int4              MaxVtraceKey = sizeof (tgg00_VtraceKey);
    tsp00_Int4              OutKeyLen;
    /* */
    tgg11_BdTwoKeysTrace    KeyTrace;

    KeyTrace.bdtkTrType_gg11.becomes(bdtrTwoKeys_egg11);
    KeyTrace.bdtkKeyLen_gg11 [0] = KeyLen1;
    KeyTrace.bdtkKeyLen_gg11 [1] = KeyLen2;

    OutKeyLen = (KeyLen1 > MaxVtraceKey) ? MaxVtraceKey : KeyLen1;

    if ( pKey1 != 0 )
        SAPDB_RangeMove( __FILE__, 7,
               PERM_KEY_MXSP00, sizeof (KeyTrace.bdtkKey_gg11 [0]),
               pKey1,                     POS_OFF_DIFF_BD00,
               KeyTrace.bdtkKey_gg11 [0], POS_OFF_DIFF_BD00,
               OutKeyLen, AuxError);
    AuxError = e_ok; // ignore move error

    OutKeyLen = (KeyLen2 > MaxVtraceKey) ? MaxVtraceKey : KeyLen2;

    if ( pKey2 != 0 )
        SAPDB_RangeMove( __FILE__, 8,
               PERM_KEY_MXSP00, sizeof (KeyTrace.bdtkKey_gg11 [1]),
               pKey2,                     POS_OFF_DIFF_BD00,
               KeyTrace.bdtkKey_gg11 [1], POS_OFF_DIFF_BD00,
               OutKeyLen, AuxError);

    Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject),
                       REINTERPRET_CAST( tsp00_BytePtr, &KeyTrace ), sizeof (KeyTrace) );
}

/*---------------------------------------------------------------------------*/

externC void
b03MergeInvLists (tgg00_TransContext &Trans,
                  tgg00_FileId       &InvFileId,
                  tgg00_FileId       &DestFileId,
                  tgg00_TwoKeys      &StartKeyPair,
                  tgg00_TwoKeys      &StopKeyPair,
                  tsp00_Bool          bStopPrimKeyExists,
                  tsp00_Bool          bCountOnly,
                  tsp00_Int4         &NumPrimKeys)
{
    ROUTINE_DBG_MEO00 ("b03MergeInvLists");

    /* This routine generates a merge set from the inversion lists of
    inversion InvFileId. It begins with the list that is identified by
    the list key StartKeyPair.listkey and ends with the list that is
    identified by the list key StopKeyPair.listkey.  If bStopPrimKeyExists 
    has not been assigned a value, all primary keys for the relevant list
    are used, beginning with StartKeyPair.reckey.  Otherwise, the desired
    primary keys for any one list are selected via the interval
    StartKeyPair.reckey - StopKeyPair.reckey. The primary keys from the 
    merge set are then inserted in the temporary inversion file DestFileId*/

    /* vtrace output for slowknl */
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd merge inv");
    t01treeid (bi, "MergeFileId ", InvFileId);
    if (! bCountOnly) t01treeid (bi, "DestFileId  ", DestFileId);
    t01lkey (bi, StartKeyPair.listkey);
    t01lkey (bi, StartKeyPair.reckey);
    t01lkey (bi, StopKeyPair.listkey);
    if  (bStopPrimKeyExists) t01lkey (bi, StopKeyPair.reckey);
    t01bool (bi, "bCountOnly  ", bCountOnly);
#   endif

    tgg00_BasisError   &TrError = Trans.trError_gg00;
    tsp00_LcompResult   Lresult;

    if  (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        bd03_MultiLKeyVtrace( Trans, StartKeyPair.listkey, StartKeyPair.reckey,
                              StopKeyPair.listkey, StopKeyPair.reckey, bStopPrimKeyExists );
    };

    if (b01downfilesystem)
        TrError = e_shutdown;
    else
    {
        /* check input keys */
        if  ((StartKeyPair.listkey.keyLen_gg00() > PERM_KEY_MXSP00)     ||
                (StartKeyPair.reckey.keyLen_gg00()   > PERM_KEY_MXSP00)     ||
                (StopKeyPair.listkey.keyLen_gg00()   > PERM_KEY_MXSP00)     ||
                (StartKeyPair.listkey.keyLen_gg00()  < 0)                   ||
                (StartKeyPair.reckey.keyLen_gg00()   < 0)                   ||
                (StopKeyPair.listkey.keyLen_gg00()   < 0)                   ||
                (bStopPrimKeyExists                                       &&
                 ((StopKeyPair.reckey.keyLen_gg00()   > PERM_KEY_MXSP00) ||
                  (StopKeyPair.reckey.keyLen_gg00()    < 0))))
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            tsp00_KeyPtr  pStartSecKey     = tsp00_KeyPtr (&StartKeyPair.listkey.keyVal_gg00());
            tsp00_Int2    StartSecKeyLen   = StartKeyPair.listkey.keyLen_gg00();
            tsp00_KeyPtr  pStopSecKey      = tsp00_KeyPtr (&StopKeyPair.listkey.keyVal_gg00());
            tsp00_Int4    StopSecKeyLen    = StopKeyPair.listkey.keyLen_gg00();
            tsp00_KeyPtr  pStartPrimKey    = tsp00_KeyPtr (&StartKeyPair.reckey.keyVal_gg00());
            tsp00_Int4    StartPrimKeyLen  = StartKeyPair.reckey.keyLen_gg00();
            tsp00_KeyPtr  pStopPrimKey     = tsp00_KeyPtr (&StopKeyPair.reckey.keyVal_gg00());
            tsp00_Int4    StopPrimKeyLen   = StopKeyPair.reckey.keyLen_gg00();

            if (bStopPrimKeyExists)
            {
                s30cmp (pStartPrimKey, POS_OFF_DIFF_BD00, StartPrimKeyLen,
                        pStopPrimKey, POS_OFF_DIFF_BD00, StopPrimKeyLen, Lresult);
                if (l_equal == Lresult) pStopPrimKey = pStartPrimKey;
            }
            else
            {
                pStopPrimKey   = NULL;
                StopPrimKeyLen = 0;
            }

            s30cmp (pStartSecKey, POS_OFF_DIFF_BD00, StartSecKeyLen,
                    pStopSecKey, POS_OFF_DIFF_BD00, StopSecKeyLen, Lresult);

            if (l_equal == Lresult)
                pStopSecKey   = pStartSecKey;

            if( e_ok == TrError )
            {
                cbd300_InvCurrent   InvCurrent(Trans, InvFileId,
                                               tbd_node_request::fromConst(nr_for_read),
                                               tgg00_MessType::fromConst(m_select),
                                               tgg00_VtraceType::fromConst(b03merge),
                                               ! IGNORE_INV_ERROR_BD300);
                if (e_ok == TrError)
                {
                    if (bCountOnly)
                    {
                        bd403CountPrimKeysInInvLists (InvCurrent, pStartSecKey, StartSecKeyLen,
                                                      pStopSecKey, StopSecKeyLen, pStartPrimKey, StartPrimKeyLen, pStopPrimKey,
                                                      StopPrimKeyLen, NumPrimKeys);
                    }
                    else
                    {
                        /* create handle to access the result file */
                        cbd300_InvCurrent   DestCurrent(Trans, DestFileId,
                                                        tbd_node_request::fromConst(nr_for_read),
                                                        tgg00_MessType::fromConst(m_nil),
                                                        tgg00_VtraceType::fromConst(b03merge),
                                                        IGNORE_INV_ERROR_BD300);

                        if (e_ok == TrError)
                        {
                            bd403MergePrimKeysInInvLists (InvCurrent, DestCurrent, pStartSecKey, StartSecKeyLen,
                                                          pStopSecKey, StopSecKeyLen, pStartPrimKey, StartPrimKeyLen,
                                                          pStopPrimKey, StopPrimKeyLen, NumPrimKeys);

                            DestFileId = DestCurrent.curr_tree_id;
                        }
                    };
                    InvFileId = InvCurrent.curr_tree_id;
                }
            }

            /* map general error onto an index error */
            if (e_file_not_found == TrError)
                TrError = e_inv_not_found;

            if ((0    == NumPrimKeys) &&
                    (e_ok == TrError))
            {
                TrError = e_file_empty;
            }
        }
    }

    /* vtrace output */
    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        tgg11_BdRootTrace RootTrace;
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrRoot_gg11   = InvFileId.fileRoot_gg00();
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03merge),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }

    /* vtrace output for slowknl */
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    if  (bCountOnly)
        t01int4 (bi, "NumPrimKeys       ", NumPrimKeys);
    t01basis_error (bi, "merge    err", TrError);
#   endif
}

/*---------------------------------------------------------------------------*/

externC void
b03next_invkey (tgg00_TransContext &Trans,
                tgg00_FileId       &FileId,
                tsp00_Bool          bIncludeSecKey,
                tgg00_Lkey         &SecKey)   //[inp/out]

{
    ROUTINE_DBG_MEO00 ("b03next_invkey");

    /*
    (a) if bIncludeSecKey == True this function returns in Trans.trError_gg00 
        either e_ok if an inversion list with the secondary key SecKey exists 
        or e_inv_list_not_found if not.
    (b) if bIncludeSecKey == True next secondary key following SecKey in 
        in ascending order is returned in SecKey. If no such key exists
        Trans.trError_gg00 is set to e_no_next_invkey.

      Possible acknowledgements in Trans.trError_gg00:

      - e_ok
      - e_inv_not_found
      - e_no_next_invkey
      - e_inv_list_not_found
      - e_shutdown
    */


    tgg00_BasisError  &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace  RootTrace;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        tgg00_Lkey PrimKey;
        PrimKey.keyLen_gg00() = 0;

        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03_LKeyVtrace (Trans, SecKey, PrimKey, b03next_list);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd next invk");
    t01treeid (bi, "FileId      ", FileId);
    t01sname  (bi, "SecKey:     ");
    t01lkey   (bi, SecKey);
    t01bool   (bi, "Inclusive   ", bIncludeSecKey);
#   endif


    if ( b01downfilesystem )
    {
        TrError = e_shutdown;
    }
    else
    {
        if ( (SecKey.keyLen_gg00() > PERM_KEY_MXSP00) || (SecKey.keyLen_gg00()  < 0) )
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            TrError = e_ok;

            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_select),
                                          tgg00_VtraceType::fromConst(b03next_list),
                                          ! IGNORE_INV_ERROR_BD300);

            if (e_ok == TrError)
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                bd402NextFromInvTree (InvCurrent,(bIncludeSecKey == TRUE),
                                      tsp00_KeyPtr(&(SecKey.keyVal_gg00())), SecKey.keyLen_gg00());

                FileId = InvCurrent.curr_tree_id;

                if ( e_file_not_found == TrError )
                    TrError = e_inv_not_found;
            }
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "NextSecKey: ");
    t01lkey   (bi, SecKey);
    t01basis_error (bi, "next inv err", TrError);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03next_list),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd03StatisticsUpdate (tgg00_TransContext &Trans,
                      tgg00_FileId       &FileId,
                      tsp00_Int4         &NumLeaves,
                      tsp00_Int4         &AvgPrimKeyCountPerPage,
                      tsp00_Bool         bOnlyCountLeaves)
{
    ROUTINE_DBG_MEO00 ("bd03StatisticsUpdate");
    /* PTS            1107153, A.K. 17.07.2000                              */
    /* if bOnlyCountLeaves is true AvgPrimKeyCountPerPage is not calculated */

    tgg00_BasisError  &TrError = Trans.trError_gg00;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd StatUpdat");
    t01treeid (bi, "FileId      ", FileId);
    t01bool   (bi, "OnlyCountLea", bOnlyCountLeaves);
#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        TrError = e_ok;

        cbd300_InvCurrent InvCurrent (Trans, FileId,
                                      tbd_node_request::fromConst(nr_for_read),
                                      tgg00_MessType::fromConst(m_select),
                                      tgg00_VtraceType::fromConst(b03get),
                                      ! IGNORE_INV_ERROR_BD300);

        if (e_ok == TrError)
        {
            bd401StatisticsUpdate (InvCurrent, NumLeaves, AvgPrimKeyCountPerPage, (TRUE==bOnlyCountLeaves));

            if (e_file_not_found == TrError)
                TrError = e_inv_not_found;
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "StatUpd err ", TrError);
    t01p2int4      (bi, "NumLeaves   ", NumLeaves, "AvgPrimKeyCo", AvgPrimKeyCountPerPage);
#   endif
}


/*---------------------------------------------------------------------------*/

externC void
bd03ParticipantCreateIndex (
    tgg00_TransContext     &Trans,             // [IN/OUT]
    tgg00_FileId           &PrimFileId,        // [IN]
    tgg00_FileId           &InvFileId,         // [IN]
    tgg00_StackDesc        &StackDesc,         // [IN]
    tsp00_KeyPtr            pStartPrimKey,     // [IN]
    tsp00_KeyPtr            pStopPrimKey,      // [IN]
    tsp00_Int4              StartPrimKeyLen,   // [IN]
    tsp00_Int4              StopPrimKeyLen,    // [IN]
    tsp00_Int4              NumLeavesToScan,   // [IN]
    tsp00_Int4              AvgRecordsPerLeaf, // [IN]
    tsp00_PageNo            PrimQueuePno,      // [IN]
    tsp00_PageNo            SecQueuePno,       // [IN]
    tsp00_Bool              bUseMemorySort,    // [IN]
    tgg00_IndexMode_Param   IndexMode,         // [IN]
    tgg00_IndexCounter     &IndexCounter,      // [OUT]
    tsp00_Int4             &TempLeafPageCount, // [OUT]
    tsp00_PageNo           &FirstLeafPno,      // [OUT]
    tsp00_Bool             &bEntriesExist      // [OUT]
)
{
    ROUTINE_DBG_MEO00 ("bd03ParticipantCreateIndex");


    tgg00_BasisError  &TrError = Trans.trError_gg00;


    bEntriesExist     = false;
    FirstLeafPno      = NIL_PAGE_NO_GG00;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        bd03KeyVtrace (Trans, pStartPrimKey, StartPrimKeyLen,
                       pStopPrimKey, StopPrimKeyLen, b03pcreate);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01int4   (bi, "PrimQueuePno", PrimQueuePno);
    t01int4   (bi, "SecQueuePno ", SecQueuePno);
    t01int4   (bi, "NumLeavesToS", NumLeavesToScan);
    t01int4   (bi, "AvgRecPerLea", AvgRecordsPerLeaf);
    t01int4   (bi, "IndexMode   ", IndexMode);
    t01bool   (bi, "UseMemSort  ", bUseMemorySort);

    if  (0 == StartPrimKeyLen) t01sname (bi, "ZEROSTARTKEY");
    else t01buf (bi, pStartPrimKey, 1, StartPrimKeyLen);

    if  (0 == StopPrimKeyLen)  t01sname (bi, "ZEROSTOPKEY ");
    else t01buf (bi, pStopPrimKey, 1, StopPrimKeyLen);
#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        if ((StartPrimKeyLen > PERM_KEY_MXSP00)  ||
                (StartPrimKeyLen < 0              )  ||
                (StopPrimKeyLen  > PERM_KEY_MXSP00)  ||
                (StopPrimKeyLen  < 0             ))
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            TrError = e_ok;
            tbd_current_tree PrimCurrent;

            bd30GetTree (Trans, PrimFileId, PrimCurrent, m_create_index_parallel,
                ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );

            if (e_ok == TrError)
            {
                tbd_current_tree InvCurrent;

                bd30BuildCurrent (Trans, InvFileId, InvCurrent);

                if (e_ok == TrError)
                {
                    if ( PrimCurrent.currExtendedFileTypeSet_bd00.includes(eftsClustered_egg00) )
                        InvCurrent.currExtendedFileTypeSet_bd00.addElement(eftsClustered_egg00);

                    bd405ParticipantCreateIndex (InvCurrent, PrimCurrent, StackDesc,
                                                 pStartPrimKey, pStopPrimKey, StartPrimKeyLen, StopPrimKeyLen,
                                                 NumLeavesToScan, AvgRecordsPerLeaf, PrimQueuePno, SecQueuePno,
                                                 bUseMemorySort, tgg00_IndexMode::fromInt(IndexMode), IndexCounter,
                                                 TempLeafPageCount, FirstLeafPno, bEntriesExist );
                }
            }
            bd30ReleaseTree (PrimCurrent);
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01bool   (bi, "EntryExist  ", bEntriesExist);
    t01p2int4 (bi, "PrimKeyCnt  ", IndexCounter.idc_prim_keycount
               ,   "NullCnt     ", IndexCounter.idc_nullcount);
    t01p2int4 (bi, "SecKeyCnt   ", IndexCounter.idc_sec_keycount
               ,   "temp pages  ", TempLeafPageCount);    
    t01p2int4 (bi, "PrimLeafNode", IndexCounter.idc_prim_leafnodes
               ,   "SecLeafNodes", IndexCounter.idc_sec_leafnodes);
    if (NIL_PAGE_NO_GG00 != FirstLeafPno)
        t01int4   (bi, "FirstLeafPno", FirstLeafPno);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        tgg11_BdRootTrace  RootTrace;

        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = Trans.trError_gg00;
        RootTrace.bdrRoot_gg11   = InvFileId.fileRoot_gg00();

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03pcreate),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd03ReleaseInvTree (tgg00_TransContext &Trans,
                    tgg00_FileId       &FileId)
{
    ROUTINE_DBG_MEO00 ("bd03ReleaseInvTree");

    tgg00_BasisError   &TrError = Trans.trError_gg00;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd rel itree");
    t01treeid (bi, "FileId      ", FileId);
#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
        return;
    }
     
    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
    {
        Trace_CommonEntry traceEntry ( tgg00_Debug::fromConst(ta), tgg00_VtraceType::fromConst(td_none), 
                                       reinterpret_cast< const tsp00_Byte*> ( "bd03ReleaseInvTree" ), 18);
    }

    cbd300_InvCurrent InvCurrent (Trans, FileId,
                                  tbd_node_request::fromConst(nr_for_update), /* PTS 1105868 UH 07-03-2000 */ //PTS 1105704 TS 2000-02-23
                                  tgg00_MessType::fromConst(m_drop),
                                  tgg00_VtraceType::fromConst(b01p_destroy),
                                  IGNORE_INV_ERROR_BD300);

    if ( e_ok == TrError )
        bd400DropInvTree (InvCurrent);

    if ( e_disk_not_accessible != TrError )
        TrError = e_ok;
}

/*---------------------------------------------------------------------------*/

externC void
b03select_invrec (tgg00_TransContext      &Trans,
                  tgg00_TwoFileIds        &FileIds,
                  tgg00_TwoKeys           &StartKeyPair,
                  tgg00_TwoKeys           &StopKeyPair,
                  tgg00_Lkey              &StartKey,
                  tgg00_BdInvSet          &InvRangeSet,
                  tsp00_Int4               RecBufSize,
                  tsp00_MoveObjPtr         pRecBuf,
                  tgg00_SelectFieldsParam &SelectParam,
                  tgg00_StackDesc         &StackDesc,
                  tgg00_LockReqMode       &GrantedLock)
{
    ROUTINE_DBG_MEO00 ("b03select_invrec");

    tgg00_BasisError   &TrError = Trans.trError_gg00;

    // e_buffer_limit will be returned only if the given  buffer is too
    // small and no row was inserted! If the buffer is to small but at
    // least one row was inserted the messType2 is adjusted and the error
    // code is set to e_ok.
    // if e_no_next_record is returned no more result rows are available
    // and its not (!) guaranteed that the given search keys are updated in
    // this way that a subsequent call returns e_no_next_record again and
    // a m_result_cnt of zero! The same behaviour for e_no_prev_record.

    /*
    Furnishes *pRecBuf  with a record from the file fnames.fn that must fulfill
    a qualification specified by parameters SelectParam and StackDesc.
    The primary key for the search in file fnames.fn is determined via
    the parameters StartKeyPair, StopKeyPair, StartKey, InvRangeSet
    and SelectParam.bd_mess2_type in the inversion fnames.invfn.
    The parameter StartKeyPair, consisting of listkey (inversion list key) and
    reckey (primary key in the inversion list) gives the starting position
    for the search in the inversion. Stop and StopKeyPair specify the point
    at which the search in the inversion is broken off.
      
    The parameter SelectParam.sfp_bd_mess2_type(), which gives the search
    direction, can be assigned the following values:

    -mm_next   : In the inversion list with the list key listkey,
                 a search is carried out for the next primary key that is
                 greater than the primary key entered in reckey.  Under certain
                 circumstances, the search continues on into the inversion list
                 with the next greatest list key starting at primary key StartKey.
                 In the primary file, the
                 qualification of the record belonging to this primary key is
                 checked.  If this qualification is fulfilled, e_ok is returned;
                 if not, a search is carried out for the next primary key in the
                 inversion.  If no further primary key is present,
                 e_no_next_record is sent to TrError.

     -mm_prev  : In the inversion list with the list key listkey,
                 a search is carried out for the next primary key that is
                 smaller than the primary key entered in reckey.  Under certain
                 circumstances, the search continues on into the inversion list
                 with the next smallest list key starting at primary key start key.
                 In the primary file, the
                 qualification of the record belonging to this primary key is
                 checked.  If this qualification is fulfilled, e_ok is returned;
                 if not, a search is carried out for the next primary key in the
                 inversion.  If no further primary key is present,
                 b_no_prev_record is sent to TrError.

     -mm_first : The same occurs as with mm_next, with the
                 following difference:  The primary key in the inversion list with
                 the list key listkey that is transferred when the procedure is
                 called is also used for the qualification check in the primary

     -mm_last  : The same occurs as with mm_prev, with the
                 following difference :  The primary key in the inversion list with
                 the list key listkey that is transferred when the procedure is
                 called is also used for the qualification check in the primary

     -mm_direct: In the inversion with the list key listkey, a
                 search is carried out for the primary key reckey.  If reckey is
                 found, it is used in the primary file to check the qualification
                 of the record; if not, e_key_not_found or e_inv_list_not_found
                 is returned to e.  If the qualification is not fulfilled,
                 kb_record_not_qualified is returned.
    */

    tgg00_VtraceType   TraceObject;
    tgg11_BdRootTrace  RootTrace;
    bool               bPrimaryFileErr = false;

    // UH 2002-08-01 initialization depends not on enabled vtrace - begin
    switch (SelectParam.sfp_bd_mess2_type())
    {
    case (mm_first) :
                case (mm_direct): TraceObject.becomes(b03first_qual); break;
    case (mm_next)  : TraceObject.becomes(b03next_qual);  break;
    case (mm_prev)  : TraceObject.becomes(b03prev_qual);  break;
    case (mm_last)  : TraceObject.becomes(b03last_qual);  break;
    }
    RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
    // UH 2002-08-01 initialization depends not on enabled vtrace - end

    /* long output to vtrace */
    if  (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
{
        if ( g01vtrace.vtrStrategy_gg00 )
            bd03_MultiLKeyVtrace (Trans, 
                    StartKeyPair.listkey, StartKeyPair.reckey, 
                    StopKeyPair.listkey, StopKeyPair.reckey, InvRangeSet.includes(primary_stop),
                    TraceObject);
        else
            bd03_LKeyVtrace (Trans, StartKeyPair.listkey, StartKeyPair.reckey, TraceObject);
        bd03_WriteFileIdsAndInputKeysIntoVtrace (FileIds, StartKeyPair, StopKeyPair, StartKey, InvRangeSet, SelectParam);
    }

    if (b01downfilesystem)
        TrError = e_shutdown;

    if (e_ok == TrError)
    {
        cbd300_InvCurrent InvCurrent (Trans, FileIds.inv_id,
                                      tbd_node_request::fromConst(nr_for_read),
                                      SelectParam.sfp_bd_mess_type(),
                                      TraceObject,
                                      ! IGNORE_INV_ERROR_BD300);

        RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

        if (e_ok != TrError)
        {
            if (e_file_not_found == TrError) TrError = e_inv_not_found;
        }
        else
        {
            if ((StartKeyPair.reckey.keyLen_gg00()  > PERM_KEY_MXSP00)  ||
                    (StartKeyPair.reckey.keyLen_gg00()  < 0)                ||
                    (StartKeyPair.listkey.keyLen_gg00() > PERM_KEY_MXSP00)  ||
                    (StartKeyPair.listkey.keyLen_gg00() < 0))
            {
                TrError = e_illegal_key;
            }
            else
            {
                /* determine pointers to start and stop keys */
                tsp00_KeyPtr  pStartSecKey     = tsp00_KeyPtr (&StartKeyPair.listkey.keyVal_gg00());
                tsp00_Int2   &StartSecKeyLen   = StartKeyPair.listkey.keyLen_gg00();
                
                tsp00_KeyPtr  pStopSecKey      = reinterpret_cast <tsp00_KeyPtr> (&StopKeyPair.listkey.keyVal_gg00());
                tsp00_Int4    StopSecKeyLen    = StopKeyPair.listkey.keyLen_gg00();
                
                tsp00_KeyPtr  pStartPrimKey    = tsp00_KeyPtr (&StartKey.keyVal_gg00());
                tsp00_Int4    StartPrimKeyLen  = StartKey.keyLen_gg00();
                
                tsp00_KeyPtr  pStopPrimKey     = tsp00_KeyPtr (&StopKeyPair.reckey.keyVal_gg00());
                tsp00_Int4    StopPrimKeyLen   = StopKeyPair.reckey.keyLen_gg00();
                
                tsp00_KeyPtr  pFirstPrimKey    = tsp00_KeyPtr (&StartKeyPair.reckey.keyVal_gg00());
                tsp00_Int2   &FirstPrimKeyLen  = StartKeyPair.reckey.keyLen_gg00();

                bd03_OptimizeKeyPointers (SelectParam, InvRangeSet,
                                          pStartSecKey,  StartSecKeyLen,
                                          pStopSecKey,   StopSecKeyLen,
                                          pStartPrimKey, StartPrimKeyLen,
                                          pStopPrimKey,  StopPrimKeyLen,
                                          pFirstPrimKey, FirstPrimKeyLen);

                if (InvCurrent.curr_tree_id.fileType_gg00().includes(ftsPerm_egg00))
                {
                    bd402SearchIndexForQualifiedRecords (InvCurrent, FileIds.file_id,
                                                         pStartSecKey, StartSecKeyLen, pStopSecKey, StopSecKeyLen,
                                                         pStartPrimKey, StartPrimKeyLen, pStopPrimKey,  StopPrimKeyLen,
                                                         pFirstPrimKey, FirstPrimKeyLen,
                                                         RecBufSize, pRecBuf, SelectParam, StackDesc,
                                                         GrantedLock, bPrimaryFileErr);
                }
                else
                {
                    tgg00_QualKind  AuxQualKind = SelectParam.sfp_bd_inv_only();
                    SelectParam.sfp_bd_inv_only ().becomes(primary_only);

                    bd402SearchResultFileForQualifiedRecords (InvCurrent, FileIds.file_id,
                            pFirstPrimKey, FirstPrimKeyLen, RecBufSize, pRecBuf,
                            SelectParam, StackDesc, GrantedLock);

                    SelectParam.sfp_bd_inv_only().becomes (AuxQualKind);
                }
                FileIds.inv_id = InvCurrent.curr_tree_id;
            }

            /* interpret error of previous select routines */
            if (e_ok != TrError)
            {
                if(bPrimaryFileErr)
                {   /* since the error does not occur due to an error in the */
                    /* index tree but in the primary tree, it must not be    */
                    /* considered in the destructor of InvCurrent            */
                    InvCurrent.bd300SkipErrorHandling ();
                }
                else
                {
                    if  (e_file_not_found == TrError)
                        TrError = e_inv_not_found;
                }

                if ((e_buffer_limit == TrError) && (mm_direct !=SelectParam.sfp_bd_mess2_type()))
                    TrError = e_ok;
                else
                    if  (e_bad_file == TrError) TrError = e_bad_datapage;
            }
        }
    }

    /* output to vtrace */
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if ((mm_last == SelectParam.sfp_bd_mess2_type()) ||
            (mm_prev == SelectParam.sfp_bd_mess2_type()))
        t01basis_error (bi, "pqual inv er", TrError);
    else
        t01basis_error (bi, "nqual inv er", TrError);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        if ( g01vtrace.vtrStrategy_gg00 )
        {
            tgg11_BdIntTrace  ResTrace;
            
            ResTrace.bdiTrType_gg11.becomes(bdtrInt_egg11);
            ResTrace.bdiInt_gg11[0] = SelectParam.sfp_m_result_cnt();
            Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject),
                               REINTERPRET_CAST( tsp00_BytePtr, &ResTrace ), sizeof (ResTrace) );
        }
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
b03test_unique_inv (tgg00_TransContext &Trans,
                    tgg00_FileId         &FileId,
                    tgg00_Lkey           &SecKey,
                    tgg00_Lkey           &PrimKey)
{
    ROUTINE_DBG_MEO00 ("b03test_unique_inv");

    tgg00_BasisError  &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace  RootTrace;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03_LKeyVtrace (Trans, SecKey, PrimKey, b03empty);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd test uniq");
    t01treeid (bi, "FileId      ", FileId);
    t01sname  (bi, "SecKey:     ");
    t01lkey   (bi, SecKey);
    t01sname  (bi, "PrimKey:    ");
    t01lkey   (bi, PrimKey);
#   endif

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        if ( (SecKey.keyLen_gg00()  > PERM_KEY_MXSP00) ||
                (SecKey.keyLen_gg00()  < 0              ) ||
                (PrimKey.keyLen_gg00() > PERM_KEY_MXSP00) ||
                (PrimKey.keyLen_gg00() < 0              )  )
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            TrError = e_ok;

            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_key),
                                          tgg00_VtraceType::fromConst(b03empty),
                                          ! IGNORE_INV_ERROR_BD300);

            if (e_ok == TrError)
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                if (e_ok == TrError)
                {
                    bd400TestUniqueInv (InvCurrent,
                                        tsp00_KeyPtr(&SecKey.keyVal_gg00()), SecKey.keyLen_gg00(),
                                        tsp00_KeyPtr(&PrimKey.keyVal_gg00()), PrimKey.keyLen_gg00() );
                    FileId = InvCurrent.curr_tree_id;
                }

                if (e_file_not_found == TrError)
                    TrError = e_inv_not_found;
            }
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "test uniq er", TrError);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03empty),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd03VerifyInvTree (tgg00_TransContext &Trans,
                   tgg00_FileId       &FileId,
                   tsp00_Bool          IsCold,
                   tsp00_Bool          bWithExtendedCheck,
                   tsp00_Int4         &NumPages)
{
    ROUTINE_DBG_MEO00 ("bd03VerifyInvTree");

    tgg00_BasisError &TrError = Trans.trError_gg00;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname  (bi, "bd verify   ");
    t01treeid (bi, "FileId      ", FileId);
#   endif

    if (b01downfilesystem)
        TrError = e_shutdown;
    else
    {
        cbd300_InvCurrent InvCurrent (Trans, FileId,
                                      tbd_node_request::fromConst(nr_for_read),
                                      tgg00_MessType::fromConst(m_verify),
                                      tgg00_VtraceType::fromConst(b03get),
                                      ! IGNORE_INV_ERROR_BD300);

        tgg00_IndexCounter counter;
    
        counter.idc_sec_leafnodes              = 0; /* not  used */
        counter.idc_sec_keycount               = 0; /* only used */
        counter.idc_prim_keycount              = 0; /* only used */
        
        if (e_ok == TrError)
            bd400Verify (InvCurrent, TRUE == IsCold, TRUE == bWithExtendedCheck, NumPages, counter);
    
        if ( IsCold && e_ok == TrError )
            bd03_CheckPageAndEntryCount(InvCurrent, counter);
    }
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "verify inv  ", TrError);
#   endif
}

/*---------------------------------------------------------------------------*/

externC void
bd03SetToNotAccessible (
    tgg00_TransContext &Trans,
    tgg00_FileId       &FileId )
{
    ROUTINE_DBG_MEO00 ("bd03SetToNotAccessible");

    tgg00_BasisError  &TrError = Trans.trError_gg00;

    if( b01downfilesystem )
    {
        TrError = e_shutdown;
    }
    else
    {
        TrError = e_ok;

        cbd300_InvCurrent invCurrent( Trans, FileId,
                                      tbd_node_request::fromConst( nr_for_read ),
                                      tgg00_MessType::fromConst( m_set),
                                      tgg00_VtraceType::fromConst( b03add ),
                                      IGNORE_INV_ERROR_BD300);

        invCurrent.bd300SkipErrorHandling();

        invCurrent.bd300SetFileNotAccessible( false );

        if( e_file_not_found == TrError )
            TrError = e_inv_not_found;
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error( bi, "set not acc ", TrError );
#   endif
}

/*---------------------------------------------------------------------------*/

externC void
bd03ExtractIndex (
    tgg00_TransContext &trans,
    tsp00_PageNo       root,
    tsp00_VFilename    &hostFileName )
{
    ROUTINE_DBG_MEO00 ("bd03ExtractIndex");
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01name( bi, "===> extract index" );
    t01int4( bi, "fileRoot    ", root );
#   endif
    
    if( b01downfilesystem ){
        trans.trError_gg00 = e_shutdown;
    }
    else
    {
        trans.trError_gg00 = e_ok;
        tgg00_FileId invFileId;
        
        invFileId = b01niltree_id;
        invFileId.fileRoot_gg00() = root;
        invFileId.fileType_gg00().clear();
        invFileId.fileType_gg00().addElement( ftsPerm_egg00 );
        invFileId.fileType_gg00().addElement( ftsConcurrent_egg00 );
        invFileId.fileTfn_gg00().becomes( tfnMulti_egg00 );
        
        cbd300_InvCurrent invCurrent( trans, invFileId,
                                      tbd_node_request::fromConst( nr_for_read ),
                                      tgg00_MessType::fromConst( m_select), 
                                      tgg00_VtraceType::fromConst( b03get ),
                                      IGNORE_INV_ERROR_BD300 );

        if( e_ok == trans.trError_gg00 ){
            bd400ExtractIndex( invCurrent, hostFileName );
        }
    }
    if(
        ( e_old_fileversion == trans.trError_gg00 ) ||
        ( e_file_not_found  == trans.trError_gg00 ) ||
        ( e_inv_not_found   == trans.trError_gg00 )
        )
    {
        trans.trError_gg00 = e_invalid_root;
    }

    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error( bi, "Extract idx ", trans.trError_gg00 );
#   endif
}

/*---------------------------------------------------------------------------*/

externC void 
bd03CreateIndexFile(
    tgg00_TransContext    &trans,
    tgg00_FileId          &invFileId,
    tgg00_FileId          &tableFileId,
    const tsp00_Int2      indexNo )
{
    ROUTINE_DBG_MEO00( "bd03CreateIndexFile" );

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01name( bi, "===> CreateIdxFile" );
#   endif

    tgg00_BasisError  &trError = trans.trError_gg00;

    if( b01downfilesystem )
        trError = e_shutdown;
    else
    {
        trError = e_ok;
        bd03_CreateIndexFile( trans, invFileId ); 
        if( e_ok == trError ) 
        {
            cbd300_InvCurrent invCurrent( trans,
                                          invFileId,
                                          tbd_node_request::fromConst( nr_for_update ),
                                          tgg00_MessType::fromConst( m_create_index ), 
                                          tgg00_VtraceType::fromConst( b03create ),
                                          IGNORE_INV_ERROR_BD300);

            SAPDB_Bool bPagesClustered = false;
            if( e_ok == trError )
                bPagesClustered = bd998ArePagesClustered( trans, tableFileId.fileTabId_gg00() ) ? true : false;

            if( e_ok == trError )
            {
                tgg00_IndexCounter dummy;
                dummy.idc_sec_leafnodes                = 1;
                dummy.idc_sec_keycount                 = 0;
                dummy.idc_prim_keycount                = 0;
                invCurrent.currLeafPageCountDelta_bd00 = dummy.idc_sec_leafnodes;
                bd03_AddIndexToFileDirectory( invCurrent, tableFileId, indexNo, dummy, true, bPagesClustered );
                if( e_ok != trError )
                    b13free_node( invCurrent.currRootNptrs_bd00, invCurrent );
            }
        }
        #ifdef TEST_STATISTICS
        if (e_ok == trans.trError_gg00)
            bd03_CheckFiledirectoryStatistics (trans, invFileId);
        #endif        
    }
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error( bi, "create err  ", trans.trError_gg00 );
#   endif
    if (( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
    {
        if( e_ok == trError ) 
        {
            const FileDir_FileNo tableFileNo( tableFileId.fileTabId_gg00());
            const FileDir_FileNo invFileNo( invFileId.fileTabId_gg00());

            Kernel_VTrace() << "bd03CreateIndexFile: " 
                << NewLine << "->" << " InvFileNo: "   << invFileNo
                << NewLine << "->" << " TableFileNo: " << tableFileNo
                << NewLine << "->" << " IndexNo: "     << indexNo
                << NewLine << "->" << " InvRoot: "     << invFileId.fileRoot_gg00();
        }
        else
            Kernel_VTrace() << "bd03CreateIndexFile/BaseError: " << trError;
    }
}

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE) OMS KEYS                                         * 
 *===========================================================================*/

externC void 
bd03CreateOmsKeyPartitionFile(
    tgg00_TransContext    &trans,
    Msg_List              &msgList,
    tgg00_FileId          &omsKeyFileId,
    const FileDir_FileNo  &omsKeyFileNo,
    const FileDir_FileNo  &omsParentFileNo,
    const tsp00_Int2      partitionNo,
    pasbool               addToFileDir)
{
    ROUTINE_DBG_MEO00( "bd03CreateOmsKeyFile" );

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01name( bi, "===> CreateKeyFile" );
#   endif

    tgg00_BasisError  &trError = trans.trError_gg00;
    Converter_Version convVersion;

    if( b01downfilesystem ){
        trError = e_shutdown;
    }
    else
    {
        trError = e_ok;
        bd03_CreateIndexFile( trans, omsKeyFileId ); 
        
        if( e_ok != trError ) 
            return;
        
        cbd300_InvCurrent invCurrent( trans, omsKeyFileId, tbd_node_request::fromConst( nr_for_read ),
                                      tgg00_MessType::fromConst( m_create_file ), 
                                      tgg00_VtraceType::fromConst( b03create ),
                                      IGNORE_INV_ERROR_BD300 );
        if( e_ok != trError ) 
            return;
        
        invCurrent.bd300LockTreeExclusive(); // synchronize addFdir with savepoint
        
        if( e_ok != trError ) 
            return;
        
        if (addToFileDir)  /* PTS 1134858 FF*/
        {
            
            convVersion = bd20GetDestinationConverterVersion( trans.trTaskId_gg00, 
                invCurrent.currRootNptrs_bd00.np_cbptr());
            
            FileDir_OmsKeyPartition omsKeyFileEntry;
            
            const FileDir_ReturnCode retCode = 
                FileDir_IDirectory::SessionInstance( trans ).AddOmsKeyPartitionFile( 
                trans.trTaskId_gg00, omsKeyFileNo, omsParentFileNo, convVersion, 
                partitionNo, omsKeyFileId.fileRoot_gg00(), msgList, &omsKeyFileEntry );
            
            switch ( retCode )
            {
            case FileDir_Okay:
                omsKeyFileEntry.SetLeafPageCount( 1 );
                break;
            case FileDir_FileNoExists:              // PTS 1137268 UH 2005-08-10
                trError = e_duplicate_filename;     // This error is necessary so that it might be ignored during redo
                break;
            case FileDir_OutOfMemory:               // PTS 1137268 UH 2005-08-10
                trError = e_no_more_memory;         // handle as much as can
                break;
            default:
                b13free_node( invCurrent.currRootNptrs_bd00, invCurrent );
                trError = e_wrong_obj_file_id;
                break;
            }
        }
    }
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error( bi, "create err  ", trans.trError_gg00 );
#   endif
    if (( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
    {
        if( e_ok == trError ) 
        {
            Kernel_VTrace() << "bd03CreateOmsKey: " 
                << NewLine << "->" << " TaskId:"           << trans.trTaskId_gg00
                << NewLine << "->" << " FileNo:"           << omsKeyFileNo
                << NewLine << "->" << " Parent-FileNo:"    << omsParentFileNo
                << NewLine << "->" << " ConverterVersion:" << convVersion
                << NewLine << "->" << " Partition-Idx:"    << partitionNo
                << NewLine << "->" << " Root:"             << omsKeyFileId.fileRoot_gg00();
        }
        else
            Kernel_VTrace() << "bd03CreateOmsKey/BaseError: " << trError;
    }
}

/*-----------------------------------------------------------------------------------*/

externC void
bd03OmsAddInv (tgg00_TransContext &Trans,
               tgg00_FileId       &FileId,
               tsp00_KeyPtr        pSecKey,
               tsp00_Int4          SecKeyLen,
               tsp00_KeyPtr        pPrimKey,
               tsp00_Int4          PrimKeyLen,
               tsp00_Int4          FoundPrimKeySize,
               tsp00_KeyPtr        pFoundPrimKey,
               tsp00_Int4         &FoundPrimKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd03OmsAddInv");

    tgg00_BasisError  &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace  RootTrace;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03KeyVtrace (Trans, pSecKey, SecKeyLen, pPrimKey, PrimKeyLen, b03add);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname   (bi, "bd add inv  ");
    t01treeid  (bi, "FileId      ", FileId);
    t01sname   (bi, "SecKey:     ");
    t01moveobj (bi, pSecKey, POS_OFF_DIFF_BD00, SecKeyLen);
    t01sname   (bi, "PrimKey:    ");
    t01moveobj (bi, pPrimKey, POS_OFF_DIFF_BD00, PrimKeyLen);
#   endif

    if (FileId.fileType_gg00().includes (ftsTemp_egg00 ))
        b73cmd_count (icadd_inv);
    else
        b73cmd_count (iadd_inv);

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        if ( (SecKeyLen  > PERM_KEY_MXSP00) ||
                (SecKeyLen  < 0              ) ||
                (PrimKeyLen > PERM_KEY_MXSP00) ||
                (PrimKeyLen < 0              )  )
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            TrError = e_ok;

            FileId.fileBdUse_gg00().addElement(bd_write_acc);

            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_new),
                                          tgg00_VtraceType::fromConst(b03add),
                                          ! IGNORE_INV_ERROR_BD300);

            InvCurrent.bd300SkipErrorHandling();

            if (e_ok == TrError)
            {

                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                bd400AddToOmsInvTree (InvCurrent, pSecKey, SecKeyLen, pPrimKey, PrimKeyLen,
                                      FoundPrimKeySize, pFoundPrimKey, FoundPrimKeyLen);
                FileId = InvCurrent.curr_tree_id;

                if (e_file_not_found == TrError)
                    TrError = e_inv_not_found;

            }

            FileId.fileBdUse_gg00().delElement(bd_write_acc);
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "add inv err ", TrError);
    t01sname   (bi, "FndPrimKey: ");
    t01moveobj (bi, pFoundPrimKey, POS_OFF_DIFF_BD00, FoundPrimKeyLen);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03add),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*-----------------------------------------------------------------------------------*/
/* conditional delete - PTS 1131960 FF */
externC void
bd03OmsDelInv (tgg00_TransContext &Trans,
               tgg00_FileId       &FileId,
               tsp00_KeyPtr        pSecKey,
               tsp00_Int4          SecKeyLen,
               tsp00_KeyPtr        pPrimKey,
               tsp00_Int4          PrimKeyLen,
               tsp00_Int4          FoundPrimKeySize,
               tsp00_KeyPtr        pFoundPrimKey,
               tsp00_Int4          FoundPrimKeyToCompareSize,
               tsp00_Int4         &FoundPrimKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd03OmsDelInv");

    const bool         UNLOCK = true;

    tgg00_BasisError  &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace  RootTrace;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03KeyVtrace (Trans, pSecKey, SecKeyLen, NULL, 0, b03del);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname   (bi, "bd del inv  ");
    t01treeid  (bi, "FileId      ", FileId);
    t01sname   (bi, "SecKey:     ");
    t01moveobj (bi, pSecKey, POS_OFF_DIFF_BD00, SecKeyLen);
#   endif

    if (FileId.fileType_gg00().includes (ftsTemp_egg00 ))
        b73cmd_count (icdel_inv);
    else
        b73cmd_count (idel_inv);

    if (b01downfilesystem)
    {
        TrError = e_shutdown;
    }
    else
    {
        if ( (SecKeyLen  > PERM_KEY_MXSP00) ||
                (SecKeyLen  < 0              )  )
        {
            TrError = e_illegal_keylength;
        }
        else
        {
            TrError = e_ok;

            FileId.fileBdUse_gg00().addElement(bd_write_acc);

            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_delete),
                                          tgg00_VtraceType::fromConst(b03del),
                                          ! IGNORE_INV_ERROR_BD300);

            InvCurrent.bd300SkipErrorHandling();

            if (e_ok == TrError)
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                bd400DelFromOmsInvTree (InvCurrent, pSecKey, SecKeyLen, pPrimKey, PrimKeyLen,
                    FoundPrimKeySize, pFoundPrimKey, FoundPrimKeyToCompareSize, FoundPrimKeyLen);

                FileId = InvCurrent.curr_tree_id;

                if (e_file_not_found == TrError)
                    TrError = e_inv_not_found;
            }

            FileId.fileBdUse_gg00().delElement(bd_write_acc);
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "del inv err ", TrError);
#   endif

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = TrError;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03del),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*-----------------------------------------------------------------------------------*/

externC void
bd03OmsGetInv (tgg00_TransContext &Trans,
               tgg00_FileId       &FileId,
               tsp00_KeyPtr        pSecKey,
               tsp00_Int4          SecKeyLen,
               tsp00_Int4          PrimKeySize,
               tsp00_KeyPtr        pPrimKey,
               tsp00_Int4         &PrimKeyLen)
{
    ROUTINE_DBG_MEO00 ("bd03OmsGetInv");

    tgg00_BasisError   &TrError = Trans.trError_gg00;
    tgg11_BdRootTrace   RootTrace;

    /* write vtrace entries */
    if  (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrRoot_gg11 = NIL_PAGE_NO_GG00;
        bd03KeyVtrace (Trans, pSecKey, SecKeyLen, NULL, 0, b03get);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01sname   (bi, "bd get inv  ");
    t01treeid  (bi, "FileId      ", FileId);
    t01sname   (bi, "SecKey:     ");
    t01moveobj (bi, pSecKey, POS_OFF_DIFF_BD00, SecKeyLen);
#   endif

    /* update statictics about usage of index */
    SAPDBERR_ASSERT_STATE (!FileId.fileType_gg00().includes (ftsTemp_egg00));
    b73cmd_count (iget_inv);

    if (b01downfilesystem)
        TrError = e_shutdown;

    if (e_ok == TrError)
    {
        if ((SecKeyLen  > PERM_KEY_MXSP00) || (SecKeyLen  < 0))
            TrError = e_illegal_keylength;
        else
        {
            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_select),
                                          tgg00_VtraceType::fromConst(b03get),
                                          ! IGNORE_INV_ERROR_BD300);

            InvCurrent.bd300SkipErrorHandling();

            if (e_ok == TrError)
            {
                RootTrace.bdrRoot_gg11 = InvCurrent.curr_tree_id.fileRoot_gg00();

                bd400GetFromOmsInvTree (InvCurrent, pSecKey, SecKeyLen, PrimKeySize, pPrimKey, PrimKeyLen);
                FileId = InvCurrent.curr_tree_id;
            }
            if (e_file_not_found == TrError)
                TrError = e_inv_not_found;
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01basis_error (bi, "get inv err ", Trans.trError_gg00);
#   endif

    if  (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
    {
        RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
        RootTrace.bdrError_gg11  = Trans.trError_gg00;

        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03get),
                           REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );
    }
}

/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------*/

static void bd03_FillTree (cbd300_InvCurrent &InvCurrent,
                           tsp00_Int4         minRootLevel)
{
    tgg00_Lkey  SecKey;
    tgg00_Lkey  PrimKey;
    tsp00_Int4 *pCounter = reinterpret_cast<tsp00_Int4*>(&SecKey.keyVal_gg00());
    bool        dummySecKeyFound;
    tsp00_Int4  leafcount = 0;

    SecKey.keyLen_gg00() = PERM_KEY_MXSP00;
    SecKey.keyVarOffSpace_gg00() = 0;
    SecKey.keyVarCntSpace_gg00() = 0;
    
    pCounter = pCounter + 250;
    SAPDB_MemFillNoCheck (&SecKey.keyVal_gg00(), 0xAA, SecKey.keyLen_gg00());
    
    PrimKey.keyLen_gg00() = PERM_KEY_MXSP00;
    PrimKey.keyVarOffSpace_gg00() = 0;
    PrimKey.keyVarCntSpace_gg00() = 0;
    
    SAPDB_MemFillNoCheck (&PrimKey.keyVal_gg00(), 0xBB, PrimKey.keyLen_gg00());

    *pCounter = 0;
    
    while (InvCurrent.bd300GetRootLevel() < minRootLevel )
    {

        bd400AddToInvTree (InvCurrent,
                           tsp00_KeyPtr(&SecKey.keyVal_gg00()), SecKey.keyLen_gg00(),
                           tsp00_KeyPtr(&PrimKey.keyVal_gg00()), PrimKey.keyLen_gg00(),
                           dummySecKeyFound);
        if ( InvCurrent.curr_trans->trError_gg00 != e_ok )
            return;
        ++(*pCounter); // !!! machine swapping !!!
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03TestTreeInit (tgg00_FileId &InvFileId)
{
    InvFileId = b01niltree_id;
    InvFileId.fileType_gg00().clear();
    InvFileId.fileType_gg00().addElement(ftsPerm_egg00);
    InvFileId.fileType_gg00().addElement(ftsConcurrent_egg00);
    InvFileId.fileName_gg00()[0] = 0xFF;
    InvFileId.fileName_gg00()[1] = 0xFF;
    InvFileId.fileName_gg00()[2] = 0xFF;
    InvFileId.fileName_gg00()[3] = 0xFF;
    InvFileId.fileName_gg00()[4] = 0xFF;
    InvFileId.fileName_gg00()[5] = 'U';
    InvFileId.fileName_gg00()[6] = 'W';
    InvFileId.fileName_gg00()[7] = 'E';
    InvFileId.fileTfn_gg00().becomes(tfnMulti_egg00);
    InvFileId.fileTfnNo_gg00()[0] = 1;
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03TestTreeDrop (tgg00_TransContext &trans,tgg00_FileId &InvFileId)
{
    ROUTINE_DBG_MEO00 ("bd03TestTreeDrop");
    b01pdestroy_perm_file(trans,InvFileId);
    trans.trError_gg00 = e_ok;
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03TestTreeCreate (tgg00_TransContext &trans,tgg00_FileId &InvFileId)
{
    ROUTINE_DBG_MEO00 ("bd03TestTreeCreate");
    bd03_CreateIndexFile (trans, InvFileId);
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03TestTreeFill (tgg00_TransContext &trans,tgg00_FileId &InvFileId)
{
    ROUTINE_DBG_MEO00 ("bd03TestTreeFill");

    InvFileId.fileBdUse_gg00().addElement(bd_write_acc);

    cbd300_InvCurrent InvCurrent (trans, InvFileId,
                                  tbd_node_request::fromConst(nr_for_update),
                                  tgg00_MessType::fromConst(m_insert),
                                  tgg00_VtraceType::fromConst(b03create),
                                  ! IGNORE_INV_ERROR_BD300);

    
    if ( e_ok == trans.trError_gg00 )
    {
        const tsp00_Int4 rootLevel = 4;
        bd03_FillTree (InvCurrent, rootLevel);
        if ( e_ok == trans.trError_gg00
             &&
             InvCurrent.bd300GetRootLevel() != rootLevel )
            trans.trError_gg00 = e_not_implemented;
    }

    InvFileId = InvCurrent.curr_tree_id;
    InvFileId.fileBdUse_gg00().delElement(bd_write_acc);
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03TestTreeCheck (tgg00_TransContext &trans,tgg00_FileId &InvFileId)
{
    ROUTINE_DBG_MEO00 ("bd03TestTreeCheck");

    tsp00_Int4 NumPages = 0;

    bd03VerifyInvTree (trans,InvFileId,false,false,NumPages);
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03TestReadPrev (tgg00_TransContext &trans,tgg00_FileId &InvFileId)
{
    ROUTINE_DBG_MEO00 ("bd03TestReadPrev");

    tgg00_BasisError &trError = trans.trError_gg00;
    
    InvFileId.fileBdUse_gg00().addElement(bd_write_acc);

    cbd300_InvCurrent InvCurrent (trans, InvFileId,
                                  tbd_node_request::fromConst(nr_for_update),
                                  tgg00_MessType::fromConst(m_insert),
                                  tgg00_VtraceType::fromConst(b03create),
                                  ! IGNORE_INV_ERROR_BD300);


    InvFileId = InvCurrent.curr_tree_id;
        
    if ( e_ok == trError )
    {
        cbd600_Node auxNode (InvCurrent, tbd_node_request::fromConst(nr_for_read));
        cbd600_Node node    (InvCurrent, tbd_node_request::fromConst(nr_for_read));
        cbd500_Tree tree    (node);

        if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 )
        {
            Trace_CommonEntry traceEntry ( tgg00_Debug::fromConst(ta), tgg00_VtraceType::fromConst(td_none), 
                                           reinterpret_cast< const tsp00_Byte*> ( "bd03TestTree" ), 12);
        }

        tsp00_PageNo prevPno;
        tsp00_Int4   leftToRightLeafCount;
        tsp00_Int4   rightToLeftLeafCount;
        tsp00_Int2   currentLevel = LEAF_LEVEL_BD00;
        
        const tsp00_Int4 rootLevel = 5;

        while ( e_ok == trError
                &&
                currentLevel < rootLevel )
        {
            //****************************************************************
            rightToLeftLeafCount = 0;
            tree.bd500SetToNodeOnEdge (false, currentLevel);
            while ( e_ok == trError && node.bd600IsAccessable() )
            {
                if ( node.bd600Level() != currentLevel )
                    trError = e_not_implemented;
                else
                {
                    ++rightToLeftLeafCount;
                    
                    prevPno = tree.bd500LeftNeighborId(auxNode);
                    if ( trError == e_ok )
                    {
                        if ( auxNode.bd600IsAccessable() )
                            trError = e_not_implemented;
                        if ( trError == e_ok )
                            tree.bd500PrevNode();
                        if ( trError == e_ok
                             &&
                             prevPno != NIL_PAGE_NO_GG00
                             &&
                             node.bd600IsAccessable()
                             &&
                             prevPno != node.bd600GetId() )
                        {
                            trError = e_not_implemented;
                        }
                    }
                }
            }
            //****************************************************************
            if ( e_ok == trError )
            {
                leftToRightLeafCount = 0;
                tree.bd500SetToNodeOnEdge (true, currentLevel);
                while ( e_ok == trError && node.bd600IsAccessable() )
                {
                    if ( node.bd600Level() != currentLevel )
                        trError = e_not_implemented;
                    else
                    {
                        ++leftToRightLeafCount;
                        tree.bd500NextNode();
                    }
                }
            }
            //****************************************************************
            if ( e_ok == trError
                 &&
                 leftToRightLeafCount != rightToLeftLeafCount )
                trError = e_not_implemented;
            //****************************************************************
            ++currentLevel;
        }
    }

    InvFileId.fileBdUse_gg00().delElement(bd_write_acc);
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03SimpleInvStatistics (
    tgg00_TransContext    &  Trans,
    tgg00_FileId          &  FileId,
    tsp00_Int4            &  Leafnodes,
    tsp00_Int4            &  SecKeyCnt)
{
    ROUTINE_DBG_MEO00 ("b03SimpleInvStatistics");

    tgg00_BasisError  &TrError = Trans.trError_gg00;

    const tsp00_Int4    cUndefined = -2; //this value corresponds to cak_is_undefined in vak001

    FileDir_IDirectory &fdir    = FileDir_IDirectory::SessionInstance(Trans);
    Msg_List           &msgList = Trans_Context::GetContext(Trans).ErrorList();
    FileDir_Index       indexEntry;
    FileDir_ReturnCode  fDirRc  = fdir.GetIndexFile(Trans.trTaskId_gg00,
                                                    FileId.fileTabId_gg00(),
                                                    indexEntry,
                                                    msgList);
    if ( FileDir_Okay == fDirRc )
    {
        if ( indexEntry.AreCountersInitialized() &&
             (indexEntry.GetFileState() == Data_FileOk) )
        {
            const SAPDB_UInt8 indexEntryCount = indexEntry.GetEntryCount(Trans.trTaskId_gg00);

            if ( indexEntryCount <= SAPDB_MAX_INT4 )
                SecKeyCnt = static_cast<SAPDB_Int4>(indexEntryCount);
            else
                SecKeyCnt = SAPDB_MAX_INT4;

            Leafnodes = indexEntry.GetLeafPageCount(Trans.trTaskId_gg00);
        }
        else
        {
            cbd300_InvCurrent InvCurrent (Trans, FileId,
                                          tbd_node_request::fromConst(nr_for_read),
                                          tgg00_MessType::fromConst(m_select),
                                          tgg00_VtraceType::fromConst(b03get),
                                          ! IGNORE_INV_ERROR_BD300);
            if (e_ok == TrError)
            {
                const tsp00_PageNo &Root = InvCurrent.curr_tree_id.fileRoot_gg00();
                cbd600_Node         Node (InvCurrent, tbd_node_request::fromConst(nr_for_read), Root);
                if (e_ok == TrError)
                {
                    Leafnodes = Node.bd600LeafCount ();
                    SecKeyCnt = cUndefined;
                }
            }

            if (e_file_not_found == TrError)
                TrError = e_inv_not_found;
        }
    }
    else
    {
        switch ( fDirRc )
        {
            case FileDir_FileNoNotFound:
                TrError = e_inv_not_found;
                msgList.ClearMessageList();
                break;
            default:
                TrError = e_system_error;
        }
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03GetInvPageCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate VAR_ARRAY_REF  indexSurr,
    SAPDB_Int4            VAR_VALUE_REF  leafPageCount)
{
    ROUTINE_DBG_MEO00 ("bd03GetInvPageCount");

    trans.trError_gg00 = e_ok;

    SAPDB_Int4 dummyPageCount;
    SAPDB_Int4 dummySecKeyCount;
    FileDir_Index indexFile;
    if ( ! bd03_GetInvPageAndRecordCount(
             trans, indexSurr, indexFile, leafPageCount, dummySecKeyCount ) )
    {
        if ( trans.trError_gg00 != e_ok )
            return;

        // counters in file directory not yet available, check index tree
        tgg00_Lkey startKey;
        tgg00_Lkey stopKey;
        const tsp00_Bool bCountRecords = true;
        tgg00_FileId indexFileId;
        
        startKey.keyLen_gg00() = 0;
        stopKey.keyLen_gg00()  = PERM_KEY_MXSP00;
        stopKey.keyVal_gg00()  = b01fullkey;

        indexFile.BuildFileId( indexFileId );

        b03calculate_page_count(
            trans, indexFileId, startKey, stopKey, ! bCountRecords,
            leafPageCount, dummyPageCount, dummySecKeyCount);
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03GetInvPageAndRecordCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate VAR_ARRAY_REF  indexSurr,
    SAPDB_Int4            VAR_VALUE_REF  leafPageCount,
    SAPDB_Int4            VAR_VALUE_REF  secKeyCount)
{
    ROUTINE_DBG_MEO00 ("bd03GetInvPageAndRecordCount");

    trans.trError_gg00 = e_ok;

    FileDir_Index indexFile;
    if ( ! bd03_GetInvPageAndRecordCount(
             trans, indexSurr, indexFile, leafPageCount, secKeyCount ) )
    {
        if ( trans.trError_gg00 != e_ok )
            return;

        // counters in file directory not yet available, check index tree
        tgg00_Lkey startKey;
        tgg00_Lkey stopKey;
        SAPDB_Int4 dummyPageCount;
        const tsp00_Bool bCountRecords = true;
        tgg00_FileId indexFileId;

        startKey.keyLen_gg00() = 0;
        stopKey.keyLen_gg00()  = PERM_KEY_MXSP00;
        stopKey.keyVal_gg00()  = b01fullkey;
    
        indexFile.BuildFileId( indexFileId );

        b03calculate_page_count(
            trans, indexFileId, startKey, stopKey, bCountRecords,
            leafPageCount, dummyPageCount, secKeyCount);
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void bd03SetIndexLeafCaching (
    integer                  value)
{
    ROUTINE_DBG_MEO00 ("bd03SetIndexLeafCaching");
    Data_IndexLeafCaching = value;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/ 
