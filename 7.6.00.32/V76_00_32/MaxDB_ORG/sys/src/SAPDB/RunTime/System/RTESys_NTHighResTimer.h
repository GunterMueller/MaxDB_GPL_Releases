/*!---------------------------------------------------------------------
  @file           RTESys_NTHighResTimer.h
  @author         RaymondR
  @brief          High Resolution Timer

              A counter is a general term used in programming to refer to an incrementing 
              variable. Some systems include a high-resolution performance counter that 
              provides high-resolution elapsed times.
  @see            


                If you need a timer with higher precision, use the high-resolution timer.
                For more information, see the class RTESys_NTHighResTimer below

\if EMIT_LICENCE

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



\endif
---------------------------------------------------------------------*/



#ifndef RTESYS_NTHIGHRESTIMER_H
#define RTESYS_NTHIGHRESTIMER_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"
#include "RunTime/RTE_CompilerFeatures.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! @name extern "C" Functions */
 /*@{*/
/* for function descritpions refer to class RTEMem_AWEAllocator
*/
/*!--------------------------------------------------------------------
   @description    C functions used to access high resolution counter functionality in 
              pascal or c modules.


                For function descritpions refer to class RTESys_NTHighResTimer.

 --------------------------------------------------------------------*/

typedef               enum { cCPUTimer, cQueryPerfCntr, cTickCount } cTimerType;

externC bool           RTESys_NTHRTimerInitializeAsync    ( cTimerType  Timer );
externC bool           RTESys_NTHRTimerInitializeAsyncWait();
externC SAPDB_UInt8    RTESys_NTHRTimerCounter            ();

externC SAPDB_UInt8    RTESys_NTHRTimerTimeDiffNanoSec    ( SAPDB_UInt8 StartCounter,
                                                            SAPDB_UInt8 StopCounter,
                                                            bool        CallTimeAdjusted );

externC SAPDB_UInt8    RTESys_NTHRTimerTimeDiffMicroSec   ( SAPDB_UInt8 StartCounter,
                                                            SAPDB_UInt8 StopCounter,
                                                            bool        CallTimeAdjusted );
    
externC SAPDB_UInt8    RTESys_NTHRTimerTimeDiffMilliSec   ( SAPDB_UInt8 StartCounter,
                                                            SAPDB_UInt8 StopCounter,
                                                            bool        CallTimeAdjusted );

externC SAPDB_UInt8    RTESys_NTHRTimerTimeDiffSec      ( SAPDB_UInt8 StartCounter,
                                                          SAPDB_UInt8 StopCounter,
                                                          bool        CallTimeAdjusted );

externC void           RTESys_NTHRTimerTimeDiffSecAndMilliSec ( SAPDB_UInt8 StartCounter,
                                                                SAPDB_UInt8 StopCounter,
                                                                SAPDB_UInt4 *Sec,
                                                                SAPDB_UInt4 *MilliSec,
                                                                bool        CallTimeAdjusted );
externC void           RTESys_NTHRTimerTimeDiffSecAndMicroSec ( SAPDB_UInt8 StartCounter,
                                                                SAPDB_UInt8 StopCounter,
                                                                SAPDB_UInt4 *Sec,
                                                                SAPDB_UInt4 *MicroSec,
                                                                bool        CallTimeAdjusted );

externC void           RTESys_NTHRTimerCounterToSecAndMilliSec( SAPDB_UInt8 Counter,
                                                                SAPDB_UInt4 *Sec,
                                                                SAPDB_UInt4 *MilliSec );

externC void           RTESys_NTHRTimerCounterToSecAndMicroSec( SAPDB_UInt8 Counter,
                                                                SAPDB_UInt4 *Sec,
                                                                SAPDB_UInt4 *MicroSec );

externC void           RTESys_NTHRTimerSecAndMilliSec         ( SAPDB_UInt4 *Sec,
                                                                SAPDB_UInt4 *MilliSec );

externC void           RTESys_NTHRTimerSecAndMicroSec         ( SAPDB_UInt4 *Sec,
                                                                SAPDB_UInt4 *MicroSec );

externC SAPDB_UInt8    RTESys_NTHRTimerMicroSeconds();


/*@}*/

#endif  /* RTESYS_NTHIGHRESTIMER_H */
