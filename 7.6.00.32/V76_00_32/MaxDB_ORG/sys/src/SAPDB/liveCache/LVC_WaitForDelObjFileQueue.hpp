/****************************************************************************

  module      : LVC_WaitForDelObjFileQueue.hpp

  -------------------------------------------------------------------------

  responsible : d025251

  special area: 
  description : 


  last changed: 2001-10-16  13:18
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/


#ifndef LVC_WAITFORDELOBJFILEQUEUE_HPP
#define LVC_WAITFORDELOBJFILEQUEUE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include "heo55k.h"  // vbegexcl
#include "heo56.h"   // vresume
#include "heo56.h"   // vresume
#include "hgg08.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

# define NIL_ITEM_NO     -1
# define NIL_OBJ_FILE_NO SAPDB_MAX_UINT4

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
// PTS 1112228 AK 19/10/2001 
struct LVC_WaitItem
{
	tsp00_TaskId        m_TaskId;
	FileDir_FileNo      m_ObjFileNo;
	SAPDB_Int           m_NextItem; 

	LVC_WaitItem() 
		: m_NextItem(NIL_ITEM_NO)
        , m_TaskId(cgg_nil_pid)
	{
        m_ObjFileNo.Invalidate();
    };
};

/*---------------------------------------------------------------------------*/
// PTS 1112228 AK 19/10/2001 
class LVC_WaitForDelObjFileQueue
{
public:
	inline void Insert (tsp00_TaskId         TaskId,
	                    FileDir_FileNo &ObjFileNo);

    inline void WakeUpWaitingTasks(tsp00_TaskId    TaskId,
                                   FileDir_FileNo &ObjFileNo);

    inline SAPDB_Bool GetFirstWaitForDelEntry (tsp00_TaskId    &WaitingTaskId,
                                               FileDir_FileNo &ObjFileNo);

    inline SAPDB_Bool AnyObjFileToDelete();
    
    inline LVC_WaitForDelObjFileQueue  (SAPDBMem_IRawAllocator &Allocator,
        SAPDB_UInt              NumMaxTasks);  

private:
    inline SAPDB_Bool GetWaitingTaskId (tsp00_TaskId    &WaitingTaskId,
                                        FileDir_FileNo &ObjFileNo);

private:
SAPDB_UInt m_NumOccupiedItems; 
SAPDB_Int  m_FirstFreeItemNo;
SAPDB_Int  m_FirstOccupiedItemNo;
Container_Vector<LVC_WaitItem> m_WaitList;  
};

/*---------------------------------------------------------------------------*/

LVC_WaitForDelObjFileQueue::LVC_WaitForDelObjFileQueue (SAPDBMem_IRawAllocator &Allocator,
		                                                SAPDB_UInt              NumMaxTasks)
:
m_WaitList(Allocator),
m_NumOccupiedItems    (0),
m_FirstOccupiedItemNo (NIL_ITEM_NO),
m_FirstFreeItemNo     (0) 
{
	m_WaitList.Initialize(NumMaxTasks);
	for (SAPDB_UInt i=0; i<NumMaxTasks-1; ++i)
		m_WaitList[i].m_NextItem = (i+1);
}

/*---------------------------------------------------------------------------*/
														
inline void 
LVC_WaitForDelObjFileQueue::Insert (tsp00_TaskId         TaskId,
	                                FileDir_FileNo &ObjFileNo)
{

#   if COMPILEMODE_MEO00 >= QUICK_MEO00 
    g08excl_check (TaskId, g08garbage);
#   endif


    /* get number of free item */
	const SAPDB_Int CurrentItemNo = m_FirstFreeItemNo;

	/* check that tere is really an item free */
	SAPDBERR_ASSERT_STATE (m_NumOccupiedItems < m_WaitList.GetSize());
	SAPDBERR_ASSERT_STATE (CurrentItemNo != NIL_ITEM_NO);

	/* remove one item from the free item list */
	m_FirstFreeItemNo = m_WaitList[CurrentItemNo].m_NextItem;

	/* insert item into list of occupied items */
	m_NumOccupiedItems++;
    m_WaitList[CurrentItemNo].m_NextItem = m_FirstOccupiedItemNo;
    m_FirstOccupiedItemNo                = CurrentItemNo;

	/* set item content */
	m_WaitList[CurrentItemNo].m_TaskId    = TaskId;
	m_WaitList[CurrentItemNo].m_ObjFileNo = ObjFileNo;

}

