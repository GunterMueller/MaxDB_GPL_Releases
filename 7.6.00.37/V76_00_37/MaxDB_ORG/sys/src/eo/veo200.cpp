/*!
  @file           veo200.cpp
  @author         RaymondR
  @special area   event list handling
  @brief          Creating, queueing and managing event list
  @see            

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

//
//   FreeBSD portions added by Kai Mosebach, 
//   For more informations see : http://www.komadev.de/sapdb
//


\endif
*/



#if defined(UNICODE)
# define _UNICODE
#endif


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(OSF1)
#include <sys/types.h>
#endif

#define _POSIX_C_SOURCE 199506L

#if defined(SUN) || defined(SOLARIS)
/* needed to get all wanted prototypes... Otherwise yield() may be missing... */
#define __EXTENSIONS__ 1
#endif

#include <time.h>
#ifndef FREEBSD
#include <malloc.h>
#endif
#include "geo200.h"
#include "geo41.h"
#include "geo007_1.h"

#if defined(KERNEL_LZU)
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp" /* nocheck */
#endif


#if defined(_WIN32)
# include <crtdbg.h>
#else /* UNIX */
# include <assert.h>

# if defined(_DEBUG)
#  define _ASSERT      assert
# else
#  define _ASSERT(_assertion)
# endif
#endif

#include "SAPDBCommon/SAPDB_string.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

# define MIN_EVENTMSG_TEMP_BUFFER_SIZE         (1024 + MAX_ARGUMENT_LENGTH_EO200)
# define TIME_HEADER_FORMAT_STR                _T("%04d-%02d-%02d %02d:%02d:%02d")
# define TIME_HEADER_FORMAT_STR_LEN            19

# define BIG_TIME_HEADER_FORMAT_STR            _T("%04d-%02d-%02d %02d:%02d:%02d.%03d")
# define BIG_TIME_HEADER_FORMAT_STR_LEN        23


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define  ALIGNMENT_VAL_64BIT     sizeof( tsp00_Uint8 )
#define  ALIGN_VAL(_val)                    \
           ((((_val) - 1) / (ALIGNMENT_VAL_64BIT) + 1 ) * (ALIGNMENT_VAL_64BIT))


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

//
//  This header is needed by 'eo200_StoreEventList' and 'eo200_LoadEventList'
//  The swap object returned by 'GetValues' is used to swap all data
//  during event list load (eo200_LoadEventList).
//
class EventDataHeader : private teo41_Swap
{
private:
  struct _EventDataHeader
  {
    tsp00_Uint4            Version;
    tsp00_Uint1            SwapType;
    tsp00_Uint1            Filler1;
    tsp00_Uint2            NumOfEvents;
    tsp00_Uint4            DataLen;
    tsp00_Uint1            Filler2;
  };

public:
  EventDataHeader ( void * const   Buffer ) { m_Header = (_EventDataHeader*) Buffer; };

  static tsp00_Uint4  GetHeaderLen    () { return sizeof(_EventDataHeader); };
  
  void                SetValues ( tsp00_Uint2 const          NumOfEvents,
                                  tsp00_Uint4 const          DataLen );
                      
  bool                GetValues ( tsp00_Uint4                &Version,
                                  tsp00_Uint2                &NumOfEvents,
                                  tsp00_Uint4                &DataLen,
                                  teo41_Swap                 &Swap,
                                  teo200_EventList * const   pEventList = NULL );
  private:
    _EventDataHeader *  m_Header;
};


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

const teo200_EventList::teo200_Time  teo200_EventList::EmptyTimeValue = { 0,0,0,0,0,0,0,0 };


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

teo200_EventList::~teo200_EventList()
{  
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::~teo200_EventList"));

  eo200_ClearEventList();
}

/*---------------------------------------------------------------------------*/

teo200_EventList::teo200_EventList( teo001_SystemRc const   rc,
                                    _TCHAR const * const    FuncName,
                                    teo200_EventID const    EventID,
                                    teo200_EventType const  EventType,
                                    _TCHAR const            EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                                    _TCHAR const * const    pEventFormatStr, 
                                    ... )
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::teo200_EventList"));

  va_list  VarArgs;

  va_start ( VarArgs, pEventFormatStr);
  eo200_Create ( rc, FuncName, EventID, EventType, EventLabel, pEventFormatStr, VarArgs );
  va_end ( VarArgs );
}

