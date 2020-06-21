/*!

  @file           RTESys_NTHighResTimer.cpp
  @ingroup        Runtime
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/System/RTESys_NTHighResTimer.h"
#include "RunTime/System/RTESys_NTHighResTimer.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined( _M_IX86 )
# define CPUID   __asm  XOR  EBX, EBX   \
                 __asm _emit 0x0F       \
                 __asm _emit 0xA2      /* CPUID CHANGES EBX: SHOW IT TO CC     */
#endif

#if defined( _M_IA64 )
extern "C" {
  unsigned __int64 __getReg(int whichReg);
  void             __mf(void);
# pragma intrinsic(__getReg)
# pragma intrinsic(__mf)
}

# define  CPUID
# define _IA64_REG_AR_ITC 3116
#endif

#if defined( _M_AMD64 )
#define CPUID
#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!--------------------------------------------------------------------
   @class     NTHighResTimer_ASM
   @brief    High Resolution Timer

   This class implements a high-resolution timer via CPU features.

 --------------------------------------------------------------------*/

class NTHighResTimer_ASM : public RTESys_NTHighResTimer
{
public:
  NTHighResTimer_ASM () : m_LastCounterVal(0),
                          m_LeapCorrection(false) {;}

  virtual SAPDB_UInt8  Counter();

private:
  virtual bool         InitTimer( bool       quickLeapCheck );
  virtual TimerType    Type() {return CPUTimer; };
  inline void          SleepTimeDelta ( SAPDB_UInt4    SleepTime,
                                        SAPDB_UInt8    &DeltaPerf, 
                                        SAPDB_UInt8    &DeltaCount );

protected:
  bool                 m_LeapCorrection;   ///< leap year correction
  SAPDB_UInt8          m_LastCounterVal;   ///< counter offset
};

/*---------------------------------------------------------------------------*/

inline void NTHighResTimer_ASM::SleepTimeDelta ( SAPDB_UInt4    SleepTime,
                                                 SAPDB_UInt8    &DeltaPerf, 
                                                 SAPDB_UInt8    &DeltaCount )
{
  LARGE_INTEGER  PerfStart, PerfEnd;
  SAPDB_UInt8    CounterStart, CounterEnd;
  CPUID; // serialize
  QueryPerformanceCounter( &PerfStart );
  CounterStart = Counter();
  Sleep( SleepTime );
  CPUID; // serialize
  CounterEnd = Counter();
  QueryPerformanceCounter( &PerfEnd );

  DeltaPerf  = PerfEnd.QuadPart - PerfStart.QuadPart;
  DeltaCount = CounterEnd - CounterStart;
}


/*---------------------------------------------------------------------------*/

