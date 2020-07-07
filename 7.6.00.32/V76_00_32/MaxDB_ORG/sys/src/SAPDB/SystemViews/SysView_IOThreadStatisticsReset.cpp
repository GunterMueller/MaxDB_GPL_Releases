/****************************************************************************/
/*!

  @file        SysView_IOThreadStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      JoergM

  @ingroup     SystemViews
  @brief       This module implements the "IOThreadStatisticsReset" view class.

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
#include "SystemViews/SysView_IOThreadStatisticsReset.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "geo00_0.h"
#include "geo50_0.h"
#include "geo002.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/RTE_KGS.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

//===========
#if defined(_WIN32)
//===========
#define DEVSP_NAME(DevNo_)         (kgs.Dev0.pIOCtrl[DevNo_].szDevspace)
#define IS_DEVICE_ATTACHED(DevNo_) ( kgs.Dev0.pIOCtrl[DevNo_].ulAttached != 0 \
                                  && kgs.Dev0.pIOCtrl[DevNo_].ulDetachPending == 0 )

#define FIRST_DEV                (kgs.pFirstDeviCtrl)
#define FIRST_UKT                (kgs.pFirstUKTCtrl)
#define LAST_UKT                 (kgs.pLastUKTCtrl)
#define DEVI_IN_USE(_pDeviCtrl)  (_pDeviCtrl->ThrdCtrlHeader.ThrdState != KT_INITIALIZED )
#define VOLUME_TYPE_OF_DEVICE(DevNo_) (kgs.Dev0.pIOCtrl[DevNo_].DevspaceType)
//===========
#else
//===========
#define DEVSP_NAME(DevNo_) (KGS->dev0.dev[DevNo_].devspace)

#define IS_DEVICE_ATTACHED(DevNo_) ( KGS->dev0.dev[DevNo_].devspace[0] != 0 \
                                  && KGS->dev0.dev[DevNo_].ulDetachPending == 0 )
typedef struct DEV_PCB      *PDEVI_CTRL_REC ;
#define FIRST_DEV                (KGS->first_dev)
#define FIRST_UKT                (kgs->first_kp)
#define LAST_UKT                 (kgs->last_kp)
#define DEVI_IN_USE(_pDeviCtrl)  (_pDeviCtrl->state != KT_INITIALIZED )
#define PUKT_CTRL_REC struct ten50_UKT_Control *
#define VOLUME_TYPE_OF_DEVICE(DevNo_) ( KGS->dev0.dev[DevNo_].devtype )
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

void SysView_IOThreadStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOThreadStatisticsReset::Create", SysView_Trace, 5);

    // Column 0
	m_Table->AppendCol (ITOCT_TIMESTAMP ,SV_RESETDATE, 0);
    // Column 1
	m_Table->AppendCol (ITOCT_FIXED ,SV_VOLUMEID, 10);
    // Column 2
	m_Table->AppendCol (ITOCT_FIXED ,SV_READCOUNT, 20);
    // Column 3
	m_Table->AppendCol (ITOCT_FIXED ,SV_READPAGECOUNT, 20);
    // Column 4
	m_Table->AppendCol (ITOCT_FIXED ,SV_READTIME, 20);
    // Column 5
    m_Table->AppendCol (ITOCT_FIXED ,SV_READTIMEMEASUREMENTCOUNT, 20);
    // Column 6
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITECOUNT, 20);
    // Column 7
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITEPAGECOUNT, 20);
    // Column 8
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITETIME, 20);
    // Column 9
    m_Table->AppendCol (ITOCT_FIXED ,SV_WRITETIMEMEASUREMENTCOUNT, 20);
    // Column 10
	m_Table->AppendCol (ITOCT_FIXED ,SV_PENDINGIOCOUNT, 5);
    // Column 11
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXPENDINGIOCOUNT, 5);
    // Column 12
	m_Table->AppendCol (ITOCT_CHAR ,SV_PATH, 256);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_IOThreadStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOThreadStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_IOTHREADSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_IOThreadStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOThreadStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_IOTHREADSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_IOThreadStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IOThreadStatisticsReset::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();

    if (SysView_SysViewBase::m_Context.IsOk())
    {
        SAPDB_UInt4 DevNo;
        for ( DevNo = 0; DevNo < (SAPDB_UInt4)XPARAM(lMaxDevspaces); ++DevNo )
        {
            if ( IS_DEVICE_ATTACHED(DevNo) )
            {
                struct volumeStatistic
                {
                    teo52_AbsoluteTime readTiming;
                    SAPDB_UInt8        readPageCount;
                    teo52_AbsoluteTime writeTiming;
                    SAPDB_UInt8        writePageCount;
                    SAPDB_UInt4        pendingIOQueueLen;
                    SAPDB_UInt4        maxIOQueueLen;
                } deviceRecord;
                memset(&deviceRecord, 0, sizeof(deviceRecord));

                PDEVI_CTRL_REC pDeviCtrl;

                for ( SAPDB_UInt4 ulDevi = 0; ulDevi < (SAPDB_UInt4) XPARAM(lDeviPerDevspace); ulDevi++ )
                {
                    pDeviCtrl = FIRST_DEV +
                                DevNo * XPARAM(lDeviPerDevspace) + ulDevi;

                    if (   DEVI_IN_USE(pDeviCtrl) 
                       && !pDeviCtrl->DevIOStat.ResetPending
                       && ( pDeviCtrl->DevIOStat.DevRead.IOStat.TimeStat.ulCount > 0 
                         || pDeviCtrl->DevIOStat.DevWrite.IOStat.TimeStat.ulCount > 0 ) )
                    {
                        deviceRecord.readTiming.ulCount = 
                            pDeviCtrl->DevIOStat.DevRead.IOStat.TimeStat.ulCount;
                        deviceRecord.readPageCount = 
                            pDeviCtrl->DevIOStat.DevRead.ulPageCount;
                        deviceRecord.readTiming.ulAbsMicroSeconds =
                            pDeviCtrl->DevIOStat.DevRead.IOStat.TimeStat.ulAbsMicroSeconds;
                        deviceRecord.readTiming.ulMeasurements = 
                            pDeviCtrl->DevIOStat.DevRead.IOStat.TimeStat.ulMeasurements;
                        deviceRecord.writeTiming.ulCount = 
                            pDeviCtrl->DevIOStat.DevWrite.IOStat.TimeStat.ulCount;
                        deviceRecord.writePageCount = 
                            pDeviCtrl->DevIOStat.DevWrite.ulPageCount;
                        deviceRecord.writeTiming.ulAbsMicroSeconds =
                            pDeviCtrl->DevIOStat.DevWrite.IOStat.TimeStat.ulAbsMicroSeconds;
                        deviceRecord.writeTiming.ulMeasurements = 
                            pDeviCtrl->DevIOStat.DevWrite.IOStat.TimeStat.ulMeasurements;
                        deviceRecord.pendingIOQueueLen =
                            pDeviCtrl->DevIOStat.ulIOQueLen;
                        deviceRecord.maxIOQueueLen =
                            pDeviCtrl->DevIOStat.ulMaxIOQueLen;

                        // Column 0 SV_RESETDATE
                        AddColumn( RTE_KGS::Instance().GetLastIOThreadStatisticsReset(), AsTimestamp );        
                        // Column 1 SV_VOLUMEID
	                    AddColumn( (SAPDB_UInt4)(DevNo * XPARAM(lDeviPerDevspace) + ulDevi) );
                        // Column 2 SV_READCOUNT
	                    AddColumn( deviceRecord.readTiming.ulCount );
                        // Column 3 SV_READPAGECOUNT
	                    AddColumn( deviceRecord.readPageCount );
                        // Column 4 SV_READTIME
                        AddColumn( deviceRecord.readTiming.ulAbsMicroSeconds );
                        // Column 5 SV_READTIMEMEASUREMENTCOUNT
                        AddColumn( deviceRecord.readTiming.ulMeasurements );
                        // Column 6 SV_WRITECOUNT
	                    AddColumn( deviceRecord.writeTiming.ulCount );
                        // Column 7 SV_WRITEPAGECOUNT
	                    AddColumn( deviceRecord.writePageCount );
                        // Column 8 SV_WRITETIME
                        AddColumn( deviceRecord.writeTiming.ulAbsMicroSeconds );
                        // Column 9 SV_WRITETIMEMEASUREMENTCOUNT
                        AddColumn( deviceRecord.writeTiming.ulMeasurements );
                        // Column 10 SV_PENDINGIOCOUNT
	                    AddColumn( deviceRecord.pendingIOQueueLen );
                        // Column 11 SV_MAXPENDINGIOCOUNT
	                    AddColumn( deviceRecord.maxIOQueueLen );
                        // Column 12 SV_PATH
	                    AddColumn( (SAPDB_Char *)&(DEVSP_NAME(DevNo)[0]) );
                    }
                }

            } /* if IS_DEVICE_ATTACHED */
        } /* loop over DevNo */
    }
    return;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
