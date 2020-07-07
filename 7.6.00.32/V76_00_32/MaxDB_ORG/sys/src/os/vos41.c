/*!
  @file           vos41.c
  @author         RaymondR
  @brief          Create and remove IPC resources
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
#include "gos41.h"
#include "heo00.h"
#include "heo46.h"

#if defined (KERNEL)
# include "gos00k.h"            /* nocheck */  
#endif

#include "geo007_1.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS41C : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */
static VOID sql41c_build_global_name_space_name ( PSZ         Path,           // - IN
                                                  PSZ         Name,           // - IN
                                                  PSZ         GlobalName );   // - OUT


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

LONG sql41c_create_event_sem ( PHANDLE                 phevSysSem,
                               PSZ                  pszSemPath,
                               PSZ                  pszName,
                               ULONG                flAttr,          // - OS/2
                               BOOL                 fState,          // - OS/2
                               PSECURITY_ATTRIBUTES pSA )            // - WIN32
{
# undef  MF__
# define MF__ MOD__"sql41c_create_event_sem"

  APIRET      rc;
  PATHNAME    szSemName;
  PSZ         pszSemName;
  HEV         hev;
  
  DBGPAS;
  
  /*
   * Create Event Semaphore
   */
  if (( pszSemPath != NULL ) && ( pszName != NULL ))
  {
    sql41c_build_global_name_space_name ( pszSemPath, pszName, szSemName );
    pszSemName = szSemName;
  }
  else
  {
    strcpy(szSemName, "UNNAMED" );
    pszSemName = NULL;
  }
  
  DBG3 ((MF__, "DosCreateEventSem: %s", szSemName));
  
  hev = CreateEvent (pSA, FALSE, FALSE, pszSemName);
  
  rc  = GetLastError ();
  
  switch ( rc )
  {
  case NO_ERROR:
    *phevSysSem = hev;
    return ( NO_ERROR );
    
  case ERROR_ALREADY_EXISTS:
    *phevSysSem = (HEV)INVALID_HANDLE_VALUE;
    CloseHandle (hev);
    return (ERROR_DUPLICATE_NAME);
    
  default:
    *phevSysSem = (HEV)INVALID_HANDLE_VALUE;
    break;
  }
  
  MSGD (( ERR_CREATING_SEM, szSemName, rc));
  DBG1 ((MF__, "Error on creating semaphore: %s, rc = %u\n", szSemName, rc));
  
  return (rc);
  }

/*------------------------------*/

LONG sql41c_open_event_sem ( PHANDLE     phevSysSem,
                             PSZ      pszSemPath,
                             PSZ      pszName,
                             ULONG    ulNoMsgRc )
{
# undef  MF__
# define MF__ MOD__"sql41c_open_event_sem"
  APIRET      rc = NO_ERROR;
  PATHNAME    szSemName;
  PSZ         pszSemName;
  
  DBGPAS;
  
  /*
   * Open Event Semaphore
   */
  if (( pszSemPath != NULL ) && ( pszName != NULL ))
  {
    sql41c_build_global_name_space_name ( pszSemPath, pszName, szSemName );
    pszSemName = szSemName;
  }
  else
  {
    strcpy(szSemName, "UNNAMED" );
    pszSemName = NULL;
  }
  
  
  *phevSysSem = OpenEvent(EVENT_ALL_ACCESS, FALSE, pszSemName);
  
  if ( *phevSysSem )
    return (NO_ERROR);
  else
  {
    rc = GetLastError();
    
    if (( sql02_get_platform_id() != VER_PLATFORM_WIN32_NT ) &&
      ( rc                      == ERROR_INVALID_NAME ))
      return (ERROR_SEM_NOT_FOUND);          // --- no error message
    if ( rc == ERROR_FILE_NOT_FOUND )
      return (ERROR_SEM_NOT_FOUND);          // --- no error message
    if ( ulNoMsgRc == rc )
      return (rc);                           // --- no error message
  }
  
  MSGD (( ERR_OPENING_SEM, szSemName, rc));
  DBG1 ((MF__, "Error on opening semaphore '%s', rc = %u\n", szSemName, rc));
  
  return (rc);
  }

/*------------------------------*/

LONG sql41c_post_event_sem ( HANDLE hevSysSem, char const * pszSemDesc )
{
# undef  MF__
# define MF__ MOD__"sql41c_post_event_sem"
  APIRET      rc;
  
  DBGPAS;
  
  if ( SetEvent( hevSysSem ))
    return (NO_ERROR);
  
  rc = GetLastError();
  
  if ((rc != NO_ERROR) && (rc != ERROR_ALREADY_POSTED))
  {
    MSGD (( ERR_POSTING_SEM, pszSemDesc, rc));
    DBG1 ((MF__, "Error on posting semaphore: %s, rc = %u\n", pszSemDesc, rc));
  }
  
  return (rc);
}