bool NTHighResTimer_ASM::InitTimer( bool       quickLeapCheck )
{
  LARGE_INTEGER PerfFrequency, Tmp;
  SAPDB_UInt8   DeltaCount1, DeltaCount2, DeltaCount,
                DeltaPerf1, DeltaPerf2, DeltaPerf;


# if defined( _M_IX86 )
   if ( IsProcessorFeaturePresent( PF_RDTSC_INSTRUCTION_AVAILABLE ) == 0 )
     return false;
#  endif

   // check for timer leaps 
   if ( LeapCheck( quickLeapCheck ) == false )
   {
     RTE_Message ( SAPDBErr_MessageList( RTE_CONTEXT, RTEWARN_UNEXPECTED_TIMER_LEAPS, "CPU" ));
     return false;
   }

  /*
   * Warm up, implizit initialization and check of availibility
   */
  if(( !QueryPerformanceFrequency( &PerfFrequency ) ) ||
     ( !QueryPerformanceCounter( &Tmp ) ))
    return false;

  /*------------------------------------------------------------------*/
  /* Execute the measurement twice for different sleeptime:           */
  /* by subtraction of the results the systematic errors will         */
  /* be eliminated                                                    */
  /*------------------------------------------------------------------*/
  SleepTimeDelta ( 300, DeltaPerf1, DeltaCount1);
  SleepTimeDelta ( 150, DeltaPerf2, DeltaCount2);

  /*------------------------------------------------------------------*/
  /* unit of DeltaPerf: u = 1/PerfFrequency s                         */
  /*------------------------------------------------------------------*/
  DeltaPerf  = DeltaPerf1 - DeltaPerf2;
  DeltaCount = DeltaCount1 - DeltaCount2;

  /*------------------------------------------------------------------*/
  /* clockrate  = DeltaCount / ( DeltaPerf / PerfFrequency ) Hz       */
  /*            = DeltaCount * PerfFrequency / DeltaPerf Hz           */
  /*                                                                  */
  /* speed      = clockrate / Hz                                      */
  /* clockrate  = speed * Hz                                          */
  /*------------------------------------------------------------------*/
  m_Frequency = DeltaCount * PerfFrequency.QuadPart / DeltaPerf;


  m_LeapCorrection = true; // enable leap correction

  return m_Frequency > 0;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt8 NTHighResTimer_ASM::Counter()
{
  SAPDB_UInt8   NewVal;
  SAPDB_UInt8   LastCounterVal; 

#   if defined( _M_IX86 )
    {
     LARGE_INTEGER Val;

     __asm
     {
       _emit 0x0F                /* rdtsc - result in EDX:EAX            */
       _emit 0x31

       MOV   Val.LowPart,  EAX
       MOV   Val.HighPart, EDX
     }
     NewVal = Val.QuadPart;
    }
#   elif defined( _M_IA64 )
     NewVal = __getReg(_IA64_REG_AR_ITC);
#   elif defined( _M_AMD64 )
     NewVal = ReadTimeStampCounter();
#   else
     NewVal = 0;
#   endif

    if ( m_LeapCorrection )
    {
        RTESys_CompareAndExchange( m_LastCounterVal, 0, 0, LastCounterVal );

        if ( LastCounterVal > NewVal ) 
            NewVal = LastCounterVal;
        else
        {
            while ( !RTESys_CompareAndExchange( m_LastCounterVal,   // value to exchange
                                                LastCounterVal,     // expected element
                                                NewVal,             // my new value
                                                LastCounterVal ))   // current last counter value value
            {
                if ( LastCounterVal > NewVal ) 
                {
                    NewVal = LastCounterVal;
                    break;
                }
            }
        }
    }

  return NewVal;
}

/*!--------------------------------------------------------------------
  @class          NTHighResTimer_PerfCount
   @description    Windows High Resolution Timer


                This class implements a high-resolution timer via 'QueryPerformanceCounter'.

 --------------------------------------------------------------------*/

class NTHighResTimer_PerfCount : public RTESys_NTHighResTimer
{
public:
  virtual SAPDB_UInt8  Counter();

private:
  virtual bool          InitTimer( bool       quickLeapCheck );
  virtual TimerType    Type() {return QueryPerfCntr; };
};

/*---------------------------------------------------------------------------*/

bool NTHighResTimer_PerfCount::InitTimer( bool       quickLeapCheck )
{
  LARGE_INTEGER Freq;

  // check for timer leaps 
  if ( LeapCheck( quickLeapCheck ) == false )
  {
    RTE_Message ( SAPDBErr_MessageList( RTE_CONTEXT, RTEWARN_UNEXPECTED_TIMER_LEAPS, "PerfCount" ));
    return false;
  }

  if ( QueryPerformanceFrequency( &Freq ))
  {
    m_Frequency = Freq.QuadPart;
    return true;
  }

  return false;
}
/*---------------------------------------------------------------------------*/

SAPDB_UInt8 NTHighResTimer_PerfCount::Counter()
{
  LARGE_INTEGER Count;

  if ( !QueryPerformanceCounter(&Count) )
    return 0;

  return Count.QuadPart;
}

/*!--------------------------------------------------------------------
  @class          NTHighResTimer_TickCount
  @description    Windows High Resolution Timer


  This class implements a high-resolution timer via 'GetTickCount'.

 --------------------------------------------------------------------*/

class NTHighResTimer_TickCount : public RTESys_NTHighResTimer
{
public:
  NTHighResTimer_TickCount ();

  virtual SAPDB_UInt8  Counter();

private:
  virtual bool         InitTimer( bool       quickLeapCheck );
  virtual TimerType    Type() {return TickCount; };

private:
  SAPDB_UInt8         m_MilliSec;
  RTESync_Spinlock    m_Lock;             // used for locked scopes
};

/*---------------------------------------------------------------------------*/

NTHighResTimer_TickCount::NTHighResTimer_TickCount () 
                         : m_MilliSec(0), m_Lock(0) 
{
}

/*---------------------------------------------------------------------------*/

bool NTHighResTimer_TickCount::InitTimer( bool       quickLeapCheck )
{
  m_Frequency = 1000;
  return true;
}
/*---------------------------------------------------------------------------*/

SAPDB_UInt8 NTHighResTimer_TickCount::Counter()
{
  SAPDB_UInt4         TickCount;
  SAPDB_UInt8         MilliSec;

  // Locked scope
  { RTESync_LockedScope LockedScope( m_Lock );
    MilliSec = m_MilliSec;
  }

  TickCount = GetTickCount();

  if ( (SAPDB_UInt4)MilliSec > TickCount ) // counter overrun ?
    MilliSec += 0x100000000I64;  // increment upper value

  MilliSec = ( MilliSec & ~0xFFFFFFFFI64 ) | TickCount; // calculate new value

  // Locked scope
  { RTESync_LockedScope LockedScope( m_Lock );

    if ( m_MilliSec < MilliSec ) // Meantime, a second thread might changed this value! 
      m_MilliSec = MilliSec;
  }

  return m_MilliSec;
}

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTESys_NTHighResTimer::RTESys_NTHighResTimer()
                      : m_Frequency(0), m_Available(false), m_StartCount(0), m_CallTime(0)
{
}

/*---------------------------------------------------------------------------*/

// Computes the the counts for one counter call
//

#define LOOP_CNTS                200
#define NUM_OF_MEASUREMENTS      11

SAPDB_UInt8 RTESys_NTHighResTimer::CallcCallTime()
{
  SAPDB_UInt8                 CounterVals[NUM_OF_MEASUREMENTS];
  SAPDB_UInt8                 Min = (SAPDB_UInt8)-1;
  int                         i, j;

  for( i = 0, j = 0; i < LOOP_CNTS; i++, j = 0)
  {
    // use several sequential calls to avoid cache and pipline artifacts
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    CounterVals[j++] = RTESys_NTHighResTimer::Instance().Counter();
    SAPDBERR_ASSERT_STATE(NUM_OF_MEASUREMENTS == j);

    for ( j = 1; j < NUM_OF_MEASUREMENTS; j++ )
      Min = (CounterVals[j] - CounterVals[j-1]) < Min ? (CounterVals[j] - CounterVals[j-1]) : Min;
   }

  return Min;
}

/*---------------------------------------------------------------------------*/

#define  SECONDS_PER_YEAR   ( 60 * 60 * 24 * 365 )
#define  SECONDS_PER_MONTH  (SECONDS_PER_YEAR / 12)

void RTESys_NTHighResTimer::OverrunUpTime ( SAPDB_UInt8 &Years,
                                            SAPDB_UInt4 &Month )
{
  SAPDB_UInt8 MaxSeconds  = 0xFFFFFFFFFFFFFFFFI64  / m_Frequency;

  Years = MaxSeconds / SECONDS_PER_YEAR;
  Month = (SAPDB_UInt4)((MaxSeconds % SECONDS_PER_YEAR ) / SECONDS_PER_MONTH);
}

/*---------------------------------------------------------------------------*/

// check time values for strictly monotonous increasing
bool RTESys_NTHighResTimer::LeapCheck( bool       quickLeapCheck )
{

  SAPDB_UInt  maxLoop = quickLeapCheck ? 500 : 3000;

  SYSTEM_INFO     SysInfo;
  GetSystemInfo( &SysInfo );

  DWORD_PTR	SystemAffinityMask;
  DWORD_PTR	ProcessAffinityMask;
  DWORD_PTR	Proccesor;

  if ( SysInfo.dwNumberOfProcessors == 1 )
    return true;

  if ( !GetProcessAffinityMask( GetCurrentProcess(),
                                &ProcessAffinityMask,
                                &SystemAffinityMask ))
  {
    return true;
  }

  SAPDB_UInt8  LastCounterVal = Counter();

  for ( SAPDB_UInt Repeat = 0; Repeat < 500; Repeat++ )
  {
    for ( Proccesor = 1; Proccesor != 0; Proccesor <<= 1 ) 
    {
      if ( ProcessAffinityMask & Proccesor)
      {
        if ( SetThreadAffinityMask( GetCurrentThread(), Proccesor))
        {
          SAPDB_UInt8  CounterVal = Counter();

          if( CounterVal <= LastCounterVal )
          {
            SetThreadAffinityMask( GetCurrentThread(), ProcessAffinityMask );
            return false;
          }

          LastCounterVal = CounterVal;
        }
      }
    }
  }
  SetThreadAffinityMask( GetCurrentThread(), ProcessAffinityMask );

  return true;
}

/*---------------------------------------------------------------------------*/

RTESys_NTHighResTimer & RTESys_NTHighResTimer::Instance()
{
  if ( m_Instance)
     return *m_Instance;

  TimerType type = QueryPerfCntr;
  Initialize ( type );

  SAPDBERR_ASSERT_STATE(m_Instance != NULL);

  return *m_Instance;
}

/*---------------------------------------------------------------------------*/

bool RTESys_NTHighResTimer::Initialize ( TimerType  &Timer,
                                         bool       quickLeapCheck )
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, max(max(sizeof(NTHighResTimer_TickCount), sizeof(NTHighResTimer_ASM)), sizeof(NTHighResTimer_PerfCount)) );

  if ( Timer == CPUTimer )
  {
    m_Instance = new (Space) NTHighResTimer_ASM();

    if ( m_Instance->InitTimer( quickLeapCheck ) == false )
      Timer = QueryPerfCntr;
  }

  if ( Timer == QueryPerfCntr )
  {
    m_Instance = new (Space) NTHighResTimer_PerfCount();

    if ( m_Instance->InitTimer( quickLeapCheck ) == false )
      Timer = TickCount;
  }

  if ( Timer == TickCount )
  {
    m_Instance = new (Space) NTHighResTimer_TickCount();

    if ( false == m_Instance->InitTimer( quickLeapCheck ))
    {
        m_Instance = 0;
        return false;
    }
  }

  m_Instance->m_StartCount = m_Instance->Counter();
  m_Instance->m_CallTime   = m_Instance->CallcCallTime();

  return true;
}

