/*!
  @file           geo200.h
  @author         RaymondR
  @special area   event list handling
  @brief          Creating, queueing and managing event lists
  @see            


                C++ exception handling required!

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




#ifndef GEO200_H
#define GEO200_H
#if defined( __cplusplus )

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(_WIN32) || defined(WIN32)

#if defined ( UNICODE )
# define  _UNICODE 
#endif
# include <windows.h>
# include <TCHAR.H>
# include <stdio.h>
#else
# include <stdio.h>
# include <time.h>
#endif

#include  "geo001.h"
#include  "geo47.h"
#include  "geo60.h"
#include  "gsp00.h"

class teo41_Swap;


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define STRSIZE_E0200(_Str)  (sizeof(_Str)/sizeof(_Str[0]))

# define ERR_TYPE_EO200                        teo200_EventList::Error_eeo200
# define INFO_TYPE_EO200                       teo200_EventList::Info_eeo200
# define WRN_TYPE_EO200                        teo200_EventList::Warning_eeo200

/* - this defines should only be used by geo007_1.h */
#if !defined(ERR_TYPE)
# define ERR_TYPE                              teo200_EventList::Error_eeo200
#endif

#if !defined(WRN_TYPE)
# define WRN_TYPE                              teo200_EventList::Info_eeo200
#endif

#if !defined(INFO_TYPE)
# define INFO_TYPE                             teo200_EventList::Warning_eeo200
#endif
                                               
# define EVENT_TYPE_ERR_EO200                  _T("ERROR")
# define EVENT_TYPE_ERR_SHORT_EO200            _T("ERR")
# define EVENT_TYPE_INFO_EO200                 _T("INFO")
# define EVENT_TYPE_INFO_SHORT_EO200           _T("")
# define EVENT_TYPE_WRN_EO200                  _T("WARNING")
# define EVENT_TYPE_ERN_SHORT_EO200            _T("WRN")

# define EVENTLIST_VERSION_EO200               1
                                         
# ifndef NO_ERROR
#  define NO_ERROR                             0
# endif
#endif /* __cplusplus */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*! @name Macros and defines */
 /*@(*/

/*!
   @Description    Throw event if pointer is NULL


                Use this macro for functions with eventlist pointer parameter,
                where a null-pointer means that the eventlist has to be thrown.
   @code           
    
    void  MyFunc ( ....., teo200_EventList   *pEventList )
    {
      ...
    
      teo200_EventList EvtLst( "MyFunc", 4711, INFO_TYPE_EO200, "Tasking",
                               "Task started, T%d", TaskID );
    
      // - throw 'EvtLst' if 'pEventList' is NULL
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
    
    
    @endcode        
 */

#define COPY_OR_THROW_EVENTLIST_EO200( _EvtLstPtr, _EvtLst)          \
                  if ( (_EvtLstPtr) != NULL )                        \
                    *(_EvtLstPtr) = _EvtLst;                         \
                  else                                               \
                    throw _EvtLst;



/*!
   @Description    Use this macros if you are working with eventlist pointers.

 */

#define CLEAR_EVENTLIST_EO200( _EvtLstPtr )                          \
                  if ( (_EvtLstPtr) != NULL )                        \
                    (_EvtLstPtr)->eo200_ClearEventList();

#define EVENTLIST_IS_EMPTY_EO200( _EvtLstPtr )                       \
                  (( (_EvtLstPtr) != NULL ) ? (_EvtLstPtr)->IsEmpty() : true )

/*! EndDeclaration */

/*!
   @Description    Some defines

 */

#define MAX_EVENTLABEL_LEN_EO200              8
#define MAX_ARGUMENT_LENGTH_EO200             2048

/*! EndDeclaration */

/*@)*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


#if defined( __cplusplus )

/*---------------------------------------------------------------------------*/
/*------------------------- class teo200_EventList ---------------------------*/
/*---------------------------------------------------------------------------*/

