/*!
  @file           veo570.cpp
  @author         RaymondR
  @special area   Memory management functions.
  @brief          Memory management functions.
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



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "gsp00.h"
#include    "geo570.h"
#include    "geo007_1.h"
#include    "geo60.h"

#if defined(_WIN32)
# include   <gos00.h>
#else /* UNIX */
# include   <stdlib.h>
#endif


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
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

teo570_AllocPool *   teo570_GlobalAlloc::m_Pool = NULL;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

//-----------------------------------------------------------------------------
//------------------------- class teo570_AllocPool ----------------------------
//-----------------------------------------------------------------------------

teo570_AllocPool::~teo570_AllocPool() 
{
  ROUTINE_DBG_MEO60 (_T("teo570_AllocPool::~teo570_AllocPool"));
}

/*---------------------------------------------------------------------------*/

size_t teo570_AllocPool::eo570_MemAvail( void const *        PoolHint,
                                         teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo570_AllocPool::eo570_MemAvail"));

  return MEM_UNKNOWN_EO570;
}

/*---------------------------------------------------------------------------*/

bool teo570_AllocPool::eo570_Validate( void *              Buffer,
                                       void const *        PoolHint )
{
  ROUTINE_DBG_MEO60 (_T("teo570_AllocPool::eo570_Validate"));

  return true;
}

//-----------------------------------------------------------------------------
//------------------------ class teo570_GlobalAlloc ---------------------------
//-----------------------------------------------------------------------------

void teo570_GlobalAlloc::eo570_SetPool ( teo570_AllocPool *   Pool )
{
  ROUTINE_DBG_MEO60 (_T("teo570_GlobalAlloc::eo570_SetPool"));

  m_Pool = Pool;

  return;
}

//-----------------------------------------------------------------------------

void * teo570_GlobalAlloc::eo570_Allocate ( size_t                    Size, 
                                            char const * const        FileName,
                                            unsigned long             LineNo,
                                            void  const *             PoolHint,
                                            teo200_EventList *        pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo570_GlobalAlloc::eo570_Allocate"));

  void *              Buffer;
  teo001_SystemRc     rc;

  if ( m_Pool != NULL )
  {
    Buffer = m_Pool->eo570_Allocate( Size, FileName, LineNo, PoolHint, pEventList );
  }
  else
  {
#   if defined(_WIN32)
     Buffer = HeapAlloc(GetProcessHeap(), 0, Size); 

     rc = (Buffer != NULL) ? NO_ERROR_EO001 : GetLastError();

#   else /* UNIX */
     Buffer = malloc ( Size );

     rc = (Buffer != NULL) ? NO_ERROR_EO001 : ERROR_NOT_ENOUGH_MEMORY_EO001;
#   endif

    if ( rc != NO_ERROR_EO001 )
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_ALLOC_MEMORY__FILE_LINE__, 
                               rc, FileName, LineNo );

      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
  }

  return Buffer;
}

//-----------------------------------------------------------------------------

void * teo570_GlobalAlloc::eo570_Reallocate ( void *                    OldBuffer,
                                              size_t                    NewSize,
                                              char const * const        FileName,
                                              unsigned long             LineNo,
                                              void const *              PoolHint,
                                              teo200_EventList *        pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo570_GlobalAlloc::eo570_Reallocate"));

  void *   NewBuffer;

  teo001_SystemRc    rc;

  if ( m_Pool != NULL )
  {
    NewBuffer = m_Pool->eo570_Reallocate( OldBuffer, NewSize, FileName, LineNo, 
                                          PoolHint, pEventList );
  }
  else
  {
#   if defined(_WIN32)
     NewBuffer = HeapReAlloc( GetProcessHeap(), 0, OldBuffer, NewSize ); 

     rc = (NewBuffer != NULL) ? NO_ERROR_EO001 : GetLastError();

#   else /* UNIX */
     NewBuffer = realloc ( OldBuffer, NewSize );

     rc = (NewBuffer != NULL) ? NO_ERROR_EO001 : ERROR_NOT_ENOUGH_MEMORY_EO001;
#   endif

    if ( rc != NO_ERROR_EO001 )
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_REALLOC_MEMORY__FILE_LINE__, 
                               rc, FileName, LineNo );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
  }

  return NewBuffer;
}

//-----------------------------------------------------------------------------

void teo570_GlobalAlloc::eo570_Deallocate ( void *                    Buffer,
                                            char const * const        FileName,
                                            unsigned long             LineNo,
                                            void const *              PoolHint,
                                            teo200_EventList *        pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo570_GlobalAlloc::eo570_Deallocate"));

  if ( m_Pool != NULL )
  {
    m_Pool->eo570_Deallocate( Buffer, FileName, LineNo, PoolHint, pEventList );
  }
  else
  {
#   if defined(_WIN32)

     if ( HeapFree( GetProcessHeap(), 0, Buffer ) == FALSE )
     {
       teo001_SystemRc    rc = GetLastError();
       
       teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_REALLOC_MEMORY__FILE_LINE__, 
                                rc, FileName, LineNo );
       COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
#   else /* UNIX */
     free ( Buffer );
#   endif
  }

  return;
}

//-----------------------------------------------------------------------------

size_t teo570_GlobalAlloc::eo570_MemAvail ( void const *              PoolHint,
                                            teo200_EventList *        pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo570_GlobalAlloc::eo570_MemAvail"));

  size_t MemAvail = MEM_UNKNOWN_EO570;

  if ( m_Pool != NULL )
    MemAvail = m_Pool->eo570_MemAvail( PoolHint, pEventList );

  return MemAvail;
}

/*---------------------------------------------------------------------------*/

bool teo570_GlobalAlloc::eo570_Validate( void *              Buffer,
                                         void const *        PoolHint )
{
  ROUTINE_DBG_MEO60 (_T("teo570_GlobalAlloc::eo570_Validate"));

  if ( m_Pool != NULL )
    return m_Pool->eo570_Validate( Buffer, PoolHint );

  return true;
}

//-----------------------------------------------------------------------------
//----------------------------- Old C-Interface -------------------------------
//-----------------------------------------------------------------------------

extern "C"  teo001_SystemRc  eo570_Allocate   ( void **             Buffer,
                                                size_t              Size,
                                                char const * const  FileName,
                                                unsigned long       LineNo )
{
  ROUTINE_DBG_MEO60 (_T("eo570_Allocate"));

  teo001_SystemRc    rc                = NO_ERROR_EO001;
  static bool        MessOutputPending = false;
  
  try 
  {
    *Buffer = teo570_GlobalAlloc::eo570_Allocate( Size, FileName, LineNo, NO_HINT_EO570 );
  }
  catch ( teo200_EventList EvtLst )
  {
    // --- message output might produce additional allocation errors
    if ( !MessOutputPending )
    {
      MessOutputPending = true;
      MSGCD_EVTLST (EvtLst);
      MessOutputPending = false;
    }
  }

  return rc;
}

//-----------------------------------------------------------------------------

extern "C"  void  eo570_Deallocate ( void *              Buffer,
                                     char const * const  FileName,
                                     unsigned long       LineNo )
{
  ROUTINE_DBG_MEO60 (_T("eo570_Deallocate"));

  try 
  {
    teo570_GlobalAlloc::eo570_Deallocate( Buffer, FileName, LineNo, NO_HINT_EO570 );
  }
  catch ( teo200_EventList EvtLst )
  {
    MSGCD_EVTLST (EvtLst);
  }

  return;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/