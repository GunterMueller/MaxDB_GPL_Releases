/*!---------------------------------------------------------------------
  @file           RTESys_NTHighResTimer.hpp
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



#ifndef RTESYS_NTHIGHRESTIMER_HPP
#define RTESYS_NTHIGHRESTIMER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define _WIN32_WINNT  0x0500
#include <windows.h>
#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/SAPDB_Singleton.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!--------------------------------------------------------------------
  @class          RTESys_NTHighResTimer
 --------------------------------------------------------------------*/

class RTESys_NTHighResTimer
{
public:
  typedef              enum { CPUTimer, QueryPerfCntr, TickCount } TimerType;

public:
  /*!------------------------------------------------------------------
     @description    Reference to single instance of RTESys_NTHighResTimer
     @return value   Reference to RTESys_NTHighResTimer instance

   ------------------------------------------------------------------*/

  static RTESys_NTHighResTimer & Instance();

  /*!------------------------------------------------------------------
     @description    Use the 'Initialize' methode first to initialize the instance and
                     to choose one of the high resolution counter types.
     @param          Timer          [in/out] - Type of high resolution timer
     @param          quickLeapCheck [in]     - Long or short timer leap check
                                              
     @return value   'False' if something went wrong.

   ------------------------------------------------------------------*/

  static bool          Initialize ( TimerType  &Timer,
                                    bool       quickLeapCheck = false );


  /*!------------------------------------------------------------------
     @description    Use the 'Initialize' methode first to initialize the instance and
                     to choose one of the high resolution counter types.
                     Asyncronous initialization is doing a long periode leap check. 
                     'InitializeAsyncWait' has to be called to wait for the end of 
                     initialization.

     @param          Timer        [in] - Type of high resolution timer
     @return value   'False' if something went wrong.

   ------------------------------------------------------------------*/

  static bool          InitializeAsync ( TimerType  Timer );

  /*!------------------------------------------------------------------
     @description    Use the 'InitializeAsyncWait' methode to wait for
                     the end of asynchronous initialization.
     @param          Timer        [out] - Type of high resolution timer
     @return value   'False' if something went wrong.

   ------------------------------------------------------------------*/

  static bool          InitializeAsyncWait ( TimerType  &Timer );

  /*!------------------------------------------------------------------
     @description    Returns the 'RTESys_NTHighResTimer' instance startup counter value.
     @return value   start counter value.

   ------------------------------------------------------------------*/

  SAPDB_UInt8          StartCount() { return m_StartCount; }

  /*!------------------------------------------------------------------
     @description    The 'Counter' methode retrieves the current value of the 
                high-resolution counter.
     @return value   Current value of the high-resolution counter.

   ------------------------------------------------------------------*/

  virtual SAPDB_UInt8  Counter()   = 0;

  /*!------------------------------------------------------------------
     @description    The 'Frequency' methode can be used to express the frequency, 
                in counts per second.
     @return value   Frequency value of the high-resolution counter.

   ------------------------------------------------------------------*/

  SAPDB_UInt8          Frequency()  { return m_Frequency; }

  /*!------------------------------------------------------------------
     @description    If the application calls 'Counter' immediately before and immediately
                after the section of code to be timed, the 'TimeDiff' methods can be
                used to calculate the elapsed time while the code executed.
     @param          StartCounter [in] - Start counter value
     @param          StopCounter [in] - Stop counter value
     @param          CallTimeAdjusted [in] - Indicates if the call time of the 'Counter' 
                                        methodes should be subtracted.
     @return value   Time difference.
     ??Prototypes<docTools.DocEntry.PrototypesEntry instance at 90ed90>

   ------------------------------------------------------------------*/

  SAPDB_UInt8          TimeDiffNanoSec ( SAPDB_UInt8 StartCounter,
                                         SAPDB_UInt8 StopCounter,
                                         bool        CallTimeAdjusted = true )
                                    { SAPDB_UInt8 CounterDiff = StopCounter - StartCounter - (CallTimeAdjusted ? m_CallTime : 0);
                                      return ( CounterDiff * 1000 * 1000 * 1000 ) / m_Frequency; }

  SAPDB_UInt8          TimeDiffMicroSec( SAPDB_UInt8 StartCounter,
                                         SAPDB_UInt8 StopCounter,
                                         bool        CallTimeAdjusted = true )
                                    { SAPDB_UInt8 CounterDiff = StopCounter - StartCounter - (CallTimeAdjusted ? m_CallTime : 0);
                                      return ( CounterDiff * 1000 * 1000 ) / m_Frequency; }
    
  SAPDB_UInt8          TimeDiffMilliSec( SAPDB_UInt8 StartCounter,
                                         SAPDB_UInt8 StopCounter,
                                         bool        CallTimeAdjusted = true )
                                    { SAPDB_UInt8 CounterDiff = StopCounter - StartCounter - (CallTimeAdjusted ? m_CallTime : 0);
                                      return (CounterDiff * 1000 ) / m_Frequency; }

  SAPDB_UInt8          TimeDiffSec     ( SAPDB_UInt8 StartCounter,
                                         SAPDB_UInt8 StopCounter,
                                         bool        CallTimeAdjusted = true )
                                    { SAPDB_UInt8 CounterDiff = StopCounter - StartCounter - (CallTimeAdjusted ? m_CallTime : 0);
                                      return CounterDiff / m_Frequency; }

