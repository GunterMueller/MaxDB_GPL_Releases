/****************************************************************************

  module      : vbd300.cpp

  -------------------------------------------------------------------------

  responsible : UweH

  special area: Class Implementation
  description : InvCurrentBasis, InvCurrent, InvCurrentSubtree 


  last changed: 1999-09-06  17:01
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

#include "gsp00.h"
#include "ggg00.h"
#include "ggg11.h"
#include "gbd00.h"
#include "gbd300.h"

#include "hsp20.h"
#include "hsp30.h"
#include "hbd01.h"
#include "hbd13.h"
#include "hbd20_9.h"
#include "hbd30.h"
#include "heo55k.h"  // excl
#include "hgg01_1.h" // g01abort,g01opmsg
#include "hgg01_3.h" // g01glob
#include "hbd06.h"   // PASCAL: Error_text_handling
#include "hbd06_1.h" // bd06IllegalRootAccess

#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "Trace/Trace_Entry.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "Messages/Msg_List.hpp"
#include "Transaction/Trans_Context.hpp"
#include "RunTime/RTE_Message.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#endif

/*---------------------------------------------------------------------------*/
// LOOK INTO VBD03.CPP TOO
// #define TEST_STATISTICS true

/*---------------------------------------------------------------------------*/

externC void
bd300FileIdRootCheck (tgg00_FileId     &FileId,
                      tgg00_BasisError &TrError)
{
    ROUTINE_DBG_MEO00 ("bd300FileIdRootCheck");

    if ( FileId.fileRootCheck_gg00() != ROOT_CHECK_GG00
            &&
            FileId.fileRootCheck_gg00() != (FileId.fileRoot_gg00() ^ ROOT_CHECK_GG00) )
        TrError = e_root_check;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01basis_error (bi, "TrError     ", TrError);
    t01int4        (bi, "RootCheck   ", FileId.fileRoot_gg00() ^ ROOT_CHECK_GG00);
#   endif
}

/*---------------------------------------------------------------------------*/

externC void
bd300SetRootCheck (tgg00_FileId &FileId)
{
    ROUTINE_DBG_MEO00 ("bd300SetRootCheck");

    FileId.fileRootCheck_gg00() = FileId.fileRoot_gg00() ^ ROOT_CHECK_GG00;
}

/*---------------------------------------------------------------------------*/

externC void
bd300LockTreeExclusive (cbd300_InvCurrent &Current)
{
    Current.bd300LockTreeExclusive();
}

/*===========================================================================*
 *  CLASS METHODS (CODE)                                                     *
 *===========================================================================*/


void
cbd300_InvCurrentBasis::bd300LockTree ()
{
    ROUTINE_DBG_MEO00 ("bd300LockTree");

    if ( NULL == currRootNptrs_bd00.np_ptr() )
    {
        bd13GetNode (*this, curr_tree_id.fileRoot_gg00(),
                     bd300_GetRootLockMode(), /* PTS 1108703 UH 2000-12-12 */
                     m_RequestKind, currRootNptrs_bd00);
    }
}

/*---------------------------------------------------------------------------*/

bool
cbd300_InvCurrentBasis::bd300LockTreeExclusive()
{
    ROUTINE_DBG_MEO00 ("bd300LockTreeExclusive");

    if ( nr_for_update != m_RequestKind )
    {
        bd300UnlockTree ();
        m_RequestKind.becomes (nr_for_update);
        bd13GetNode (*this, curr_tree_id.fileRoot_gg00(),
                     bd300_GetRootLockMode(), /* PTS 1108703 UH 2000-12-12 */
                     m_RequestKind, currRootNptrs_bd00);
        return true;
    }

    if ( NULL == currRootNptrs_bd00.np_ptr() )
    {
        m_RequestKind.becomes (nr_for_update); // for safety only
        bd300LockTree ();
        return true;
    }

    return false;
}

/*---------------------------------------------------------------------------*/