/*---------------------------------------------------------------------------*/

 static DWORD WINAPI NTHRTimerAsyncThread ( RTESys_NTHighResTimer::TimerType Timer )
 {
     return RTESys_NTHighResTimer::Initialize ( Timer, true );
 }

/*---------------------------------------------------------------------------*/

bool RTESys_NTHighResTimer::InitializeAsync ( TimerType  Timer )
{
  m_hThread = CreateThread ( 0, 0, (LPTHREAD_START_ROUTINE)NTHRTimerAsyncThread,
                             (void*) Timer, 0, NULL );

  if ( m_hThread == NULL )
        return Initialize ( Timer, false );

  return true;
}

/*---------------------------------------------------------------------------*/

bool  RTESys_NTHighResTimer::InitializeAsyncWait ( TimerType  &Timer )
{
    DWORD exitCode;

    WaitForSingleObject(m_hThread, INFINITE);

    if (( 0 == GetExitCodeThread(m_hThread, &exitCode )) ||
        ( 0 == exitCode ))
    {
        // retry initialize;
        return Initialize ( Timer, false );
    }

    Timer = m_Instance->Type();

    return true;
}

/*---------------------------------------------------------------------------*/

RTESys_NTHighResTimer * RTESys_NTHighResTimer::m_Instance = NULL;
HANDLE                  RTESys_NTHighResTimer::m_hThread  = NULL;