/*!
  @class          teo200_EventList
   @Description    Creating, queueing and managing an event list.


                The teo200_EventList class provides a functionality to store 
                and queue events like success, warning and error messages. 
                In most situations teo200_EventList is used to store the error event message
                for the last operation that failed. The implementation allows you to build up 
                a list of event messages which is as long as you like.
                teo200_EventList assists you in conserving memory space 
                by allowing two objects sharing the same values also to share the same 
                buffer spaces.  
                                        Derived (private) from teo41_Swap to use protected methods like  
                'eo41_SetPeerSwapType' and eo41_GetLocalSwapType. The methods
                'eo200_StoreEventList' and 'eo200_LoadEventList' are doing all
                necessary swapping needed if you transfer the event list data
                buffer from one machine type to another.

 */


class teo200_EventList
{
public:
  /*!
     @Description    Public types used by public member functions.

   */

  typedef tsp00_Uint4  teo200_EventID;

  typedef enum { Error_eeo200 = 1,
                 Info_eeo200,
                 Warning_eeo200 } teo200_EventType;

  typedef struct { tsp00_Int2   Year_eo200;
                   tsp00_Int2   Month_eo200;
                   tsp00_Int2   DayOfWeek_eo200;
                   tsp00_Int2   Day_eo200;
                   tsp00_Int2   Hour_eo200;
                   tsp00_Int2   Minute_eo200;
                   tsp00_Int2   Second_eo200;
                   tsp00_Int2   Milliseconds_eo200;} teo200_Time;

  

public:
  /*!
     @Description    Create a new event list object.


                The constructors containing the argument 'PrevEventList' are used to build up 
                a list of event messages.
     @param          PrevEventList [in (optional)] - An existing event list to be appended.
     @param          rc [in (optional)] - The system return code.
     @param          FuncName [in] - The name of the calling function.
     @param          EventID [in] - An unequivocal identifier corresponding to the current                                                 event message.
     @param          EventType [in] - Information type like error, warning and info                                                (ERR_TYPE_EO200, INFO_TYPE_EO200, WRN_TYPE_EO200).
     @param          EventLabel [in] - The EventLabel specifies a group of event messages                                                e.g. "Tasking", "Logging"...
     @param          pEventFormatStr [in] - The format string is similar to printf-style format strings.
     @param          ... [in (optional)] - Additional arguments.                                                The format string must contain specifications that                                                 determine the output format for the arguments.
     @Return value   none
     @code           
      
      
      void  MyFunc ()
      {
        ...
      
        teo200_EventList EvtLst( "MyFunc", 4711, INFO_TYPE_EO200, "Tasking",
                                 "Task started, T%d", TaskID );
        ...
        teo200_EventList NewEvtLst( EvtLst, "MyFunc", 0815, INFO_TYPE_EO200, "Tasking"
                                    "Task stopped, T%d", TaskID );
        ...
      }
        @endcode        
   */

  teo200_EventList( _TCHAR const * const     FuncName,
                    teo200_EventID const     EventID,
                    teo200_EventType const   EventType,
                    _TCHAR const             EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                    _TCHAR const * const     pEventFormatStr, 
                     ... );
  teo200_EventList( teo200_EventList const   &PrevEventList,
                    _TCHAR  const * const    FuncName,
                    teo200_EventID  const    EventID,
                    teo200_EventType const   EventType,
                    _TCHAR  const            EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                    _TCHAR  const * const    pEventFormatStr,
                     ... );
  teo200_EventList( teo001_SystemRc const    rc,
                    _TCHAR const * const     FuncName,
                    teo200_EventID const     EventID,
                    teo200_EventType const   EventType,
                    _TCHAR const             EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                    _TCHAR const * const     pEventFormatStr,
                     ... );
  teo200_EventList( teo200_EventList const   &PrevEventList,
                    teo001_SystemRc const    rc,
                    _TCHAR const * const     FuncName,
                    teo200_EventID const     EventID,
                    teo200_EventType const   EventType,
                    _TCHAR const             EventLabel[MAX_EVENTLABEL_LEN_EO200 + 1],
                    _TCHAR const * const     pEventFormatStr,
                     ... );

  inline teo200_EventList();

  virtual ~teo200_EventList();


  /*!
     @Description    Returns the size of the buffer needed to
              store the event list data via 'eo200_StoreEventList'.
     @param          TruncBufferSize [out] - The size you need store at least                                      one event into the buffer.
     @return         length of event list data.

   */