/*---------------------------------------------------------------------------*/

teo200_EventList::teo200_EventList( teo200_EventList const  &PrevEventList,
                                    teo001_SystemRc const   rc,
                                    _TCHAR const * const    FuncName,
                                    teo200_EventID const    EventID,
                                    teo200_EventType const  EventType,
                                    _TCHAR const            EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                                    _TCHAR const * const    pEventFormatStr, 
                                    ... )
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::teo200_EventList"));

  va_list  VarArgs;

  va_start ( VarArgs, pEventFormatStr);
  eo200_Create ( rc, FuncName, EventID, EventType, EventLabel, pEventFormatStr, VarArgs );
  va_end ( VarArgs );


#if defined(KERNEL_LZU)
  pNextEvent_eo200 = new (RTEMem_RteAllocator::Instance()) teo200_EventList;
#else
  pNextEvent_eo200 = new teo200_EventList;
#endif
  if ( pNextEvent_eo200 != NULL )
    pNextEvent_eo200->eo200_CpyEventList(PrevEventList);
}

/*---------------------------------------------------------------------------*/

teo200_EventList::teo200_EventList( _TCHAR const * const    FuncName,
                                    teo200_EventID const    EventID,
                                    teo200_EventType const  EventType,
                                    _TCHAR const            EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                                    _TCHAR const * const    pEventFormatStr, 
                                    ... )
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::teo200_EventList"));

  va_list  VarArgs;

  va_start ( VarArgs, pEventFormatStr);
  eo200_Create ( 0, FuncName, EventID, EventType, EventLabel, pEventFormatStr, VarArgs );
  va_end ( VarArgs );
}

/*---------------------------------------------------------------------------*/

teo200_EventList::teo200_EventList( teo200_EventList const  &PrevEventList,
                                    _TCHAR const * const    FuncName,
                                    teo200_EventID const    EventID,
                                    teo200_EventType const  EventType,
                                    _TCHAR const            EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                                    _TCHAR const * const    pEventFormatStr, 
                                    ... )
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::teo200_EventList"));

  va_list  VarArgs;

  va_start ( VarArgs, pEventFormatStr);
  eo200_Create ( 0, FuncName, EventID, EventType, EventLabel, pEventFormatStr, VarArgs );
  va_end ( VarArgs );


#if defined(KERNEL_LZU)
  pNextEvent_eo200 = new (RTEMem_RteAllocator::Instance()) teo200_EventList;
#else
  pNextEvent_eo200 = new teo200_EventList;
#endif

  if ( pNextEvent_eo200 != NULL )
    pNextEvent_eo200->eo200_CpyEventList(PrevEventList);
}

/*---------------------------------------------------------------------------*/

_TCHAR const *  const  teo200_EventList::eo200_EventType( bool const  Long ) const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventType"));

  TCHAR const * EvtType;

  if ( Data_eo200 != NULL )
  {
    switch ( (teo200_EventType)Data_eo200->EventType_eo200  )
    {
    case Error_eeo200:
      if ( Long ) 
        EvtType = EVENT_TYPE_ERR_EO200;
      else
        EvtType = EVENT_TYPE_ERR_SHORT_EO200;
      break;
    case Warning_eeo200:
      if ( Long ) 
        EvtType = EVENT_TYPE_WRN_EO200;
      else
        EvtType = EVENT_TYPE_ERN_SHORT_EO200;
      break;
    case Info_eeo200:
      if ( Long ) 
        EvtType = EVENT_TYPE_INFO_EO200;
      else
        EvtType = EVENT_TYPE_INFO_SHORT_EO200;
      break;
    }
  }
  else
    EvtType = _T("");

  return EvtType; 
}

/*---------------------------------------------------------------------------*/

teo200_EventList const& teo200_EventList::operator = ( teo200_EventList const &EventList )
    
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::operator ="));

  // --- do nothing on Event1 = Event1
  if ( &EventList != this )
  {
    if ( Data_eo200 != NULL ) 
      eo200_ClearEventList();

    eo200_CpyEventList( EventList );
  }

  return *this;
}