/*------------------------------*/

LONG sql41c_close_event_sem ( HANDLE hevSysSem, char const * pszSemDesc )
{
# undef  MF__
# define MF__ MOD__"sql41c_close_event_sem"
  APIRET      rc;
  
  DBGPAS;
  
  if ( CloseHandle ( hevSysSem ) )
    return (NO_ERROR);
  
  rc = GetLastError();
  
  if (rc != NO_ERROR)
  {
    MSGD (( ERR_CLOSING_SEM, pszSemDesc, rc));
    DBG1 ((MF__, "Error on closing semaphore: %s, rc = %u\n", pszSemDesc, rc));
  }
  
  return (rc);
}

/*------------------------------*/

LONG sql41c_wait_event_sem ( HANDLE         hevSysSem,
                             ULONG          ulTimeout,
                             char const    *pszSemDesc )
{
# undef  MF__
# define MF__ MOD__"sql41c_wait_event_sem"
  APIRET      rc;
  
  DBGPAS;
  
  switch (WaitForSingleObject(hevSysSem, ulTimeout))
  {
  case WAIT_OBJECT_0: return (NO_ERROR);
  case WAIT_TIMEOUT:  return (ERROR_TIMEOUT);
  case 0xffffffff:    rc = GetLastError();       break;
  default:            rc = ERROR_INVALID_HANDLE; break;
  }
  
  if ( rc != NO_ERROR )
  {
    MSGD (( ERR_WAITING_SEM, pszSemDesc, rc ));
    DBG1 ((MF__, "Error on waiting for semaphore '%s', rc = %u", pszSemDesc, rc));
    return (rc);
  }
  
  return ( rc );
}

/*------------------------------*/


LONG sql41c_create_shrd_mem ( PVOID               *ppMemBase,
                              ULONG                ulMemSize,
                              PSZ                  pszShrdMemPath,
                              PSZ                  pszName,
                              PSECURITY_ATTRIBUTES pSA,              // - WIN32
                              HANDLE               *phFileMapping)   // - WIN32
{
# undef  MF__
# define MF__ MOD__"sql41c_create_shrd_mem"
  APIRET      rc = NO_ERROR;
  PATHNAME    szShrdMem;
  PSZ         pszShrdMem;
  
  DBGPAS;
  
  if (pszShrdMemPath)
  {
    sql41c_build_global_name_space_name ( pszShrdMemPath, pszName, szShrdMem );
    pszShrdMem = szShrdMem;
  }
  else
  {
    strcpy (szShrdMem, "UNNAMED");
    pszShrdMem = NULL;
  }
  DBG3 ((MF__, "DosAllocSharedMem: %s", szShrdMem));
  
  if (!(*phFileMapping = CreateFileMapping ( (HANDLE)-1L, pSA, PAGE_READWRITE | SEC_COMMIT, 0,
                                             ulMemSize, pszShrdMem ) ))
  {
    rc             = GetLastError();
    *ppMemBase     = NULL;
    *phFileMapping = (HEV)INVALID_HANDLE_VALUE;

    DBG1((MF__, "CreateFileMapping failed, rc=%d", rc));
    return (rc);
  }
  if (!(*ppMemBase = MapViewOfFile (*phFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0)))
  {
    rc             = GetLastError();
    *ppMemBase     = NULL;
    CloseHandle (*phFileMapping);
    *phFileMapping = (HEV)INVALID_HANDLE_VALUE;

    DBG1((MF__, "MapViewOfFile failed, rc=%d", rc));
    return (rc);
  }
  
  if ( rc != NO_ERROR )
  {
    MSGD (( ERR_CREATE_SHRD_MEM, szShrdMem, rc ));
    DBG1 ((MF__, "Cannot create shared memory segment: %s, rc = %u\n", szShrdMem, rc));
    *ppMemBase     = NULL;
    *phFileMapping = (HEV)INVALID_HANDLE_VALUE;

    return (rc);
  }
  
  return (NO_ERROR);
  }

/*------------------------------*/

