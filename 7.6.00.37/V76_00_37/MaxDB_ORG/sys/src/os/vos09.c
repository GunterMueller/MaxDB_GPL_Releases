/*!
  @file           vos09.c
  @author         RaymondR
  @brief          handle manager
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo46.h"

/*
 * DEFINES
 */
#define MOD__  "VOS09C : "
#define MF__   MOD__"UNDEFINED"

#define   HANDLES_PER_PAGE    ( 2048 / sizeof (HANDLE_INFO_REC))

#define H_FREE                0x0
#define H_USED                0x1
#define H_DATA_VALID          0x2
#define H_FREE_REQUEST        0x4

/*
 * MACROS
 */
#if defined(_WIN32)
 #define  INIT_CRIT_SEC(_pCritSec)         InitializeCriticalSection(_pCritSec)
 #define  ENTER_CRIT_SEC(_pCritSec)        EnterCriticalSection(_pCritSec)
 #define  EXIT_CRIT_SEC(_pCritSec)         LeaveCriticalSection(_pCritSec)
 #define  DELETE_CRIT_SEC(_pCritSec)       DeleteCriticalSection(_pCritSec)
#endif

/*
 * LOCAL TYPE AND STRUCT DEFINITIONS
 */
typedef struct handle_info_record
  {
  USHORT                          usState;
  USHORT                          usLockCnt;

  CRITICAL_SECTION                CritSec;

  PVOID                           pData;
  ULONG                           ulHType;
  ULONG                           ulID;
  } HANDLE_INFO_REC;

typedef HANDLE_INFO_REC           *PHANDLE_INFO_REC;


typedef struct handle_page_record
  {
  ULONG                           ulHandlesUsed;
  HANDLE_INFO_REC                 HandleInfo[HANDLES_PER_PAGE];
  } HANDLE_PAGE_REC;

typedef HANDLE_PAGE_REC           *PHANDLE_PAGE_REC;

typedef struct handle_page_dir_record
  {
  LONG                            lExclusive;
  ULONG                           ulEmptyPages;
  ULONG                           ulMaxPages;
  PHANDLE_PAGE_REC                HandlePage[1];
  } HANDLE_PAGE_DIR_REC;

typedef HANDLE_PAGE_DIR_REC       *PHANDLE_PAGE_DIR_REC;


//
//
//    Handle Page Directory
// +--------------------------+
// |  MaxPages                |            Handle Page 1
// +--------------------------+         +---------------------------------+
// |  HandlePage (1)          |-------->|  HandleInfo (1)                 |
// +--------------------------+         +---------------------------------+
// |  HandlePage (2)          |---+     .                                 .
// +--------------------------+   |     +---------------------------------+
// .                          .   |     |  HandleInfo (HANDLES_PER_PAGE)  |
// .                          .   |     +---------------------------------+
// .                          .   |
// .                          .   |
// .                          .   |
// .                          .   |        Handle Page 2
// .                          .   |     +---------------------------------+
// +--------------------------+   +---->|  HandleInfo (1)                 |
// |  HandlePage (MaxPages-1) |->NULL   +---------------------------------+
// +--------------------------+         .                                 .
// |  HandlePage (MaxPages)   |->NULL   +---------------------------------+
// +--------------------------+         |  HandleInfo (HANDLES_PER_PAGE)  |
//                                      +---------------------------------+
//


/*
 * EXTERNAL VARIABLES
 */


