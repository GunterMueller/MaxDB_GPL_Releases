/*!
  @file           veo572.cpp
  @author         RaymondR
  @special area   user storage management
  @brief          user storage management
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

#if defined(_WIN32)
# include   <gos00.h>
#else /* UNIX */
#endif
#include    "geo572.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

#if defined(_WIN32)

 class t_AllocPool : protected teo570_AllocPool
 {
 public:
   t_AllocPool ( teo200_EventList *       pEventList = NULL );
   ~t_AllocPool ();

   void * eo570_Allocate  ( size_t                   Size,
                            char const * const       FileName,
                            unsigned long            LineNo,
                            void const *             PoolHint,
                            teo200_EventList *       pEventList );

   void * eo570_Reallocate( void *                   Buffer,
                            size_t                   NewSize,
                            char const * const       FileName,
                            unsigned long            LineNo,
                            void const *             PoolHint,
                            teo200_EventList *       pEventList);

   void   eo570_Deallocate( void *                   Buffer,
                            char const * const       FileName,
                            unsigned long            LineNo,
                            void const *             PoolHint,
                            teo200_EventList *       pEventList);

   bool  eo570_Validate   ( void *                   Buffer,
                            void const *             PoolHint );

 private:
   HANDLE         m_Heap;
 };

#endif




/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

#if defined(_WIN32)
 static t_AllocPool*   Pool = NULL;
#endif

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


#if defined(_WIN32)

 extern "C"  void eo572_CreateAllocPool ()
 {
   ROUTINE_DBG_MEO60 (_T("eo572_CreateAllocPool"));

   if ( Pool == NULL )
   {
     try 
     {
       Pool = new t_AllocPool;
     }
     catch ( teo200_EventList EvtLst )
     {
       MSGALL_EVTLST ( EvtLst );
     }
   }
 }

 //-----------------------------------------------------------------------------

 extern "C"  void eo572_DestroyAllocPool ()
 {
   ROUTINE_DBG_MEO60 (_T("eo572_DestroyAllocPool"));

   delete Pool;
 }
#endif

//-----------------------------------------------------------------------------

extern "C"  void sqlallocat  ( tsp00_Int4       Size,
                               tsp00_Uint1**  Buffer,
                               tsp00_BoolAddr Ok )
{
  ROUTINE_DBG_MEO60 (_T("sqlallocat"));

  *Buffer = NULL;

  #if defined(_WIN32)
   if ( Pool == NULL )
     eo572_CreateAllocPool ();
  #endif

  try 
  {
    *Buffer = (tsp00_Uint1*) teo570_GlobalAlloc::eo570_Allocate( Size, __FILE__,__LINE__,
                                                                 NO_HINT_EO570 );
  }
  catch ( teo200_EventList EvtLst )
  {
    MSGCD_EVTLST (EvtLst);
    *Buffer = NULL;
  }

  *Ok = (tsp00_Bool)( *Buffer != NULL );

  return;
}

//-----------------------------------------------------------------------------

extern "C"  void sqlfree ( tsp00_Uint1*  Buffer )
{
  ROUTINE_DBG_MEO60 (_T("sqlfree"));
   
  DBGIN;
  
  try 
  {
    teo570_GlobalAlloc::eo570_Deallocate( Buffer, __FILE__,__LINE__, NO_HINT_EO570 );
  }
  catch ( teo200_EventList EvtLst )
  {
    MSGCD_EVTLST (EvtLst);
  }

  return;
}

//-----------------------------------------------------------------------------

extern "C"  void sqlreallocat  ( tsp00_Int4        Size,
                                 tsp00_Uint1**   Buffer,
                                 tsp00_BoolAddr  Ok )
{
  #if defined(_WIN32)
   if ( Pool == NULL )
     eo572_CreateAllocPool ();
  #endif

  try 
  {
    *Buffer = (tsp00_Uint1*)teo570_GlobalAlloc::eo570_Reallocate( *Buffer, Size, 
                                                                  __FILE__,__LINE__, 
                                                                  NO_HINT_EO570 );
  }
  catch ( teo200_EventList EvtLst )
  {
    MSGCD_EVTLST (EvtLst);
    *Buffer = NULL;
  }

  *Ok = (tsp00_Bool)( *Buffer != NULL );

  return;
}