  tsp00_Uint4 const eo200_DataBufferSize( tsp00_Uint4 * const   TruncBufferSize = NULL ) const; 

  /*!
     @Description    The 'eo200_StoreEventList' function stores the event list data
              into a buffer.


                This function can be used to transfer event lists over
                network or store them into files. 
                To get the size of the buffer you need store the event list data
                use 'eo200_DataBufferSize'.
     @param          pBuffer [in] - Pointer to the buffer receiving                                     event list data. If NULL                                      'eo200_StoreEventList' only returns                                     the required buffer size.
     @param          BufferSize [in] - The size of the event list data buffer.
     @param          DataLen [out] - The length of event list data.
     @param          Truncate [in] - Truncate if buffer ti small.
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error.
     @return         true if deleted or not existing
             false else (setting *pEventList)

   */

 bool eo200_StoreEventList ( void * const              pBuffer,
                             tsp00_Uint4 const         BufferSize,
                             tsp00_Uint4               &DataLen,
                             bool const                Truncate   = false,
                             teo200_EventList * const  pEventList = NULL ) const;


  /*!
     @Description    The 'eo200_LoadEventList' function loads the event list data
              from a buffer previously stored with 'eo200_StoreEventList'.


                This methods is doing all necessary swapping needed to 
                transfer the event list data buffer from one machine type 
                to another.
     @param          pBuffer [in] - Pointer to the buffer containing                                     the event list data.
     @param          DataLen [in] - Length of the event list data.
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error.
     @return         true if deleted or not existing
             false else (setting *pEventList)

   */

  bool eo200_LoadEventList  ( void * const              pBuffer,
                              tsp00_Uint4 const         DataLen,
                              teo200_EventList * const  pEventList = NULL );

  /*!
     @Description    Empty an event list object.
     @Return value   none

   */

  void eo200_ClearEventList();

  /*!
     @Description    Returns a pointer to next event object of the list.
     @Return value   Pointer to the next event list object. 
              Returns NULL if no object exists.

   */

  inline teo200_EventList * const eo200_NextEvent() const;


  /*!
     @Description    Returns the function name.
     @Return value   Function name.

   */

  inline _TCHAR const * const eo200_FunctionName() const;

  /*!
     @Description    Returns the system return code.
     @Return value   System return code.

   */

  inline teo001_SystemRc const eo200_rc() const;

  /*!
     @Description    Returns the event identifier.
     @Return value   Event identifier.

   */

  inline teo200_EventID const  eo200_EventID() const;

  /*!
     @Description    Returns the event type (warning, error, info).
     @param          Long [in (optional)] - if 'true' it returns the                           event type string in long form.
     @Return value   Event type (teo200_EventType).                     &lt;br>
              Event type in string form:                         &lt;br>
              - Long  =  true  -> "WARNING", "ERROR", "INFO"     &lt;br>
              - Long  =  false -> "WRN", "ERR", ""               &lt;br>

   */

  inline teo200_EventType const  eo200_EventType () const;
  _TCHAR const * const           eo200_EventType ( bool const  Long ) const;


  /*!
     @Description    Returns the date and time the event was created.
     @param          Long [in (optional)] - if 'true' it returns the                           date and time string in long form.
     @Return value   Event type (teo200_Time).                          &lt;br>
              Event type in string form:                         &lt;br> 
              - Long  =  true  -> "YYYY-MM-DD HH:MM:SS.sss"           &lt;br>
              - Long  =  false -> "YYYY-MM-DD HH:MM:SS"               &lt;br>

   */

  inline teo200_Time const     &eo200_EventDateTime() const;
  inline _TCHAR const * const  eo200_EventDateTime( bool const Long ) const;


  /*!
     @Description    Returns the event label.
     @Return value   Event label.

   */

  inline _TCHAR const * const  eo200_EventLabel() const;


  /*!
     @Description    Returns the event message string.
     @Return value   Event message string.

   */

  inline _TCHAR const * const  eo200_EventMsg() const;

  /*!
     @Description    This operator assigns a value to the teo200_EventList object.

   */

