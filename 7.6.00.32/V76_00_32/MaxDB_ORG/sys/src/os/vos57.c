/*!
  @file           vos57.c
  @author         RaymondR
  @brief          Basic RTE memory management
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




//
//  INCLUDE FILES
//
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"


//
//  DEFINES
//

#define MOD__  "VOS57C : "
#define MF__   MOD__"UNDEFINED"

//
//  MACROS
//

//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//
#if defined(_WIN32)
 typedef HANDLE              POOLTYP;
#else
 typedef PVOID               POOLTYP;
#endif

#if ALIGNMENT_VALUE < 4
 #define INFOAREA_SIZE       sizeof(ULONG)
#else
 #define INFOAREA_SIZE       ALIGNMENT_VALUE
#endif

#define COMP_NAME_LEN        30
#define COMP_NAME_FORMAT     "%30.30s"
//
// EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
// LOCAL VARIABLES
//
static POOLTYP    pvAllocPool   = NULL;


//
// LOCAL FUNCTION PROTOTYPES
//

#if defined (DEBUG_MEM_ALLOC)
 static VOID sql57_compress_path ( PSZ    pszCompPathName,
                                   PSZ    pszPathName,
                                   ULONG  ulCompPathLen );
#endif


//
// ========================== GLOBAL FUNCTIONS ================================
//


APIRET  sql57c_malloc   ( ULONG   ulLine,
                          PSZ     pszModuleName,
                          PPVOID  ppvMemBlock,
                          ULONG   ulSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql57c_malloc"

  static BOOL     fMessOutputPending = FALSE;
  APIRET          rc                 = NO_ERROR;
  ULONG           ulInfoAreaSize;
  #if defined (DEBUG_MEM_ALLOC)
   CHAR           szCompModName[COMP_NAME_LEN + 1];
  #endif


  DBGIN;

  DBG3  (( MF__, "Memory size requested: %ld", ulSize ));

  if ( !pvAllocPool )
    {
    #if defined(_WIN32)
     pvAllocPool = HeapCreate(0, 1, 0);

     if  (pvAllocPool == NULL)
       rc = ERROR_NOT_ENOUGH_MEMORY;
    #else
      rc = DosAllocMem (&pvAllocPool, MAX_ALLOC_POOL_SIZE,
                        PAG_READ | PAG_WRITE);
    #endif

    if ( rc != NO_ERROR )
      {
      // --- message output might produce additional allocation errors
      if ( !fMessOutputPending )
        {
        fMessOutputPending = TRUE;
        DBG1  (( MF__, "No more memory" ));
        MSGCD (( ERR_ALLOC_MEMORY, rc   ));
        fMessOutputPending = FALSE;
        }

      pvAllocPool     = NULL;
      *ppvMemBlock    = NULL;

      DBGOUT;
      return ( rc );
      }

    #if !defined(_WIN32)
     rc = DosSubSetMem ( pvAllocPool, DOSSUB_INIT | DOSSUB_SPARSE_OBJ,
                         MAX_ALLOC_POOL_SIZE );
     if ( rc != NO_ERROR )
       {
       // --- message output might produce additional allocation errors
       if ( !fMessOutputPending )
         {
         fMessOutputPending = TRUE;
         DBG1  (( MF__, "No more memory" ));
         MSGCD (( ERR_CANT_SUBSET_MEMORY, rc ));
         fMessOutputPending = FALSE;
         }

       DosFreeMem ( pvAllocPool );

       pvAllocPool   = NULL;
       *ppvMemBlock  = NULL;

       DBGOUT;
       return ( rc );
       }
    #endif
    }

  ulInfoAreaSize = ALIGN ( sizeof(ULONG), ALIGNMENT_VALUE );
  ulSize        += ulInfoAreaSize;

  if ( ulSize <= ulInfoAreaSize )
     rc = ERROR_INVALID_PARAMETER;
  else
    {
    #if defined(_WIN32)
     *ppvMemBlock = HeapAlloc(pvAllocPool, HEAP_ZERO_MEMORY, ulSize);

     if ( *ppvMemBlock == NULL )
       rc = ERROR_NOT_ENOUGH_MEMORY;
    #else
     rc  = DosSubAllocMem  ( pvAllocPool, ppvMemBlock, ulSize );
    #endif
    }

  if ( rc != NO_ERROR )
    {
    // --- message output might produce additional allocation errors
    if ( !fMessOutputPending )
      {
      fMessOutputPending = TRUE;
      DBG1  (( MF__, "No more memory" ));
      MSGCD (( ERR_CANT_ALLOC_HEAP_MEMORY, rc ));
      fMessOutputPending = FALSE;
      }

    *ppvMemBlock  = NULL;

    DBGOUT;
    return ( rc );
    }

  **( ULONG** ) ppvMemBlock  = ulSize;
  *( char** ) ppvMemBlock   += ulInfoAreaSize;

  #if defined (DEBUG_MEM_ALLOC)
   sql57_compress_path ( szCompModName, pszModuleName, COMP_NAME_LEN );

   MSGD(( 19999,INFO_TYPE, "ALLOC   ",
          "'%4d':'"COMP_NAME_FORMAT"' - 0x%08x, size: %d",
          ulLine, szCompModName, *ppvMemBlock, ulSize ));
  #endif


  DBGOUT;
  return ( rc );
  }


/*------------------------------*/


