/****************************************************************************/
/*!

  @file        SysView_ObjectLocks.cpp

  -------------------------------------------------------------------------

  @author      ThomasA

  @ingroup     SystemViews
  @brief       This module implements the "OBJECTLOCKS" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include "ggg00.h"
#include "hkb52.h"
#include "hkb501_1.h"
#include "hgg06.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_ObjectLocks.hpp"
#include "SQLManager/SQLMan_Context.hpp"


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
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_ObjectLocks::Create()
{

	m_Table->AppendCol (ITOCT_CHAR , SV_OBJECTID,        25);
	m_Table->AppendCol (ITOCT_FIXED, SV_HOLDERTASKID,    10);
	m_Table->AppendCol (ITOCT_FIXED, SV_REQUESTORTASKID, 10);
	m_Table->AppendCol (ITOCT_CHAR , SV_LOCKMODE,        10);
	m_Table->AppendCol (ITOCT_FIXED, SV_FILEID, 20);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ObjectLocks::GetColCount()
{
	return SV_CC_OBJECTLOCKS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ObjectLocks::EstimateRows()
{
	return SV_ER_OBJECTLOCKS;
}

/*---------------------------------------------------------------------------*/

void SysView_ObjectLocks::Execute()
{
    m_Table->GetCatalogTable();

    if(m_Context.IsOk())
    {
        tgg00_TransIndex entryNumber   = 0;
        tgg00_TransIndex colEntryIndex = cgg_nil_transindex;
        tgg00_TransIndex reqEntryIndex = cgg_nil_transindex;
        FileDir_FileNo   fileId;
        tgg92_KernelOid  oid;
        tgg91_TransNo    transactionId;
        tsp00_TaskId     taskidOfTrans;
        pasbool          isHolder;
        pasbool          isShareReq; /*PTS 1135178*/

		while (k52GetCollisionEntry (m_Context.GetTaskId(), entryNumber, colEntryIndex, reqEntryIndex,
			*reinterpret_cast<tsp00_C8*>(&fileId), 
			oid, transactionId, taskidOfTrans, isHolder, isShareReq) && m_Context.IsOk())
        {
            int        lineLen = 0;
            tsp00_Line line;
            gg06OidToLine (oid, lineLen, line);
            AddColumn((SAPDB_Char*)&line[0], lineLen);
            if (isHolder)
            {
                AddColumn(taskidOfTrans);
                AddColumn();
            }
            else
            {
                AddColumn();
                AddColumn(taskidOfTrans);
            }
            if (isShareReq) /*PTS 1135178*/
                AddColumn ("SHARE");
            else
                AddColumn ("EXCLUSIVE");
            AddColumn (fileId.GetUInt8());
        }

        pasbool     isExclusive;
        tsp00_Addr  contextAddr = NULL;
        while (k501GetShareLockInfo (m_Context.TransContext(), contextAddr, 
            fileId, oid, transactionId, taskidOfTrans, isExclusive, isHolder) && m_Context.IsOk())
        {
            int        lineLen = 0;
            tsp00_Line line;
            gg06OidToLine (oid, lineLen, line);
            AddColumn((SAPDB_Char*)&line[0], lineLen);
            if (isHolder)
            {
                AddColumn(taskidOfTrans);
                AddColumn();
            }
            else
            {
                AddColumn();
                AddColumn(taskidOfTrans);
            }
            if (isExclusive)    
			AddColumn ("EXCLUSIVE");
            else
			AddColumn ("SHARE");
            AddColumn (fileId.GetUInt8());
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