/*===========================================================================*
 *  C - FUNCTIONS                                                            *
 *===========================================================================*/

externC bool  RTESys_NTHRTimerInitializeAsync ( cTimerType  cTimer )
{
  RTESys_NTHighResTimer::TimerType  Timer;

  switch ( cTimer )
  {
  case cCPUTimer:
    Timer = RTESys_NTHighResTimer::CPUTimer;
    break;
  case cQueryPerfCntr:
    Timer = RTESys_NTHighResTimer::QueryPerfCntr;
    break;
  case cTickCount:
    Timer = RTESys_NTHighResTimer::TickCount;
    break;
  }
    
  return RTESys_NTHighResTimer::InitializeAsync( Timer );
}

/*---------------------------------------------------------------------------*/

externC bool  RTESys_NTHRTimerInitializeAsyncWait ()
{
  RTESys_NTHighResTimer::TimerType  Timer;

  return RTESys_NTHighResTimer::InitializeAsyncWait ( Timer );
}
/*---------------------------------------------------------------------------*/

externC SAPDB_UInt8 RTESys_NTHRTimerCounter()
{
  return RTESys_NTHighResTimer::Instance().Counter();
}


/*---------------------------------------------------------------------------*/

externC SAPDB_UInt8 RTESys_NTHRTimerTimeDiffNanoSec ( SAPDB_UInt8 StartCounter,
                                                      SAPDB_UInt8 StopCounter,
                                                      bool        CallTimeAdjusted )
{
  return RTESys_NTHighResTimer::Instance().TimeDiffNanoSec( StartCounter, StopCounter, CallTimeAdjusted );
}