APIRET  sql57c_free ( ULONG   ulLine,
                      PSZ     pszModuleName,
                      PVOID   pvMemBlock )
  {
  #undef  MF__
  #define MF__ MOD__"sql57c_free"

  APIRET          rc       = NO_ERROR;
  ULONG           ulSize   = 0;
  ULONG           ulInfoAreaSize;
  #if defined (DEBUG_MEM_ALLOC)
   CHAR           szCompModName[COMP_NAME_LEN + 1];
  #endif


  DBGIN;

  if (( pvMemBlock ) && ( pvAllocPool ))
    {
    ulInfoAreaSize = ALIGN ( sizeof(ULONG), ALIGNMENT_VALUE );
    pvMemBlock     = (char*) pvMemBlock - ulInfoAreaSize;
    ulSize         = *(ULONG*) pvMemBlock;

    #if defined (DEBUG_MEM_ALLOC)
     sql57_compress_path ( szCompModName, pszModuleName, COMP_NAME_LEN );

     MSGD(( 19999,INFO_TYPE, "FREE    ",
            "'%4d':'"COMP_NAME_FORMAT"' - 0x%08x, size: %d",
            ulLine, szCompModName,
            (char*)pvMemBlock + ulInfoAreaSize, ulSize));
    #endif



    #if defined(_WIN32)
     if ( !HeapFree(pvAllocPool, 0, pvMemBlock) )
       rc = ERROR_INVALID_BLOCK;
    #else
     rc = DosSubFreeMem ( pvAllocPool, pvMemBlock, ulSize );
    #endif
    }
  else
    {
    rc = ERROR_INVALID_BLOCK;

    #if defined (DEBUG_MEM_ALLOC)
     sql57_compress_path ( szCompModName, pszModuleName, COMP_NAME_LEN );

     MSGD(( 19999,INFO_TYPE, "FREE    ",
            "'%4d':'"COMP_NAME_FORMAT"' - NULL",
            ulLine, szCompModName ));
    #endif
    }

  if ( rc != NO_ERROR)
    {
    DBG1  (( MF__, "Can not free memory" ));
    MSGD (( ERR_CANT_FREE_HEAP_MEMORY, rc ));
    }


  DBGOUT;

  return ( rc );
  }

/*------------------------------*/

APIRET  sql57c_realloc  ( ULONG   ulLine,
                          PSZ     pszModuleName,
                          PPVOID  ppvMemBlock,
                          ULONG   ulSize )
  {
  #undef  MF__
  #define MF__ MOD__"sql57c_realloc"

  PCHAR           pcOldMemBlock      = (PCHAR ) *ppvMemBlock ;
  PULONG          pulOldMemBlockSize  ;
  APIRET          rc                 = NO_ERROR;

  DBGIN;

  DBG3  (( MF__, "Memory size requested: %ld", ulSize ));
  rc = sql57c_malloc ( ulLine, pszModuleName, ppvMemBlock, ulSize );

  if ( rc == NO_ERROR )
    {
    pulOldMemBlockSize   = (PULONG )(( char* ) pcOldMemBlock - ALIGN ( sizeof(ULONG), ALIGNMENT_VALUE ));
    *pulOldMemBlockSize -= ALIGN ( sizeof(ULONG), ALIGNMENT_VALUE ) ;

    SAPDB_memcpy ( (char *) *ppvMemBlock, (char *) pcOldMemBlock, *pulOldMemBlockSize ) ;

    rc = sql57c_free ( ulLine, pszModuleName,( char* ) pcOldMemBlock);
    }

  DBGOUT;

  return ( rc );
  }

/*------------------------------*/

VOID sql57c_heap_destroy ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql57c_heap_destroy"

  if ( pvAllocPool )
    {
    #if defined(_WIN32)
     HeapDestroy (pvAllocPool);
     pvAllocPool = NULL;
    #endif
    }

  DBGOUT;

  return;
  }


//
// ========================== LOCAL FUNCTIONS =================================
//


//
// =============================== END ========================================
//