/*---------------------------------------------------------------------------*/

inline void 
LVC_WaitForDelObjFileQueue::WakeUpWaitingTasks (tsp00_TaskId    TaskId,
	                                            FileDir_FileNo &ObjFileNo)
{
#   if COMPILEMODE_MEO00 >= QUICK_MEO00 
    g08excl_check (TaskId, g08garbage);
#   endif

	tsp00_TaskId WaitingTaskId;
	
	while (GetWaitingTaskId (WaitingTaskId, ObjFileNo))
	{
		vendexcl (TaskId, g08garbage);
		//=========================================
		vresume (WaitingTaskId);
		//=========================================
		vbegexcl (TaskId, g08garbage);
	}
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool 
LVC_WaitForDelObjFileQueue::GetFirstWaitForDelEntry (tsp00_TaskId    &WaitingTaskId,
	                                                 FileDir_FileNo &ObjFileNo)
{
	SAPDB_Int *pNextItem = &m_FirstOccupiedItemNo;
	
    if (*pNextItem != NIL_ITEM_NO)
    {
        SAPDB_Int WaitingItem  = *pNextItem;
        WaitingTaskId          = m_WaitList[WaitingItem].m_TaskId;
        ObjFileNo              = m_WaitList[WaitingItem].m_ObjFileNo; 

        /* remove item from list of occupied items */
        *pNextItem = m_WaitList[WaitingItem].m_NextItem;
        m_NumOccupiedItems--;

        /* insert item into chain of free items */
        m_WaitList[WaitingItem].m_NextItem  = m_FirstFreeItemNo;
        m_FirstFreeItemNo                   =  WaitingItem; 

        /* reset item values to nil */
        m_WaitList[WaitingItem].m_TaskId    = cgg_nil_pid;
        m_WaitList[WaitingItem].m_ObjFileNo.Invalidate();
        return true;
    }
	return false;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool 
LVC_WaitForDelObjFileQueue::GetWaitingTaskId (tsp00_TaskId    &WaitingTaskId,
                                              FileDir_FileNo &ObjFileNo)
{
	SAPDB_Int *pNextItem = &m_FirstOccupiedItemNo;
	
	while (*pNextItem != NIL_ITEM_NO)
	{
		if (m_WaitList[*pNextItem].m_ObjFileNo != ObjFileNo)
			pNextItem = &(m_WaitList[*pNextItem].m_NextItem);
		else
		{
			SAPDB_Int WaitingItem  = *pNextItem;
			WaitingTaskId          = m_WaitList[WaitingItem].m_TaskId;
			
			/* remove item from list of occupied items */
			*pNextItem = m_WaitList[WaitingItem].m_NextItem;
			m_NumOccupiedItems--;
			
			/* insert item into chain of free items */
			m_WaitList[WaitingItem].m_NextItem  = m_FirstFreeItemNo;
			m_FirstFreeItemNo                   =  WaitingItem; 

            /* reset item values to nil */
			m_WaitList[WaitingItem].m_TaskId    = cgg_nil_pid;
            m_WaitList[WaitingItem].m_ObjFileNo.Invalidate();
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool
LVC_WaitForDelObjFileQueue::AnyObjFileToDelete()
{
    return m_FirstOccupiedItemNo != NIL_ITEM_NO;
}

/*---------------------------------------------------------------------------*/

#endif  /* LVC_WAITFORDELOBJFILEQUEUE_HPP */