  teo200_EventList const& operator = ( const teo200_EventList& EventList );

  /*!
     @Description    The operator bool() delivers 'true' if the event list object 
              contains an event.
     @code           
      
      teo200_EventList EvtLst;
      ...
      if ( EvtLst == false )
      {
        .. EvtLst is empty ..
      }
      
        @endcode        
   */

  inline operator bool() const;

  /*!
     @Description    The IsEmpty function delivers 'true' if the event list object 
              is empty.
     @code           
      
      teo200_EventList EvtLst;
      ...
      if ( EvtLst.IsEmpty == true )
      {
        .. EvtLst is empty ..
      }
      
        @endcode        
   */

  inline bool const  IsEmpty() const;


public:
  /* --- don't use this constructor directly */
  inline teo200_EventList ( const teo200_EventList& EventList );

/*--------------------------------------------
                  private
 --------------------------------------------*/

private:
  void                        eo200_Create           ( teo001_SystemRc const   rc,
                                                       _TCHAR const * const    FuncName,
                                                       teo200_EventID const    EventID,
                                                       teo200_EventType const  EventType,
                                                       _TCHAR const * const    EventLabel,
                                                       _TCHAR const * const    pEventFormatStr,
                                                       va_list                 VarArgs );
  inline void                 eo200_AddDataRefs      ();
  inline void                 eo200_SubtractDataRefs ();
  inline tsp00_Uint4          eo200_GetDataRef       ();

  inline void                 eo200_AddObjectRefs    ();
  inline void                 eo200_SubtractObjectRefs();
  inline tsp00_Uint4          eo200_GetObjectRef     ();

  void                        eo200_CpyEventList     ( teo200_EventList const    &EventList );
                                                                                 
  void                        eo200_CreateDateTime   ( teo200_Time               &EventDateTime,
                                                       _TCHAR * const            DateTimeStr,
                                                       unsigned int const        DateTimeStrSize,
                                                       _TCHAR * const            BigDateTimeStr,
                                                       unsigned int const        BigDateTimeStrSize ) const;
  tsp00_Uint4 const           eo200_EventListDataLen () const;                   
  tsp00_Uint2 const           eo200_PustEventData    ( void * const              pBuffer,
                                                       tsp00_Uint4 const         BufferSize,
                                                       tsp00_Uint4               &TotalDataLen ) const;
  bool const                  eo200_PopEventData     ( void * const              pBuffer,
                                                       teo41_Swap const          &Swap,
                                                       tsp00_Uint2 const         NumOfEventsRemain,
                                                       tsp00_Uint4 const         DataLenRemain,
                                                       teo200_EventList * const  pEventList = NULL );

public:
  teo200_EventList operator+ ( const teo200_EventList& EventList ) const;

private:

  struct teo200_EventData
  {
    tsp00_Uint4            Version_eo200;
    tsp00_Uint4            DataLen_eo200;
    tsp00_Uint4            DataRefCnt_eo200;

    teo200_Time            EventDateTime_eo200;

    tsp00_Uint4            rc_eo200;
    tsp00_Uint4            EventID_eo200;
    tsp00_Uint4            EventType_eo200;

    tsp00_Uint2            DateTimeOffset_eo200;
    tsp00_Uint2            BigDateTimeOffset_eo200;
    tsp00_Uint2            EventLabelOffset_eo200;
    tsp00_Uint2            FunctionNameOffset_eo200;
    tsp00_Uint2            EventMsgOffset_eo200;
    tsp00_Uint2            Filler_eo200;

    _TCHAR                 Strings_eo200[1]; // - the string buffer goes here!
  };

private:
  tsp00_Uint4              ObjectRefCnt_eo200;
  teo200_EventData*        Data_eo200;    // - Event Data ( NULL means 'no event' )
  teo200_EventList*        pNextEvent_eo200;

static const teo200_Time   EmptyTimeValue;
};



/*---------------------------------------------------------------------------*/

inline teo200_EventList::teo200_EventList() 
                        : ObjectRefCnt_eo200(0),
                          Data_eo200 (NULL),
                          pNextEvent_eo200(NULL)
{  
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::teo200_EventList"));
}

/*---------------------------------------------------------------------------*/