LONG sql41c_get_named_shrd_mem ( PVOID   *ppMemBase,
                                 PSZ      pszShrdMemPath,
                                 PSZ      pszName,
                                 HANDLE   *phFileMapping )           // - WIN32
{
# undef  MF__
# define MF__ MOD__"sql41c_get_named_shrd_mem"
  APIRET      rc = NO_ERROR;
  PATHNAME    szShrdMem;
  
  DBGPAS;
  
  sql41c_build_global_name_space_name ( pszShrdMemPath, pszName, szShrdMem );
  
  if (!(*phFileMapping = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, szShrdMem )))
  {
    rc = GetLastError();
  }
  else if (!(*ppMemBase = MapViewOfFile( *phFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0 )))
  {
    rc = GetLastError();
    CloseHandle ( *phFileMapping );
  }
  
  if (rc != NO_ERROR)
  {
    *ppMemBase     = NULL;
    *phFileMapping = (HEV)INVALID_HANDLE_VALUE;
    
    if (( rc != ERROR_FILE_NOT_FOUND ) && ( rc != ERROR_INVALID_NAME ))
    {
      MSGD (( ERR_GET_SHRD_MEM, szShrdMem, rc ));
      DBG1 ((MF__, "Cannot get named-shared-memory-segment: %s, rc = %u", szShrdMem, rc));
    }
    return (rc);
  }
  
  return (NO_ERROR);
  }

/*------------------------------*/

LONG sql41c_give_shrd_mem ( PVOID    pMemBase,                       // - OS/2
                            DWORD    pidDest,
                            HANDLE   hFileMapping,                   // - WIN32
                            HANDLE   *phDestFileMapping )            // - WIN32
{
# undef  MF__
# define MF__ MOD__"sql41c_give_shrd_mem"
  APIRET      rc = NO_ERROR;
  HANDLE      hProcess;
  
  DBGPAS;
  
  hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pidDest);
  
  if (!hProcess)
  {
    rc = GetLastError();
    return (rc);
  }
  
  if ( !DuplicateHandle( GetCurrentProcess(), hFileMapping,
                         hProcess, phDestFileMapping, 0, FALSE, DUPLICATE_SAME_ACCESS))
  {
    rc = GetLastError();
  }
  
  CloseHandle(hProcess);
  
  if ( rc != NO_ERROR )
    MSGD (( ERR_CANT_GIVE_SHRD_MEM, rc ));
  
  return (rc);
}

/*------------------------------*/

LONG sql41c_free_shrd_mem ( PVOID    pMemBase,
                            HANDLE   hFileMapping )                  // - WIN32
{
# undef  MF__
# define MF__ MOD__"sql41c_free_shrd_mem"
  APIRET      rc = NO_ERROR;
  
  DBGPAS;
  
  if ( !UnmapViewOfFile(pMemBase) || !CloseHandle(hFileMapping) )
    rc = GetLastError();
  
  if ( rc != NO_ERROR)
    MSGD (( ERR_FREE_SHRD_MEMORY, rc ));
  
  return (rc);
}


/*------------------------------*/

LONG sql41c_create_queue_mailslot ( PHANDLE              phHandle,
                                    PSZ                  pszPath,
                                    PSZ                  pszName,
                                    ULONG                ulReqLen,   // - WIN32
                                    PSECURITY_ATTRIBUTES pSA)        // - WIN32
{
# undef  MF__
# define MF__ MOD__"sql41c_create_queue_mailslot"
  APIRET      rc = NO_ERROR;
  PATHNAME    szName;
  
  DBGPAS;
  
  /*
  * create queue/mailslot
  */
  strcpy (szName, pszPath);
  strcat (szName, pszName);
  
  DBG3 ((MF__, "sql41c_create_queue_mailslot: %s", szName));
  
  *phHandle = CreateMailslot ( szName, ulReqLen, MAILSLOT_WAIT_FOREVER, pSA );
  
  if ( *phHandle == INVALID_HANDLE_VALUE )
    rc = GetLastError ();
  
  if (rc != NO_ERROR)
  {
    MSGD (( ERR_CREATING_QUEUE_MAILSLOT, szName, rc));
    DBG1 ((MF__, "Error on creating queue/mailslot '%s', rc = %u", szName, rc));
  }
  
  return (rc);
}

/*------------------------------*/

