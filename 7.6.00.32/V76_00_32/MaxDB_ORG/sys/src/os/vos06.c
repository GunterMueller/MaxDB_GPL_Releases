/*!
  @file           vos06.c
  @author         RaymondR
  @brief          Virtual File ( Common Part )
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




///
//  INCLUDE FILES
//
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"

#if defined (KERNEL)
# include "gos00k.h"            /* nocheck */  
#endif

#include "geo007_1.h"
#include "geo007_2.h"
#include "hos06.h"
#include "gsp01.h"
#include "heo46.h"
#include "gos44.h"


//
//  DEFINES
//

#define MOD__  "VOS06C : "
#define MF__   MOD__"UNDEFINED"

#if defined(_WIN32)
 #define FILE_CREATED           1
 #define FILE_EXISTED           2

 #define TAPE_ERROR_RETRY                          2
 #define MAX_TAPE_LOAD_RETRY_TIME                 60

 #if defined (KERNEL)
  #define  SECURITY_ATTR        &kgs.AdminSA
 #else
  #define  SECURITY_ATTR        NULL
 #endif
#endif

#define SEP_SET_SIZE            sizeof ( SepSet ) / sizeof ( SepSet [ 0 ] )
#define IGN_SET_SIZE            sizeof ( IgnSet ) / sizeof ( IgnSet [ 0 ] )


//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//
static CHAR                     SepSet [] = { 0, 0, 1, 1, 1, 0, 0, 0,
                                              0, 0, 1, 1, 1, 1, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 1, 0, 0, 1, 1, 1, 1 };

static CHAR                     IgnSet [] = { 1, 1, 0, 0, 0, 1, 1, 1,
                                              1, 0, 0, 0, 0, 0, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 0, 1, 1, 0, 0, 0, 0 };


//
//  LOCAL FUNCTION PROTOTYPES
//
LONG sql06_io_read              ( HANDLE               hfDevice,
                                         ULONG                ulDevType,
                                         PVOID                pBlockPtr,
                                         ULONG                ulBlockLength,
                                         PULONG               pulBytesRead );
LONG sql06_io_write             ( HANDLE               hfDevice,
                                         ULONG                ulDevType,
                                         PVOID                pBlockPtr,
                                         ULONG                ulBlockLength,
                                         PULONG               pulBytesWritten );
bool sql06c_handle_known_errors ( PSZ                  pszName,
                                         LONG                 rc,
                                         PCHAR                pcErrText);

#if defined (_WIN32)
 static LONG sql06c_init_dlt_tape       ( PHOSTFILE_CTRL_REC   pHF ) ;

 static LONG sql06c_get_drive_info      ( PHOSTFILE_CTRL_REC   pHF,
                                          PCHAR                pcErrText);

 static LONG sql06c_get_media_info      ( PHOSTFILE_CTRL_REC   pHF,
                                          PCHAR                pcErrText);

 static LONG sql06c_init_tape_dev       ( PHOSTFILE_CTRL_REC   pHF,
                                          PCHAR                pcErrText);

 static LONG sql06c_tape_rewind         ( PHOSTFILE_CTRL_REC   pHF,
                                          PCHAR                pcErrText);
 static LONG sql06c_tape_locking        ( PHOSTFILE_CTRL_REC   pHF,
                                          PCHAR                pcErrText,
                                          ULONG                ulOperation );
 static LONG sql06c_tape_loading        ( PHOSTFILE_CTRL_REC   pHF,
                                          PCHAR                pcErrText,
                                          ULONG                ulOperation );
 static LONG sql06c_tape_write_mark     ( PHOSTFILE_CTRL_REC   pHF,
                                          PCHAR                pcErrText);
#endif

//
// ========================== GLOBAL FUNCTIONS ================================
//


VOID sql06c_finit   (   INT2                buf_pool_size,
                        INT4*               poolptr,
                        PBOOLEAN            pbFirstCall,
                        PHOSTFILE_CTRL_REC  arcFileCtrlTab,
                        PVOID*              ppBufPool,
                        BOOLEAN*            ok                  )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_finit"

  ULONG   lIndex;

  DBGIN;

  DBG3 (( MF__, "*pbFirstCall    %f", *pbFirstCall  ));
  DBG3 (( MF__, "buf_pool_size   %d", buf_pool_size ));

  if ( *pbFirstCall == TRUE )
    {
    *pbFirstCall = FALSE;

    for ( lIndex = 0; lIndex < HF_MAX; lIndex++ )
      {
      DBG3 (( MF__, "&FileCtrlTab[lIndex] = 0x%x",
                     &( arcFileCtrlTab [ lIndex ] )));
      sql06c_init_file_ctrl ( &( arcFileCtrlTab [ lIndex ] ));
      }

    // allocate memory for the recommanded buffer and memory for the
    // internal management of this buffer
    if ( sql06c_alloc_buffer_pool ( buf_pool_size, ppBufPool ) == NO_ERROR )
      *ok = TRUE;
    else
      *ok = FALSE;

    // --- set buffer pool base pointer
    if ( buf_pool_size > 0 )
      *poolptr = ( INT4 ) (tsp00_Longint)*ppBufPool;
    else
      *poolptr = 0 ;

    DBG3 (( MF__, "*poolptr      = 0x%x", *poolptr ));
    DBG3 (( MF__, "buf_pool_size = %d",   buf_pool_size ));
    }
  else
    {
    *ok = FALSE ;
    }

  DBGOUT;

  return;
  }


/*------------------------------*/


LONG sql06c_alloc_buffer_pool ( ULONG   ulNumOfBlocks,
                                PPVOID  ppBufPool)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_alloc_buffer_pool"
  LONG     rc = NO_ERROR;
  #if !defined(_WIN32)
   LONG     ulSize;
  #endif

  DBGIN;


  #if defined(_WIN32)
    if (!(*ppBufPool = (PVOID)HeapCreate(0, 1, 0)))
        rc = ERROR_NOT_ENOUGH_MEMORY;
  #else
    // --- alloc buffer pool and one block per file for internal use
    ulSize = sizeof (VF_BLOCK) * (ulNumOfBlocks + 1) * HF_MAX;

    if((rc = DosAllocMem(ppBufPool, ulSize,
                         PAG_WRITE | PAG_READ )) != NO_ERROR)
      {
      DBG1 (( MF__, "Cannot allocate memory, rc = %u", rc ));
      MSGD (( ERR_ALLOC_MEMORY, rc                        ));
      }
    else if ((rc = DosSubSetMem(*ppBufPool, DOSSUB_INIT | DOSSUB_SPARSE_OBJ,
                                ulSize)) != NO_ERROR)
      {
      DBG1 (( MF__, "Cannot allocate memory, rc = %u", rc ));
      MSGD (( ERR_ALLOC_MEMORY, rc                        ));
      }
  #endif

  DBGOUT;

  return (rc);
  }

/*------------------------------*/

LONG    sql06c_reserve_buffer   (   PHOSTFILE_CTRL_REC  pHF,
                                    PVOID               pBufPool,
                                    VF_RESOURCE         Resource,
                                    INT2                BufCount,
                                    VF_BUFFERADDRESS*   Block,
                                    PCHAR               pcErrText   )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_reserve_buffer"


  LONG    lError = VF_OK;
  APIRET  rc     = NO_ERROR;


  DBGIN;

  // --- allocate one block for internal use
  #if defined(_WIN32)
   if (!(pHF->pBuffer = HeapAlloc((HANDLE)pBufPool, HEAP_ZERO_MEMORY,
                                   sizeof(VF_BLOCK))))
     {
     rc = ERROR_NOT_ENOUGH_MEMORY;
     }
  #else
   rc = DosSubAllocMem ( pBufPool, (PVOID*) &pHF -> pBuffer, sizeof(VF_BLOCK));
  #endif

  if ( rc != NO_ERROR )
    {
    lError         = VF_NOTOK;
    pHF -> ulState = HFS_ERROR;

    DBG1 (( MF__, ERRMSG_VF_ALLOC_BUFFER_SPACE ));
    MSGD (( ERR_ALLOC_MEMORY, rc               ));
    sql46c_build_error_string ( pcErrText,
                                ERRMSG_VF_ALLOC_BUFFER_SPACE, rc );

    DBGOUT;

    return (lError);
    }

  switch ( Resource )
    {
    case VF_STACK:
      DBG3 (( MF__, "Resource == VF_STACK" ));
      pHF->Resource = VF_STACK;

      *Block             = ( VF_BUFFERADDRESS ) 0;
      pHF->lSizeOfBlocks = sizeof ( VF_BLOCK );
      break;

    case VF_BUFFERPOOL:
      DBG3 (( MF__, "Resource == VF_BUFFERPOOL" ));

      pHF->Resource      = VF_BUFFERPOOL;
      pHF->lSizeOfBlocks = BufCount * sizeof ( VF_BLOCK );

      #if defined(_WIN32)
       if (!(pHF->pFirstBlock = HeapAlloc(pBufPool, 0, pHF->lSizeOfBlocks)))
         rc = ERROR_NOT_ENOUGH_MEMORY;
      #else
       rc = DosSubAllocMem ( pBufPool, (PVOID*) &pHF->pFirstBlock,
                             pHF -> lSizeOfBlocks );
      #endif

      if ( rc == NO_ERROR )
        *Block = ( VF_BUFFERADDRESS ) pHF->pFirstBlock;
      else
        {
        *Block       = (VF_BUFFERADDRESS) 0;
        lError       = VF_NOTOK;
        pHF->ulState = HFS_ERROR;

        DBG1 (( MF__, ERRMSG_VF_ALLOC_BUFFER_SPACE ));
        MSGD (( ERR_ALLOC_MEMORY, rc               ));
        sql46c_build_error_string ( pcErrText,
                                    ERRMSG_VF_ALLOC_BUFFER_SPACE,
                                    rc );
        }
      break;

    default:
      *Block       = ( VF_BUFFERADDRESS ) 0;
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR;

      DBG1 (( MF__, ERRMSG_VF_ILL_RESOURCE ));
      MSGD (( ERR_ILL_RESOURCE ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_RESOURCE, 0 );
      break;
    }

  DBGOUT;

  return (lError);
  }

/*------------------------------*/

LONG  sql06c_free_buffer ( PHOSTFILE_CTRL_REC  pHF,
                           PVOID               pBufPool,
                           INT2                BufCount,
                           PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_free_buffer"
  APIRET     rc      = NO_ERROR;
  LONG       lError  = VF_OK;

  DBGIN;

  // --- free internal buffer
  if (pHF->pBuffer != (PUCHAR) UNDEF)
    {
    #if defined(_WIN32)
     if (!HeapFree(pBufPool, 0, (PVOID)pHF->pBuffer))
       rc = ERROR_NOT_ENOUGH_MEMORY;
    #else
     rc = DosSubFreeMem(pBufPool, (PVOID)pHF->pBuffer, sizeof (VF_BLOCK));
    #endif

    pHF->pBuffer = (PUCHAR) UNDEF;

    if (rc != NO_ERROR)
      {
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR;

      DBG1  (( MF__, ERRMSG_VF_FREE_BUFFER_SPACE ));
      MSGCD (( ERR_CANT_FREE_MEMORY, rc         ));
      sql46c_build_error_string ( pcErrText,
                                  ERRMSG_VF_FREE_BUFFER_SPACE, rc );

      DBGOUT;
      ABORT();
      }
    }

  if (pHF->pFirstBlock != (PUCHAR) UNDEF)
    {
    #if defined(_WIN32)
      if (!HeapFree(pBufPool, 0, (PVOID)pHF->pFirstBlock))
        rc = ERROR_NOT_ENOUGH_MEMORY;
    #else
     rc = DosSubFreeMem( pBufPool, (PVOID)pHF->pFirstBlock, pHF->lSizeOfBlocks);
    #endif

    pHF->pFirstBlock = (PUCHAR) UNDEF;

    if (rc != NO_ERROR)
      {
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR;

      DBG1  (( MF__, ERRMSG_VF_FREE_BUFFER_SPACE ));
      MSGCD (( ERR_CANT_FREE_MEMORY, rc         ));
      sql46c_build_error_string ( pcErrText,
                                  ERRMSG_VF_FREE_BUFFER_SPACE, rc );

      ABORT();
      }
    else if ((pHF->Resource      != VF_STACK) &&
             (pHF->lSizeOfBlocks != BufCount * (int)sizeof (VF_BLOCK)))
      {
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR;

      DBG1 (( MF__, ERRMSG_VF_FREE_BUFPOOL_COUNT_DIFF ));
      MSGD (( ERR_FREE_BUFPOOL_COUNT_DIFF              ));
      sql46c_build_error_string ( pcErrText,
                                  ERRMSG_VF_FREE_BUFPOOL_COUNT_DIFF, 0 );
      }
    }

  DBGOUT;
  return (lError);
}

/*------------------------------*/

