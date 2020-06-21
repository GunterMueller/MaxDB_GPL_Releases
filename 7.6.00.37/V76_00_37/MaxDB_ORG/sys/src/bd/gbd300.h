/*!
  @file           gbd300.h
  @ingroup        InvTree Handling
  @author         UweH
  @brief          Currents for Tree's

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


#ifndef GBD300_H
#define GBD300_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd05.h"
#include "gbd450.h"
#include "ConsistentRead/CnsRead_Manager.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

#define IGNORE_INV_ERROR_BD300     true
#define USE_LOCK_BD300             true
#define IS_REMOVED_BD300           true

/// base inv tree handler
class cbd300_InvCurrentBasis : public cbd05_Current
{
public:
	cbd300_InvCurrentBasis(){};
    cbd300_InvCurrentBasis (tgg00_TransContext     &Trans,
							const tbd_node_request &RequestKind,
		                    const tgg00_MessType   &MessType,
		                    const tgg00_VtraceType &TraceObject,
                            bool                    bIgnoreInvError);

    void  bd300InitInvCurrentBasis (tgg00_TransContext     &Trans,
									const tbd_node_request &RequestKind,
									const tgg00_MessType   &MessType,
									const tgg00_VtraceType &TraceObject,
									bool                    bIgnoreInvError);

    bool               bd300ConsistentReadSupport() const;
	void               bd300LockTree   ();
    bool               bd300IsLocked() const;
    bool               bd300IsSubTree() const;
    bool               bd300IsExclusiveLocked() const;
	virtual void       bd300UnlockTree (bool bTreeIsRemoved = false); /* PTS 1106088 UH 23-03-2000 */
    bool               bd300LockTreeExclusive ();

	void               bd300FreeRoot ();
	tsp00_PageNo       bd300GetRootId() const;
	tsp00_Int4         bd300GetRootLeafCount() const;
	tsp00_Int4         bd300GetRootLevel() const;
	void               bd300ReleaseRoot ();

	tbd_node_ptrs    & bd300RootPtrs();
    tbd_node_request & bd300RootRequestKind();

	void               bd300SetTraceObject(tgg00_VtraceType_Enum TraceObject);
	void               bd300SkipErrorHandling ();

    void               bd300UpdatePageCounter();
    void               bd300UpdateEntryCounter(SAPDB_Int4 counterDifference) const;
    void               bd300GetStatisticsOfFiledirectory(SAPDB_UInt8 &leafPageCount,
                                                         SAPDB_UInt8 &indexPageCount,
                                                         SAPDB_UInt8 &entryCount) const;
    bool               bd300ArePagesClustered() const;

	static bool        bd300_IsInvError (tgg00_BasisError BasisError);
    
protected:
    tbd00_PageLockMode_Enum bd300_GetRootLockMode(); /* PTS 1108703 UH 2000-12-12 new */

	tgg00_VtraceType_Enum   m_TraceObject;
    bool                    m_bIgnoreInvError;
    bool                    m_bSkipErrorHandling;
	tbd_node_request        m_RequestKind;
    bool                    m_ConsistentReadSupport;
    /// This current is the current of the master tree
    cbd300_InvCurrentBasis *m_pMasterCurrent;

friend
    class cbd300_InvSubTreeCurrent;
};

/*---------------------------------------------------------------------------*/

/// inv tree handler
class cbd300_InvCurrent : public cbd300_InvCurrentBasis 
{
public:

	cbd300_InvCurrent (){};
	
    cbd300_InvCurrent (tgg00_TransContext    &Trans,
					  tgg00_FileId           &FileId,
				   	  const tbd_node_request &RequestKind,
					  const tgg00_MessType   &MessType,
					  const tgg00_VtraceType &TraceObject,
                      bool                    bIgnoreInvError);

	~cbd300_InvCurrent ();

	virtual void bd300UnlockTree (bool bTreeIsRemoved = false); /* PTS 1106088 UH 23-03-2000 */
	
    void bd300UpdateLeafCount();
    
    void bd300InitInvCurrent(tgg00_TransContext    &Trans,
					         tgg00_FileId           &FileId,
				   	         const tbd_node_request &RequestKind,
					         const tgg00_MessType   &MessType,
					         const tgg00_VtraceType &TraceObject,
                             bool                    bIgnoreInvError);
  	
    void bd300SetFileNotAccessible( bool bSetErrorToNotAccessible = true );
};

/*---------------------------------------------------------------------------*/

/// inv subtree handler
class cbd300_InvSubTreeCurrent : public cbd300_InvCurrentBasis
{
public:
	// create a new file and assign current to it 
	cbd300_InvSubTreeCurrent (cbd300_InvCurrentBasis &InvCurrent,
                              bool                    bNewRoot = true);
	// assign current to an existing file
	cbd300_InvSubTreeCurrent (cbd300_InvCurrentBasis   &InvCurrent,
                              const tbd_node_request   &RequestKind,
		                      cbd450_InvListRefSubTree &InvListRefSubTree,
		                      bool                      bUpdateable);
                                    
