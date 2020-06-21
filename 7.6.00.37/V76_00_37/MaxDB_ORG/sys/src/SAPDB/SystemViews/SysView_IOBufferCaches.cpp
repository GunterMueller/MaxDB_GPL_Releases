/****************************************************************************/
/*!

  @file        SysView_IOBufferCaches.cpp

  -------------------------------------------------------------------------

  @author      Henrik

  @ingroup     SystemViews
  @brief       This module implements the "IOBUFFERCACHES" view class.

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
#include "SystemViews/SysView_IOBufferCaches.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"

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

void SysView_IOBufferCaches::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOBufferCaches::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED ,"TOTALSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"DATACACHEUSEDSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"CONVERTERUSEDSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"LOGQUEUEUSEDSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"IOMANAGEMENTUSEDSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"FILEDIRECTORYUSEDSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"RESTARTRECORDUSEDSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"BLOCKALLOCATORUSEDSIZE", 20);
	m_Table->AppendCol (ITOCT_FIXED ,"UNUSEDSIZE", 20);
    m_Table->AppendCol (ITOCT_FIXED ,"INDEXCREATIONUSEDSIZE", 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_IOBufferCaches::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOBufferCaches::GetColCount", SysView_Trace, 5);

    return 10;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_IOBufferCaches::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOBufferCaches::EstimateRows", SysView_Trace, 5);

    return SV_ER_IOBUFFERCACHES;
}

/*---------------------------------------------------------------------------*/

void SysView_IOBufferCaches::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOBufferCaches::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();

    FrameCtrl_ICommon &frameCtrl = FrameCtrl_ICommon::GetInstance();

    const SAPDB_UInt pageSize    = frameCtrl.GetPageSize() / 1024;
    const SAPDB_UInt total       = frameCtrl.GetCacheSize() * pageSize;
    const SAPDB_UInt dataCache   = frameCtrl.GetDataCacheSize() * pageSize;
    const SAPDB_UInt converter   = frameCtrl.GetConverterSize() * pageSize;
    const SAPDB_UInt logQueue    = frameCtrl.GetLogQueueSize() * pageSize;
    const SAPDB_UInt ioManager   = frameCtrl.GetIOManagementSize() * pageSize;
    const SAPDB_UInt fileDir     = frameCtrl.GetFileDirSize() * pageSize;
    const SAPDB_UInt restartRec  = frameCtrl.GetRestartRecordSize() * pageSize;
    const SAPDB_UInt blockAlloc  = frameCtrl.GetBlockAllocatorSize() * pageSize;
    const SAPDB_UInt unused      = frameCtrl.GetFreeFrameCount() * pageSize;
    const SAPDB_UInt createIndex = frameCtrl.GetCreateIndexSize() * pageSize;

    m_Table->MoveToCol( ITOVT_UINT4, &total, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &dataCache, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &converter, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &logQueue, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &ioManager, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &fileDir, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &restartRec, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &blockAlloc, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &unused, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &createIndex, 0 );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_IOBufferCaches::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOBufferCaches::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