/*---------------------------------------------------------------------------*/

teo200_EventList teo200_EventList::operator + ( teo200_EventList const &EventList ) const
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::operator +"));
  teo200_EventList       Result = *this;
  teo200_EventList*      pTmp   = &Result;

  // --- searching for the end
  while ( pTmp->pNextEvent_eo200 != NULL )
    pTmp = pTmp->pNextEvent_eo200;

  // ---  Append 'EventList'
  

#if defined(KERNEL_LZU)
  pTmp->pNextEvent_eo200 = new (RTEMem_RteAllocator::Instance()) teo200_EventList;
#else
  pTmp->pNextEvent_eo200 = new teo200_EventList;
#endif


  if ( pTmp->pNextEvent_eo200 != NULL )
    pTmp->pNextEvent_eo200->eo200_CpyEventList(EventList);


  return Result;
}

/*---------------------------------------------------------------------------*/

void teo200_EventList::eo200_ClearEventList() 
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_ClearEventList"));

  // --- first of all we should release next event object!!!!
  //
  if ( pNextEvent_eo200 != NULL )
  {
    _ASSERT( ( pNextEvent_eo200->eo200_GetDataRef() >= 1 ) && 
             ( Data_eo200 != NULL )                        && 
             ( eo200_GetDataRef() >= 1 ) );

    _ASSERT( pNextEvent_eo200->eo200_GetDataRef() >= 
             pNextEvent_eo200->eo200_GetObjectRef() );

    // - are we the only one holding this next event object?
    if ( pNextEvent_eo200->eo200_GetObjectRef() == 1 ) 
    {
#if defined(KERNEL_LZU)
      destroy (pNextEvent_eo200, RTEMem_RteAllocator::Instance());
#else
      delete pNextEvent_eo200;
#endif
      pNextEvent_eo200 = NULL;
    }
  }

  // --- subtract data and object references of the whole event chain!
  eo200_SubtractDataRefs();
  eo200_SubtractObjectRefs();

  if ( Data_eo200 != NULL )
  {
    if ( eo200_GetDataRef() == 0 )
    {
#if defined(KERNEL_LZU)
      RTEMem_RteAllocator::Instance().Deallocate(Data_eo200);
#else
      delete Data_eo200;
#endif
    }

    Data_eo200       = NULL;
    pNextEvent_eo200 = NULL;
  }
}

/*---------------------------------------------------------------------------*/

tsp00_Uint4 const teo200_EventList::eo200_DataBufferSize( tsp00_Uint4 * const  TruncBufferSize ) const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_DataBufferSize"));

  if ( TruncBufferSize )  
    *TruncBufferSize = EventDataHeader::GetHeaderLen() + Data_eo200->DataLen_eo200;

  return EventDataHeader::GetHeaderLen() + eo200_EventListDataLen();
}
/*---------------------------------------------------------------------------*/