void
cbd300_InvCurrentBasis::bd300UpdatePageCounter()
{
    ROUTINE_DBG_MEO00 ("bd300UpdatePageCounter");

    if ( (currLeafPageCountDelta_bd00 != 0 || currIndexPageCountDelta_bd00 != 0)
         &&
         m_drop != curr_action
         &&
         m_verify != curr_action )
    {
        if ( m_create_index_parallel != curr_action && ! bd300IsExclusiveLocked() )
        {
            g01abort (csp3_bd_msg, csp3_n_btree, "TREE NOT EXCL. LOCKED   ", bd300GetRootId());
        }
        
        if ( bd300IsSubTree()
             &&
             (m_create_index_parallel == curr_action || m_create_index == curr_action) )
        {
            if ( 0 == m_pMasterCurrent )
            {
                g01abort (csp3_bd_msg, csp3_n_btree, "NO MASTER CURRENT       ", bd300GetRootId());
            }
            
            m_pMasterCurrent->currLeafPageCountDelta_bd00 += currLeafPageCountDelta_bd00;
            currLeafPageCountDelta_bd00 = 0;
            m_pMasterCurrent->currIndexPageCountDelta_bd00 += currIndexPageCountDelta_bd00;
            currIndexPageCountDelta_bd00 = 0;
        }
        else
        {
            Msg_List           &errlist = Trans_Context::GetContext(*curr_trans).ErrorList();
            FileDir_IDirectory &fdir    = FileDir_IDirectory::SessionInstance(*curr_trans);
            FileDir_Index       fdirIndexEntry;
    
            if ( FileDir_Okay == fdir.GetIndexFile(curr_trans->trTaskId_gg00,
                                                   FileDir_FileNo(curr_tree_id.fileTabId_gg00()),
                                                   fdirIndexEntry,
                                                   errlist) )
            {
                const Converter_Version converterVersion = bd20GetDestinationConverterVersion (curr_trans->trTaskId_gg00, currRootNptrs_bd00.np_cbptr());
    
                if ( currLeafPageCountDelta_bd00 != 0 )
                {
                    fdirIndexEntry.ModifyLeafPageCount(curr_trans->trTaskId_gg00, converterVersion, currLeafPageCountDelta_bd00);
                    currLeafPageCountDelta_bd00 = 0;
                }
                if ( currIndexPageCountDelta_bd00 != 0 )
                {
                    fdirIndexEntry.ModifyIndexPageCount(curr_trans->trTaskId_gg00, converterVersion, currIndexPageCountDelta_bd00);
                    currIndexPageCountDelta_bd00 = 0;
                }
            }
            else
            {
                // ignore errors because temp index files look like tab's
                errlist.ClearMessageList();
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
void
cbd300_InvCurrentBasis::bd300UpdateEntryCounter(SAPDB_Int4 counterDifference) const
{
    ROUTINE_DBG_MEO00 ("bd300UpdateEntryCounter");

    Trans_Context      &trans = Trans_Context::GetContext(*curr_trans);
    FileDir_IDirectory &fdir  = FileDir_IDirectory::SessionInstance(*curr_trans);
    FileDir_Index       fdirIndexEntry;
    FileDir_ReturnCode  result;
    Kernel_VTrace       trace;
 
    result = fdir.GetIndexFile( trans.Task().ID(), FileDir_FileNo(curr_tree_id.fileTabId_gg00()),
                                fdirIndexEntry, trans.ErrorList() );
    
    if ( FileDir_Okay == result )
    {
        const Converter_Version converterVersion = bd20GetDestinationConverterVersion (trans.Task().ID(), currRootNptrs_bd00.np_cbptr());
        fdirIndexEntry.ModifyEntryCount(trans.Task().ID(), converterVersion, counterDifference);
        #ifdef SAPDB_SLOW
        t01p2int4 (bi, "RootNode    ", currRootNptrs_bd00.np_ptr()->nd_id(),
                       "CounterDiff ", counterDifference);
        #endif
        #ifdef TEST_STATISTICS
        trace << "bd300UpdateEntryCounter: Root: " << currRootNptrs_bd00.np_ptr()->nd_id()
              << ", counterDiff: " << counterDifference;
        #endif
    }
    else
    {
        #ifdef SAPDB_SLOW
        t01p2int4 (bi, "RootNode ERR", currRootNptrs_bd00.np_ptr()->nd_id(),
                       "CounterDiff ", counterDifference);
        #endif
        #ifdef TEST_STATISTICS
        trace << "bd300UpdateEntryCounter: Root: " << currRootNptrs_bd00.np_ptr()->nd_id()
              << ", counterDiff: " << counterDifference
              << ", ERROR: " << result;
        #endif
        // ignore errors because temp index files look like tab's
        trans.ErrorList().ClearMessageList();
    }
}

/*---------------------------------------------------------------------------*/

void cbd300_InvCurrent::bd300UnlockTree (bool bTreeIsRemoved)
{
    ROUTINE_DBG_MEO00 ("cbd300_InvCurrent::bd300UnlockTree");
	
	tgg00_BasisError  &TrError = this->curr_trans->trError_gg00;
	
	cbd300_InvCurrentBasis::bd300UnlockTree (); /* PTS 1106088 UH 23-03-2000 */

	// PTS 1112332 UH 2001-10-30 Moved b06illegal_root_access() from cbd300_InvCurrent::bd300UnlockTree()
    if (e_ok != curr_trans->trError_gg00)
		bd06IllegalRootAccess (*(this->curr_trans), this->curr_tree_id, 
                               m_verify != this->curr_action);
    
    if ( ! m_bSkipErrorHandling && bd300_IsInvError (TrError) )
	{
        // PTS 1103712 TS 1999-08-31 
		if ( (e_file_not_accessible   != TrError)           ||
             (m_create_index_parallel == this->curr_action) ||
             (m_create_index          == this->curr_action)  )
			bd300SetFileNotAccessible ();

		if (m_bIgnoreInvError)
            this->curr_trans->trError_gg00 = e_ok;
	}
}

/*---------------------------------------------------------------------------*/

void
cbd300_InvCurrentBasis::bd300UnlockTree (bool bTreeIsRemoved) /* PTS 1106088 UH 23-03-2000 */
{
    ROUTINE_DBG_MEO00 ("bd300UnlockTree");

    tgg00_BasisError  &TrError = curr_trans->trError_gg00;

	if ( e_ok == TrError )
        bd300UpdatePageCounter();

    if (NULL != currRootNptrs_bd00.np_ptr())
    {
        tbd_node_ptrs AuxNptrsRoot = currRootNptrs_bd00;

        currNumLeaves_bd00 = currRootNptrs_bd00.np_ptr()->ndLeafCount_bd00();
#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        t01p2int4 (bi, "RootNode    ", currRootNptrs_bd00.np_ptr()->nd_id(),
                       "LeafCount   ", currRootNptrs_bd00.np_ptr()->ndLeafCount_bd00());
        t01p2int4 (bi, "Leaf Delta  ", currLeafPageCountDelta_bd00,
                       "Index Delta ", currIndexPageCountDelta_bd00);
#       endif

        // Monitor read accesses to detect non used indexes within the data base


        currRootNptrs_bd00.np_ptr  () = NULL;
        currRootNptrs_bd00.np_cbptr() = NULL;

        /* An error may be set outside and is handled here.       */
        /* In cause of bTreeIsRemoved is set currRootUpdated_bd00 */
        /* is set too                                             */

        if (
            ((e_ok == TrError) && (currRootUpdated_bd00))
            || // PTS 1104446 TS 1999-11-12 - PTS 1105022 TS 2000-01-06
            (
                ((m_create_index_parallel == curr_action)||(m_create_index == curr_action))
                &&
                ((e_ok == TrError) || (e_file_not_accessible == TrError)))
        )
        {
            if (g01glob.treecheck)
            {
                if(( ! bTreeIsRemoved ) && ( e_ok == TrError ))
                {
                    if ( AuxNptrsRoot.np_ptr()->nd_level() > LEAF_LEVEL_BD00
                         ||
                         AuxNptrsRoot.np_ptr()->nd_pt2() != pt2InvSubtree_egg00 )
                        bd13CheckNode( curr_trans->trTaskId_gg00, TrError, AuxNptrsRoot, curr_tree_id, true ); // PTS 1125100 TS 2003-11-21
                    
                    if( e_ok != TrError )
                    {
                        b06write_filename_and_root( curr_tree_id );
                    }
                }
                
                tbd_nodeptr &pNode = AuxNptrsRoot.np_ptr();
                tsp00_Int4 Offset0 = pNode->nd_pointer_list() [MAX_POINTERINDEX_BD00 - POS_OFF_DIFF_BD00] - POS_OFF_DIFF_BD00;
                tgg00_RecPtr pRec = REINTERPRET_CAST(tgg00_RecPtr,REINTERPRET_CAST(tsp00_BytePtr,pNode)+Offset0);
                
                if ( (pNode->nd_level() > LEAF_LEVEL_BD00)
                    &&
                    (pNode->nd_record_cnt() > 0)
                    &&
                    (pRec->recKeyLen_gg00() != 0) )
                {
                    TrError = e_invalid_index_structure;
                }
            }
            if ( ! bTreeIsRemoved ) /* PTS 1106088 UH 23-03-2000 */
            {
                b13w_release_node (AuxNptrsRoot, *this);
            }
            currRootUpdated_bd00 = false;
        }
        else
            b13r_release_node (AuxNptrsRoot, *this, lru_normal);
    }
    if( currFreeQueue_bd00.fqTop_bd00 > 1) 
        bd13FreePnosAfterUnlock( *this );
}

/*---------------------------------------------------------------------------*/

void
cbd300_InvCurrentBasis::bd300FreeRoot ()
{
    ROUTINE_DBG_MEO00 ("bd300FreeRoot");

    if (NULL != currRootNptrs_bd00.np_ptr())
    {
        tbd_node_ptrs AuxNptrsRoot = currRootNptrs_bd00;

        /* RootNode is not released with UnlockTree */
        --currLeafPageCountDelta_bd00;
        bd300UnlockTree (IS_REMOVED_BD300); /* PTS 1106088 UH 23-03-2000 */
        b13free_node (AuxNptrsRoot, *this);
    }
}

/*---------------------------------------------------------------------------*/

void
cbd300_InvCurrent::bd300InitInvCurrent
(tgg00_TransContext     &Trans,
 tgg00_FileId           &FileId,
 const tbd_node_request &RequestKind,
 const tgg00_MessType   &MessType,
 const tgg00_VtraceType &TraceObject,
 bool                    bIgnoreInvError)
{
    ROUTINE_DBG_MEO00 ("bd300InitInvCurrent");

    tgg00_BasisError  &TrError = Trans.trError_gg00;
    Msg_List          &errorList = reinterpret_cast<Trans_Context*>(&Trans)->ErrorList();

    bd300InitInvCurrentBasis (Trans, RequestKind, MessType, TraceObject, bIgnoreInvError);
    if (e_ok != TrError) return;

    FileDir_IDirectory &fdir = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_Generic     indexFile;

    switch ( fdir.GetFile(Trans.trTaskId_gg00, FileId.fileTabId_gg00(), indexFile, errorList) )
    {
        case FileDir_Okay:
            break;
        case FileDir_FileNoNotFound:
            TrError = e_inv_not_found;
            break;
        default:
            TrError = e_not_implemented;
    }

    if ( e_ok == TrError
         &&
         indexFile.GetFileType() != Data_IndexFile
         &&
         indexFile.GetFileType() != Data_TableFile
         &&
         indexFile.GetFileType() != Data_OmsKeyPartitionFile )
    {
        // TableFile is file type of temp inv files
        TrError = e_invalid_filetype;
        return;
    }
    
    if ( e_ok == TrError )
    {
        /* check file state */
        if (Data_FileNotAccessible == indexFile.GetFileState())
        {
            TrError = e_file_not_accessible;
            return;
        }
        else if (FileId.fileBdUse_gg00().includes (bd_write_acc))
        {
            if (Data_FileBad == indexFile.GetFileState())
            {
                TrError = e_bad_invfile;
                return;
            }
        }
        
        if ( indexFile.ArePagesClustered() )
            currExtendedFileTypeSet_bd00.addElement( eftsClustered_egg00 );
        else
            currExtendedFileTypeSet_bd00.delElement( eftsClustered_egg00 );
        
        tgg00_BdUseSet useSet = FileId.fileBdUse_gg00();
        indexFile.BuildFileId (FileId);
        FileId.fileBdUse_gg00() = useSet;
    }
    else if ( e_inv_not_found == TrError
              &&
              FileId.fileRoot_gg00() != NIL_PAGE_NO_GG00 )
    {    
        /* during create index there is no file directory entry yet,
         * but the root page of the file is known */
        TrError = e_ok;
        bd300FileIdRootCheck (FileId, TrError);
    }

    if (e_ok != TrError)
        return;

    /* assign FileId to the FileId in current */
    this->curr_tree_id = FileId;

    /* get pointer to root page of index tree */
    bd13GetNode (*this, FileId.fileRoot_gg00(), plmLock_ebd00, m_RequestKind, currRootNptrs_bd00);

    // PTS 1105704 TS 2000-02-23
    //if (e_ok != TrError)
    //  b06illegal_root_access (*this);
    // PTS 1105704
}

/*---------------------------------------------------------------------------*/

void
cbd300_InvCurrent::bd300SetFileNotAccessible( bool bSetErrorToNotAccessible )
{
    ROUTINE_DBG_MEO00 ("bd300SetFileNotAccessible");

    tgg11_BdRootTrace  RootTrace;
    tgg00_BasisError  &TrError = this->curr_trans->trError_gg00;

    RootTrace.bdrRoot_gg11   = this->curr_tree_id.fileRoot_gg00();
    RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
    RootTrace.bdrError_gg11  = TrError;
    Msg_List  errorList;

    Trace_CommonEntry entry( tgg00_Debug::fromConst(bd),
                      tgg00_VtraceType::fromConst(m_TraceObject),
                      REINTERPRET_CAST( tsp00_BytePtr, &RootTrace ), sizeof (RootTrace) );

    if (this->curr_tree_id.fileType_gg00().includes (ftsPerm_egg00))
    {
        tbd_file_state_set AuxFileState;
        AuxFileState.clear();
        const tgg00_BasisError    originalError = TrError;
        TrError = e_ok;

        FileDir_ISharedDirectory::Instance().IncrementBadIndexCount();

        AuxFileState.addElement (f_not_accessible);

        if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() )
            b06file_opmsg (csp3_index_not_restored, this->curr_tree_id);
        else
        {
            b06file_opmsg (csp3_bad_index, this->curr_tree_id);

            if( e_ok != originalError ) // write reason for corruption into knldiag.err
            {
                char msgText[40]       = "Reason \"";
                const int remainingLen = sizeof( msgText ) - strlen( msgText ) -1;

                const char*   errCodeText    = Kernel_BasisErrorToResultString( originalError );
                int           errCodeTextLen = strlen( errCodeText );

                if( errCodeTextLen > remainingLen ){
                    errCodeTextLen = remainingLen;
                }
                strncat( msgText, errCodeText, errCodeTextLen );
                strncat( msgText, "\"", 1);
                g01optextmsg (sp3p_knldiag, sp3m_error, csp3_bad_index, csp3_n_index, msgText );
            }
        }

        FileDir_Index indexFile;
        FileDir_ReturnCode retCode = 
            FileDir_IDirectory::SessionInstance (*(this->curr_trans)).GetIndexFile(
                this->curr_trans->trTaskId_gg00,
                this->curr_tree_id.fileTabId_gg00(), indexFile, errorList );
        if (indexFile.IsAssigned())
            indexFile.SetFileState (Data_FileNotAccessible);
    }
    if( bSetErrorToNotAccessible )
        TrError = e_file_not_accessible;
}

/*---------------------------------------------------------------------------*/

cbd300_InvCurrent::~cbd300_InvCurrent ()
{
    ROUTINE_DBG_MEO00 ("cbd300_~InvCurrent");

    bd300UnlockTree ();

    // PTS 1112332 UH 2001-10-30 Moved b06illegal_root_access() to cbd300_InvCurrent::bd300UnlockTree()
    // PTS 1105704 TS 2000-02-23
    //if (e_ok != curr_trans->trError_gg00)
    //  b06illegal_root_access (*this);
    // PTS 1105704

    if (0 != this->currCounter_bd00)
    {
        g01abort (BD300_OPEN_DATA_CACHE_ACCESSES_SP03, csp3_n_io,
                  "BD300CHECK: counter <> 0", this->currCounter_bd00);
    }
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if ( e_ok != curr_trans->trError_gg00)
        t01basis_error (bd_inv, "TrError     ", this->curr_trans->trError_gg00);
#   endif
}

/*---------------------------------------------------------------------------*/

void
cbd300_InvSubTreeCurrent::bd300SetSubRoot (cbd450_InvListRefSubTree &InvListRefSubTree,
        bool                      bUpdateable)
{
    ROUTINE_DBG_MEO00 ("bd300SetSubRoot");

    const tsp00_PageNo SubRoot        = InvListRefSubTree.bd450SubRoot();
    curr_tree_id.fileRoot_gg00()      = SubRoot;
    curr_tree_id.fileRootCheck_gg00() = ROOT_CHECK_GG00;

    /* get pointer to the Root page of the SubTree */
    bd13GetNode (*this, SubRoot, bd300_GetRootLockMode(), /* PTS 1108703 UH 2000-12-12 */
                 m_RequestKind, currRootNptrs_bd00);
    if (e_ok != this->curr_trans->trError_gg00) return;

    if ( bUpdateable )
    {
        /* store current NumLeaves */
        currNumLeaves_bd00 = currRootNptrs_bd00.np_ptr()->ndLeafCount_bd00();

#       if COMPILEMODE_MEO00 >= SLOW_MEO00
        t01p2int4 (bi, "OldLeafCount", InvListRefSubTree.bd450NumLeaves(), "NewLeafCount", currNumLeaves_bd00);
#       endif

        /* copy number of leaves from subroot to reference record */
        InvListRefSubTree.bd450SetNumLeaves (currNumLeaves_bd00);
    }
}

/*---------------------------------------------------------------------------*/

void
cbd300_InvSubTreeCurrent::bd300NewRoot (const tsp00_PageNo invroot) // PTS 1120695 UH 2003-10-27
{
    ROUTINE_DBG_MEO00 ("bd300NewRoot");

    tgg00_BasisError  &TrError = this->curr_trans->trError_gg00;
    tbd_nodeptr       &pNode   = currRootNptrs_bd00.np_ptr();

    curr_tree_id.fileRoot_gg00()      = NIL_PAGE_NO_GG00;
    curr_tree_id.fileRootCheck_gg00() = ROOT_CHECK_GG00;

    /* get pointer to the Root page of the SubTree */
    b13new_root (currRootNptrs_bd00, *this);
    if (e_ok != TrError) return;

    /* occupy Root in current */
    curr_tree_id.fileRoot_gg00() = pNode->nd_id();

	/* store root of invtree above in the root of the subtree for debugging purpose */
	pNode->ndInvRoot_bd00()     = invroot; // PTS 1120695 UH 2003-10-27
	pNode->ndPageVersion_bd00() = 1;
        

    currNumLeaves_bd00 = pNode->ndLeafCount_bd00();
}

/*---------------------------------------------------------------------------*/

cbd300_InvSubTreeCurrent::cbd300_InvSubTreeCurrent ( cbd300_InvCurrentBasis &InvCurrent,
                                                     bool                    bNewRoot)
: cbd300_InvCurrentBasis (*InvCurrent.curr_trans,
                           tbd_node_request::fromConst(nr_for_update),
                           InvCurrent.curr_action,
                           tgg00_VtraceType::fromConst(td_none),
                           ! IGNORE_INV_ERROR_BD300)
{
    ROUTINE_DBG_MEO00 ("cbd300_InvSubTreeCurrent_NEW");

    if (e_ok != this->curr_trans->trError_gg00) return;
    
    m_pMasterCurrent = &InvCurrent;

    if ( InvCurrent.currExtendedFileTypeSet_bd00.includes(eftsClustered_egg00) )
        this->currExtendedFileTypeSet_bd00.addElement(eftsClustered_egg00);

    /* create current to access index subtree */
    curr_tree_id                      = InvCurrent.curr_tree_id;
    curr_tree_id.fileTfn_gg00().becomes(tgg00_Tfn::fromConst(tfnInvSubtree_egg00));

    if ( bNewRoot )
        bd300NewRoot(InvCurrent.bd300GetRootId()); // PTS 1120695 UH 2003-10-27
	else
    {
		// PTS 1120695 UH 2003-10-27
        if ( currRootNptrs_bd00.np_ptr() != 0 )
            currRootNptrs_bd00.np_ptr()->ndInvRoot_bd00() = InvCurrent.bd300GetRootId();
    }
}

/*---------------------------------------------------------------------------*/

cbd300_InvSubTreeCurrent::~cbd300_InvSubTreeCurrent ()
{
    ROUTINE_DBG_MEO00 ("cbd300_~InvSubTreeCurrent");

    tbd_nodeptr &pNode = currRootNptrs_bd00.np_ptr();

    if (NULL == pNode) return;

    /* release SubTreeRoot */
    bd300UnlockTree ();

    if (0 != this->currCounter_bd00)
    {
        g01abort (BD300_OPEN_DATA_CACHE_ACCESSES_SP03, csp3_n_io,
                  "BD300CHECK: counter <> 0", this->currCounter_bd00);
    }
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    if ( e_ok != this->curr_trans->trError_gg00)
        t01basis_error (bd_inv, "TrError     ", this->curr_trans->trError_gg00);
#   endif
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/ 
