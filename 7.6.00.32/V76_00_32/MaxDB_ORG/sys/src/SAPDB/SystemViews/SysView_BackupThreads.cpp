/****************************************************************************/
/*!

  @file        SysView_BackupThreads.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "BackupThreads" view class.

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

#define KERNEL_LZU
#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_BackupThreads.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "geo00_0.h"
#include "geo50_0.h"
#include "geo002.h"
#include "SAPDBCommon/SAPDB_sprintf.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

//===========
#if defined(_WIN32)
//===========
#define THREAD_ID_OF_DEVI(pCtrl_)  ((SAPDB_UInt4)(pCtrl_)->ThrdCtrlHeader.Tid)
#define LOCK_OF_DEVI(pCtrl_)       &(pCtrl_)->lExclusive
#define DEVI_IN_USE(pCtrl_)        ((pCtrl_)->ThrdCtrlHeader.ThrdState != KT_INITIALIZED )
#define NUMBER_OF_ASYNC_DEV        (kgs.ulNumOfAsyncis)
#define FIRST_ASYNC_DEV            (kgs.pFirstAsynciCtrl)
#define ASYNC_DEVICE_NAME(DevNo_)  &(kgs.pFirstAsynciCtrl[DevNo_].szLogDeviceName[0])
#define OPENED_FOR_WRITING(DevNo_) (kgs.pFirstAsynciCtrl[DevNo_].fForWriting != 0)
//===========
#else
//===========
typedef struct DEV_PCB            *PASYNCI_CTRL_REC;
#define THREAD_ID_OF_DEVI(pCtrl_)  ((SAPDB_UInt4)(pCtrl_)->tid)
#define LOCK_OF_DEVI(pCtrl_)       (pCtrl_)->alone
#define DEVI_IN_USE(pCtrl_)        ((pCtrl_)->state != KT_INITIALIZED )
#define NUMBER_OF_ASYNC_DEV        (kgs->maxdevspaces + kgs->maxtapes)
#define FIRST_ASYNC_DEV            (kgs->first_asdev)
#define ASYNC_DEVICE_NAME(DevNo_)  &(kgs->dev0.dev[kgs->maxdevspaces + (DevNo_)].devspace[0])
#define OPENED_FOR_WRITING(DevNo_) (kgs->dev0.dev[kgs->maxdevspaces + (DevNo_)].asyn_write)
//===========
#endif
//===========


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_BackupThreads::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackupThreads::Create", SysView_Trace, 5);

    // Column 1
	m_Table->AppendCol (ITOCT_FIXED ,SV_THREADID, 10);
    // Column 2
	m_Table->AppendCol (ITOCT_CHAR ,SV_THREADNAME, 16);
    // Column 3
	m_Table->AppendCol (ITOCT_CHAR ,SV_IODIRECTION, 6);
    // Column 4
	m_Table->AppendCol (ITOCT_FIXED ,SV_IOCOUNT, 20);
    // Column 5
	m_Table->AppendCol (ITOCT_FIXED ,SV_IOPAGECOUNT, 20);
    // Column 6
	m_Table->AppendCol (ITOCT_FIXED ,SV_IOTIME, 20);
    // Column 7
    m_Table->AppendCol (ITOCT_FIXED ,SV_IOTIMEMEASUREMENTCOUNT, 20);
    // Column 8
	m_Table->AppendCol (ITOCT_FIXED ,SV_PENDINGIOCOUNT, 5);
    // Column 9
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXPENDINGIOCOUNT, 5);
    // Column 10
	m_Table->AppendCol (ITOCT_CHAR ,SV_PATH, 256);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_BackupThreads::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackupThreads::GetColCount", SysView_Trace, 5);

    return SV_CC_BACKUPTHREADS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_BackupThreads::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackupThreads::EstimateRows", SysView_Trace, 5);

    return SV_ER_BACKUPTHREADS;
}

/*---------------------------------------------------------------------------*/

void SysView_BackupThreads::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackupThreads::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();

    if (SysView_SysViewBase::m_Context.IsOk())
    {
        for ( SAPDB_ULong DevNo = 0; DevNo < NUMBER_OF_ASYNC_DEV; ++DevNo )
        {
            PASYNCI_CTRL_REC pAsynciCtrl = FIRST_ASYNC_DEV + DevNo;

            if ( *ASYNC_DEVICE_NAME(DevNo) )
            {
                SAPDB_Bool forWriting = OPENED_FOR_WRITING(DevNo);

                teo52_IOStatistic &ioStat = ( forWriting
                            ? pAsynciCtrl->DevIOStat.DevWrite
                            : pAsynciCtrl->DevIOStat.DevRead );

                if ( ioStat.IOStat.TotalTimeStat.ulCount > 0 )
                {
                    SAPDB_Char workerThreadName[64];

                    SAPDB_sprintf(&workerThreadName[0], sizeof(workerThreadName), 
                        "BACKUP_%lu", DevNo );

                    // Column 1 SV_THREADID
	                AddColumn( THREAD_ID_OF_DEVI(pAsynciCtrl) );
                    // Column 2 SV_THREADNAME
	                AddColumn( &workerThreadName[0] );
                    // Column 3 SV_IODIRECTION
                    AddColumn( forWriting ? "WRITE" : "READ" );
                    // Column 4 SV_IOCOUNT
	                AddColumn( ioStat.IOStat.TotalTimeStat.ulCount );
                    // Column 5 SV_IOPAGECOUNT
	                AddColumn( ioStat.TotalPageCount );
                    // Column 6 SV_IOTIME
                    AddColumn( ioStat.IOStat.TotalTimeStat.ulAbsMicroSeconds );
                    // Column 7 SV_IOTIMEMEASUREMENTCOUNT
                    AddColumn( ioStat.IOStat.TotalTimeStat.ulMeasurements );
                    // Column 8 SV_PENDINGIOCOUNT
	                AddColumn( pAsynciCtrl->DevIOStat.ulIOQueLen );
                    // Column 9 SV_MAXPENDINGIOCOUNT
	                AddColumn( pAsynciCtrl->DevIOStat.TotalMaxIOQueLen );
                    // Column 10 SV_PATH
	                AddColumn( ASYNC_DEVICE_NAME(DevNo) );
                } /* if counter > 0 */
            } /* if DEVI_IN_USE */
        } /* loop over DevNo for async dev */
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