bool teo200_EventList::eo200_StoreEventList( void * const              pBuffer,
                                             tsp00_Uint4 const         BufferSize,
                                             tsp00_Uint4               &DataLen,
                                             bool const                Truncate,
                                             teo200_EventList * const  pEventList ) const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_StoreEventList"));

  
  bool                     Ok   = true;
  tsp00_Uint2              NumOfEvents;
  EventDataHeader          Header(pBuffer);

  if ( Data_eo200 != NULL )
  {
    tsp00_Uint4  BufferSizeRequired = Header.GetHeaderLen() + eo200_EventListDataLen();

    if ((( BufferSize >= BufferSizeRequired )  || ( Truncate   == true )) &&
        (  BufferSize >= Header.GetHeaderLen() + Data_eo200->DataLen_eo200 ))
    {
#     if defined _DEBUG
       SAPDB_memset ( pBuffer, 0xAA, BufferSize );
#     endif

      DataLen     = Header.GetHeaderLen();  // set current data length
      NumOfEvents = eo200_PustEventData( pBuffer, BufferSize, DataLen );

      Header.SetValues( NumOfEvents, DataLen );
    }
    else 
    {
      Ok = false;
      teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_WRONG_BUFFER_SIZE, 
                               BufferSize, BufferSizeRequired );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo200_EventList::eo200_LoadEventList( void * const              pBuffer,
                                            tsp00_Uint4 const         DataLen,
                                            teo200_EventList * const  pEventList )
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_LoadEventList"));

  tsp00_Uint2              NumOfEvents;
  tsp00_Uint4              DataLenRemain;
  bool                     Ok      = true;
  char *                   pTmp    = (char*) pBuffer;
  teo41_Swap               Swap;
  tsp00_Uint4              Version;
  EventDataHeader          Header(pBuffer);

  eo200_ClearEventList();

  // --- Load some header values
  Ok = Header.GetValues ( Version, NumOfEvents, DataLenRemain, Swap, pEventList );

  if (( Ok ) && ( DataLenRemain > DataLen ))
  {
    Ok = false;
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_INCONSISTENT_BUFFER_CONTENT ); 
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
  }
  else
  {
    DataLenRemain -= Header.GetHeaderLen();  // set current data length
    pTmp          += Header.GetHeaderLen();

    // --- Load event list data
    Ok = eo200_PopEventData( pTmp, Swap, NumOfEvents, DataLenRemain, pEventList );
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

tsp00_Uint4 const  teo200_EventList::eo200_EventListDataLen() const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventListDataLen"));

  tsp00_Uint4 Size = 0;

  if ( Data_eo200 != NULL )
    Size = Data_eo200->DataLen_eo200;

  if ( pNextEvent_eo200 != NULL )
    Size += pNextEvent_eo200->eo200_EventListDataLen();

 return Size;
}

/*---------------------------------------------------------------------------*/

tsp00_Uint2 const  teo200_EventList::eo200_PustEventData( void * const            pBuffer,
                                                          tsp00_Uint4 const       BufferSize,
                                                          tsp00_Uint4             &TotalDataLen ) const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_PustEventData"));
  teo200_EventData  *pData;
  tsp00_Uint2       Events = 0;

  if ( (BufferSize - TotalDataLen) >= Data_eo200->DataLen_eo200 )
  {
    pData = (teo200_EventData*)((char*)pBuffer + TotalDataLen);

    SAPDB_memcpy ( pData, Data_eo200, Data_eo200->DataLen_eo200 );

    pData->DataRefCnt_eo200 = 0; // Reset reference counter
    TotalDataLen           += Data_eo200->DataLen_eo200;
    Events                  = 1;
    
    if ( pNextEvent_eo200 != NULL )
      Events += pNextEvent_eo200->eo200_PustEventData( pBuffer, BufferSize, TotalDataLen );
  }

 return Events;
}

/*---------------------------------------------------------------------------*/

bool const teo200_EventList::eo200_PopEventData( void * const              pBuffer,
                                                 teo41_Swap const          &Swap,  
                                                 tsp00_Uint2 const         NumOfEventsRemain,
                                                 tsp00_Uint4 const         DataLenRemain,
                                                 teo200_EventList * const  pEventList )
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_PopEventData"));
  bool                     Ok = true;
  teo200_EventData const * pData = (teo200_EventData const *)pBuffer;
  tsp00_Uint4              DataLen;

  Swap.eo41_SwapValue(pData->DataLen_eo200, DataLen );

  if ( DataLen > DataLenRemain )
  {
    Ok = false;
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_INCONSISTENT_BUFFER_CONTENT ); 
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
  }

  if ( Ok )
  {
#if defined(KERNEL_LZU)
    Data_eo200          = (teo200_EventData*)RTEMem_RteAllocator::Instance().Allocate(DataLen);
#else
    Data_eo200          = (teo200_EventData*)new _TCHAR[ DataLen ];
#endif

    ObjectRefCnt_eo200  = 1;
    pNextEvent_eo200    = NULL;

    if ( Data_eo200 != NULL )
    {
      SAPDB_memcpy ( Data_eo200, pData, DataLen );

      Swap.eo41_SwapValue(Data_eo200->Version_eo200 );
      Swap.eo41_SwapValue(Data_eo200->DataLen_eo200 );
      Swap.eo41_SwapValue(Data_eo200->rc_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventID_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventType_eo200 );
      Swap.eo41_SwapValue(Data_eo200->DateTimeOffset_eo200 );
      Swap.eo41_SwapValue(Data_eo200->BigDateTimeOffset_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventLabelOffset_eo200 );
      Swap.eo41_SwapValue(Data_eo200->FunctionNameOffset_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventMsgOffset_eo200 );

      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.Year_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.Month_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.DayOfWeek_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.Day_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.Hour_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.Minute_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.Second_eo200 );
      Swap.eo41_SwapValue(Data_eo200->EventDateTime_eo200.Milliseconds_eo200 );
      Data_eo200->DataRefCnt_eo200 = 1; // - Each event data is referenced only once
    }

    if ( NumOfEventsRemain > 1 )
    {
#if defined(KERNEL_LZU)
      pNextEvent_eo200 = new (RTEMem_RteAllocator::Instance()) teo200_EventList;
#else
      pNextEvent_eo200 = new teo200_EventList;
#endif
      if ( pNextEvent_eo200 != NULL )
      {
        char *   pTmp = (char*) pBuffer;

        pTmp += DataLen;

        Ok = pNextEvent_eo200->eo200_PopEventData( pTmp, Swap, 
                                                   NumOfEventsRemain - 1, 
                                                   DataLenRemain - DataLen, 
                                                   pEventList );
      }
    }
  }

 return Ok;
}