/*---------------------------------------------------------------------------*/

externC SAPDB_UInt8 RTESys_NTHRTimerTimeDiffMicroSec ( SAPDB_UInt8 StartCounter,
                                                       SAPDB_UInt8 StopCounter,
                                                       bool        CallTimeAdjusted )
{
  return RTESys_NTHighResTimer::Instance().TimeDiffMicroSec( StartCounter, StopCounter, CallTimeAdjusted );
}
    

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt8 RTESys_NTHRTimerTimeDiffMilliSec ( SAPDB_UInt8 StartCounter,
                                                       SAPDB_UInt8 StopCounter,
                                                       bool        CallTimeAdjusted )
{
  return RTESys_NTHighResTimer::Instance().TimeDiffMilliSec( StartCounter, StopCounter, CallTimeAdjusted );
}


/*---------------------------------------------------------------------------*/

externC SAPDB_UInt8  RTESys_NTHRTimerTimeDiffSec ( SAPDB_UInt8 StartCounter,
                                                   SAPDB_UInt8 StopCounter,
                                                   bool        CallTimeAdjusted )
{
  return RTESys_NTHighResTimer::Instance().TimeDiffSec( StartCounter, StopCounter, CallTimeAdjusted );
}


/*---------------------------------------------------------------------------*/

externC void  RTESys_NTHRTimerTimeDiffSecAndMilliSec ( SAPDB_UInt8 StartCounter,
                                                       SAPDB_UInt8 StopCounter,
                                                       SAPDB_UInt4 *Sec,
                                                       SAPDB_UInt4 *MilliSec,
                                                       bool        CallTimeAdjusted )
{
  SAPDB_UInt8 ui8Sec, ui8MilliSec;

  RTESys_NTHighResTimer::Instance().TimeDiffSecAndMilliSec( StartCounter, StopCounter, ui8Sec, ui8MilliSec, CallTimeAdjusted );

  *Sec      = static_cast<SAPDB_UInt4>(ui8Sec);
  *MilliSec = static_cast<SAPDB_UInt4>(ui8MilliSec);
}