inline teo200_EventList::teo200_EventList( teo200_EventList const &EventList )
                        : ObjectRefCnt_eo200(0),
                          Data_eo200 (NULL),
                          pNextEvent_eo200(NULL)
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::teo200_EventList"));
  eo200_CpyEventList( EventList );
}

/*---------------------------------------------------------------------------*/

inline teo200_EventList * const teo200_EventList::eo200_NextEvent() const
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_NextEvent"));

  return pNextEvent_eo200;
}

/*---------------------------------------------------------------------------*/

inline void teo200_EventList::eo200_AddDataRefs()
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_AddDataRefs"));

  Data_eo200->DataRefCnt_eo200++;

  if ( pNextEvent_eo200 != NULL )
    pNextEvent_eo200->eo200_AddDataRefs();

  return;
}

/*---------------------------------------------------------------------------*/

inline void teo200_EventList::eo200_SubtractDataRefs()
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_SubtractDataRefs"));

  if (( Data_eo200 != NULL ) && ( Data_eo200->DataRefCnt_eo200 > 0 ))
    Data_eo200->DataRefCnt_eo200--;

  if ( pNextEvent_eo200 != NULL )
    pNextEvent_eo200->eo200_SubtractDataRefs();

  return;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Uint4 teo200_EventList::eo200_GetDataRef()
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_GetDataRef"));

  return Data_eo200->DataRefCnt_eo200;
}

/*---------------------------------------------------------------------------*/

inline void teo200_EventList::eo200_AddObjectRefs()
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_AddObjectRefs"));

  ObjectRefCnt_eo200++;

  if ( pNextEvent_eo200 != NULL )
    pNextEvent_eo200->eo200_AddObjectRefs();

  return;
}

/*---------------------------------------------------------------------------*/

inline void teo200_EventList::eo200_SubtractObjectRefs()
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_SubtractObjectRefs"));

  if ( ObjectRefCnt_eo200 > 0 )
    ObjectRefCnt_eo200--;

  if ( pNextEvent_eo200 != NULL )
    pNextEvent_eo200->eo200_SubtractObjectRefs();

  return;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Uint4 teo200_EventList::eo200_GetObjectRef()
{
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_GetObjectRef"));

  return ObjectRefCnt_eo200;
}

/*---------------------------------------------------------------------------*/

inline teo200_EventList::operator bool() const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::operator bool()"));

  return (Data_eo200 != NULL); 
}

/*---------------------------------------------------------------------------*/

inline bool const teo200_EventList::IsEmpty() const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::IsEmpty()"));

  return Data_eo200 == NULL; 
}

/*---------------------------------------------------------------------------*/

inline teo001_SystemRc const teo200_EventList::eo200_rc() const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_rc"));

  return ( Data_eo200 != NULL ) ? (teo001_SystemRc)Data_eo200->rc_eo200 : NO_ERROR; 
}

/*---------------------------------------------------------------------------*/

inline const teo200_EventList::teo200_EventID  teo200_EventList::eo200_EventID() const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventID"));

  return ( Data_eo200 != NULL ) ? (teo200_EventID)Data_eo200->EventID_eo200 : (teo200_EventID)0;
}

/*---------------------------------------------------------------------------*/

inline teo200_EventList::teo200_Time const& teo200_EventList::eo200_EventDateTime() const                  
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventDateTime"));

  return ( Data_eo200 != NULL ) ? Data_eo200->EventDateTime_eo200 : EmptyTimeValue;
}

/*---------------------------------------------------------------------------*/

inline const teo200_EventList::teo200_EventType teo200_EventList::eo200_EventType() const                  
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventType"));

  return ( Data_eo200 != NULL ) ? (teo200_EventType)Data_eo200->EventType_eo200 : Info_eeo200; 
}

/*---------------------------------------------------------------------------*/

inline _TCHAR const * const teo200_EventList::eo200_EventLabel() const   
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventLabel"));

  return ( Data_eo200 != NULL ) ? (Data_eo200->Strings_eo200 + Data_eo200->EventLabelOffset_eo200) : _T(""); 
}

/*---------------------------------------------------------------------------*/