/*---------------------------------------------------------------------------*/

void teo200_EventList::eo200_CreateDateTime( teo200_Time            &EventDateTime,
                                             _TCHAR * const         DateTimeStr,
                                             unsigned int const     DateTimeStrSize,
                                             _TCHAR * const         BigDateTimeStr,
                                             unsigned int const     BigDateTimeStrSize ) const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_CreateDateTime"));
# if defined(_WIN32) || defined(WIN32)
  SYSTEMTIME      DateTime;
# else 
  time_t          DateTime;
#ifdef _REENTRANT
  struct tm       tm_buffer;
#endif
  struct tm       *pDateTime_tm;
# endif

  //
  // --- determine time 
  //
# if defined(_WIN32) || defined(WIN32)
  GetLocalTime( &DateTime );
  
  EventDateTime.Year_eo200         = DateTime.wYear;
  EventDateTime.Month_eo200        = DateTime.wMonth;
  EventDateTime.DayOfWeek_eo200    = DateTime.wDayOfWeek;
  EventDateTime.Day_eo200          = DateTime.wDay;
  EventDateTime.Hour_eo200         = DateTime.wHour;
  EventDateTime.Minute_eo200       = DateTime.wMinute;
  EventDateTime.Second_eo200       = DateTime.wSecond;
  EventDateTime.Milliseconds_eo200 = DateTime.wMilliseconds;
# else 
  time ( &DateTime );
  
#ifdef _REENTRANT
  pDateTime_tm = localtime_r ( &DateTime, &tm_buffer );
#else
  pDateTime_tm = localtime ( &DateTime );
#endif
  
  EventDateTime.Year_eo200         = pDateTime_tm->tm_year + 1900;
  EventDateTime.Month_eo200        = pDateTime_tm->tm_mon + 1; // PTS 1105505
  EventDateTime.DayOfWeek_eo200    = pDateTime_tm->tm_wday;
  EventDateTime.Day_eo200          = pDateTime_tm->tm_mday;
  EventDateTime.Hour_eo200         = pDateTime_tm->tm_hour;
  EventDateTime.Minute_eo200       = pDateTime_tm->tm_min;
  EventDateTime.Second_eo200       = pDateTime_tm->tm_sec;
  EventDateTime.Milliseconds_eo200 = 0;
# endif


# if defined(_WIN32) || defined(WIN32)

  DateTimeStr[DateTimeStrSize - 1]       = _T('\0');
  BigDateTimeStr[BigDateTimeStrSize - 1] = _T('\0');

  _sntprintf( (TCHAR *)BigDateTimeStr, BigDateTimeStrSize - 1, BIG_TIME_HEADER_FORMAT_STR,
              EventDateTime.Year_eo200,
              EventDateTime.Month_eo200,
              EventDateTime.Day_eo200,
              EventDateTime.Hour_eo200,
              EventDateTime.Minute_eo200,
              EventDateTime.Second_eo200,
              EventDateTime.Milliseconds_eo200 );

  _sntprintf( (TCHAR *)DateTimeStr, DateTimeStrSize - 1, TIME_HEADER_FORMAT_STR,
              EventDateTime.Year_eo200,
              EventDateTime.Month_eo200,
              EventDateTime.Day_eo200,
              EventDateTime.Hour_eo200,
              EventDateTime.Minute_eo200,
              EventDateTime.Second_eo200 );