/*---------------------------------------------------------------------------*/

externC void  RTESys_NTHRTimerTimeDiffSecAndMicroSec ( SAPDB_UInt8 StartCounter,
                                                       SAPDB_UInt8 StopCounter,
                                                       SAPDB_UInt4 *Sec,
                                                       SAPDB_UInt4 *MicroSec,
                                                       bool        CallTimeAdjusted )
{
  SAPDB_UInt8 ui8Sec, ui8MicroSec;

  RTESys_NTHighResTimer::Instance().TimeDiffSecAndMicroSec( StartCounter, StopCounter, ui8Sec, ui8MicroSec, CallTimeAdjusted );

  *Sec      = static_cast<SAPDB_UInt4>(ui8Sec);
  *MicroSec = static_cast<SAPDB_UInt4>(ui8MicroSec);
}


/*---------------------------------------------------------------------------*/

externC void  RTESys_NTHRTimerCounterToSecAndMilliSec( SAPDB_UInt8 Counter,
                                                       SAPDB_UInt4 *Sec,
                                                       SAPDB_UInt4 *MilliSec )
{
  SAPDB_UInt8 ui8Sec, ui8MilliSec;

  RTESys_NTHighResTimer::Instance().CounterToSecAndMilliSec( Counter, ui8Sec, ui8MilliSec );

  *Sec      = static_cast<SAPDB_UInt4>(ui8Sec);
  *MilliSec = static_cast<SAPDB_UInt4>(ui8MilliSec);
}


/*---------------------------------------------------------------------------*/

externC void  RTESys_NTHRTimerCounterToSecAndMicroSec( SAPDB_UInt8 Counter,
                                                       SAPDB_UInt4 *Sec,
                                                       SAPDB_UInt4 *MicroSec )
{
  SAPDB_UInt8 ui8Sec, ui8MicroSec;

  RTESys_NTHighResTimer::Instance().CounterToSecAndMicroSec( Counter, ui8Sec, ui8MicroSec );

  *Sec      = static_cast<SAPDB_UInt4>(ui8Sec);
  *MicroSec = static_cast<SAPDB_UInt4>(ui8MicroSec);
}

/*---------------------------------------------------------------------------*/

externC void  RTESys_NTHRTimeSecAndMilliSec( SAPDB_UInt4 *Sec,
                                             SAPDB_UInt4 *MilliSec )
{
  SAPDB_UInt8 ui8Sec, ui8MilliSec, Counter;

  Counter = RTESys_NTHighResTimer::Instance().Counter();

  RTESys_NTHighResTimer::Instance().CounterToSecAndMilliSec( Counter, ui8Sec, ui8MilliSec );

  *Sec      = static_cast<SAPDB_UInt4>(ui8Sec);
  *MilliSec = static_cast<SAPDB_UInt4>(ui8MilliSec);
}


/*---------------------------------------------------------------------------*/

externC void  RTESys_NTHRTimerSecAndMicroSec( SAPDB_UInt4 *Sec,
                                              SAPDB_UInt4 *MicroSec )
{
  SAPDB_UInt8 ui8Sec, ui8MicroSec, Counter;

  Counter = RTESys_NTHighResTimer::Instance().Counter();

  RTESys_NTHighResTimer::Instance().CounterToSecAndMicroSec( Counter, ui8Sec, ui8MicroSec );

  *Sec      = static_cast<SAPDB_UInt4>(ui8Sec);
  *MicroSec = static_cast<SAPDB_UInt4>(ui8MicroSec);
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt8 RTESys_NTHRTimerMicroSeconds()
{
  SAPDB_UInt8 Counter;

  Counter = RTESys_NTHighResTimer::Instance().Counter(); /* rescale to microseconds */

  return RTESys_NTHighResTimer::Instance().CounterToMicroSeconds( Counter );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