//-----------------------------------------------------------------------------

extern "C"  void sqlvalidatealloc  ( tsp00_Uint1*   Buffer,
                                     tsp00_BoolAddr  Ok )
{
  #if defined(_WIN32)
   if ( Pool == NULL )
     eo572_CreateAllocPool ();
  #endif

  *Ok = (tsp00_Bool)teo570_GlobalAlloc::eo570_Validate( Buffer, NO_HINT_EO570 );

  return;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

#if defined(_WIN32)

 t_AllocPool::t_AllocPool ( teo200_EventList *       pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_AllocPool::t_AllocPool"));

   teo001_SystemRc     rc;

   m_Heap = HeapCreate(0, 1, 0);

   if ( m_Heap == NULL )
   {
     rc = GetLastError();
     
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CREATE_HEAP, rc );
     
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
   }
 }


 //-----------------------------------------------------------------------------

 t_AllocPool::~t_AllocPool ()
 {
   ROUTINE_DBG_MEO60 (_T("t_AllocPool::~t_AllocPool"));

   if ( m_Heap != NULL)
     HeapDestroy( m_Heap );
 }

 //-----------------------------------------------------------------------------

 void * t_AllocPool::eo570_Allocate ( size_t                    Size, 
                                      char const * const        FileName,
                                      unsigned long             LineNo,
                                      void const *              PoolHint,
                                      teo200_EventList *        pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_AllocPool::eo570_Allocate"));
   
   void *              Buffer;
   teo001_SystemRc     rc;
   
   Buffer = HeapAlloc( m_Heap, 0, Size );
   
   if ( Buffer == NULL )
   {
     rc = GetLastError();
     
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_ALLOC_MEMORY__FILE_LINE__, 
                              rc, FileName, LineNo );
     
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
   }
   
   return Buffer;
 }
 
 //-----------------------------------------------------------------------------
 
 void * t_AllocPool::eo570_Reallocate ( void *                    OldBuffer,
                                        size_t                    NewSize,
                                        char const * const        FileName,
                                        unsigned long             LineNo,
                                        void const *              PoolHint,
                                        teo200_EventList *        pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_AllocPool::eo570_Reallocate"));
   
   void *             NewBuffer;
   teo001_SystemRc    rc;
   
   NewBuffer = HeapReAlloc( m_Heap, 0, OldBuffer, NewSize ); 
   
   if ( NewBuffer == NULL )
   {
     rc = GetLastError();
     
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_REALLOC_MEMORY__FILE_LINE__, 
                              rc, FileName, LineNo );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
   }
   
   return NewBuffer;
 }
 
 //-----------------------------------------------------------------------------
 
 void t_AllocPool::eo570_Deallocate ( void *                    Buffer,
                                      char const * const        FileName,
                                      unsigned long             LineNo,
                                      void const *              PoolHint,
                                      teo200_EventList *        pEventList )
 {
   ROUTINE_DBG_MEO60 (_T("t_AllocPool::eo570_Deallocate"));

   teo001_SystemRc    rc;
   
   if ( HeapFree( m_Heap, 0, Buffer ) == FALSE )
   {
     rc = GetLastError();
     
     teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_REALLOC_MEMORY__FILE_LINE__, 
                              rc, FileName, LineNo );
     COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
   }
   
   return;
 }

 /*---------------------------------------------------------------------------*/

 bool t_AllocPool::eo570_Validate( void *              Buffer,
                                   void const *        PoolHint )
 {
   ROUTINE_DBG_MEO60 (_T("t_AllocPool::eo570_Validate"));

   return (HeapValidate( m_Heap, 0, Buffer ) == TRUE);
 }

#else /* UNIX */

#endif


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/