# else // - !WIN32

  _stprintf ( (TCHAR *)BigDateTimeStr, BIG_TIME_HEADER_FORMAT_STR,
              EventDateTime.Year_eo200,
              EventDateTime.Month_eo200,
              EventDateTime.Day_eo200,
              EventDateTime.Hour_eo200,
              EventDateTime.Minute_eo200,
              EventDateTime.Second_eo200,
              EventDateTime.Milliseconds_eo200 );

  _stprintf ( (TCHAR *)DateTimeStr, TIME_HEADER_FORMAT_STR,
              EventDateTime.Year_eo200,
              EventDateTime.Month_eo200,
              EventDateTime.Day_eo200,
              EventDateTime.Hour_eo200,
              EventDateTime.Minute_eo200,
              EventDateTime.Second_eo200 );
# endif

  return;
}

/*---------------------------------------------------------------------------*/

void teo200_EventList::eo200_CpyEventList( teo200_EventList const &EventList )
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_CpyEventList"));

  pNextEvent_eo200 = EventList.pNextEvent_eo200;
  Data_eo200       = EventList.Data_eo200;
  
  // --- add data and object references of the whole event chain!
  eo200_AddObjectRefs();
  eo200_AddDataRefs();
}

/*---------------------------------------------------------------------------*/

