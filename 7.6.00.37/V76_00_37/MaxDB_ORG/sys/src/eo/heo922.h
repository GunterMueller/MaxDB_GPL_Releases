/*!
  @file           heo922.h
  @author         JoergM
  @special area   x_cons
  @brief          internal sysmon / console
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



#ifndef HEO922_H
#define HEO922_H

#include "gsp92.h"

externC tsp00_Bool vGetRteInfoRegionNext ( tsp92_RteRegionInfo *RteRegionInfo );

externC tsp00_Bool vGetRteInfoRegionFirst ( tsp92_RteRegionInfo *RteRegionInfo );

externC tsp00_Bool vGetRteInfoIONext ( tsp92_RteIOInfo *RteIOInfo );

externC tsp00_Bool vGetRteInfoIOFirst ( tsp92_RteIOInfo *RteIOInfo );

externC tsp00_Bool vGetRteInfoBackupNext ( tsp92_RteBackupInfo *RteBackupInfo );

externC tsp00_Bool vGetRteInfoStorageNext ( tsp92_RteStorageInfo *RteStorageInfo );

externC tsp00_Bool vGetRteInfoStorageFirst ( tsp92_RteStorageInfo *RteStorageInfo );

externC tsp00_Bool vGetRteInfoSpecThreadsNext ( tsp92_RteSpecThreadsInfo *RteSpecThreadsInfo );

externC tsp00_Bool vGetRteInfoBackupFirst ( tsp92_RteBackupInfo *RteBackupInfo );

externC tsp00_Bool vGetRteInfoSpecThreadsFirst ( tsp92_RteSpecThreadsInfo *RteSpecThreadsInfo );

externC tsp00_Bool vGetRteInfoUKTNext ( tsp92_RteUKTInfo *RteUKTInfo  );

externC tsp00_Bool vGetRteInfoUKTFirst ( tsp92_RteUKTInfo *RteUKTInfo );

externC tsp00_Bool vGetRteInfoIODetailsNext ( tsp92_RteIODetailsInfo *RteIODetailsInfo );

externC tsp00_Bool vGetRteInfoIODetailsFirst ( tsp92_RteIODetailsInfo *RteIODetailsInfo );

externC tsp00_Bool vGetRteInfoTaskNext ( tsp92_RteTaskInfo *RteTaskInfo );

externC tsp00_Bool vGetRteInfoTaskFirst ( tsp92_RteTaskInfo                 *RteTaskInfo,
                                          tsp92_RteTaskInfoStateToShow_Enum  StateToShow,
                                          tsp92_RteTaskInfoTypeToShow_Enum   TypeToShow );

externC void vGetRteInfoTaskDetails ( tsp00_TaskId taskid, tsp92_RteTaskDetailsInfo *RteTaskDetailsInfo );

externC tsp00_Bool vGetRteInfoTaskDetailsNext ( tsp92_RteTaskDetailsInfo  *RteTaskDetailsInfo );

externC tsp00_Bool vGetRteInfoTaskDetailsFirst ( tsp92_RteTaskDetailsInfo  *RteTaskDetailsInfo );

externC void vGetRteInfoTotalCnt ( tsp92_RteTotalCntInfo *RteTotalCntInfo ) ;

externC tsp00_Bool vGetRteInfoAllocatorFirst ( tsp92_RteAllocatorInfo *RteAllocatorInfo );

externC tsp00_Bool vGetRteInfoAllocatorNext ( tsp92_RteAllocatorInfo *RteAllocatorInfo );

externC tsp00_Bool vGetRteInfoSpinlockFirst ( tsp92_RteSpinlockInfo *RteSpinlockInfo );

externC tsp00_Bool vGetRteInfoSpinlockNext ( tsp92_RteSpinlockInfo *RteSpinlockInfo );

externC tsp00_Bool vGetRteInfoConnectionFirst ( tsp92_RteConnectionInfo *RteConnectionInfo );

externC tsp00_Bool vGetRteInfoConnectionNext ( tsp92_RteConnectionInfo *RteConnectionInfo );

#endif