LONG  sql06c_search ( PSZ                pszHostFileName,
                      PHOSTFILE_CTRL_REC arcFileCtrlTab,
                      INT4               * pHostFileNo,
                      PHOSTFILE_CTRL_REC *ppHF,
                      PCHAR              pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_search"
  ULONG    ulIndex;

  DBGIN;

  *pHostFileNo = UNDEF;

  for ( ulIndex = 0; ulIndex < HF_MAX ; ulIndex++ )
    {
    if ( strcmp ( arcFileCtrlTab[ulIndex].szFileName, pszHostFileName ) == 0 )
      {
      // --- file already in use !
      DBG3 (( MF__, "file '%s' in use", pszHostFileName));

      DBG1 (( MF__, ERRMSG_VF_FILE_SHARING_VIOLATION ));
      MSGD (( ERR_FILE_SHARING_VIOLATION, pszHostFileName ));
      sql46c_build_error_string ( pcErrText,
                                  ERRMSG_VF_FILE_SHARING_VIOLATION, 0 );

      DBGOUT;

      return (VF_NOTOK);
      }
    }

  // --- is threre an unused file number ?
  for ( ulIndex = 0; ulIndex < HF_MAX ; ulIndex++ )
    {
    if ( arcFileCtrlTab[ulIndex].ulState == HFS_UNUSED)
      {
      // --- file available
      *pHostFileNo                    = ulIndex + 1;
      arcFileCtrlTab[ulIndex].ulState = HFS_RESERVED;
      break;
      }
    }

  if ( * pHostFileNo == UNDEF )
    {
    DBG1 (( MF__, ERRMSG_VF_NO_FREE_FILE_DESCRIPTOR ));
    MSGD (( ERR_NO_FREE_FILE_DESCRIPTOR              ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_NO_FREE_FILE_DESCRIPTOR, 0 );

    DBGOUT;

    return (VF_NOTOK);
    }

  // ---- set pointer to file control record
  *ppHF = &arcFileCtrlTab[* pHostFileNo - 1];

  DBGOUT;
  return (VF_OK);
  }

/*------------------------------*/

LONG sql06c_tape_open ( PSZ                 pszTapeName,
                        PHOSTFILE_CTRL_REC  pHF,
                        PCHAR               pcErrText,
                        bool                fTapeRead )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_tape_open"
  #if defined(_WIN32)
   ERRORTEXT          TmpErrtext;
  #endif

  #if defined(_WIN32)
   APIRET                     rc            = NO_ERROR;
   LONG                       lError        = VF_OK;

   DBGIN;

   DBG3 ((MF__, "Open tape '%s'", pszTapeName ));

   pHF->hfFileHandle = (HANDLE)CreateFile( pszTapeName,
                                          GENERIC_READ          |
                                          GENERIC_WRITE,
                                          0,
                                          SECURITY_ATTR,
                                          OPEN_EXISTING,
                                          0,
                                          NULL);

   if ( pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     {
     rc = GetLastError();

     pHF->ulState      = HFS_ERROR;
     lError            = VF_NOTOK;
     pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

     if ( sql06c_handle_known_errors ( pszTapeName, rc , pcErrText ) == FALSE )
       {
       DBG1 (( MF__, "Cannot open tape '%s', rc = %d", pszTapeName, rc ));
       MSGD (( ERR_OPEN, pszTapeName, rc                               ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc       );
       }
     }
   else // -- no error
     {
     sql06c_init_dlt_tape ( pHF ) ;

     if ( (( lError = sql06c_get_drive_info( pHF, pcErrText )) == VF_OK ) &&
          (( lError = sql06c_tape_loading  ( pHF, pcErrText , TAPE_LOAD))
                                                               == VF_OK ) &&
          (( lError = sql06c_tape_locking  ( pHF, pcErrText , TAPE_LOCK))
                                                               == VF_OK ) &&
          (( lError = sql06c_get_media_info( pHF, pcErrText )) == VF_OK ) &&
          (( lError = sql06c_init_tape_dev ( pHF, pcErrText )) == VF_OK ) &&
          (( lError = sql06c_tape_rewind   ( pHF, pcErrText )) == VF_OK ))
       {
       pHF->lMaxFilePos  = 0 ;
       pHF->ulState      = HFS_USED;

       if (fTapeRead)
         {
         lError = sql06c_get_desc ( pHF, pcErrText );

         if (lError == VF_EOF)
           {
           pHF->ulState = HFS_ERROR;
           lError       = VF_NOTOK;
           sql46c_build_error_string ( pcErrText,
                                       ERRMSG_VF_INVALID_TAPE_CONTENTS,  0 );
           }
         }
       else
         lError = sql06c_put_desc ( pHF, pcErrText );
       }

     if (lError == VF_NOTOK)
       {
       sql06c_tape_rewind(pHF, pcErrText );
       sql06c_tape_locking(pHF, TmpErrtext , TAPE_UNLOCK);
       }
     }

   DBG3 (( MF__, "lRecordLength       %d", pHF->lRecordLength));
   DBG3 (( MF__, "ulState             %d", pHF->ulState ));
   DBG3 (( MF__, "lError              %d", lError ));

   DBGOUT;
   return (lError);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
   return ( VF_NOTOK );
  #endif
  }

/*------------------------------*/

// Opens the file for writing and writes the first block if the record length
// is 4kB

LONG sql06c_write_open ( PSZ                 pszHostFileName,
                         PHOSTFILE_CTRL_REC  pHF,
                         PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_write_open"

  #if !defined(_WIN32)
   ULONG       Action;
  #endif
  APIRET       rc      = NO_ERROR;
  LONG         lError  = VF_OK;
  PATHNAME     szPhysFileName;


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  #if defined(_WIN32)
   pHF->hfFileHandle = (HANDLE)CreateFile( szPhysFileName,
                                          GENERIC_READ            |
                                          GENERIC_WRITE,
                                          FILE_SHARE_READ,
                                          SECURITY_ATTR,
                                          OPEN_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL,
                                          NULL);

   if ( pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     rc = GetLastError();

  #else
   rc = DosOpen( szPhysFileName,
                 &pHF->hfFileHandle,
                 &Action,
                 0,
                 FILE_NORMAL,
                 OPEN_ACTION_CREATE_IF_NEW  |
                 OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_FLAGS_FAIL_ON_ERROR   |
                 OPEN_SHARE_DENYWRITE       |
                 OPEN_ACCESS_READWRITE,
                 NOEABUF);
  #endif

  if ( rc != NO_ERROR )
    {
    pHF->ulState      = HFS_ERROR;
    lError            = VF_NOTOK;
    pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

    if ( sql06c_handle_known_errors ( pszHostFileName,
                                      rc , pcErrText ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
      MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc           );
      }
    }
  else // -- no error
    {
    DBG3 ((MF__, "Open file '%s'", pszHostFileName ));

    pHF->lMaxFilePos     = 0 ;

    // --- get file size (pHF->lMaxFilePos) and file type (pHF->ulFileType)
    lError = sql06c_status ( pHF, pcErrText );

    if (lError == VF_OK)
      {
      if ( pHF->ulFileType == VFT_DISK )
        {
        if ( pHF->lMaxFilePos == 0 )
          {
          pHF->ulState = HFS_USED;
          lError = sql06c_put_desc ( pHF, pcErrText );
          }
        else
          {
          lError       = VF_NOTOK;
          pHF->ulState = HFS_ERROR ;

          DBG1 (( MF__, ERRMSG_VF_FILE_EXIST ));
          MSGD (( ERR_FILE_EXIST              ));
          sql46c_build_error_string ( pcErrText, ERRMSG_VF_FILE_EXIST, 0 );
          }
        }
      else
        {
        pHF->ulState = HFS_USED;
        lError = sql06c_put_desc ( pHF, pcErrText );
        }
      }
    }

  DBG3 (( MF__, "lRecordLength       %d", pHF->lRecordLength));
  DBG3 (( MF__, "ulState             %d", pHF->ulState ));
  DBG3 (( MF__, "lError              %d", lError ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * Opens the file for overwriting
 */

LONG sql06c_overwrite_open ( PSZ                 pszHostFileName,
                             PHOSTFILE_CTRL_REC  pHF,
                             PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_overwrite_open"
  #if !defined(_WIN32)
   ULONG       Action;
  #endif
  APIRET       rc      = NO_ERROR;
  LONG         lError  = VF_OK;
  PATHNAME     szPhysFileName;


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  #if defined(_WIN32)
   pHF->hfFileHandle = (HANDLE)CreateFile( szPhysFileName,
                                          GENERIC_READ            |
                                          GENERIC_WRITE,
                                          FILE_SHARE_READ,
                                          SECURITY_ATTR,
                                          CREATE_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL,
                                          NULL);

   if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     rc = GetLastError();

  #else
   rc = DosOpen( szPhysFileName,
                 &pHF->hfFileHandle,
                 &Action,
                 0,
                 FILE_NORMAL,
                 OPEN_ACTION_CREATE_IF_NEW  |
                 OPEN_ACTION_REPLACE_IF_EXISTS,
                 OPEN_FLAGS_FAIL_ON_ERROR   |
                 OPEN_SHARE_DENYWRITE       |
                 OPEN_ACCESS_READWRITE,
                 NOEABUF);
  #endif

  if ( rc != NO_ERROR )
    {
    pHF->ulState      = HFS_ERROR;
    lError            = VF_NOTOK;
    pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

    if ( sql06c_handle_known_errors ( pszHostFileName,
                                      rc , pcErrText ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
      MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc           );
      }
    }
  else // -- no error
    {
    pHF->lMaxFilePos     = 0 ;

    // --- get file size (pHF->lMaxFilePos) and file type (pHF->ulFileType)
    lError = sql06c_status ( pHF, pcErrText );

    if (lError == VF_OK)
      {
      pHF->ulState = HFS_USED;
      lError = sql06c_put_desc ( pHF, pcErrText );
      }
    }

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * Opens the file for reading
 */

LONG sql06c_read_open ( PSZ                 pszHostFileName,
                        PHOSTFILE_CTRL_REC  pHF,
                        PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_read_open"
  #if !defined(_WIN32)
   ULONG       Action;
  #endif
  APIRET       rc     = NO_ERROR;
  LONG         lError = VF_OK;
  PATHNAME     szPhysFileName;


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  #if defined(_WIN32)
   pHF->hfFileHandle = (HANDLE)CreateFile( szPhysFileName,
                                          GENERIC_READ,
                                          FILE_SHARE_WRITE        |
                                          FILE_SHARE_READ,
                                          SECURITY_ATTR,
                                          OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL,
                                          NULL);

   if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     rc = GetLastError();

  #else
   rc = DosOpen( szPhysFileName,
                 &pHF->hfFileHandle,
                 &Action,
                 0,
                 FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW    |
                 OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_FLAGS_FAIL_ON_ERROR   |
                 OPEN_SHARE_DENYNONE        |
                 OPEN_ACCESS_READONLY,
                 NOEABUF);
  #endif

  if ( rc != NO_ERROR )
    {
    pHF->ulState      = HFS_ERROR;
    lError            = VF_NOTOK;
    pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

    if ( rc == ERROR_FILE_NOT_FOUND )
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRONG_FILE_DEVICE_NAME, 0 );
    else
      {
      if ( sql06c_handle_known_errors ( pszHostFileName,
                                        rc , pcErrText ) == FALSE )
        {
        DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
        MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
        sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc           );
        }
      }
    }
  else // -- no error
    {
    // --- get file size (pHF->lMaxFilePos) and file type (pHF->ulFileType)
    lError = sql06c_status ( pHF, pcErrText );

    if (lError == VF_OK)
      {
      pHF->ulState = HFS_USED;
      lError = sql06c_get_desc ( pHF, pcErrText );
      }
    }

  DBGOUT;
  return (lError);
  }


/*------------------------------*/

LONG sql06c_con_open ( PHOSTFILE_CTRL_REC  pHF,
                       PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_con_open"
  LONG         lError = VF_OK;
  #if defined(_WIN32)
   APIRET      rc     = NO_ERROR;
  #endif

  DBGIN;

  #if defined(_WIN32)
   if (pHF->lDevType == DT_CONIN )
     pHF->hfFileHandle = (HANDLE)GetStdHandle(STD_INPUT_HANDLE);
   else
     pHF->hfFileHandle = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);

   if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     {
     rc                = GetLastError();
     pHF->ulState      = HFS_ERROR;
     lError            = VF_NOTOK;
     pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

     DBG1 (( MF__, "Cannot open file '%s', rc = %d", pHF->szFileName, rc ));
     MSGD (( ERR_OPEN, pHF->szFileName, rc ));
     sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN, rc  );
     }
   else
     {
     pHF->ulState       = HFS_USED;
     pHF->lMaxFilePos   = 0;

     if ( pHF->FileFormat != VF_PLAINTEXT )
       {
       if (pHF->lDevType == DT_CONIN )
         lError = sql06c_get_desc ( pHF, pcErrText );
       else
         lError = sql06c_put_desc ( pHF, pcErrText );
       }
     else
       {
       pHF->ulEffFileFormat = pHF->FileFormat;
       pHF->lRecordLength   = 0;
       pHF->lPos            = -1;
       pHF->lReadCount      = sizeof (VF_BLOCK);
       }
     }

  #else
   pHF->ulState       = HFS_USED;
   pHF->lMaxFilePos   = 0;

   if (pHF->lDevType == DT_CONIN )
     pHF->hfFileHandle = STDIN;
   else
     pHF->hfFileHandle = STDOUT;

   if ( pHF->FileFormat != VF_PLAINTEXT )
     {
     if (pHF->lDevType == DT_CONIN )
       lError = sql06c_get_desc ( pHF, pcErrText );
     else
       lError = sql06c_put_desc ( pHF, pcErrText );
     }
   else
     {
     pHF->ulEffFileFormat = pHF->FileFormat;
     pHF->lRecordLength   = 0;
     pHF->lPos            = -1;
     pHF->lReadCount      = sizeof (VF_BLOCK);
     }
  #endif


  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * Opens the file to append records
 */

LONG  sql06c_append_open ( PSZ                 pszHostFileName,
                           PHOSTFILE_CTRL_REC  pHF,
                           PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_append_open"
  APIRET       rc                = NO_ERROR;
  LONG         lError            = VF_OK;
  ULONG        ulCmpFileFormat   = pHF->FileFormat;
  LONG         lCmpRecordLength  = pHF->lRecordLength;
  ULONG        ulEndOfFilePosition;
  ULONG        Action;
  PATHNAME     szPhysFileName;


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  #if defined(_WIN32)
    pHF->hfFileHandle = (HANDLE)CreateFile( szPhysFileName,
                                           GENERIC_READ          |
                                           GENERIC_WRITE,
                                           FILE_SHARE_READ,
                                           SECURITY_ATTR,
                                           OPEN_EXISTING,
                                           FILE_ATTRIBUTE_NORMAL,
                                           NULL);

    if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE)
      {
      pHF->hfFileHandle = (HANDLE)CreateFile( szPhysFileName,
                                             GENERIC_READ          |
                                             GENERIC_WRITE,
                                             FILE_SHARE_READ,
                                             SECURITY_ATTR,
                                             CREATE_NEW,
                                             FILE_ATTRIBUTE_NORMAL,
                                             NULL);
      if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE)
        rc = GetLastError();
      else
        Action = FILE_CREATED;
      }
    else
      Action = FILE_EXISTED;
  #else
    rc = DosOpen( szPhysFileName,
                  &pHF->hfFileHandle,
                  &Action,
                  0,
                  FILE_NORMAL,
                  OPEN_ACTION_CREATE_IF_NEW  |
                  OPEN_ACTION_OPEN_IF_EXISTS,
                  OPEN_FLAGS_FAIL_ON_ERROR   |
                  OPEN_SHARE_DENYWRITE       |
                  OPEN_ACCESS_READWRITE,
                  NOEABUF);
  #endif

  if ( rc != NO_ERROR )
    {
    pHF->ulState      = HFS_ERROR;
    lError            = VF_NOTOK;
    pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

    if ( sql06c_handle_known_errors ( pszHostFileName,
                                      rc , pcErrText ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
      MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc           );
      }
    }
  else // -- no error
    {
    // --- file not created ?
    if (Action != FILE_CREATED)
      {
      // --- get file size (pHF->lMaxFilePos) and file type (pHF->ulFileType)
      lError = sql06c_status ( pHF, pcErrText );

      if (lError == VF_OK)
        {
        if ( pHF->ulFileType == VFT_DISK )
          {
          pHF->ulState = HFS_USED;

          if (pHF->lMaxFilePos == 0)
            lError = sql06c_put_desc ( pHF, pcErrText );
          else
            {
            lError = sql06c_get_desc ( pHF, pcErrText );

            // --- append to empty text files is OK
            if ( (lError == VF_EOF) && (ulCmpFileFormat == VF_PLAINTEXT ))
              {
              pHF->FileFormat    = VF_PLAINTEXT;
              pHF->ulState       = HFS_USED;
              pHF->lRecordLength = 0;
              lError             = VF_OK;
              }
            else if ( lError == VF_OK )
              {
              if ( ((ulCmpFileFormat  == pHF->FileFormat ) &&
                    (lCmpRecordLength == pHF->lRecordLength)) ||
                   ( ulCmpFileFormat  != VF_RECORD          ) )
                {
                lError = sql06c_posbuffer ( pHF, 0, FILE_END,
                                            &ulEndOfFilePosition,
                                            pcErrText );
                }
              else
                {
                // --- no seek
                pHF->ulState = HFS_ERROR;
                lError       = VF_NOSEEK;

                DBG1 (( MF__, ERRMSG_VF_ILL_FILE_DESC ));
                MSGD (( ERR_ILL_FILE_DESC              ));
                sql46c_build_error_string ( pcErrText,
                                            ERRMSG_VF_ILL_FILE_DESC, 0 );
                }
              }
            }
          }
        else
          {
          pHF->ulState      = HFS_ERROR;
          lError            = VF_NOTOK;

          DBG1 (( MF__, ERRMSG_VF_ILL_APPEND_DEVICE ));
          MSGD (( ERR_ILL_APPEND_DEVICE              ));
          sql46c_build_error_string ( pcErrText,
                                      ERRMSG_VF_ILL_APPEND_DEVICE, 0 );
          }
        }
      }
    else // --- file created
      {
      pHF->lMaxFilePos = 0;

      // --- get file size (pHF->lMaxFilePos) and file type (pHF->ulFileType)
      lError = sql06c_status ( pHF, pcErrText );

      if (lError == VF_OK)
        {
        pHF->ulState = HFS_USED;
        lError = sql06c_put_desc ( pHF, pcErrText );
        }
      }
    }

  DBG3 (( MF__, "ulState %d", pHF->ulState ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG sql06c_pipe_write_open ( PSZ                 pszHostFileName,
                              PHOSTFILE_CTRL_REC  pHF,
                              PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_pipe_write_open"

  APIRET       rc      = NO_ERROR;
  LONG         lError  = VF_OK;
  PATHNAME     szPhysFileName;
  ULONG        ulPipeBuffSize;
  #if !defined(_WIN32)
   ULONG       Action;
  #endif


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  if (( pHF->lRecordLength <  sizeof(VF_BLOCK)) ||
      ( pHF->FileFormat    == VF_4K_BLOCK  )    ||
      ( pHF->FileFormat    == VF_PLAINTEXT ))
    {
    ulPipeBuffSize = sizeof(VF_BLOCK);
    }
  else if ( pHF->lRecordLength > KBYTE_64 )
    ulPipeBuffSize = KBYTE_64;
  else
    ulPipeBuffSize = pHF->lRecordLength;

  #if defined(_WIN32)
   pHF->hfFileHandle = (HANDLE)CreateNamedPipe( szPhysFileName,
                                               PIPE_ACCESS_OUTBOUND,
                                               PIPE_TYPE_BYTE     |
                                               PIPE_READMODE_BYTE |
                                               PIPE_WAIT,
                                               1,
                                               ulPipeBuffSize,
                                               ulPipeBuffSize,
                                               INFINITE,
                                               NULL);

   if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     rc = GetLastError();

   if (rc == NO_ERROR)
     {
     if (!ConnectNamedPipe((HANDLE)pHF->hfFileHandle, NULL))
       {
       rc = GetLastError();
       CloseHandle((HANDLE)pHF->hfFileHandle);
       }
     }
   else if (rc == ERROR_PIPE_BUSY)
     {

     pHF->hfFileHandle = (HANDLE)CreateFile( szPhysFileName,
                                            GENERIC_WRITE,
                                            FILE_SHARE_READ,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL );

     if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
       rc = GetLastError();
     else
       rc = NO_ERROR;
     }
  #else
   rc = DosCreateNPipe( szPhysFileName,
                        &pHF->hfFileHandle,
                        NP_ACCESS_OUTBOUND,
                        NP_TYPE_BYTE       |
                        NP_READMODE_BYTE   |
                        NP_WAIT | 0x01,
                        ulPipeBuffSize,
                        ulPipeBuffSize,
                        -1 );

   if (rc == NO_ERROR)
     {
     rc = DosConnectNPipe( pHF->hfFileHandle );

     if (rc != NO_ERROR)
       DosClose(pHF->hfFileHandle);
     }
   else if (rc == ERROR_PIPE_BUSY)
     {
     rc = DosOpen( szPhysFileName,
                   &pHF->hfFileHandle,
                   &Action,
                   0,
                   FILE_NORMAL,
                   OPEN_ACTION_FAIL_IF_NEW |
                   OPEN_ACTION_OPEN_IF_EXISTS,
                   OPEN_ACCESS_WRITEONLY |
                   OPEN_SHARE_DENYWRITE,
                   NOEABUF);
     }
  #endif

  if ( rc != NO_ERROR )
    {
    pHF->ulState      = HFS_ERROR;
    lError            = VF_NOTOK;
    pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

    if ( sql06c_handle_known_errors ( pszHostFileName,
                                      rc , pcErrText ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
      MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN,  rc           );
      }
    }
  else // -- no error
    {
    DBG3 ((MF__, "Open file '%s'", pszHostFileName ));

    pHF->ulFileType  = VFT_PIPE;
    pHF->lMaxFilePos = 0;
    pHF->ulState     = HFS_USED;

    lError = sql06c_put_desc ( pHF, pcErrText );
    }

  DBG3 (( MF__, "lRecordLength       %d", pHF->lRecordLength));
  DBG3 (( MF__, "ulState             %d", pHF->ulState ));
  DBG3 (( MF__, "lError              %d", lError ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG sql06c_pipe_read_open ( PSZ                 pszHostFileName,
                             PHOSTFILE_CTRL_REC  pHF,
                             PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_pipe_read_open"
  APIRET       rc     = NO_ERROR;
  LONG         lError = VF_OK;
  PATHNAME     szPhysFileName;
  ULONG        ulPipeBuffSize = sizeof(VF_BLOCK) * 2;
  #if !defined(_WIN32)
   ULONG       Action;
  #endif


  DBGIN;

  sql44c_subst_log_parts ( szPhysFileName, pszHostFileName );

  #if defined(_WIN32)
   pHF->hfFileHandle = (HANDLE)CreateNamedPipe( szPhysFileName,
                                               PIPE_ACCESS_INBOUND,
                                               PIPE_TYPE_BYTE     |
                                               PIPE_READMODE_BYTE |
                                               PIPE_WAIT,
                                               1,
                                               ulPipeBuffSize,
                                               ulPipeBuffSize,
                                               INFINITE,
                                               NULL);

   if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     rc = GetLastError();

   if (rc == NO_ERROR)
     {
     if (!ConnectNamedPipe((HANDLE)pHF->hfFileHandle, NULL))
       {
       rc = GetLastError();
       CloseHandle((HANDLE)pHF->hfFileHandle);
       }
     }
   else if (rc == ERROR_PIPE_BUSY)
     {
     pHF->hfFileHandle = (HANDLE)CreateFile( szPhysFileName,
                                            GENERIC_READ,
                                            FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL );

     if (pHF->hfFileHandle == (HANDLE)INVALID_HANDLE_VALUE )
       rc = GetLastError();
     else
       rc = NO_ERROR;
     }
  #else
   rc = DosCreateNPipe( szPhysFileName,
                        &pHF->hfFileHandle,
                        NP_ACCESS_INBOUND,
                        NP_TYPE_BYTE       |
                        NP_READMODE_BYTE   |
                        NP_WAIT | 0x01,
                        ulPipeBuffSize,
                        ulPipeBuffSize,
                        -1 );

   if (rc == NO_ERROR)
     {
     rc = DosConnectNPipe( pHF->hfFileHandle );

     if (rc != NO_ERROR)
       DosClose(pHF->hfFileHandle);
     }
   else if (rc == ERROR_PIPE_BUSY)
     {
     rc = DosOpen( szPhysFileName,
                   &pHF->hfFileHandle,
                   &Action,
                   0,
                   FILE_NORMAL,
                   OPEN_ACTION_FAIL_IF_NEW |
                   OPEN_ACTION_OPEN_IF_EXISTS,
                   OPEN_ACCESS_READONLY |
                   OPEN_SHARE_DENYREAD,
                   NOEABUF);
     }
  #endif


  if ( rc != NO_ERROR )
    {
    pHF->ulState      = HFS_ERROR;
    lError            = VF_NOTOK;
    pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

    if ( sql06c_handle_known_errors ( pszHostFileName,
                                      rc , pcErrText ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", pszHostFileName, rc ));
      MSGD (( ERR_OPEN, pszHostFileName, rc                               ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_OPEN, rc  );
      }
    }
  else // -- no error
    {
    pHF->ulFileType  = VFT_PIPE;
    pHF->lMaxFilePos = 0;
    pHF->ulState     = HFS_USED;

    lError = sql06c_get_desc ( pHF, pcErrText );
    }

  DBGOUT;
  return (lError);
  }

/*------------------------------*/


LONG sql06c_close ( PHOSTFILE_CTRL_REC  pHF,
                    bool                fErase,
                    PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_close"

  APIRET              rc      = NO_ERROR;
  LONG                lError  = VF_OK;
  PATHNAME            szPhysFileName;
  #if defined(_WIN32)
   ERRORTEXT          TmpErrtext;
  #endif

  DBGIN;

  if (( fErase ) && ( pHF->lDevType != DT_TAPE ))
    sql44c_subst_log_parts ( szPhysFileName, pHF->szFileName );
  else
    fErase = FALSE;


  #if defined(_WIN32)
   if ( pHF->lDevType == DT_TAPE )
     {
     if ( pHF->Direction != VREAD )
       lError = (VF_RETURN) sql06c_tape_write_mark(pHF, pcErrText );

     if ( lError == VF_NOTOK )
       {
       sql06c_tape_rewind(pHF, pcErrText );
       sql06c_tape_locking(pHF, TmpErrtext , TAPE_UNLOCK);
       }
     else
       {
       lError = sql06c_tape_rewind( pHF, pcErrText );

       if ( lError == VF_OK )
         lError = (VF_RETURN) sql06c_tape_locking(pHF, pcErrText, TAPE_UNLOCK );
       else
         sql06c_tape_locking(pHF, TmpErrtext, TAPE_UNLOCK);
       }
       if ( lError == VF_OK && pHF->fNxtInAutoLoader )
          pHF->lNxtInAutoLoaderError =
                     sql06c_tape_loading( pHF, pcErrText , TAPE_UNLOAD) ;
     }
   else if ( pHF->lDevType == DT_PIPE )
     {
     FlushFileBuffers( (HANDLE)pHF->hfFileHandle );
     }
  #endif

  if ( lError == VF_NOTOK )
    {
    // --- close file
    CLOSE_FILE( pHF->hfFileHandle );

    if ( fErase )
      {
      DELETE_FILE( szPhysFileName );
      }
    }
  else
    {
    // --- close file
    rc = CLOSE_FILE( pHF->hfFileHandle );

    if (rc != NO_ERROR)
      {
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_CLOSE, rc );
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR ;

      if ( fErase )
        {
        DELETE_FILE( szPhysFileName );
        }
      }
    else if ( fErase )
      {
      rc = DELETE_FILE( szPhysFileName );

      if (rc != NO_ERROR)
        {
        sql46c_build_error_string ( pcErrText, ERRMSG_VF_DELETE, rc );
        lError       = VF_NOTOK;
        pHF->ulState = HFS_ERROR ;
        }
      }
    }

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG sql06c_status ( PHOSTFILE_CTRL_REC  pHF,
                     PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_status"
  APIRET                   rc     = NO_ERROR;
  LONG                     lError = VF_OK;
  #if defined(_WIN32)
    LONG                   FileSize;
  #else
    FILESTATUS3            fstsStatus3;
    ULONG                  ulDeviceAttr;
  #endif

  DBGIN;

  #if defined(_WIN32)
   pHF->ulFileType = GetFileType((HANDLE)pHF->hfFileHandle);

   switch (pHF->ulFileType)
     {
     case FILE_TYPE_DISK :
       pHF->ulFileType = VFT_DISK;
       break;
     case FILE_TYPE_CHAR :
       pHF->ulFileType = VFT_CHARACTER_DEVICE;
       break;
     case FILE_TYPE_PIPE :
       pHF->ulFileType = VFT_PIPE;
       break;
     default:
       pHF->lMaxFilePos  = 0;
       pHF->ulFileType   = (ULONG)UNDEF;
       break;
     }

   // --- determine pHF->lMaxFilePos...
   if (((FileSize = GetFileSize((HANDLE)pHF->hfFileHandle, NULL)) == -1L) &&
       ( pHF->ulFileType == VFT_DISK ))
     {
     rc = GetLastError();
     DBG1 (( MF__, ERRMSG_VF_GET_FILE_STATUS ));
     MSGD (( ERR_GET_FILE_STATUS, rc         ));
     sql46c_build_error_string ( pcErrText, ERRMSG_VF_GET_FILE_STATUS, rc );

     pHF->ulState   = HFS_ERROR;
     lError         = VF_NOTOK;
     }
   else
     pHF->lMaxFilePos = FileSize;
  #else
   rc = DosQueryFileInfo(pHF->hfFileHandle,
                         FIL_STANDARD,
                         &fstsStatus3,
                         sizeof(FILESTATUS3));

   if (rc == NO_ERROR)
     {
     rc = DosQueryHType(pHF->hfFileHandle, &pHF->ulFileType, &ulDeviceAttr);

     if (rc == NO_ERROR)
       {
       // - As type may be a combination with HANDTYPE_NETWORK
       //   the HANDTYPE_NETWORK bit is cleared to simplify the
       //   CASE-statement.

       pHF->ulFileType &= 7;     // -- extract the handle class

       switch (pHF->ulFileType)
         {
         case HANDTYPE_PIPE  :
           pHF->lMaxFilePos  = fstsStatus3.cbFile;
           pHF->ulFileType   = VFT_PIPE;
           break;
         case HANDTYPE_FILE  :
           pHF->lMaxFilePos  = fstsStatus3.cbFile;
           pHF->ulFileType   = VFT_DISK;
           break;
         case HANDTYPE_DEVICE:
           pHF->lMaxFilePos  = fstsStatus3.cbFile;
           pHF->ulFileType   = VFT_CHARACTER_DEVICE;
           break;
         default:
           pHF->lMaxFilePos  = 0;
           pHF->ulFileType   = UNDEF;
           break;
         }
       }
     else
       {
       DBG1 (( MF__, ERRMSG_VF_GET_FILE_TYPE ));
       MSGD (( ERR_GET_FILE_TYPE, rc         ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_GET_FILE_TYPE, rc );

       pHF->ulState   = HFS_ERROR;
       lError         = VF_NOTOK;
       }
     }
   else
     {
     DBG1 (( MF__, ERRMSG_VF_GET_FILE_STATUS ));
     MSGD (( ERR_GET_FILE_STATUS, rc         ));
     sql46c_build_error_string ( pcErrText, ERRMSG_VF_GET_FILE_STATUS, rc );

     pHF->ulState   = HFS_ERROR;
     lError         = VF_NOTOK;
     }
  #endif

  DBG3 (( MF__, "pHF -> ulFileType &= 7  =  %d", pHF -> ulFileType ));
  DBG3 (( MF__, "pHF -> lMaxFilePos = %d", pHF -> lMaxFilePos ));
  DBG3 (( MF__, "pHF -> ulState = %d", pHF -> ulState ));
  DBG3 (( MF__, "lError = %d", lError ));

  DBGOUT;

  return (lError);
  }


/*------------------------------*/


// Read ASCII variable length records

LONG    sql06c_read_asc_var (   PHOSTFILE_CTRL_REC  prcHostFile,
                                VF_BUFFERADDRESS    pReadBlock,
                                INT4*               plLength,
                                PCHAR               acErrText       )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_read_asc_var"


  LONG    lError      = VF_OK;
  ULONG   ulDestPos;                  // pReadBlock position
  LONG    lByteNumber;
  LONG    lTabExpand;
  UCHAR   cCurrentChar;
  bool    bFoundCR    = FALSE;


  DBGIN;


  ulDestPos  = 0;
  *plLength  = 0;

  for (;;)
    {
    prcHostFile->lPos++;

    if ( ulDestPos >= sizeof(VF_BLOCK) )
      {
      // --- block position is out of range
      DBG3 (( MF__, "( ulDestPos >= sizeof ( VF_BLOCK ) - 1 )"
                    "( %d >= %d - 1)", ulDestPos, sizeof ( VF_BLOCK )));
      DBG3 (( MF__, "prcHostFile -> lPos %d", prcHostFile -> lPos ));

      prcHostFile->ulState = HFS_ERROR;
      lError               = VF_NOTOK;

      DBG1 (( MF__, ERRMSG_VF_MAX_REC ));
      MSGD (( ERR_MAX_REC ));
      sql46c_build_error_string ( acErrText, ERRMSG_VF_MAX_REC, 0 );

      DBGOUT;
      return (lError);
      }

    if ( prcHostFile -> lPos < prcHostFile -> lMaxPos )
      {
      // --- handle current char
      cCurrentChar = prcHostFile->pBuffer [ prcHostFile -> lPos ];

      if ( bFoundCR == TRUE )
        {
        bFoundCR = FALSE;
        if ( cCurrentChar != '\n')      // --- CR found without LF
          prcHostFile->lPos--;          // --- ignore character

        DBGOUT;
        return (lError);
        }

      if ( cCurrentChar == '\r' )
        {
        bFoundCR = TRUE;
        continue;                   // --- go peek ahead at next char
        }

      if ( ((int)cCurrentChar < SEP_SET_SIZE) && SepSet[cCurrentChar] != 0 )
        {
        DBG3 (( MF__, "SeptSet found %d", cCurrentChar ));

        DBGOUT;
        return (lError);
        }

      if ( cCurrentChar == HT )
        {
        // --- expand
        lTabExpand = HT_SIZE - ( ulDestPos % HT_SIZE );

        if ( ulDestPos + lTabExpand <= sizeof ( VF_BLOCK ))
          {
          for ( lByteNumber = 0;
                lByteNumber < lTabExpand;
                lByteNumber++ )
            {
            *( pReadBlock + ulDestPos ) = ' ';
            ulDestPos++;
            }
          }

        *plLength = ulDestPos;
        }
      else
        {
        if ( ((int)cCurrentChar >= IGN_SET_SIZE ) ||
             IgnSet[cCurrentChar] == 0 )
          {
          // --- transfer
          *( pReadBlock + ulDestPos ) = cCurrentChar;
//        *( pReadBlock + ulDestPos + 1 ) = NUL;
          ulDestPos++;
          }
        *plLength = ulDestPos;
        }
      }
    else
      {
      // --- read next block into prcHostFile -> pBuffer
      //     'prcHostFile->ReadCount' Bytes
      lError = sql06c_getbuffer ( prcHostFile, acErrText );

      if ( lError != VF_OK )
        {
        if ( lError == VF_EOF && *plLength > 0 )
          lError = VF_OK ;                  // F.H. last line without CR !!!
        DBGOUT;
        return (lError);
        }
      }
    }

  DBGOUT;

  return (lError);
  }

/*------------------------------*/

/*
 * Read binary variable length records
 */

LONG sql06c_read_bin_var ( PHOSTFILE_CTRL_REC  pHF,
                           VF_BUFFERADDRESS    block,
                           INT4                * length,
                           PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_read_bin_var"
  LONG       lError  = VF_OK;
  ULONG      ulThisLen;
  LONG       lCurrFilePos,
             lNext4KPos;

  DBGIN;

  DBG3 (( MF__, "block = %d", block ));

  ulThisLen = 0;

  if ( pHF->lPos < pHF->lMaxPos - 2 )
    {
    // --- get description of next record
    ulThisLen  = 256 * pHF->pBuffer[++pHF->lPos];
    ulThisLen +=       pHF->pBuffer[++pHF->lPos];
    }

  if ( ulThisLen == 0 )
    {
    // --- read next block to pHF->pBuffer
    lCurrFilePos = pHF->lFilePos + pHF->lPos + 1;
    lNext4KPos = lCurrFilePos - (lCurrFilePos % sizeof (VF_BLOCK));

    if ( (lCurrFilePos % sizeof (VF_BLOCK)) != 0 )
      lNext4KPos += sizeof (VF_BLOCK);

    pHF->lPos += (UINT2)(lNext4KPos - lCurrFilePos);
    if ( pHF->lPos + 1 >= pHF->lMaxPos )
      {
      // --- read next block into pHF->pBuffer  ( 'pHF->ReadCount' Bytes )
      lError = sql06c_getbuffer ( pHF, pcErrText );
      }

    if ( lError == VF_OK )
      {
      // --- get description of next record
      ulThisLen  = 256 * pHF->pBuffer[pHF->lPos+1] + pHF->pBuffer[pHF->lPos+2];
      pHF->lPos += 2;
      }
    else
      {
      // --- return of getting next buffer was not ok
      *length = 0;
      DBGOUT;
      return (lError);
      }
    }

  if ( (ulThisLen == 0) || (ulThisLen >= (ULONG)(pHF->lMaxPos - pHF->lPos)) )
    {
    // --- intern error
    DBG3 (( MF__, "ulThisLen      = %d\n", ulThisLen));
    DBG3 (( MF__, "pHF -> lPos    = %d\n", pHF -> lPos));
    DBG3 (( MF__, "pHF -> lMaxPos = %d\n", pHF -> lMaxPos));

    pHF->ulState = HFS_ERROR;
    lError       = VF_NOTOK;
    *length      = 0;

    DBG1 (( MF__, ERRMSG_VF_ILL_NEXT_REC_LEN ));
    MSGD (( ERR_ILL_NEXT_REC_LEN              ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_NEXT_REC_LEN, 0 );

    DBGOUT;
    return (lError);
    }

  SAPDB_memcpy ( block, &pHF->pBuffer[pHF->lPos + 1], ulThisLen );

  pHF->lPos += ulThisLen;

  if ( pHF->lPos < (pHF->lMaxPos - 1) && (! odd ( pHF->lPos )) )
    {
    pHF->lPos++;
    }

  *length = ulThisLen;

  DBG3 (( MF__, "*length                     = %d", *length ));
  DBG3 (( MF__, "lError                      = %d", lError ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * Read binary fixed length records
 */

LONG sql06c_read_bin_fix ( PHOSTFILE_CTRL_REC  pHF,
                           VF_BUFFERADDRESS    block,
                           INT4                * length,
                           PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_read_bin_fix"
  LONG      lError  = VF_OK;
  ULONG     ulNumOfBytes;       // --- place in buffer
  ULONG     ulByteRemainder;    // --- remainder
  ULONG     ulDestPos;          // --- block pos.

  DBGIN;

  DBG3 (( MF__, "pHF -> lRecordLength   = %d", pHF -> lRecordLength ));
  DBG3 (( MF__, "pHF -> ulEffFileFormat = %d", pHF -> ulEffFileFormat ));

  if ((pHF->lRecordLength   >= sizeof (VF_BLOCK)) ||
      (pHF->ulEffFileFormat == VF_4K_BLOCK)       ||
      (pHF->ulEffFileFormat == VF_STREAM))
    {
    if ( pHF->ulEffFileFormat == VF_STREAM )
      {
      pHF->lEndOfBuffer = *length ;
      }
    else
      {
      pHF->lEndOfBuffer = pHF->lRecordLength ;
      * length          = pHF->lRecordLength ;
      }

    DBG3 (( MF__, "* length             = %d", * length ));
    DBG3 (( MF__, "pHF -> lRecordLength = %d", pHF -> lRecordLength ));

    DBG3 (( MF__, "block = %d", block ));

    // ---- read the file content
    lError = sql06c_get_xk_buffer ( pHF, block, length, pcErrText );

    pHF->lPos = pHF->lMaxPos - 1;
    *length   = pHF->lMaxPos ;

    DBG3 (( MF__, "* length   = %d", *length ));
    DBG3 (( MF__, "pHF->lPos  = %d", pHF->lPos ));
    }
  else
    {
    // --- record length < 4kB
    ulNumOfBytes = (pHF->lMaxPos - pHF->lPos - 1);

    // --- enough place for the whole record?
    if ( (LONG)ulNumOfBytes > pHF->lRecordLength )
      {
      ulNumOfBytes    = pHF->lRecordLength;
      ulByteRemainder = 0;
      }
    else
      {
      // --- use remainder
      ulByteRemainder = pHF->lRecordLength - ulNumOfBytes;
      }

    ulDestPos = 0;

    if ( ulNumOfBytes > 0 )
      {
      // --- use current buffer
      SAPDB_memcpy ( block, &pHF->pBuffer[pHF->lPos+1], ulNumOfBytes );

      pHF->lPos += ulNumOfBytes;
      ulDestPos  = ulNumOfBytes;
      }

    if ( ulByteRemainder > 0 )
      {
      // --- read next block into pHF->pBuffer  ( 'pHF->ReadCount' Bytes )
      lError = sql06c_getbuffer ( pHF, pcErrText );

      if ( lError == VF_OK)
        {
        SAPDB_memcpy ( &block[ulDestPos], pHF->pBuffer, ulByteRemainder);
        pHF->lPos = ulByteRemainder - 1;
        }
      }

    if ( lError == VF_OK )
      * length = pHF->lRecordLength ;
    else
      *length = 0;
    }

  DBG3 (( MF__, "*length              %d", *length ));
  DBG3 (( MF__, "lError               %d", lError  ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * Write ASCII record
 */

LONG sql06c_write_asc_var ( PHOSTFILE_CTRL_REC  pHF,
                            VF_BUFFERADDRESS    block,
                            INT4                length,
                            PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_write_asc_var"
  ULONG      ulNumOfBytes;
  ULONG      ulByteRemainder;
  LONG       lSourcePos;
  ULONG      ulBytesWritten;
  LONG       lError      = VF_OK;
  APIRET     rc          = NO_ERROR;

  DBGIN;

  if ( length < 0 )
    {
    lError       = VF_NOTOK;
    pHF->ulState = HFS_ERROR;

    DBG1 (( MF__, ERRMSG_VF_MIN_REC ));
    MSGD (( ERR_MIN_REC              ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_MIN_REC, 0 );
    }
  else if ( length > sizeof (VF_BLOCK))
    {
    lError       = VF_NOTOK;
    pHF->ulState = HFS_ERROR;

    DBG1 (( MF__, ERRMSG_VF_MAX_REC ));
    MSGD (( ERR_MAX_REC              ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_MAX_REC, 0 );
    }
  else
    {
    if ( pHF->fBlocked == TRUE )
      {
      // --- blocked
      ulNumOfBytes = (pHF->lMaxPos - pHF->lPos - 1);

      // --- enough space?
      if ( ulNumOfBytes > (ULONG)length )
        {
        ulByteRemainder   = 0;
        ulNumOfBytes      = length;
        }
      else
        {
        // --- use remainder
        ulByteRemainder = length - ulNumOfBytes;
        }

      // --- block position
      lSourcePos = 0;

      if ( ulNumOfBytes > 0 )
        {
        // --- use current buffer
        SAPDB_memcpy ( & pHF->pBuffer[pHF->lPos + 1], block, ulNumOfBytes );

        pHF->lPos   = pHF->lPos + ulNumOfBytes;
        lSourcePos  = ulNumOfBytes;
        }

      if ( pHF->lPos == pHF->lMaxPos - 1 )
        {
        // --- write pHF->pBuffer ( 'pHF->lPos' bytes )
        lError = sql06c_putbuffer ( pHF, pcErrText );
        }

      if ( lError == VF_OK)
        {
        if ( ulByteRemainder > 0 )
          {
          // --- new buffer
          SAPDB_memcpy ( &pHF->pBuffer[pHF->lPos + 1], &block[lSourcePos],
                       ulByteRemainder );
          pHF->lPos = ulByteRemainder - 1;
          }

        pHF->pBuffer[++pHF->lPos] = CR;

        if ( pHF->lPos == pHF->lMaxPos - 1 )
          {
          // --- write pHF->pBuffer ( 'pHF->lPos' bytes )
          lError = sql06c_putbuffer ( pHF, pcErrText );
          }

        pHF->pBuffer[++pHF->lPos] = LF;

        if ( pHF->lPos < pHF->lMaxPos - 1 )
          pHF->pBuffer[pHF->lPos + 1] = '\0';
        }
      }
    else
      {
      // --- unblocked, use current buffer
      DBG3 (( MF__, "unblocked %d", length  ));

      ulNumOfBytes = length;
      pHF->lPos    = 0;

      SAPDB_memcpy ( pHF->pBuffer, block, ulNumOfBytes);
      pHF->pBuffer [ ulNumOfBytes++ ] = CR;
      pHF->pBuffer [ ulNumOfBytes++ ] = LF;

      rc = sql06_io_write ( pHF->hfFileHandle, pHF->lDevType,
                            pHF->pBuffer, ulNumOfBytes, &ulBytesWritten);

      if ((rc != NO_ERROR) || (ulBytesWritten != ulNumOfBytes))
        {
        lError       = VF_NOTOK;
        pHF->ulState = HFS_ERROR;

        if ( sql06c_handle_known_errors( pHF->szFileName,
                                         rc, pcErrText ) == FALSE )
          {
          DBG1 (( MF__, ERRMSG_VF_WRITE ));
          MSGD (( ERR_WRITE, rc         ));
          sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRITE, rc );
          }
        }
      else
        {
        DBG3 (( MF__, "ulBytesWritten %d", ulBytesWritten ));
        lError       = VF_OK;
        pHF->ulState = HFS_USED;
        pHF->lPos    = UNDEF;
        }
      }
    }

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG sql06c_write_bin_var ( PHOSTFILE_CTRL_REC  pHF,
                            VF_BUFFERADDRESS    block,
                            INT4                length,
                            PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_write_bin_var"
  LONG       lError  = VF_OK;
  LONG       lCurrFilePos,
             lNext4KPos;

  DBGIN;

  DBG3 (( MF__, "length  = %d", length ));

  lError = VF_OK ;

  if ( length > 0 )
    {
    if ( length < sizeof (VF_BLOCK) - 1 )
      {
      // --- TRUE data record :
      //     The following ensures that variable length
      //     records are not split across 4K record
      //     boundaries.  This allows any variable file
      //     file to read with only a 4K buffer, even
      //     if it was created with a larger buffer.

      lCurrFilePos = pHF->lFilePos + pHF->lPos + 1 ;
      lNext4KPos  = (lCurrFilePos + 1) - ( (lCurrFilePos + 1) % sizeof (VF_BLOCK));

      DBG3 (( MF__, "pHF -> lFilePos  = %d", pHF -> lFilePos ));
      DBG3 (( MF__, "pHF -> lPos      = %d", pHF -> lPos ));
      DBG3 (( MF__, "lCurrFilePos     = %d", lCurrFilePos ));
      DBG3 (( MF__, "lNext4KPos       = %d", lNext4KPos ));

      if ( (lCurrFilePos + 1) % sizeof (VF_BLOCK) != 0 )
        lNext4KPos += sizeof (VF_BLOCK);

      DBG3 (( MF__, "lNext4KPos                 = %d", lNext4KPos ));
      DBG3 (( MF__, "lNext4KPos-(lCurrFilePos+1)= %d", lNext4KPos - (lCurrFilePos + 1)));

      if ( lNext4KPos - ( lCurrFilePos + 1 ) <= length + 2 )
        {
        // --- space in pHF->pHF_buf less than record to write
        memset ( & pHF->pBuffer [pHF->lPos + 1], NUL, (lNext4KPos - (lCurrFilePos + 1)));

        pHF->lPos += lNext4KPos - lCurrFilePos - 1;

        DBG3 (( MF__, "pHF->lPos = %d", pHF -> lPos ));

        if ( pHF->lPos + 1 >= pHF->lMaxPos )
          {
          // --- write pHF->pBuffer ( 'pHF->lPos' bytes )
          lError = sql06c_putbuffer ( pHF, pcErrText );
          }
        }

      if ( lError == VF_OK)
        {
        pHF->pBuffer[ ++pHF->lPos ] = (UCHAR) (length / 256);
        pHF->pBuffer[ ++pHF->lPos ] = (UCHAR) (length % 256);

        DBG3 (( MF__, "pHF -> pBuffer              = 0x%x", pHF->pBuffer ));
        DBG3 (( MF__, "pHF -> lPos                 = %d", pHF->lPos ));
        DBG3 (( MF__, "Cpy to address bufptr[pos+1]= 0x%x", &pHF->pBuffer[pHF->lPos+1] ));

        SAPDB_memcpy ( &(pHF->pBuffer[pHF->lPos + 1] ), block, length );
        pHF->lPos = pHF->lPos + length ;

        DBG3 ((MF__, "pHF->lPos = %d", pHF->lPos ));
        }

      if (pHF->lPos < (pHF->lMaxPos - 1) && ( !odd(pHF->lPos)))
        {
        // --- odd
        pHF->pBuffer[++pHF->lPos] = (UCHAR) NUL;
        }
      }
    else
      {
      // --- length >= 4094
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR;

        DBG1 (( MF__, ERRMSG_VF_ILL_REC_LEN ));
        MSGD (( ERR_ILL_REC_LEN ));
        sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_REC_LEN, 0 );
      }
    }

  DBG3 (( MF__, "pHF->lPos = %d", pHF->lPos ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG sql06c_write_bin_fix ( PHOSTFILE_CTRL_REC  pHF,
                            VF_BUFFERADDRESS    block,
                            INT4                length,
                            PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_write_bin_fix"
  LONG       lError  = VF_OK;
  ULONG      ulNumOfBytes;
  ULONG      ulByteRemainder;
  long       lSourcePos;

  DBGIN;

  DBG3 (( MF__, "pHF->lRecordLength   %d", pHF->lRecordLength ));

  if ((pHF->lRecordLength >= sizeof (VF_BLOCK)) ||
      (pHF->FileFormat  == VF_4K_BLOCK)         ||
      (pHF->FileFormat  == VF_STREAM))
      {
      // --- buffer unit
      lError = sql06c_put_xk_buffer ( pHF, block, length, pcErrText );
      }
   else
      {
      // --- part of buffer
      ulNumOfBytes = (pHF->lMaxPos - pHF->lPos - 1);

      // --- enough space?
      if ( ulNumOfBytes > (ULONG)pHF->lRecordLength )
        {
        ulNumOfBytes     = pHF->lRecordLength;
        ulByteRemainder  = 0;
        }
      else
        {
        // --- use remainder
        ulByteRemainder = pHF->lRecordLength - ulNumOfBytes;
        }

      // --- block position
      lSourcePos = 0;

      if ( ulNumOfBytes > 0 )
        {
        // --- use current buffer
        SAPDB_memcpy ( &pHF->pBuffer[pHF->lPos+1], block, ulNumOfBytes );
        pHF->lPos  += ulNumOfBytes;
        lSourcePos  = ulNumOfBytes;
        }

      DBG3 (( MF__, "ulNumOfBytes         %d", ulNumOfBytes));
      DBG3 (( MF__, "ulByteRemainder      %d", ulByteRemainder));
      DBG3 (( MF__, "pHF->lPos            %d", pHF->lPos ));
      DBG3 (( MF__, "pHF->lEndOfBuffer    %d", pHF->lEndOfBuffer ));
      DBG3 (( MF__, "pHF->lFilePos        %d", pHF->lFilePos ));

      if ( ulByteRemainder >  0 )
        {
        // --- write pHF->pBuffer ( 'pHF->lPos' bytes )
        lError = sql06c_putbuffer ( pHF, pcErrText );

        if ( lError == VF_OK)
          {
          if (( lSourcePos + ulByteRemainder <= sizeof (VF_BLOCK) )   ||
              ( pHF->lPos  + ulByteRemainder <= sizeof (VF_BLOCK) ))
            {
            SAPDB_memcpy ( &pHF->pBuffer[pHF->lPos+1],
                     &block[lSourcePos],
                     ulByteRemainder );
            }
          else
            {
            lError       = VF_NOTOK;
            pHF->ulState = HFS_ERROR;

            DBG1 (( MF__, ERRMSG_VF_ILL_DATA_SIZE ));
            MSGD (( ERR_ILL_DATA_SIZE              ));
            sql46c_build_error_string ( pcErrText,
                                        ERRMSG_VF_ILL_DATA_SIZE, 0 );
            }

          pHF->lPos = ulByteRemainder - 1;
          }
        }
      }

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * The first block is read and the file format got (from byte 3 and 4)
 */

LONG sql06c_get_desc ( PHOSTFILE_CTRL_REC  pHF,
                       PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_get_desc"
  LONG      lError = VF_OK;
  ULONG     ulFileFormatRequest = pHF->FileFormat;
  DBGIN;

  if ( pHF->FileFormat == VF_4K_BLOCK )
    {
    pHF->lRecordLength  = KBYTE_4;
    pHF->lPos           = - 1;
    }
  else
    {
    bool    fBigHeader      = pHF->lHeaderLength > sizeof (VF_BLOCK);
    PUCHAR  pSaveBufferAdr  = pHF->pBuffer ;
    // --- read next block into pHF->pBuffer  ( 'pHF->ReadCount' Bytes )
    pHF->lReadCount = pHF->lHeaderLength ; // sizeof (VF_BLOCK);
    if ( fBigHeader )
      pHF->pBuffer = pHF->pFirstBlock ;    // enough place for big header

    lError = sql06c_getbuffer ( pHF,  pcErrText );

    if ( lError == VF_OK )
      {
      if ( pHF->pBuffer[0] == (UCHAR)NUL  &&
           pHF->pBuffer[1] == (UCHAR)NUL    )
        {
#if 0
        if ( pHF->pBuffer[2] == (UCHAR)0xff &&
             pHF->pBuffer[3] == (UCHAR)0xff &&
             pHF->pBuffer[4] == (UCHAR)0xff &&
             pHF->pBuffer[5] == (UCHAR)0xff   )
#else
        // --- allow to read files from rel3.1.1
        if ( pHF->pBuffer[2] == (UCHAR)0xff &&
             pHF->pBuffer[3] == (UCHAR)0xff &&
            (pHF->pBuffer[4] == (UCHAR)0xff || pHF->pBuffer[4] == (UCHAR)0x00) &&
            (pHF->pBuffer[5] == (UCHAR)0xFF || pHF->pBuffer[5] == (UCHAR)0x00))
#endif
          {
          pHF->lRecordLength  = - 1;
          pHF->lPos           = KBYTE_4 - 1;
          pHF->FileFormat     = VF_STREAM;
          }
        else
          {
          pHF->lRecordLength = 16777216 * pHF->pBuffer[4] +
                                  65536 * pHF->pBuffer[5] +
                                    256 * pHF->pBuffer[2] +
                                          pHF->pBuffer[3];

          if ( pHF->lRecordLength == 0 )
            {
            pHF->lPos         = KBYTE_4 - 1;
            pHF->FileFormat   = VF_RECORD;
            }
          else
            {
            if ( pHF->lRecordLength > KBYTE_64         ||
                 pHF->lRecordLength < MAGIC_REC_LEN    ||
                 ( ( pHF->lRecordLength > KBYTE_4        ||
                     pHF->lSizeOfBlocks > KBYTE_4 ) &&
                     pHF->lRecordLength != pHF->lSizeOfBlocks )
                 )

              {
              DBG1 (( MF__, ERRMSG_VF_ILL_FILE_SPEC ));
              MSGD (( ERR_ILL_FILE_SPEC              ));
              MSGD (( ERR_BLOCK_SIZE_MISMATCH,
                      pHF->lSizeOfBlocks ,pHF->lRecordLength  ));
              sql46c_build_error_string ( pcErrText,
                                          ERRMSG_VF_ILL_FILE_SPEC, 0 );

              pHF->ulState   = HFS_ERROR;
              lError         = VF_NOTOK;
              }
            else
              {
              pHF->lPos         = KBYTE_4 - 1;
              pHF->FileFormat   = VF_RECORD;
              }
            }
          }
        }
      else
        {
        pHF->lRecordLength = 0;
        pHF->lPos          = -1;
        pHF->FileFormat    = VF_PLAINTEXT ;
        }

      if ( ulFileFormatRequest == VF_STREAM )
        pHF->ulEffFileFormat = VF_STREAM;
      else
        pHF->ulEffFileFormat = pHF->FileFormat;
      }
    if ( fBigHeader )
      pHF->pBuffer = pSaveBufferAdr ; // pBuffer unused ( read_fix ) but freed
    }

  DBG3 (( MF__, "pHF->lPos               = %d", pHF->lPos   ));
  DBG3 (( MF__, "pHF->lRecordLength      = %d", pHF->lRecordLength));
  DBG3 (( MF__, "pHF->FileFormat         = %d", pHF->FileFormat ));
  DBG3 (( MF__, "pHF->ulEffFileFormat    = %d", pHF->ulEffFileFormat));


  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * The description is put (3. and 4.byte);
 *
 * If record length is 4kB the first block is written;
 */

LONG sql06c_put_desc ( PHOSTFILE_CTRL_REC  pHF,
                       PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_put_desc"
  INT4         lDescLen  = KBYTE_4;
  LONG         lError    = VF_OK;
  bool         fLengthOK;

  DBGIN;

  if ( pHF->FileFormat == VF_4K_BLOCK )
    {
    pHF->lRecordLength  = KBYTE_4;
    pHF->lPos           = - 1;
    }
  else
    {
    // --- write description if the file format is equal to 'VF_STREAM' or 'VF_RECORD'
    if (( pHF->FileFormat == VF_RECORD ) || ( pHF->FileFormat == VF_STREAM ))
      {
      // - length ok ?
      fLengthOK = ( pHF->lRecordLength  == 0  ) ||
                  ( pHF->lRecordLength  == -1 ) ||
                  ((pHF->lRecordLength  >= MAGIC_REC_LEN ) &&
                   (pHF->lRecordLength  <= KBYTE_64 ));

      if ( fLengthOK == FALSE )
        {
        DBG1 (( MF__, ERRMSG_VF_ILL_FILE_SPEC ));
        MSGD (( ERR_ILL_FILE_SPEC              ));
        sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_FILE_SPEC, 0 );

        pHF->ulState = HFS_ERROR;
        lError       = VF_NOTOK;

        DBGOUT;

        return (lError);
        }

      memset ( &pHF->pBuffer[0] , (char) 0 , lDescLen );

      // - put description
      if ( pHF->FileFormat == VF_STREAM )
        {
        pHF->lRecordLength = - 1;
        pHF->pBuffer[2]    = (UCHAR) 0xff;
        pHF->pBuffer[3]    = (UCHAR) 0xff;
        pHF->pBuffer[4]    = (UCHAR) 0xff;
        pHF->pBuffer[5]    = (UCHAR) 0xff;
        }
      else
        {
        //
        // --- 0x12345678 ->  pBuffer[2] = 0x56
        //                    pBuffer[3] = 0x78
        //                    pBuffer[4] = 0x12
        //                    pBuffer[5] = 0x34
        //
        pHF->pBuffer[2] = (UCHAR) (( pHF->lRecordLength % KBYTE_64 ) / 256);
        pHF->pBuffer[3] = (UCHAR) (( pHF->lRecordLength % KBYTE_64 ) % 256);
        pHF->pBuffer[4] = (UCHAR) (( pHF->lRecordLength / KBYTE_64 ) / 256);
        pHF->pBuffer[5] = (UCHAR) (( pHF->lRecordLength / KBYTE_64 ) % 256);
        }

      pHF->lPos = lDescLen - 1;

      DBG3 (( MF__, "pHF->lPos         = %d", pHF->lPos   ));
      DBG3 (( MF__, "pHF->lMaxFilePos  = %d", pHF->lMaxFilePos ));
      DBG3 (( MF__, "pHF->lFilePos     = %d", pHF->lFilePos ));

      // --- exceeds the current file position the maximum ?
      if ( pHF->lFilePos + pHF->lPos > pHF->lMaxFilePos )
        {
        // --- set a new maximum of file positon
        pHF->lMaxFilePos = pHF->lFilePos + pHF->lPos;
        DBG3 (( MF__, "pHF->lMaxFilePos = %d", pHF->lMaxFilePos ));
        }

      // --- write pHF->pBuffer ( 'pHF->lPos' bytes )
      lError = sql06c_putbuffer ( pHF, pcErrText );
      }
    }

  DBG3 (( MF__, "pHF->lPos = %d", pHF->lPos ));
  DBG3 (( MF__, "lError    = %d", lError ));

  DBGOUT;

  return (lError);
  }

/*------------------------------*/

LONG  sql06c_putbuffer ( PHOSTFILE_CTRL_REC  pHF,
                         PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_putbuffer"
  ULONG          ulBytesToWrite;
  ULONG          ulBytesWritten;
  APIRET         rc      = NO_ERROR;
  LONG           lError  = VF_OK;

  DBGIN;

  DBG3 (( MF__, "pHF->lPos    = %d", pHF->lPos ));
  DBG3 (( MF__, "pHF->pBuffer = 0x%x", pHF->pBuffer ));

  ulBytesWritten  = 0;
  ulBytesToWrite  = 0;

  if (( pHF->Direction == VWRITE     ) ||
      ( pHF->Direction == VOVERWRITE ) ||
      ( pHF->Direction == VAPPEND    ))
    {
    // --- something inside the buffer ?
    if ( pHF->lPos < 0 )
      {
      DBG3 (( MF__, "Buffer empty" ));

      DBGOUT;
      return (VF_OK);
      }

    ulBytesToWrite = pHF->lPos + 1;

    DBG3 (( MF__, "ulBytesToWrite  = %d", ulBytesToWrite ));

    rc = sql06_io_write ( pHF->hfFileHandle, pHF->lDevType,
                          pHF->pBuffer, ulBytesToWrite, &ulBytesWritten);

    DBG3 (( MF__, "ulBytesWritten = %d\n", ulBytesWritten ));

    // --- write failed ?
    if ( (rc != NO_ERROR) || (ulBytesWritten != ulBytesToWrite) )
      {
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR;

      if ( sql06c_handle_known_errors( pHF->szFileName,
                                       rc, pcErrText ) == FALSE )
        {
        DBG1 (( MF__, ERRMSG_VF_WRITE ));
        MSGD (( ERR_WRITE, rc         ));
        sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRITE, rc );
        }
      }
    else
      {
      DBG3 (( MF__, "write ok,written bytes(ulBytesToWrite)= %d", ulBytesToWrite ));
      DBG3 (( MF__, "pHF->lFilePos (before)                = %d", pHF->lFilePos  ));

      pHF->lFilePos += pHF->lPos + 1 ;
      pHF->lMaxPos   = pHF->lEndOfBuffer;
      pHF->lPos      = UNDEF ;

      DBG3 (( MF__, "pHF -> lFilePos (after)     = %d\n", pHF -> lFilePos  ));
      DBG3 (( MF__, "pHF -> lEndOfBuffer=lMaxPos = %d\n", pHF -> lEndOfBuffer  ));
      DBG3 (( MF__, "pHF -> lPos                 = %d\n", pHF -> lPos ));
      }
    }
  else
    {
    lError       = VF_NOTOK;
    pHF->ulState = HFS_ERROR;

    DBG1 (( MF__, ERRMSG_VF_ILL_DIREC ));
    MSGD (( ERR_ILL_DIREC             ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_DIREC, 0 );
    }

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG  sql06c_getbuffer ( PHOSTFILE_CTRL_REC  pHF,
                         PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_getbuffer"
  ULONG      ulBytesRead;
  LONG       rc          = NO_ERROR;
  LONG       lError      = VF_OK;

  DBGIN;

  DBG3 (( MF__, "pHF->lReadCount = %d", pHF -> lReadCount )) ;

  ulBytesRead = 0;

  if (( pHF->Direction == VREAD ) || ( pHF->Direction == VAPPEND ))
    {
    rc = sql06_io_read ( pHF->hfFileHandle, pHF->lDevType,
                         pHF->pBuffer, pHF->lReadCount, &ulBytesRead );

    DBG3 (( MF__, "ulBytesRead  = %d", ulBytesRead ));

    pHF->lReadCount = pHF->lEndOfBuffer ;  // --- read full buffer next time

    // --- read failed?
    if (( rc != NO_ERROR )                &&
        ( rc != ERROR_HANDLE_EOF )        &&
        ( rc != ERROR_NO_DATA_DETECTED )  &&
        ( rc != ERROR_FILEMARK_DETECTED ) &&
        ( rc != ERROR_END_OF_MEDIA )      &&
        ( rc != ERROR_PIPE_NOT_CONNECTED) &&
        ( rc != ERROR_BROKEN_PIPE ))
      {
      lError       = VF_NOTOK;
      pHF->ulState = HFS_ERROR;
      pHF->lMaxPos = UNDEF;

      if ( sql06c_handle_known_errors( pHF->szFileName,
                                       rc, pcErrText ) == FALSE )
        {
        DBG1 (( MF__, ERRMSG_VF_READ ));
        MSGD (( ERR_READ, rc         ));
        sql46c_build_error_string ( pcErrText, ERRMSG_VF_READ, rc );
        }
      }
    else
      {
      if ( ulBytesRead == 0 )
        {
        lError       = VF_EOF;
        pHF->ulState = HFS_EOF;
        pHF->lMaxPos = UNDEF;
        }
      else
        pHF->ulState = HFS_USED;

      /* pHF->lFilePos += pHF->lPos + 1; */
      pHF->lFilePos += pHF->lMaxPos;
      pHF->lPos      = UNDEF;
      pHF->lMaxPos   = ulBytesRead;
      }
    }
  else
    {
    lError       = VF_NOTOK;
    pHF->ulState = HFS_ERROR;

    DBG1 (( MF__, ERRMSG_VF_ILL_DIREC ));
    MSGD (( ERR_ILL_DIREC             ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_DIREC, 0 );
    }

  DBG3 (( MF__, "pHF -> lPos       = %d", pHF -> lPos ));
  DBG3 (( MF__, "pHF -> lMaxPos    = %d", pHF -> lMaxPos ));
  DBG3 (( MF__, "pHF -> lFilePos   = %d", pHF -> lFilePos ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG sql06c_get_xk_buffer ( PHOSTFILE_CTRL_REC  pHF,
                            VF_BUFFERADDRESS    block,
                            INT4                * length,
                            PCHAR               pcErrText )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_get_xk_buffer"
  LONG      lError       = VF_OK;
  APIRET    rc           = NO_ERROR;
  ULONG     ulBytesRead  = 0;

  DBGIN;

  DBG3 (( MF__, "pHF -> hfFileHandle = %d", pHF->hfFileHandle ));
  DBG3 (( MF__, "* length            = %d", *length ));
  DBG3 (( MF__, "block               = %d", block ));

  if (((pHF->Direction == VREAD) || (pHF->Direction == VAPPEND)) &&
      ( pHF->ulState == HFS_USED ))
    {
    pHF->lReadCount = pHF->lEndOfBuffer;

    // --- read
    rc = sql06_io_read ( pHF->hfFileHandle, pHF->lDevType,
                         block, *length, &ulBytesRead );

    if (( rc != NO_ERROR )                &&
        ( rc != ERROR_HANDLE_EOF )        &&
        ( rc != ERROR_NO_DATA_DETECTED )  &&
        ( rc != ERROR_FILEMARK_DETECTED ) &&
        ( rc != ERROR_END_OF_MEDIA )      &&
        ( rc != ERROR_PIPE_NOT_CONNECTED) &&
        ( rc != ERROR_BROKEN_PIPE ))
      {
      lError       = VF_NOTOK ;
      pHF->ulState = HFS_ERROR ;
      pHF->lMaxPos = UNDEF ;

      if ( sql06c_handle_known_errors( pHF->szFileName,
                                       rc, pcErrText ) == FALSE )
        {
        DBG1 (( MF__, ERRMSG_VF_READ ));
        MSGD (( ERR_READ, rc         ));
        sql46c_build_error_string ( pcErrText, ERRMSG_VF_READ, rc );
        }
      }
    else
      {
      DBG3 (( MF__, "ulBytesRead = %d", ulBytesRead ));

      if ( ulBytesRead == 0 )
        {
        lError       = VF_EOF;
        pHF->ulState = HFS_EOF;
        pHF->lMaxPos = UNDEF;
        }
      else
        {
        lError       = VF_OK;
        pHF->ulState = HFS_USED;
        }

      pHF->lFilePos += pHF->lPos + 1;
      pHF->lPos      = UNDEF;
      pHF->lMaxPos   = ulBytesRead;

      DBG3 (( MF__, "pHF->lFilePos = %d", pHF->lFilePos ))
      }
    }
  else
    {
    pHF->ulState = HFS_ERROR;
    lError       = VF_NOTOK;

    DBG1 (( MF__, ERRMSG_VF_ILL_DIREC ));
    MSGD (( ERR_ILL_DIREC             ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_DIREC, 0 );
    }

  DBG3 (( MF__, "pHF->lPos      = %d", pHF -> lPos ));
  DBG3 (( MF__, "pHF->lMaxPos   = %d", pHF -> lMaxPos ));
  DBG3 (( MF__, "pHF->lFilePos  = %d", pHF -> lFilePos ));
  DBG3 (( MF__, "lError         = %d", lError       ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

LONG sql06c_put_xk_buffer ( PHOSTFILE_CTRL_REC  pHF,
                            VF_BUFFERADDRESS    block,
                            INT4                length,
                            PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_put_xk_buffer"
  LONG      lError  = VF_OK;
  APIRET    rc      = NO_ERROR;
  ULONG     ulBytesWritten;

  DBGIN;

  ulBytesWritten = 0;
  pHF->lPos      = length - 1;

  if ((( pHF->Direction == VWRITE     ) ||
       ( pHF->Direction == VOVERWRITE ) ||
       ( pHF->Direction == VAPPEND    )) && ( pHF->ulState == HFS_USED ))
    {
    // --- something to write
    if ( length > 0 )
      {
      rc = sql06_io_write ( pHF->hfFileHandle, pHF->lDevType,
                            block, length, &ulBytesWritten);

      if ( (rc != NO_ERROR) || (ulBytesWritten != (ULONG)length) )
        {
        // --- write failed
        lError       = VF_NOTOK;
        pHF->ulState = HFS_ERROR ;

        if ( sql06c_handle_known_errors( pHF->szFileName,
                                         rc, pcErrText ) == FALSE )
          {
          DBG1 (( MF__, ERRMSG_VF_WRITE ));
          MSGD (( ERR_WRITE, rc         ));
          sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRITE, rc );
          }
        }
      else
        {
        DBG3 (( MF__, "ulBytesWritten = %d\n", ulBytesWritten ));

        pHF->lFilePos += length;
        pHF->lMaxPos   = pHF->lEndOfBuffer;
        pHF->lPos      = UNDEF ;

        DBG3 (( MF__, "pHF -> lFilePos = %d", pHF ->lFilePos))
        }
      }
    }
  else
    {
    lError       = VF_NOTOK;
    pHF->ulState = HFS_ERROR;

    DBG1 (( MF__, ERRMSG_VF_ILL_DIREC ));
    MSGD (( ERR_ILL_DIREC             ));
    sql46c_build_error_string ( pcErrText, ERRMSG_VF_ILL_DIREC, 0 );
    }

  DBG3 (( MF__, "error %d", lError       ));

  DBGOUT;
  return (lError);
  }

/*------------------------------*/

/*
 * lPositions file and pHf->pBuffer
 */

LONG    sql06c_posbuffer    ( PHOSTFILE_CTRL_REC  pHF,
                              LONG                lDistance,
                              ULONG               ulMoveType,
                              PULONG              pulNewPointer,
                              PCHAR               pcErrText       )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_posbuffer"
  APIRET  rc      = NO_ERROR;
  LONG    lError  = VF_OK;


  DBGIN;

  DBG3 (( MF__, "lDistance         = %d", lDistance         ));
  DBG3 (( MF__, "ulMoveType        = %d", ulMoveType        ));
  DBG3 (( MF__, "pHF -> ulFileType = %d", pHF -> ulFileType ));

  if ( pHF->ulFileType == VFT_DISK )
    {
    rc = SET_FILE_PTR ( pHF->hfFileHandle,     // File handle
                        lDistance,             // Distance to move
                        ulMoveType,            // Method of moving
                        pulNewPointer,         // New filepointer location
                        NULL );

    if ( rc != NO_ERROR )
      {
      lError            = VF_NOTOK;
      pHF -> ulState    = HFS_ERROR;
      pHF -> lMaxPos    = 0;
      pHF -> lReadCount = sizeof ( VF_BLOCK );
      pHF -> lPos       = UNDEF;

      DBG1 (( MF__, ERRMSG_VF_SEEK ));
      MSGD (( ERR_SEEK, rc         ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_SEEK, rc );
      }
    else
      {
      lError            = VF_OK;
      pHF -> ulState    = HFS_USED;
      pHF -> lPos       = UNDEF;
      pHF -> lReadCount = pHF -> lEndOfBuffer -
                       ( *pulNewPointer % pHF -> lEndOfBuffer );
      pHF -> lMaxPos    = pHF -> lReadCount;
      pHF -> lFilePos   = *pulNewPointer - 1;
      }

    DBG3 (( MF__, "*pulNewPointer                     = %d",
            *pulNewPointer));
    DBG3 (( MF__, "pHF -> lMaxPos = pHF -> lReadCount = %d",
            pHF->lMaxPos ));
    DBG3 (( MF__, "pHF -> lPos                        = %d",
            pHF->lPos ));
    DBG3 (( MF__, "pHF -> lEndOfBuffer                = %d",
            pHF -> lEndOfBuffer ));
    DBG3 (( MF__, "pHF -> lFilePos                    = %d",
            pHF->lFilePos ));
    }

  DBGOUT;

  return (lError);
  }

/*------------------------------*/

// - Although the virtual file uses the value UNDEF to indicate an invalid
//   file handle, some of the tool components (precompiler) also assume that
//   0 will also be recognized as invalid.

BOOLEAN sql06c_valid_fileno (   INT4                lHostFileNo,
                                PHOSTFILE_CTRL_REC  arcFileCtrlTab  )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_valid_fileno"


  #ifdef  DEBUG_RTE
   int nIndex;
  #endif


  DBGIN;


  // --- 'HostFileNo' unvalid ?
  DBG3 (( MF__, "Hostfile number = %d", lHostFileNo ));

  #ifdef  DEBUG_RTE
   for ( nIndex = 0; nIndex < HF_MAX; nIndex++ )
    {
    DBG3 (( MF__, "arcFileCtrlTab [ %d ].ulState = %d",
            nIndex, arcFileCtrlTab [ nIndex ].ulState ));
    }
  #endif

  if ( lHostFileNo < 1      ||
       lHostFileNo > HF_MAX ||
       arcFileCtrlTab [ lHostFileNo - 1 ].ulState == HFS_UNUSED )
    {
    DBG3 (( MF__, "lHostfile '%d' out of range", lHostFileNo ));

    DBGOUT;
    return (FALSE);
    }

  DBGOUT;

  return (TRUE);
  }


/*------------------------------*/


VOID sql06c_init_file_ctrl ( PHOSTFILE_CTRL_REC  pHF )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_init_file_ctrl"

  DBGIN;

  pHF->szFileName[0]      = NUL;
  pHF->Resource           = VF_STACK;
  pHF->hfFileHandle       = (HANDLE)INVALID_HANDLE_VALUE;
  pHF->lHeaderLength      = sizeof (VF_BLOCK) ;
  pHF->lRecordLength      =  0;
  pHF->lPos               = -1;
  pHF->lMaxPos            = -1;
  pHF->lFilePos           = -1;
  pHF->lReadCount         =  0;
  pHF->lMaxFilePos        = -1;
  pHF->FileFormat         = VF_UNKNOWN;
  pHF->ulEffFileFormat    = VF_UNKNOWN;
  pHF->ulFileType         = (ULONG)UNDEF;
  pHF->fBlocked           = FALSE;
  pHF->ulState            = HFS_UNUSED;
  pHF->pBuffer            = (PUCHAR)UNDEF;
  pHF->lEndOfBuffer       = 0;
  pHF->pFirstBlock        = (PUCHAR)UNDEF;
  pHF->lSizeOfBlocks      = 0;


  #if defined _WIN32
   pHF->ulTapeFeatures    = 0;
   pHF->ulMaxTapeBlkSize  = 0;
   pHF->fNxtInAutoLoader  = FALSE;
   pHF->lNxtInAutoLoaderError = VF_OK ;
  #endif

  DBGOUT;
  return;
  }

/*------------------------------*/

BOOLEAN sql06c_blocked_file (   VF_FILENAMEC    Filename,
                                ULONG           FileFormat,
                                INT4            RecordLength    )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_blocked_file"
  #define         NUM_NON_BLOCKED_EXTENSIONS 6
  VF_FILENAMEC    TempFilename;
  PSZ             pFound;
  CHAR            szNonBlockExt [ NUM_NON_BLOCKED_EXTENSIONS ] [ 7 ]
                        = { ".PROT", ".PRT", ".PCL", ".LST", ".PCT", ".LOG" };
  ULONG           ulCnt;

  DBGIN;

  strcpy( TempFilename, Filename );
  strupr( TempFilename );

  if (( FileFormat == VF_RECORD  ) ||
      ( FileFormat == VF_STREAM  ) ||
        RecordLength )
    {
    DBG3 (( MF__, "return TRUE" ));

    DBGOUT;
    return (TRUE);
    }

  for ( ulCnt = 0; ulCnt < NUM_NON_BLOCKED_EXTENSIONS ; ulCnt++ )
    {
    pFound  = strstr ( TempFilename , szNonBlockExt[ulCnt] );

    // --- file extention found?
    if ( (pFound != NULL) &&
         (strlen(pFound) == strlen(szNonBlockExt[ulCnt]) ) )
      {
      DBG3 (( MF__, "return FALSE" ));

      DBGOUT;
      return (FALSE);
      }
    }

  DBG3 (( MF__, "return TRUE" ));

  DBGOUT;
  return (TRUE);
  }

/*------------------------------*/

LONG sql06_io_read ( HANDLE    hfDevice,
                            ULONG     ulDevType,
                            PVOID     pBlockPtr,
                            ULONG     ulBlockLength,
                            PULONG    pulBytesRead )
  {
  #undef  MF__
  #define MF__ MOD__"sql05_io_read"
  ULONG           ulLength;
  ULONG           ulLengthWanted;
  APIRET          rc       = NO_ERROR;
  PCHAR           pBuffer;
  #if defined(_WIN32)
   INT            Retry    = TAPE_ERROR_RETRY;
  #endif

  DBGIN;

  switch ( ulDevType )
    {
    case DT_PIPE:
      pBuffer        = pBlockPtr;
      ulLengthWanted = ulBlockLength;
      do
        {
        rc = READ_FILE ( hfDevice, pBuffer,
                         ulLengthWanted, &ulLength );

        ulLengthWanted -= ulLength;
        pBuffer        += ulLength;
        }
      while (( ulLengthWanted > 0 ) && ( rc == NO_ERROR ) && ( ulLength != 0 ));

      *pulBytesRead = ulBlockLength - ulLengthWanted;
      break;

    case DT_TAPE:
      #if defined(_WIN32)
       pBuffer        = pBlockPtr;
       ulLengthWanted = ulBlockLength;
       do
         {
         rc = READ_FILE ( hfDevice, pBuffer,
                          ulLengthWanted, &ulLength );

         ulLengthWanted -= ulLength;
         pBuffer        += ulLength;
         }
       while (( rc == ERROR_BUS_RESET ||
                rc == ERROR_IO_DEVICE ||
                rc == ERROR_CRC    ) &&
              ( ulLengthWanted > 0 ) && --Retry );

       *pulBytesRead = ulBlockLength - ulLengthWanted;
      #else
       rc = READ_FILE ( hfDevice, pBlockPtr, ulBlockLength, pulBytesRead );
      #endif

      break;

    default:
      rc = READ_FILE ( hfDevice, pBlockPtr, ulBlockLength, pulBytesRead );
      break;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql06_io_write ( HANDLE    hfDevice,
                      ULONG     ulDevType,
                      PVOID     pBlockPtr,
                      ULONG     ulBlockLength,
                      PULONG    pulBytesWritten )
  {
  #undef  MF__
  #define MF__ MOD__"sql06_io_write"
  APIRET          rc       = NO_ERROR;
  #if defined(_WIN32)
   ULONG          ulLength;
   ULONG          ulLengthWanted;
   PCHAR          pBuffer;
   INT            Retry    = TAPE_ERROR_RETRY;
  #endif

  DBGIN;

  switch ( ulDevType )
    {
    case DT_TAPE:
      #if defined(_WIN32)
       pBuffer        = pBlockPtr;
       ulLengthWanted = ulBlockLength;
       do
         {
         rc = WRITE_FILE ( hfDevice, pBuffer, ulLengthWanted, &ulLength );

         ulLengthWanted -= ulLength;
         pBuffer        += ulLength;
         }
       while (( rc == ERROR_BUS_RESET || rc == ERROR_IO_DEVICE ) &&
              ( ulLengthWanted > 0 ) && --Retry );

       *pulBytesWritten = ulBlockLength - ulLengthWanted;
      #else
       rc = WRITE_FILE ( hfDevice, pBlockPtr, ulBlockLength, pulBytesWritten );
      #endif

      break;

    default:
      rc = WRITE_FILE ( hfDevice, pBlockPtr, ulBlockLength, pulBytesWritten );
      break;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

bool sql06c_handle_known_errors ( PSZ                 pszName,
                                         LONG                rc,
                                         PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_handle_known_errors"

  DBGIN;


  switch ( rc )
    {
    #if defined(_WIN32)
     case ERROR_END_OF_MEDIA:
       DBG1 (( MF__, ERRMSG_VF_TAPE_END_OF_MEDIA ));
       MSGD (( ERR_TAPE_END_OF_MEDIA, pszName ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_END_OF_MEDIA, 0 );
       return ( TRUE );

     case ERROR_FILEMARK_DETECTED:
     case ERROR_SETMARK_DETECTED :
       DBG1 (( MF__, ERRMSG_VF_TAPE_FILE_SET_MARK ));
       MSGD (( ERR_TAPE_FILE_SET_MARK, pszName ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_FILE_SET_MARK, 0 );
       return ( TRUE );

     case ERROR_DEVICE_NOT_PARTITIONED:
       DBG1 (( MF__, ERRMSG_VF_TAPE_NOT_PATITIONED ));
       MSGD (( ERR_TAPE_NOT_PATITIONED, pszName ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_NOT_PATITIONED, 0 );
       return ( TRUE );

     case ERROR_NO_MEDIA_IN_DRIVE:
       DBG1 (( MF__, ERRMSG_VF_TAPE_NO_MEDIA_IN_DRIVE ));
       MSGD (( ERR_TAPE_NO_MEDIA_IN_DRIVE, pszName ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_NO_MEDIA_IN_DRIVE,
                                   0 );

       return ( TRUE );

     case ERROR_MEDIA_CHANGED:
       DBG1 (( MF__, ERRMSG_VF_TAPE_MEDIA_CHANGED ));
       MSGD (( ERR_TAPE_MEDIA_CHANGED, pszName ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_MEDIA_CHANGED, 0 );
       return ( TRUE );

     case ERROR_NOT_READY :
     case ERROR_BUSY :
       DBG1 (( MF__, ERRMSG_VF_DEVICE_NOT_READY  ));
       MSGD (( ERR_DEVICE_NOT_READY, pszName ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_DEVICE_NOT_READY, 0 );
       return ( TRUE );
    #endif

    case ERROR_SHARING_VIOLATION:
      DBG1 (( MF__, ERRMSG_VF_FILE_SHARING_VIOLATION ));
      MSGD (( ERR_FILE_SHARING_VIOLATION, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_FILE_SHARING_VIOLATION, 0 );
      return ( TRUE );

    case ERROR_DISK_FULL:
      DBG1 (( MF__, ERRMSG_VF_DISK_FULL ));
      MSGD (( ERR_DISK_FULL, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_DISK_FULL, 0 );
      return ( TRUE );

    case ERROR_PATH_NOT_FOUND:
      DBG1 (( MF__, ERRMSG_VF_WRONG_PATH ));
      MSGD (( ERR_WRONG_PATH, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRONG_PATH, 0 );
      return ( TRUE );

    case ERROR_FILE_NOT_FOUND:
      DBG1 (( MF__, ERRMSG_VF_WRONG_FILE_DEVICE_NAME ));
      MSGD (( ERR_WRONG_FILE_DEVICE_NAME, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_WRONG_FILE_DEVICE_NAME, 0 );
      return ( TRUE );

    case ERROR_WRITE_PROTECT:
      DBG1 (( MF__, ERRMSG_VF_MEDIA_WRITE_PROTECTED ));
      MSGD (( ERR_MEDIA_WRITE_PROTECTED, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_MEDIA_WRITE_PROTECTED, 0 );
      return ( TRUE );

    case ERROR_ACCESS_DENIED :
      DBG1 (( MF__, ERRMSG_VF_ACCESS_DENIED ));
      MSGD (( ERR_ACCESS_DENIED_ON_XX, pszName ));
      sql46c_build_error_string ( pcErrText, ERRMSG_VF_ACCESS_DENIED, 0 );
      return ( TRUE );

    }

  DBGOUT;
  return (FALSE);
  }

/*------------------------------*/

#if defined (_WIN32)

static LONG sql06c_init_dlt_tape ( PHOSTFILE_CTRL_REC  pHF )
{
  #undef  MF__
  #define MF__ MOD__"sql06c_init_dlt_tape"

  APIRET                     rc             = NO_ERROR;
  INT                        Retry          = TAPE_ERROR_RETRY;

  DBGIN;

  //
  // --- try to load tape.
  //     it's needed by some tape drives (DLT) as first operation
  //
  do
    {
    rc = PrepareTape ( (HANDLE)pHF->hfFileHandle, TAPE_LOAD , FALSE);
    }
  while (( rc == ERROR_MEDIA_CHANGED        ||
           rc == ERROR_END_OF_MEDIA         ||
           rc == ERROR_BEGINNING_OF_MEDIA   ||
           rc == ERROR_IO_DEVICE            ||
           rc == ERROR_BUS_RESET            ) && --Retry);
  //
  // --- no error handling, ignore all errors
  //

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

static LONG sql06c_get_drive_info ( PHOSTFILE_CTRL_REC  pHF,
                                    PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_get_drive_info"

  #if defined(_WIN32)
   APIRET                     rc         = NO_ERROR;
   LONG                       lError     = VF_OK;
   INT                        Retry      = TAPE_ERROR_RETRY;
   TAPE_GET_DRIVE_PARAMETERS  DriveInfo;
   DWORD                      dwInfoSize;

   DBGIN;

   //
   //  --- get tape device information
   //
   do
     {
     dwInfoSize  = sizeof ( DriveInfo );
     memset ( &DriveInfo, 0, sizeof(DriveInfo));

     rc = GetTapeParameters( (HANDLE)pHF->hfFileHandle,
                             GET_TAPE_DRIVE_INFORMATION,
                             &dwInfoSize, &DriveInfo );
     }
   while (( rc == ERROR_BUS_RESET          ||
            rc == ERROR_IO_DEVICE          ||
            rc == ERROR_END_OF_MEDIA       ||
            rc == ERROR_BEGINNING_OF_MEDIA ||
            rc == ERROR_MEDIA_CHANGED )    && --Retry);

   if (( rc != NO_ERROR )                 &&
       ( rc != ERROR_MEDIA_CHANGED )      &&
       ( rc != ERROR_END_OF_MEDIA )       &&
       ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
       ( rc != ERROR_INVALID_FUNCTION ))
     {
     pHF->ulState      = HFS_ERROR;
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( pHF->szFileName,
                                       rc , pcErrText ) == FALSE )
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
       MSGD (( ERR_TAPE_PARAM, rc         ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_PARAM, rc );
       }

     DBGOUT;
     return (lError);
     }
   else if ( rc != ERROR_INVALID_FUNCTION )
     {
     pHF->ulMaxTapeBlkSize = DriveInfo.MaximumBlockSize;
     pHF->fECC             = DriveInfo.ECC;
     pHF->fCompression     = DriveInfo.Compression;
     pHF->fDataPadding     = DriveInfo.DataPadding;
     pHF->ulTapeFeatures   = 0;

     if (( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_COMPRESSION ) &&
         ( DriveInfo.FeaturesLow  & TAPE_DRIVE_COMPRESSION ))
       pHF->ulTapeFeatures |= TAPE_SET_COMPRESSION;
     if (( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_ECC ) &&
         ( DriveInfo.FeaturesLow  & TAPE_DRIVE_ECC ))
       pHF->ulTapeFeatures |= TAPE_SET_ECC;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_BLOCK_SIZE )
       pHF->ulTapeFeatures |= TAPE_SET_BLOCK_SIZE;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_LOCK_UNLOCK )
       pHF->ulTapeFeatures |= TAPE_LOCK_UNLOCK;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_LOAD_UNLOAD )
       pHF->ulTapeFeatures |= TAPE_LOAD_UNLOAD;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_REPORT_SMKS )
       pHF->ulTapeFeatures |= TAPE_REPORT_SMKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_LONG_FMKS )
       pHF->ulTapeFeatures |= TAPE_WRITE_LONG_FMKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_SHORT_FMKS )
       pHF->ulTapeFeatures |= TAPE_WRITE_SHORT_FMKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_FILEMARKS )
       pHF->ulTapeFeatures |= TAPE_WRITE_FILEMARKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_MARK_IMMED )
       pHF->ulTapeFeatures |= TAPE_WRITE_MARK_IMMED;
     }

   DBGOUT;
   return (lError);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
   return ( VF_NOTOK );
  #endif
  }

/*------------------------------*/

static LONG sql06c_get_media_info ( PHOSTFILE_CTRL_REC  pHF,
                                    PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_get_media_info"

  #if defined(_WIN32)
   APIRET                     rc         = NO_ERROR;
   LONG                       lError     = VF_OK;
   INT                        Retry      = TAPE_ERROR_RETRY;
   TAPE_GET_MEDIA_PARAMETERS  MediaInfo;
   DWORD                      dwInfoSize;

   DBGIN;

   do
     {
     //
     //  --- get media information
     //
     dwInfoSize  = sizeof ( MediaInfo );
     memset ( &MediaInfo, 0, sizeof(MediaInfo));

     rc = GetTapeParameters( (HANDLE)pHF->hfFileHandle,
                             GET_TAPE_MEDIA_INFORMATION,
                             &dwInfoSize, &MediaInfo );
     }
   while (( rc == ERROR_BUS_RESET            ||
            rc == ERROR_IO_DEVICE            ||
            rc == ERROR_END_OF_MEDIA         ||
            rc == ERROR_BEGINNING_OF_MEDIA   ||
            rc == ERROR_MEDIA_CHANGED ) && --Retry);

   if (( rc != NO_ERROR )                 &&
       ( rc != ERROR_MEDIA_CHANGED )      &&
       ( rc != ERROR_END_OF_MEDIA )       &&
       ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
       ( rc != ERROR_INVALID_FUNCTION ))
     {
     pHF->ulState      = HFS_ERROR;
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( pHF->szFileName,
                                       rc , pcErrText ) == FALSE )
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
       MSGD (( ERR_TAPE_PARAM, rc         ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_PARAM, rc );
       }
     DBGOUT;
     return (lError);
     }
   else if ( rc != ERROR_INVALID_FUNCTION )
     {
     if ( MediaInfo.PartitionCount > 1)
       MSGD (( INFO_TAPE_WRONG_PART_CNT ));
     }

   DBGOUT;
   return (lError);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
   return ( VF_NOTOK );
  #endif
  }

/*------------------------------*/

static LONG sql06c_init_tape_dev( PHOSTFILE_CTRL_REC pHF,
                                  PCHAR              pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_init_tape_dev"

  #if defined(_WIN32)
   APIRET                     rc         = NO_ERROR;
   LONG                       lError     = VF_OK;
   INT                        Retry      = TAPE_ERROR_RETRY;
   TAPE_SET_DRIVE_PARAMETERS  DriveInfo;
   TAPE_SET_MEDIA_PARAMETERS  MediaInfo;
   PSZ                        pszTapeCompressEnabled;

   DBGIN;

   DriveInfo.ECC                = pHF->fECC;
   DriveInfo.Compression        = pHF->fCompression;
   DriveInfo.DataPadding        = pHF->fDataPadding;
   DriveInfo.ReportSetmarks     = FALSE;
   DriveInfo.EOTWarningZoneSize = 0;

   if ( pHF->ulTapeFeatures & TAPE_SET_COMPRESSION )
     {
     rc = GETENV ( TAPE_COMPRESS_ENV_VAR, &pszTapeCompressEnabled );

     if (( rc == NO_ERROR )  && ( *pszTapeCompressEnabled )  &&
         ( ! strcmp ( "OFF", strupr ( pszTapeCompressEnabled ))))
       DriveInfo.Compression  = FALSE;
     else
       DriveInfo.Compression  = TRUE;
     }

   if ( pHF->ulTapeFeatures & TAPE_SET_ECC )
     DriveInfo.ECC = TRUE;


   do
     {
     rc = SetTapeParameters( (HANDLE)pHF->hfFileHandle,
                             SET_TAPE_DRIVE_INFORMATION,
                             &DriveInfo );
     }
   while (( rc == ERROR_BUS_RESET            ||
            rc == ERROR_IO_DEVICE            ||
            rc == ERROR_END_OF_MEDIA         ||
            rc == ERROR_BEGINNING_OF_MEDIA   ||
            rc == ERROR_MEDIA_CHANGED ) && --Retry);

   if (( rc != NO_ERROR )                 &&
       ( rc != ERROR_MEDIA_CHANGED )      &&
       ( rc != ERROR_END_OF_MEDIA )       &&
       ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
       ( rc != ERROR_INVALID_FUNCTION ))
     {
     pHF->ulState      = HFS_ERROR;
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( pHF->szFileName,
                                       rc , pcErrText ) == FALSE )
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
       MSGD (( ERR_TAPE_PARAM, rc         ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_PARAM, rc );
       }
     DBGOUT;
     return (lError);
     }

   if ( ( pHF->ulMaxTapeBlkSize != 0 )  &&
       ( pHF->ulTapeFeatures & TAPE_SET_BLOCK_SIZE ))
     {
     if ( pHF->ulMaxTapeBlkSize < (ULONG)pHF->lSizeOfBlocks )
       MediaInfo.BlockSize = pHF->ulMaxTapeBlkSize;
     else
       MediaInfo.BlockSize = pHF->lSizeOfBlocks;

     do
       {
       rc = SetTapeParameters( (HANDLE)pHF->hfFileHandle,
                               SET_TAPE_MEDIA_INFORMATION,
                               &MediaInfo );
       }
     while (( rc == ERROR_BUS_RESET            ||
              rc == ERROR_IO_DEVICE            ||
              rc == ERROR_END_OF_MEDIA         ||
              rc == ERROR_BEGINNING_OF_MEDIA   ||
              rc == ERROR_MEDIA_CHANGED ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       pHF->ulState      = HFS_ERROR;
       lError            = VF_NOTOK;

       if ( sql06c_handle_known_errors ( pHF->szFileName,
                                         rc , pcErrText ) == FALSE )
         {
         DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
         MSGD (( ERR_TAPE_PARAM, rc         ));
         sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_PARAM, rc );
         }
       DBGOUT;
       return (lError);
       }
     }

   DBGOUT;
   return (lError);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
   return ( VF_NOTOK );
  #endif
  }

/*------------------------------*/

static LONG sql06c_tape_rewind( PHOSTFILE_CTRL_REC  pHF,
                                PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_tape_rewind"

  #if defined(_WIN32)
   APIRET       rc      = NO_ERROR;
   LONG         lError  = VF_OK;
   INT          Retry   = TAPE_ERROR_RETRY;

   DBGIN;

   do
     {
     rc = SetTapePosition((HANDLE)pHF->hfFileHandle, TAPE_REWIND,
                          0, 0, 0, FALSE);
     }
   while (( rc == ERROR_BUS_RESET          ||
            rc == ERROR_IO_DEVICE          ||
            rc == ERROR_MEDIA_CHANGED ) && --Retry);

   if (( rc != NO_ERROR ) && ( rc != ERROR_MEDIA_CHANGED ))
     {
     pHF->ulState      = HFS_ERROR;
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( pHF->szFileName,
                                       rc, pcErrText ) == FALSE)
       {
       DBG1 (( MF__, ERRMSG_VF_REWIND_TAPE ));
       MSGD (( ERR_REWIND_TAPE, rc         ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_REWIND_TAPE, rc );
       }
     DBGOUT;
     return (lError);
     }

   DBGOUT;
   return (lError);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
   return ( VF_NOTOK );
  #endif
  }

/*------------------------------*/

static LONG sql06c_tape_locking ( PHOSTFILE_CTRL_REC  pHF,
                                  PCHAR               pcErrText,
                                  ULONG               ulOperation )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_tape_locking"

  #if defined(_WIN32)
   APIRET       rc          = NO_ERROR;
   LONG         lError      = VF_OK;
   INT          Retry       = TAPE_ERROR_RETRY;

   DBGIN;

   if ( pHF->ulTapeFeatures & TAPE_LOCK_UNLOCK )
     {
     do
       {
       rc = PrepareTape ( (HANDLE)pHF->hfFileHandle, ulOperation, FALSE);
       }
     while (( rc == ERROR_BUS_RESET            ||
              rc == ERROR_IO_DEVICE            ||
              rc == ERROR_END_OF_MEDIA         ||
              rc == ERROR_BEGINNING_OF_MEDIA   ||
              rc == ERROR_MEDIA_CHANGED ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       pHF->ulState      = HFS_ERROR;
       lError            = VF_NOTOK;
       pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

       if ( sql06c_handle_known_errors ( pHF->szFileName,
                                         rc, pcErrText ) == FALSE)
         {
         DBG1 (( MF__, ERRMSG_VF_TAPE_LOCK_UNLOCK ));
         MSGD (( ERR_TAPE_LOCK_UNLOCK, rc ));
         sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_LOCK_UNLOCK, rc );
         }
       DBGOUT;
       return (lError);
       }
     }

   DBGOUT;
   return (lError);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
   return ( VF_NOTOK );
  #endif
  }

/*------------------------------*/

static LONG sql06c_tape_loading ( PHOSTFILE_CTRL_REC  pHF,
                                  PCHAR               pcErrText,
                                  ULONG               ulOperation )
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_tape_loading"

  #if defined(_WIN32)
   APIRET       rc          = NO_ERROR;
   INT          Retry       = TAPE_ERROR_RETRY;
   LONG         lError      = VF_OK;

   DBGIN;

  if ( pHF->ulTapeFeatures & TAPE_LOAD_UNLOAD )
    {
    do
      {
      rc = PrepareTape ( (HANDLE)pHF->hfFileHandle, ulOperation , FALSE);
      if ( ulOperation == TAPE_LOAD && rc == ERROR_NO_MEDIA_IN_DRIVE )
        {
        INT WaitSec  = MAX_TAPE_LOAD_RETRY_TIME ;
        do
          {
          SLEEP(1000) ;
          rc = PrepareTape ( (HANDLE)pHF->hfFileHandle, ulOperation , FALSE);
          }
        while ( --WaitSec && rc == ERROR_NO_MEDIA_IN_DRIVE ) ;
        }
      }
    while (( rc == ERROR_MEDIA_CHANGED        ||
             rc == ERROR_END_OF_MEDIA         ||
             rc == ERROR_BEGINNING_OF_MEDIA   ||
             rc == ERROR_IO_DEVICE            ||
             rc == ERROR_BUS_RESET            ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       pHF->ulState      = HFS_ERROR;
       lError            = VF_NOTOK;
       pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

       if ( sql06c_handle_known_errors ( pHF->szFileName,
                                         rc, pcErrText ) == FALSE)
         {
         DBG1 (( MF__, ERRMSG_VF_TAPE_LOAD_UNLOAD ));
         MSGD (( ERR_TAPE_LOAD_UNLOAD, rc ));
         sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_LOAD_UNLOAD, rc );
         }
       DBGOUT;
       return ( FALSE );
       }
     }
   else
   if ( ulOperation == TAPE_UNLOAD )
     {                                // autoloader in use, don't ignore error
       sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
       DBGOUT;
       return ( VF_NOTOK );
     }
   DBGOUT;
   return (VF_OK);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );
   return ( VF_NOTOK );
  #endif
  }

/*------------------------------*/

static LONG sql06c_tape_write_mark( PHOSTFILE_CTRL_REC  pHF,
                                    PCHAR               pcErrText)
  {
  #undef  MF__
  #define MF__ MOD__"sql06c_tape_write_mark"

  #if defined(_WIN32)
   APIRET       rc             = NO_ERROR;
   LONG         lError         = VF_OK;
   LONG         lCount         = 1;
   DWORD        dwOperation    = 0;
   bool         fRetImmediate  = TRUE;
   INT          Retry          = TAPE_ERROR_RETRY;

   DBGIN;

   if ( pHF->ulTapeFeatures & TAPE_WRITE_LONG_FMKS )
     dwOperation = TAPE_LONG_FILEMARKS;
   else if ( pHF->ulTapeFeatures & TAPE_WRITE_SHORT_FMKS )
     dwOperation = TAPE_SHORT_FILEMARKS;
   else if ( pHF->ulTapeFeatures & TAPE_WRITE_FILEMARKS )
     dwOperation = TAPE_FILEMARKS;
   else
     {
     DBGOUT;
     return ( VF_OK );
     }

   if ( pHF->ulTapeFeatures & TAPE_WRITE_MARK_IMMED )
     fRetImmediate  = FALSE;

   do
     {
     rc = WriteTapemark ((HANDLE)pHF->hfFileHandle, dwOperation, lCount, FALSE);
     }
   while (( rc == ERROR_BUS_RESET ||
            rc == ERROR_IO_DEVICE ) && --Retry);

   if (( rc != NO_ERROR ) && ( rc != ERROR_INVALID_FUNCTION ))
     {
     pHF->ulState      = HFS_ERROR;
     lError            = VF_NOTOK;
     pHF->hfFileHandle = (HANDLE)INVALID_HANDLE_VALUE;

     if ( sql06c_handle_known_errors ( pHF->szFileName,
                                       rc, pcErrText ) == FALSE)
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_WRITE_MARK ));
       MSGD (( ERR_TAPE_WRITE_MARK, rc ));
       sql46c_build_error_string ( pcErrText, ERRMSG_VF_TAPE_WRITE_MARK, rc );
       }
     DBGOUT;
     return (lError);
     }

   DBGOUT;
   return (lError);
  #else
   sql46c_build_error_string ( pcErrText, "operation not supported",  0 );

   DBGOUT;
   return ( VF_NOTOK );
  #endif
  }

#endif

//
// =============================== END ========================================
//