void teo200_EventList::eo200_Create( teo001_SystemRc const   rc,
                                     _TCHAR const * const    FuncName,
                                     teo200_EventID const    EventID,
                                     teo200_EventType const  EventType,
                                     _TCHAR const * const    EventLabel,
                                     _TCHAR const * const    pEventFormatStr, 
                                     va_list                 VarArgs )
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_Create"));
  unsigned int   DateTimeSize      = TIME_HEADER_FORMAT_STR_LEN + 1;
  unsigned int   BigDateTimeSize   = BIG_TIME_HEADER_FORMAT_STR_LEN + 1; 
  unsigned int   EventLabelSize    = MAX_EVENTLABEL_LEN_EO200 + 1;  
  unsigned int   FunctionNameSize  = (unsigned int)_tcslen( FuncName ) + 1;
  _TCHAR         EventMsgBuf[MIN_EVENTMSG_TEMP_BUFFER_SIZE + 1];
  _TCHAR*        EventMsg          = EventMsgBuf;
  unsigned int   EventMsgSize;    
  tsp00_Uint4    DataSize;

  if ( pEventFormatStr == NULL )
  {
    EventMsgBuf[0] = _T('\0');
  }
  else
  {
    // --- build up the event message
    //
    EventMsgSize = (unsigned int)_tcslen( pEventFormatStr ) + MAX_ARGUMENT_LENGTH_EO200 + 1;

    if ( EventMsgSize >= MIN_EVENTMSG_TEMP_BUFFER_SIZE )
    {
       // - We should use alloca if AIX is growed up!
       //   Don't forget, remove the delete statement!
#if defined(KERNEL_LZU)
      EventMsg = (_TCHAR*)RTEMem_RteAllocator::Instance().Allocate(EventMsgSize);
#else
      EventMsg = (_TCHAR*)new _TCHAR[ EventMsgSize ];
#endif
    }

#   if defined(_WIN32) || defined(WIN32)
     _vsntprintf( EventMsg, EventMsgSize - 1, pEventFormatStr, VarArgs );
#   else 
     _vstprintf ( EventMsg, pEventFormatStr, VarArgs );
#   endif
  }

  if ( EventMsg != NULL )
  {
    EventMsgSize = (unsigned int)_tcslen( EventMsg ) + 1;
    DataSize     = ALIGN_VAL ( sizeof( *Data_eo200 ) + FunctionNameSize + 
                               DateTimeSize + BigDateTimeSize + 
                               EventLabelSize + EventMsgSize );

    // - init object members 
#if defined(KERNEL_LZU)
    Data_eo200 = (teo200_EventData*)RTEMem_RteAllocator::Instance().Allocate(DataSize);
#else
    Data_eo200 = (teo200_EventData*)new _TCHAR[DataSize];
#endif

    pNextEvent_eo200    = NULL;
    ObjectRefCnt_eo200  = 1;


#   if defined _DEBUG
    SAPDB_memset ( Data_eo200, 0xAA, DataSize );
#   endif
  }
  
  if ( Data_eo200 != NULL )
  {
    Data_eo200->Version_eo200             = EVENTLIST_VERSION_EO200;
    Data_eo200->DataLen_eo200             = DataSize;
    Data_eo200->rc_eo200                  = (tsp00_Uint4)(rc); 
    Data_eo200->EventID_eo200             = (tsp00_Uint4)(EventID);
    Data_eo200->EventType_eo200           = (tsp00_Uint4)(EventType);
    Data_eo200->DateTimeOffset_eo200      = 0;
    Data_eo200->BigDateTimeOffset_eo200   = DateTimeSize;
    Data_eo200->EventLabelOffset_eo200    = Data_eo200->BigDateTimeOffset_eo200  + BigDateTimeSize; 
    Data_eo200->FunctionNameOffset_eo200  = Data_eo200->EventLabelOffset_eo200   + EventLabelSize;
    Data_eo200->EventMsgOffset_eo200      = Data_eo200->FunctionNameOffset_eo200 + FunctionNameSize;
    Data_eo200->Filler_eo200              = 0;
    Data_eo200->DataRefCnt_eo200          = 1; // - This event data is referenced once
                                                                                
    // --- build up data and time info!
    eo200_CreateDateTime( Data_eo200->EventDateTime_eo200,
                          Data_eo200->Strings_eo200 + Data_eo200->DateTimeOffset_eo200,    DateTimeSize,
                          Data_eo200->Strings_eo200 + Data_eo200->BigDateTimeOffset_eo200, BigDateTimeSize );

    _tcscpy( Data_eo200->Strings_eo200 + Data_eo200->EventLabelOffset_eo200,   EventLabel );
    _tcscpy( Data_eo200->Strings_eo200 + Data_eo200->FunctionNameOffset_eo200, FuncName );
    _tcscpy( Data_eo200->Strings_eo200 + Data_eo200->EventMsgOffset_eo200,     EventMsg );
  }

  if ( EventMsg != EventMsgBuf )  
  { // - we should not do this if we are using alloca
#if defined(KERNEL_LZU)
    RTEMem_RteAllocator::Instance().Deallocate(EventMsg);  
#else
    delete EventMsg;   
#endif
  }
}




/*===========================================================================*
 *  LOCAL CLASSES (CODE)                                                     *
 *===========================================================================*/

void  EventDataHeader::SetValues ( tsp00_Uint2 const           NumOfEvents,
                                   tsp00_Uint4 const           DataLen )
{ 
  ROUTINE_DBG_MEO60 (_T("EventDataHeader::SetValues"));


  m_Header->Version     = EVENTLIST_VERSION_EO200; 
  m_Header->Filler1     = 0;
  m_Header->Filler2     = 0;
  m_Header->SwapType    = teo41_Swap::eo41_GetLocalSwapType ();
  m_Header->DataLen     = DataLen;
  m_Header->NumOfEvents = NumOfEvents;
} 

/*---------------------------------------------------------------------------*/

bool  EventDataHeader::GetValues ( tsp00_Uint4                 &Version,
                                   tsp00_Uint2                 &NumOfEvents,
                                   tsp00_Uint4                 &DataLen,
                                   teo41_Swap                  &Swap,
                                   teo200_EventList * const    pEventList )
{ 
  ROUTINE_DBG_MEO60 (_T("EventDataHeader::GetValues"));
  bool Ok = true;

  Ok = eo41_SetPeerSwapType( m_Header->SwapType, pEventList );

  if ( Ok )
  {
    // --- Swap the header values
    eo41_SwapValue(m_Header->Version,     Version );
    eo41_SwapValue(m_Header->NumOfEvents, NumOfEvents );
    eo41_SwapValue(m_Header->DataLen,     DataLen );

    Swap = *this;
  }

  return Ok;
} 


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