	~cbd300_InvSubTreeCurrent ();

    void bd300NewRoot(const tsp00_PageNo invroot); // PTS 1120695 UH 2003-10-27

	void  bd300SetSubRoot (cbd450_InvListRefSubTree &InvListRefSubTree,
		                   bool                      bUpdateable);
};

/*---------------------------------------------------------------------------*/
inline bool
cbd300_InvCurrentBasis::bd300IsSubTree() const
{
    return currRootNptrs_bd00.np_ptr() != 0
           &&
           pt2InvSubtree_egg00 == currRootNptrs_bd00.np_ptr()->nd_pt2();
}

/*---------------------------------------------------------------------------*/
inline bool
cbd300_InvCurrentBasis::bd300IsLocked() const
{
    return currRootNptrs_bd00.np_ptr() != 0;
}

/*---------------------------------------------------------------------------*/
inline bool
cbd300_InvCurrentBasis::bd300IsExclusiveLocked() const
{
    return bd300IsLocked() && nr_for_update == m_RequestKind;
}

/*---------------------------------------------------------------------------*/
inline bool
cbd300_InvCurrentBasis::bd300ArePagesClustered() const
{
    return currExtendedFileTypeSet_bd00.includes( eftsClustered_egg00 );
}

/*---------------------------------------------------------------------------*/

inline void 
cbd300_InvCurrentBasis::bd300InitInvCurrentBasis (tgg00_TransContext     &Trans,
												  const tbd_node_request &RequestKind,
												  const tgg00_MessType   &MessType,
												  const tgg00_VtraceType &TraceObject,
												  bool                    bIgnoreInvError)
{
    ROUTINE_DBG_MEO00 ("cbd300_InvCurrentBasis");

    bd05SetCurrent (Trans, MessType);

	m_RequestKind           = RequestKind;
	m_TraceObject           = TraceObject;
    m_bIgnoreInvError       = bIgnoreInvError;
	m_bSkipErrorHandling    = false;
    m_ConsistentReadSupport = CnsRead_Manager::IsConsistentRead_Configurated();
    m_pMasterCurrent        = 0;

#	if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01int4 (bi, "RequestKind ", m_RequestKind);
	t01bool (bi, "IgnoreInvErr", m_bIgnoreInvError);
	t01bool (bi, "SkipErrorHan", m_bSkipErrorHandling);
#	endif
}

/*---------------------------------------------------------------------------*/

inline 
cbd300_InvCurrentBasis::cbd300_InvCurrentBasis (tgg00_TransContext     &Trans,
											    const tbd_node_request &RequestKind,
		                                        const tgg00_MessType   &MessType,
		                                        const tgg00_VtraceType &TraceObject,
                                                bool                    bIgnoreInvError)
{
    bd300InitInvCurrentBasis (Trans, RequestKind, MessType, TraceObject, bIgnoreInvError);
}


/*---------------------------------------------------------------------------*/

inline bool
cbd300_InvCurrentBasis::bd300ConsistentReadSupport() const
{
    return m_ConsistentReadSupport;
}

/*---------------------------------------------------------------------------*/