  /*!------------------------------------------------------------------
     @description    If the application calls 'Counter' immediately before and immediately
                after the section of code to be timed, the 'TimeDiff' methods can be
                used to calculate the elapsed time while the code executed.
     @param          StartCounter [in] - Start counter value
     @param          StopCounter [in] - Stop counter value
     @param          Sec [out] - Time difference (Seconds)
                MilliSec/MicroSec  [out] - Time difference (Milliseconds/Microseconds)
     @param          CallTimeAdjusted [in] - Indicates if the call time of the 'Counter' 
                                           methodes should be subtracted.
     @return value   none
     ??Prototypes<docTools.DocEntry.PrototypesEntry instance at 90fb48>

   ------------------------------------------------------------------*/

  void                 TimeDiffSecAndMilliSec ( SAPDB_UInt8 StartCounter,
                                                SAPDB_UInt8 StopCounter,
                                                SAPDB_UInt8 &Sec,
                                                SAPDB_UInt8 &MilliSec,
                                                bool        CallTimeAdjusted = true )
                                    { SAPDB_UInt8 CounterDiff = StopCounter - StartCounter - (CallTimeAdjusted ? m_CallTime : 0);
                                      Sec      = CounterDiff / m_Frequency;
                                      MilliSec = ((CounterDiff * 1000 ) / m_Frequency ) % 1000; }

  void                 TimeDiffSecAndMicroSec ( SAPDB_UInt8 StartCounter,
                                                SAPDB_UInt8 StopCounter,
                                                SAPDB_UInt8 &Sec,
                                                SAPDB_UInt8 &MicroSec,
                                                bool        CallTimeAdjusted = true )
                                    { SAPDB_UInt8 CounterDiff = StopCounter - StartCounter - (CallTimeAdjusted ? m_CallTime : 0);
                                      Sec      = CounterDiff / m_Frequency;
                                      MicroSec = ((CounterDiff * 1000 * 1000 ) / m_Frequency ) % (1000 * 1000); }



  /*!------------------------------------------------------------------
     @description    Converts a counter value into seconds and milli- or microseconds.
     @param          Counter [in] - Counter value
     @param          Sec [out] - Time difference (Seconds)
                MilliSec/MicroSec  [out] - Time difference (Milliseconds/Microseconds)
     @return value   none
     ??Prototypes<docTools.DocEntry.PrototypesEntry instance at 910b38>

   ------------------------------------------------------------------*/

  void                 CounterToSecAndMilliSec( SAPDB_UInt8 Counter,
                                                SAPDB_UInt8 &Sec,
                                                SAPDB_UInt8 &MilliSec )
                                    { Sec      = Counter / m_Frequency;
                                      MilliSec = ((Counter * 1000 ) / m_Frequency ) % 1000; }

  void                 CounterToSecAndMicroSec( SAPDB_UInt8 Counter,
                                                SAPDB_UInt8 &Sec,
                                                SAPDB_UInt8 &MicroSec )
                                    { Sec      = Counter / m_Frequency;
                                      MicroSec = ((Counter * 1000 * 1000 ) / m_Frequency ) % (1000 * 1000); }

  /*!------------------------------------------------------------------
     @description    Converts a counter value into microseconds.
     @param          Counter [in] - Counter value
     @return value   number of microseconds

   ------------------------------------------------------------------*/

  SAPDB_UInt8          CounterToMicroSeconds( SAPDB_UInt8 Counter )
                                    { return (SAPDB_UInt8)( ( ((SAPDB_Int8)Counter) * (double)1000000. ) / ((SAPDB_Int8)m_Frequency) ); }


  /*!------------------------------------------------------------------
     @description    Returns the call time of the 'Counter' methode.
     @return value   call time of the 'Counter' methode

   ------------------------------------------------------------------*/

  SAPDB_UInt8          CallTime() { return m_CallTime; }

  /*!------------------------------------------------------------------
     @description    Returns the maximum system uptime until a counter overrun occurs.
     @param          Years [out] - Uptime (years)
     @param          Month [out] - Uptime (month)
     @return value   none

   ------------------------------------------------------------------*/

  void                 OverrunUpTime ( SAPDB_UInt8 &Years,
                                       SAPDB_UInt4 &Month );

protected:
  RTESys_NTHighResTimer ();

  
  bool                 LeapCheck( bool       quickLeapCheck ); // check time values for strictly monotonous increasing

private:
  virtual bool         InitTimer( bool       quickLeapCheck ) = 0;

  virtual TimerType    Type() = 0;

  SAPDB_UInt8          CallcCallTime();

protected:
  SAPDB_UInt8          m_StartCount;
  SAPDB_UInt8          m_Frequency;
  bool                 m_Available;
  SAPDB_UInt8          m_CallTime;

private:
  SAPDB_Singleton      m_ThisIsASingleton; // prevents copy and assignment

  static RTESys_NTHighResTimer *m_Instance;
  static HANDLE                 m_hThread;
};

#endif  /* RTESYS_NTHIGHRESTIMER_HPP */