LONG sql41c_open_queue_mailslot ( DWORD   *ppidOwner,                // - OS/2
                                  PHANDLE  phHandle,
                                  PSZ      pszPath,
                                  PSZ      pszName )
{
# undef  MF__
# define MF__ MOD__"sql41c_open_queue_mailslot"
  APIRET      rc = NO_ERROR;
  PATHNAME    szName;
  
  DBGPAS;
  
  /*
  * open queue/mailslot
  */
  strcpy (szName, pszPath);
  strcat (szName, pszName);
  
  DBG3 ((MF__, "sql41c_open_queue_mailslot: %s", szName));
  
  *ppidOwner = 0;
  *phHandle  = CreateFile ( szName, GENERIC_WRITE, FILE_SHARE_READ |
                            FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
  
  if ( *phHandle == INVALID_HANDLE_VALUE )
    rc = GetLastError();
  
  if (rc != NO_ERROR)
  {
    if ( rc != ERROR_FILE_NOT_FOUND )         // --- no error message
    {
      MSGD (( ERR_OPENING_QUEUE_MAILSLOT, szName, rc));
      DBG1 ((MF__, "Error on opening queue/mailslot '%s', rc = %u", szName,rc));
    }
  }
  
  return (rc);
}

/*------------------------------*/

LONG sql41c_write_queue_mailslot ( HANDLE  hHandle,
                                   PVOID   pRequest,
                                   ULONG   ulReqLen,
                                   ULONG   ulDataLen,                // - OS/2
                                   PVOID   pData )                   // - OS/2
  {
  #undef  MF__
  #define MF__ MOD__"sql41c_write_queue_mailslot"
  APIRET              rc = NO_ERROR;
  ULONG               ulBytesWritten;

  DBGPAS;

  if ( !WriteFile ( hHandle, pRequest, ulReqLen, &ulBytesWritten, NULL))
    rc = GetLastError ();

  if (rc != NO_ERROR)
    {
    if ( rc != ERROR_HANDLE_EOF )             // --- no error message ?
      {
      MSGD (( ERR_WRITING_QUEUE_MAILSLOT, rc));
      DBG1 ((MF__, "Error while writeng queue/mailslot, rc = %u", rc));
      }
    }

  return (rc);
  }

/*------------------------------*/

LONG sql41c_read_queue_mailslot ( HANDLE  hHandle,
                                  PVOID   pRequest,
                                  ULONG   ulReqLen,
                                  PULONG  pulDataLen,               // - OS/2
                                  PVOID  *ppData )                  // - OS/2
{
# undef  MF__
# define MF__ MOD__"sql41c_read_queue_mailslot"
  APIRET              rc = NO_ERROR;
  ULONG               ulBytesRead;
  ULONG               ulMessage;
  
  DBGPAS;
  
  *pulDataLen = 0;
  *ppData     = NULL;
  
  if ( sql02_get_platform_id () != VER_PLATFORM_WIN32_NT )
  {
    // --- POLLING - Windows 95 workaround !!!!!!!!
    do
    {
      if (!GetMailslotInfo( hHandle, NULL, &ulMessage, NULL, NULL ))
      {
        rc = GetLastError ();
        break;
      }
      else if ( ulMessage == MAILSLOT_NO_MESSAGE )
      {
        SLEEP ( 250 );
      }
    }
    while ( ulMessage == MAILSLOT_NO_MESSAGE );
  }
  
  if ( rc == NO_ERROR )
  {
    if ( !ReadFile ( hHandle, pRequest, ulReqLen, &ulBytesRead, NULL))
      rc = GetLastError ();
    else if (ulBytesRead != ulReqLen)
      rc = ERROR_INSUFFICIENT_BUFFER;
  }
  
  if (rc != NO_ERROR)
  {
    if ( rc == ERROR_INSUFFICIENT_BUFFER )
    {
      MSGD (( ERR_WRONG_REQ_BUF_SIZE, ulReqLen ));
      DBG1 ((MF__, "Wrong request buffer size: %u", ulReqLen ));
    }
    else
    {
      MSGD (( ERR_READING_QUEUE_MAILSLOT, rc));
      DBG1 ((MF__, "Error while reading from queue/mailslot, rc = %u", rc));
    }
  }
  
  return (rc);
  }

/*------------------------------*/

LONG sql41c_close_queue_mailslot ( HANDLE  hHandle )
{
# undef  MF__
# define MF__ MOD__"sql41c_close_queue_mailslot"
  APIRET      rc = NO_ERROR;
  
  DBGPAS;
  
  CloseHandle ( hHandle );
  
  return ( rc );
}

/*------------------------------*/

VOID sql41c_build_unique_obj_name (char *pszClientObjName )
{
# undef  MF__
# define MF__ MOD__"sql41c_get_unique_obj_name"
  ULONG                 ulStrLen;
  ULONG                 ulCurrTime;
  static ULONG          ulIdx = 0;
  static PID            Pid   = (PID)UNDEF;
  TID                   Tid;
  
  DBGPAS;
  
  if ( Pid == (PID)UNDEF )
    GETPROCESSID (&Pid);
  
  Tid        = THREADID;
  ulCurrTime = GetCurrentTime ();
  
  _ultoa ( (ULONG)Pid, pszClientObjName, 16 );
  
  ulStrLen = (ULONG)strlen ( pszClientObjName );
  pszClientObjName [ ulStrLen ] = '-';
  _ultoa ( (ULONG)Tid, pszClientObjName + ulStrLen + 1, 16 );
  
  ulStrLen = (ULONG)strlen ( pszClientObjName );
  pszClientObjName [ ulStrLen ] = '-';
  _ultoa ( ++ulIdx, pszClientObjName + ulStrLen + 1, 16 );
  
  ulStrLen = (ULONG)strlen ( pszClientObjName );
  pszClientObjName [ ulStrLen ] = '-';
  _ultoa ( ulCurrTime, pszClientObjName + ulStrLen + 1, 16 );
  
  strupr (pszClientObjName);
  
  DBG3 (( MF__, "pszClientObjName : '%s'", pszClientObjName ));
  
  return;
}

/*------------------------------*/

void * sqlAllocSharedMem(const char *Path, const unsigned int SizeInBytes)
{
#undef MF__
#define MF__ MOD__"sqlAllocSharedMem"
    APIRET rc = NO_ERROR;
    void * ShmAddress = NULL;
    HANDLE FileHandle;
    HANDLE MappingHandle;

    if ( !Path ) 
    {
        Path = "";
    }

    FileHandle = CreateFile ( Path, 
                              GENERIC_READ|GENERIC_WRITE, 
                              FILE_SHARE_READ|FILE_SHARE_WRITE,
                              NULL,
                              OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL );
    if ( FileHandle != INVALID_HANDLE_VALUE )
    {
        /* If FileMapping already exists, the MappingHandle is valid too, but SizeInBytes is ignored */
        if (!(MappingHandle = CreateFileMapping ( FileHandle, NULL, PAGE_READWRITE | SEC_COMMIT, 0,
                                                 SizeInBytes, NULL ) ))
        {
            rc             = GetLastError();
            MSGD(( ERR_CREATEFILEMAPPING_SYSCALL, Path, (long)SizeInBytes, rc ));
        }
    
        if ( MappingHandle != NULL ) 
        {
            if ( !(ShmAddress = MapViewOfFile (MappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, SizeInBytes)))
            {
                rc = GetLastError();
                MSGD(( ERR_MAPVIEWOFFILE_SYSCALL, Path, (long)SizeInBytes, rc ));
            }
            // Closing the Handle makes us HANDLE independend...
            // An internal handle will still be open, while Mapping exists
            CloseHandle (MappingHandle);
        }
        CloseHandle( FileHandle );
    }
    else
    {
        rc = GetLastError();
        MSGD(( ERR_CREATEFILE_SYSCALL, Path, rc ));
    }
    return (ShmAddress);
}

/*------------------------------*/

void sqlFreeSharedMem(void *ShmAddress, const unsigned int SizeInBytes)
{
    if ( !UnmapViewOfFile (ShmAddress) )
    {
        APIRET rc = NO_ERROR;
        rc             = GetLastError();
        MSGD(( ERR_UNMAPVIEWOFFILE_SYSCALL, ShmAddress, rc ));
    }
}

/*
 * ========================== LOCAL FUNCTIONS =================================
 */
static VOID sql41c_build_global_name_space_name ( PSZ         Path,           // - IN
                                                  PSZ         Name,           // - IN
                                                  PSZ         GlobalName )    // - OUT
{
# undef  MF__
# define MF__ MOD__"sql41c_build_global_name_space_name"

  DWORD       MajorVersion;
  DWORD       ServicePack;

  sql02_get_os_version ( &MajorVersion, NULL );

  ServicePack = sql02_get_service_pack();

  if ( sql02_validate_product_suite( PSUITE_TERMINAL_SERVER ) == true )
  {
    if ( MajorVersion == 4 && ServicePack < 4 )
    {
    strcpy (GlobalName, Path);
    strcat (GlobalName, Name);
    strcat (GlobalName, ":0");  // -- The "old" Terminal Server expands the name with an 
                                //    session specific extension (:<number>).
                                //    Only by expanding the name with an static number
                                //    (:0) gets an global name.
    }
    else
    {
    strcpy (GlobalName, "Global\\");
    strcat (GlobalName, Path);
    strcat (GlobalName, Name);
    }
  }
  else
  {
    strcpy (GlobalName, Path);
    strcat (GlobalName, Name);
  }
}
/*
 * =============================== END ========================================
 */