inline 	tsp00_PageNo
cbd300_InvCurrentBasis::bd300GetRootId() const
{
    ROUTINE_DBG_MEO00 ("bd300GetRootId");

	if (NULL == currRootNptrs_bd00.np_ptr())
		return NIL_PAGE_NO_GG00;
	else
		return currRootNptrs_bd00.np_ptr()->nd_id();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd300_InvCurrentBasis::bd300GetRootLeafCount() const
{
    ROUTINE_DBG_MEO00 ("bd300GetRootLeafCount");

	if (NULL == currRootNptrs_bd00.np_ptr())
		return -1;
	else
		return currRootNptrs_bd00.np_ptr()->ndLeafCount_bd00();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd300_InvCurrentBasis::bd300GetRootLevel() const
{
    ROUTINE_DBG_MEO00 ("bd300GetRootLevel");

	if (NULL == currRootNptrs_bd00.np_ptr())
		return -1;
	else
		return currRootNptrs_bd00.np_ptr()->nd_level();
}

/*---------------------------------------------------------------------------*/

inline void 
cbd300_InvCurrentBasis::bd300SetTraceObject(tgg00_VtraceType_Enum TraceObject)
{
	m_TraceObject = TraceObject;
}

/*---------------------------------------------------------------------------*/

inline void 
cbd300_InvCurrentBasis::bd300SkipErrorHandling ()
{
    ROUTINE_DBG_MEO00 ("cbd300_InvCurrentBasis::bd300SkipErrorHandling");
	m_bSkipErrorHandling = true;
}

/*---------------------------------------------------------------------------*/

inline tbd_node_ptrs &
cbd300_InvCurrentBasis::bd300RootPtrs()
{
	return this->currRootNptrs_bd00;
}


/*---------------------------------------------------------------------------*/

inline tbd_node_request &
cbd300_InvCurrentBasis::bd300RootRequestKind()
{
	return m_RequestKind;
}

/*---------------------------------------------------------------------------*/

inline tbd00_PageLockMode_Enum
cbd300_InvCurrentBasis::bd300_GetRootLockMode()
{
    /* PTS 1108703 UH 2000-12-12 new */
    if ( (nr_for_update           == m_RequestKind)
	     &&
	     !curr_tree_id.fileType_gg00().includes (ftsTemp_egg00)
	     &&
	     (m_create_index_parallel != curr_action)
	     &&
	     (m_create_index          != curr_action)
	     &&
	     (m_drop                  != curr_action) )
        return plmLockWithSVPSync_ebd00;
    else
        return plmLock_ebd00;

}

/*---------------------------------------------------------------------------*/

inline
cbd300_InvCurrent::cbd300_InvCurrent(tgg00_TransContext     &Trans,
									 tgg00_FileId           &FileId,
									 const tbd_node_request &RequestKind,
									 const tgg00_MessType   &MessType,
									 const tgg00_VtraceType &TraceObject,
                                     bool                    bIgnoreInvError)
{
	bd300InitInvCurrent(Trans, FileId, RequestKind, MessType, TraceObject,
        bIgnoreInvError);
	if ( e_ok == Trans.trError_gg00
         &&
         currRootNptrs_bd00.np_ptr() == 0 )
    {
        g01abort (csp3_bd_msg, csp3_n_btree, "BD300: FILEDIR ERROR    ", 0);
    }
}

/*---------------------------------------------------------------------------*/

inline bool cbd300_InvCurrentBasis::bd300_IsInvError (tgg00_BasisError BasisError)
{
	ROUTINE_DBG_MEO00 ("bd300_IsInvError");

	switch (BasisError)
	{
	case e_bad_datapage :
	case e_bad_file:
	case e_bad_invfile:
	case e_data_page_corrupted:
	case e_file_not_accessible:
	case e_illegal_branchlength:
	case e_illegal_entrylength:
	case e_illegal_entrypos:
	case e_illegal_keylength:
    case e_illegal_key:
	case e_illegal_page_no:
	case e_invalid_entrypos:
	case e_invalid_index_structure:
	case e_invalid_invlistpos:
	case e_invalid_leaves_structure:
	case e_invalid_root:
    case e_move_error:               /* PTS 1104792 UH 29-11-1999 */
	case e_no_converter_entry:
	case e_page_in_wrong_tree:
	case e_invalid_indexorder :
		return true;
	default:
		return false;
	}
}
 
/*---------------------------------------------------------------------------*/

inline void 
cbd300_InvCurrent::bd300UpdateLeafCount ()
{
    ROUTINE_DBG_MEO00 ("cbd300_InvCurrent::bd300UpdateLeafCount");
	if ( (e_ok == curr_trans->trError_gg00) && (NULL != currRootNptrs_bd00.np_ptr()) )
		currNumLeaves_bd00 = currRootNptrs_bd00.np_ptr()->ndLeafCount_bd00();
}


/*---------------------------------------------------------------------------*/

inline
cbd300_InvSubTreeCurrent::cbd300_InvSubTreeCurrent (cbd300_InvCurrentBasis   &InvCurrent,
												    const tbd_node_request   &RequestKind,
												    cbd450_InvListRefSubTree &InvListRefSubTree,
													bool                      bUpdateable)
: cbd300_InvCurrentBasis (*InvCurrent.curr_trans,
						 RequestKind,
						 InvCurrent.curr_action,
						 tgg00_VtraceType::fromConst(td_none),
						 ! IGNORE_INV_ERROR_BD300)
{
	ROUTINE_DBG_MEO00 ("cbd300_InvSubTreeCurrent_EXISTS");
    
    if (e_ok != this->curr_trans->trError_gg00) return;

    m_pMasterCurrent = &InvCurrent;

    if ( InvCurrent.currExtendedFileTypeSet_bd00.includes(eftsClustered_egg00) )
        this->currExtendedFileTypeSet_bd00.addElement(eftsClustered_egg00);

    /* create current to access index subtree */
	curr_tree_id = InvCurrent.curr_tree_id;
	curr_tree_id.fileTfn_gg00().becomes(tgg00_Tfn::fromConst(tfnInvSubtree_egg00));

    bd300SetSubRoot (InvListRefSubTree, bUpdateable);

    if (e_ok != this->curr_trans->trError_gg00) return; // PTS 1126249 UH 2003-12-12 added

	// PTS 1120695 UH 2003-10-27
	// write the root of the inv tree above into subroot for debugging purpose
	if ( nr_for_update == RequestKind
		 &&
		 currRootNptrs_bd00.np_ptr()->ndInvRoot_bd00() == 0 /*for migration*/ )
		currRootNptrs_bd00.np_ptr()->ndInvRoot_bd00() = InvCurrent.bd300GetRootId();
}

#endif  /* GBD300_H */