/*
 * EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

BOOL sql09c_init_handle_dir ( PVOID             *ppHandles,
                              ULONG             ulMaxHandles )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_init_handle_dir"
  ULONG                 rc = NO_ERROR;
  PHANDLE_PAGE_DIR_REC  pHandleDir;
  ULONG                 ulMaxPages;
  ULONG                 ulDirSize;

  *ppHandles = NULL;

  ulMaxPages = ((ulMaxHandles - 1) / HANDLES_PER_PAGE ) + 1;

  ulDirSize = sizeof(HANDLE_PAGE_DIR_REC) +
              (sizeof(PHANDLE_PAGE_REC) * ulMaxPages - 1);

  rc = ALLOC_MEM ( &pHandleDir, ulDirSize );

  if ( rc == NO_ERROR )
    {
    memset ( pHandleDir, 0, ulDirSize);

    *ppHandles             = (PVOID)pHandleDir;
    pHandleDir->ulMaxPages = ulMaxPages;

    DBGOUT;
    return ( TRUE );
    }


  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql09c_release_handle_dir ( PVOID               pHandles )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_release_handle_dir"

  ULONG                 rc = NO_ERROR;
  ULONG                 ulPage;
  PHANDLE_PAGE_DIR_REC  pHandleDir = (PHANDLE_PAGE_DIR_REC)pHandles;

  for ( ulPage = 0;
        (pHandleDir != NULL) && (ulPage < pHandleDir->ulMaxPages);
        ulPage++ )
  {
    if ( pHandleDir->HandlePage[ulPage] != NULL ) 
    {
      if ( rc == NO_ERROR )
        rc = FREE_MEM ( pHandleDir->HandlePage[ulPage] );
      else
        FREE_MEM ( pHandleDir->HandlePage[ulPage] );

      pHandleDir->HandlePage[ulPage] = NULL;
    } 
  }

  if ( rc == NO_ERROR )
    rc = FREE_MEM ( pHandleDir );
  else
    FREE_MEM ( pHandleDir );

  if ( rc == NO_ERROR )
    {
    DBGOUT;
    return ( TRUE );
    }

  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql09c_reserve_handle ( PVOID                   pHandles,
                             PULONG                  pulHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_reserve_handle"
  ULONG                 rc = NO_ERROR;
  ULONG                 ulIdx;
  ULONG                 ulPage;
  PHANDLE_PAGE_REC      pHandlePage      = NULL;
  PHANDLE_PAGE_REC      pNewHandlePage   = NULL;
  BOOL                  fNewPageInserted = FALSE;
  PHANDLE_PAGE_DIR_REC  pHandleDir       = (PHANDLE_PAGE_DIR_REC)pHandles;

  if ( !pHandleDir )
    {
    DBGOUT;
    return ( FALSE );
    }

  for ( ulPage = 0, *pulHandle = 0;
        (ulPage < pHandleDir->ulMaxPages) && (*pulHandle == 0);
        ulPage++ )
    {
    if ( pHandleDir->HandlePage[ulPage] == NULL ) // - dirty read
      {
      // --- Allocate a new handle page
      //
      rc = ALLOC_MEM ( &pNewHandlePage, sizeof(HANDLE_PAGE_REC) );

      if ( rc != NO_ERROR )
        break;

      memset ( pNewHandlePage, 0, sizeof(HANDLE_PAGE_REC) );
      }


    WAIT_UNTIL_ALONE( &pHandleDir->lExclusive );

    if ( pHandleDir->HandlePage[ulPage] == NULL )
      {
      // --- Insert a new handle page
      pHandleDir->HandlePage[ulPage] = pNewHandlePage;
      fNewPageInserted               = TRUE;

      pHandlePage = pHandleDir->HandlePage[ulPage];

      pHandlePage->ulHandlesUsed++;

      #if defined(_WIN32)
       INIT_CRIT_SEC( &pHandlePage->HandleInfo[0].CritSec );
      #endif

      pHandlePage->HandleInfo[0].usState   = H_USED;
      pHandlePage->HandleInfo[0].usLockCnt = 0;
      pHandlePage->HandleInfo[0].pData     = NULL;
      pHandlePage->HandleInfo[0].ulHType   = (ULONG)UNDEF;
      pHandlePage->HandleInfo[0].ulID      = (ULONG)UNDEF;
      *pulHandle                           = (ulPage * HANDLES_PER_PAGE)+ 1;
      }
    else
      {
      pHandlePage = pHandleDir->HandlePage[ulPage];

      for ( ulIdx = 0; ulIdx < HANDLES_PER_PAGE; ulIdx++ )
        {
        if ( pHandlePage->HandleInfo[ulIdx].usState != H_FREE )
          continue;

        if ( pHandlePage->ulHandlesUsed == 0 )
          pHandleDir->ulEmptyPages--;

        pHandlePage->ulHandlesUsed++;

        #if defined(_WIN32)
         INIT_CRIT_SEC( &pHandlePage->HandleInfo[ulIdx].CritSec );
        #endif

        pHandlePage->HandleInfo[ulIdx].usState   = H_USED;
        pHandlePage->HandleInfo[ulIdx].usLockCnt = 0;
        pHandlePage->HandleInfo[ulIdx].pData     = NULL;
        pHandlePage->HandleInfo[ulIdx].ulHType   = (ULONG)UNDEF;
        pHandlePage->HandleInfo[ulIdx].ulID      = (ULONG)UNDEF;
        *pulHandle                               = (ulPage * HANDLES_PER_PAGE) +
                                                   ulIdx + 1;
        break;
        }
      }
    CLEARLOCK( &pHandleDir->lExclusive );

    if (( pNewHandlePage != NULL) && ( fNewPageInserted == FALSE ))
      {
      // --- Any one else has aready inserted a new handle page
      FREE_MEM ( pNewHandlePage );
      }
    }

  // --- no free handle found?
  if ( *pulHandle == 0 )
    {
    DBGOUT;
    return ( FALSE );
    }

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

BOOL sql09c_get_handle_data ( PVOID                   pHandles,
                              ULONG                   ulHandle,
                              PVOID                   *ppData,
                              PULONG                  pulHType,
                              PULONG                  pulID )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_get_handle_data"
  ULONG                 ulPage;
  ULONG                 ulIdx;
  PHANDLE_PAGE_DIR_REC  pHandleDir = (PHANDLE_PAGE_DIR_REC)pHandles;
  PHANDLE_PAGE_REC      pHandlePage;

  if (( !pHandleDir ) || ( ulHandle == 0 ))
    {
    DBGOUT;
    return ( FALSE );
    }

  ulPage = (ulHandle - 1) / HANDLES_PER_PAGE;
  ulIdx  = (ulHandle - 1) % HANDLES_PER_PAGE;

  pHandlePage = pHandleDir->HandlePage[ulPage];

  if (( pHandleDir->ulMaxPages                 >  ulPage  ) &&
      ( pHandlePage                            != NULL    ) &&
      ( pHandlePage->HandleInfo[ulIdx].usState & H_DATA_VALID ))
    {
    *ppData   = pHandlePage->HandleInfo[ulIdx].pData;
    *pulHType = pHandlePage->HandleInfo[ulIdx].ulHType;
    *pulID    = pHandlePage->HandleInfo[ulIdx].ulID;

    DBGOUT;
    return ( TRUE );
    }

  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql09c_put_handle_data ( PVOID                   pHandles,
                              ULONG                   ulHandle,
                              PVOID                   pData,
                              ULONG                   ulHType,
                              ULONG                   ulID )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_put_handle_data"
  ULONG                 ulPage;
  ULONG                 ulIdx;
  PHANDLE_PAGE_DIR_REC  pHandleDir = (PHANDLE_PAGE_DIR_REC)pHandles;
  PHANDLE_PAGE_REC      pHandlePage;

  if (( !pHandleDir ) || ( ulHandle == 0 ))
    {
    DBGOUT;
    return ( FALSE );
    }

  ulPage = (ulHandle - 1) / HANDLES_PER_PAGE;
  ulIdx  = (ulHandle - 1) % HANDLES_PER_PAGE;

  pHandlePage = pHandleDir->HandlePage[ulPage];

  if (( pHandleDir->ulMaxPages                 >  ulPage ) &&
      ( pHandlePage                            != NULL   ) &&
      ( pHandlePage->HandleInfo[ulIdx].usState != H_FREE ))
    {
    // - set new data
    pHandlePage->HandleInfo[ulIdx].ulHType = ulHType;
    pHandlePage->HandleInfo[ulIdx].ulID    = ulID;
    pHandlePage->HandleInfo[ulIdx].pData   = pData;

    pHandlePage->HandleInfo[ulIdx].usState |= H_DATA_VALID;

    DBGOUT;
    return ( TRUE );
    }

  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql09c_find_next_handle ( PVOID                   pHandles,
                               PULONG                  pulHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_find_next_handle"
  ULONG                 ulPage;
  ULONG                 ulIdx;
  PHANDLE_PAGE_DIR_REC  pHandleDir = (PHANDLE_PAGE_DIR_REC)pHandles;

  if ( !pHandleDir )
    {
    DBGOUT;
    return ( FALSE );
    }

  // --- start with next handle value
  (*pulHandle)++;

  ulPage = (*pulHandle - 1) / HANDLES_PER_PAGE;
  ulIdx  = (*pulHandle - 1) % HANDLES_PER_PAGE;

  while ( pHandleDir->ulMaxPages > ulPage )
    {
    // --- page not found?
    if ( pHandleDir->HandlePage[ulPage] == NULL )
      {
      ulPage++;    // --- skip this entry!
      ulIdx = 0;
      continue;
      }

    WAIT_UNTIL_ALONE( &pHandleDir->lExclusive );

    if ( pHandleDir->HandlePage[ulPage] != NULL )
      {
      // --- search for a used handle entry in this page
      for (; ulIdx < HANDLES_PER_PAGE; ulIdx++ )
        {
        if( pHandleDir->HandlePage[ulPage]->HandleInfo[ulIdx].usState != H_FREE )
          {
          *pulHandle = (ulPage * HANDLES_PER_PAGE) + ulIdx + 1;

          CLEARLOCK( &pHandleDir->lExclusive );
          DBGOUT;
          return ( TRUE );
          }
        }
      }

    CLEARLOCK( &pHandleDir->lExclusive );

    // --- next page
    ulPage++;
    ulIdx = 0;
    }

  *pulHandle = (ULONG)UNDEF;

  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql09c_find_handle_by_id ( PVOID                   pHandles,
                                ULONG                   ulID,
                                PULONG                  pulHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_find_handle_by_id"
  ULONG                 ulPage;
  ULONG                 ulIdx;
  PHANDLE_PAGE_DIR_REC  pHandleDir = (PHANDLE_PAGE_DIR_REC)pHandles;
  PHANDLE_INFO_REC      pHandleInfo;

  if ( !pHandleDir )
    {
    DBGOUT;
    return ( FALSE );
    }

  // --- start with next handle value
  (*pulHandle)++;

  ulPage = (*pulHandle - 1) / HANDLES_PER_PAGE;
  ulIdx  = (*pulHandle - 1) % HANDLES_PER_PAGE;

  while ( pHandleDir->ulMaxPages > ulPage )
    {
    // --- page not found?
    if ( pHandleDir->HandlePage[ulPage] == NULL )
      {
      ulPage++;    // --- skip this entry!
      ulIdx = 0;
      continue;
      }

    WAIT_UNTIL_ALONE( &pHandleDir->lExclusive );

    if ( pHandleDir->HandlePage[ulPage] != NULL )
      {
      // --- search for a used handle entry in this page
      for (; ulIdx < HANDLES_PER_PAGE; ulIdx++ )
        {
        pHandleInfo = &pHandleDir->HandlePage[ulPage]->HandleInfo[ulIdx];

        if (( pHandleInfo->usState != H_FREE  ) &&
            ( pHandleInfo->ulID    == ulID    ))
          {
          *pulHandle = (ulPage * HANDLES_PER_PAGE) + ulIdx + 1;
          CLEARLOCK( &pHandleDir->lExclusive );
          DBGOUT;
          return ( TRUE );
          }
        }
      }

    CLEARLOCK( &pHandleDir->lExclusive );

    // --- next page
    ulPage++;
    ulIdx = 0;
    }

  *pulHandle = (ULONG)UNDEF;

  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql09c_lock_handle ( PVOID                   pHandles,
                          ULONG                   ulHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_lock_handle"
  ULONG                 ulPage;
  ULONG                 ulIdx;
  PHANDLE_PAGE_DIR_REC  pHandleDir = (PHANDLE_PAGE_DIR_REC)pHandles;
  PHANDLE_PAGE_REC      pHandlePage;

  if (( !pHandleDir ) || ( ulHandle == 0 ))
    {
    DBGOUT;
    return ( FALSE );
    }

  ulPage = (ulHandle - 1) / HANDLES_PER_PAGE;
  ulIdx  = (ulHandle - 1) % HANDLES_PER_PAGE;


  WAIT_UNTIL_ALONE( &pHandleDir->lExclusive );

  pHandlePage = pHandleDir->HandlePage[ulPage];

  if (( pHandleDir->ulMaxPages                 <= ulPage ) ||
      ( pHandlePage                            == NULL   ) ||
      ( pHandlePage->HandleInfo[ulIdx].usState == H_FREE ) ||
      ( pHandlePage->HandleInfo[ulIdx].usState &  H_FREE_REQUEST ))
    {
    CLEARLOCK( &pHandleDir->lExclusive );
    DBGOUT;
    return ( FALSE );
    }

  pHandlePage->HandleInfo[ulIdx].usLockCnt++;

  if ( pHandlePage->HandleInfo[ulIdx].usLockCnt == 1 )
    ENTER_CRIT_SEC( &pHandlePage->HandleInfo[ulIdx].CritSec );

  CLEARLOCK( &pHandleDir->lExclusive );

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

BOOL sql09c_unlock_handle ( PVOID                   pHandles,
                            ULONG                   ulHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_unlock_handle"
  ULONG                 ulPage;
  ULONG                 ulIdx;
  PHANDLE_PAGE_DIR_REC  pHandleDir = (PHANDLE_PAGE_DIR_REC)pHandles;
  PHANDLE_PAGE_REC      pHandlePage;

  if (( !pHandleDir ) || ( ulHandle == 0 ))
    {
    DBGOUT;
    return ( FALSE );
    }

  ulPage = (ulHandle - 1) / HANDLES_PER_PAGE;
  ulIdx  = (ulHandle - 1) % HANDLES_PER_PAGE;


  WAIT_UNTIL_ALONE( &pHandleDir->lExclusive );

  pHandlePage = pHandleDir->HandlePage[ulPage];

  if (( pHandleDir->ulMaxPages                 <= ulPage ) ||
      ( pHandlePage                            == NULL   ) ||
      ( pHandlePage->HandleInfo[ulIdx].usState == H_FREE ))
    {
    CLEARLOCK( &pHandleDir->lExclusive );
    DBGOUT;
    return ( FALSE );
    }

  pHandlePage->HandleInfo[ulIdx].usLockCnt--;

  if ( pHandlePage->HandleInfo[ulIdx].usLockCnt == 0 )
    EXIT_CRIT_SEC( &pHandlePage->HandleInfo[ulIdx].CritSec );

  CLEARLOCK( &pHandleDir->lExclusive );


  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

BOOL sql09c_free_handle  ( PVOID                   pHandles,
                           ULONG                   ulHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql09c_free_handle"
  ULONG                 ulPage;
  ULONG                 ulIdx;
  PHANDLE_PAGE_REC      pHandlePage = NULL;
  PHANDLE_PAGE_DIR_REC  pHandleDir  = (PHANDLE_PAGE_DIR_REC)pHandles;
  BOOL                  fRet        = FALSE;
  PHANDLE_INFO_REC      pHandleInfo;

  if (( !pHandleDir ) || ( ulHandle == 0 ))
    {
    DBGOUT;
    return ( FALSE );
    }

  ulPage = (ulHandle - 1) / HANDLES_PER_PAGE;
  ulIdx  = (ulHandle - 1) % HANDLES_PER_PAGE;

  //
  // -- 'sql09c_reserve_handle' and 'sql09c_free_handle' must be done
  //    by the same thread!!!!!
  //
  if (( pHandleDir->ulMaxPages         >  ulPage ) &&
      ( pHandleDir->HandlePage[ulPage] != NULL   ))
    {
    WAIT_UNTIL_ALONE( &pHandleDir->lExclusive );

    pHandlePage = pHandleDir->HandlePage[ulPage];
    pHandleInfo = &pHandlePage->HandleInfo[ulIdx];

    if ( pHandlePage->HandleInfo[ulIdx].usState != H_FREE )
      {
      // --- signal that a free request is pending
      pHandleInfo->usState |= H_FREE_REQUEST;

     #if defined(_WIN32)
      if ( pHandlePage->HandleInfo[ulIdx].usLockCnt > 0 )
        {
        CLEARLOCK( &pHandleDir->lExclusive );

        ENTER_CRIT_SEC( &pHandlePage->HandleInfo[ulIdx].CritSec );

        WAIT_UNTIL_ALONE( &pHandleDir->lExclusive );

        EXIT_CRIT_SEC( &pHandlePage->HandleInfo[ulIdx].CritSec );
        }
      #endif

      pHandlePage->ulHandlesUsed--;
      pHandleInfo->pData   = NULL;
      pHandleInfo->usState = H_FREE;
      pHandleInfo->ulID    = (ULONG)UNDEF;
      pHandleInfo->ulHType = (ULONG)UNDEF;
      fRet                 = TRUE;

      #if defined(_WIN32)
       DELETE_CRIT_SEC( &pHandlePage->HandleInfo[ulIdx].CritSec );
      #endif
      }

    if ( pHandlePage->ulHandlesUsed == 0 )
      {
      pHandleDir->ulEmptyPages++;

      // --- free the handle page if there are more then one empty pages
      //
      if ( pHandleDir->ulEmptyPages > 1 )
        {
        pHandleDir->ulEmptyPages--;
        pHandleDir->HandlePage[ulPage] = NULL;
        }
      else
        pHandlePage = NULL;
      }
    else
      pHandlePage = NULL;

    CLEARLOCK( &pHandleDir->lExclusive );

    if ( pHandlePage != NULL )
      {
      FREE_MEM ( pHandlePage );
      }
    }


  DBGOUT;
  return ( fRet );
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*
 * =============================== END ========================================
 */