inline _TCHAR const * const teo200_EventList::eo200_FunctionName() const   
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_FunctionName"));

  return ( Data_eo200 != NULL ) ? (Data_eo200->Strings_eo200 + Data_eo200->FunctionNameOffset_eo200) : _T(""); 
}

/*---------------------------------------------------------------------------*/

inline _TCHAR const * const teo200_EventList::eo200_EventMsg() const           
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventMsg"));

  return ( Data_eo200 != NULL ) ? (Data_eo200->Strings_eo200 + Data_eo200->EventMsgOffset_eo200) : _T(""); 
}

/*---------------------------------------------------------------------------*/

inline _TCHAR const * const teo200_EventList::eo200_EventDateTime( bool const  Long ) const
{ 
  ROUTINE_DBG_MEO60 (_T("teo200_EventList::eo200_EventDateTime"));
  const _TCHAR* DateTime;

  if ( Data_eo200 != NULL )
  {
    if ( Long )
      DateTime = Data_eo200->Strings_eo200 + Data_eo200->BigDateTimeOffset_eo200;
    else
      DateTime = Data_eo200->Strings_eo200 + Data_eo200->DateTimeOffset_eo200;
  }
  else
    DateTime = _T("");

  return DateTime;
}

#endif /* __cplusplus */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


/*===========================================================================*
 *  EXAMPES                                                                  *
 *===========================================================================*/

/*!
   @code           
    
    #if defined (WIN32)
    #  include <windows.h>
    #endif
    
    #include <stdarg.h>
    #include <ctype.h>
    #include <stdio.h>
    
    .#include "geo200.h"
     
    #define _DEBUG
    .#include "geo57.h"
    
    void PrintEvtLst ( teo200_EventList &EvtLst )
    {
      teo200_EventList *pTmpEvtLst = &EvtLst;
      
      if ( *pTmpEvtLst == false )
        printf ( "Event list is empty\n" );
      else
      {
        do
        {
          printf ( "%5d %s %s: %s, rc = %d - (%s)\n",
                   pTmpEvtLst->eo200_EventID(),
                   pTmpEvtLst->eo200_EventDateTime(true),
                   pTmpEvtLst->eo200_EventType(true),
                   pTmpEvtLst->eo200_EventMsg(),
                   pTmpEvtLst->eo200_rc(),
                   pTmpEvtLst->eo200_FunctionName() );
          
          pTmpEvtLst = pTmpEvtLst->eo200_NextEvent();
        }
        while ( pTmpEvtLst != NULL );
      }
    }
    
    //----------------------
    
    void func2 ( )
    {
      teo200_EventList  EvtLst2;
      
      EvtLst2 = teo200_EventList( "func2", 100003, 
                                  WRN_TYPE_EO200, "TEST",
                                  "EvtLst2 Line. %d", __LINE__ ); 
      
      throw EvtLst2;
    }
    
    //----------------------
    
    void func1 ( teo200_EventList  &EvtLst )
    {
      try
      {
        func2();
      }
      catch ( teo200_EventList EvtLst2 )
      {
        teo200_EventList EvtLst1 ( EvtLst2, "func1", 100001,
                                   INFO_TYPE_EO200, "TEST",
                                   "EvtLst1 Line. %d", __LINE__ ); 
        
        EvtLst = teo200_EventList( EvtLst1, "func1", 100000,
                                   INFO_TYPE_EO200, "TEST",
                                   "EvtLst Line. %d", __LINE__ ); 
        throw EvtLst1;
      }
    }
    
    //----------------------
    
    int main ()
    {
      teo200_EventList EvtLst;
      
      try
      {
        func1( EvtLst );
      }
      catch ( teo200_EventList EvtLstThrowed )
      {
        printf( "\nEvtLst:\n" );
        PrintEvtLst ( EvtLst );
        
        printf( "\nEvtLst throwed:\n" );
        PrintEvtLst ( EvtLstThrowed );
        
        printf( "\nEvtLst:\n" );
        EvtLst.eo200_ClearEventList();
        PrintEvtLst ( EvtLst );
      }
      
      return 0;
    }
    
    
    @endcode        
 */


#endif  /* GEO200_H */
