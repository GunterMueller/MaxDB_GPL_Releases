/*!
  @file           vos01c.c
  @author         RaymondR
  @brief          SystemRC Main Module
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
 *  DEFINES
 */
#define MOD__  "VOS01CC : "
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
VOID sql01c_get_rc_identifier ( LONG rc,  PSZ *ppszErrIdent );


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

INT APIENTRY WinMain ( HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPSTR     lpszCmdLine,
                       INT       nCmdShow )
  {
  #undef  MF__
  #define MF__ MOD__"WinMain"
  LPVOID lpMsgBuf;
  CHAR   szMessage[4096];
  CHAR   szHeader[40];
  PSZ    pszErrIdent;

  LONG   rc;

  if ( lpszCmdLine[0] == '\0' )
    {
    MessageBox( NULL, "SystemRC <return_code>",
                "Usage", MB_OK|MB_ICONINFORMATION );
    exit (1);
    }

  rc = atoi (lpszCmdLine);

  FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 rc,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                 (LPTSTR) &lpMsgBuf,
                 0,
                 NULL );

  sql01c_get_rc_identifier ( rc,  &pszErrIdent );

  sprintf ( szHeader, "SAPDB - System ReturnCode" );
  sprintf ( szMessage, "\n%s  ( rc = %d)\n\n%s\n", pszErrIdent, rc, lpMsgBuf );
  // Display the string.
  MessageBox( NULL, szMessage, szHeader, MB_OK|MB_ICONINFORMATION );

  // Free the buffer.
  LocalFree( lpMsgBuf );
  return ( NO_ERROR);
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

VOID sql01c_get_rc_identifier ( LONG rc,  PSZ *ppszErrIdent )
  {
  #undef  MF__
  #define MF__ MOD__"WinMain"

  switch ( rc )
    {
    case NO_ERROR:
      *ppszErrIdent = "NO_ERROR";
      break;
    case ERROR_INVALID_FUNCTION:
      *ppszErrIdent = "ERROR_INVALID_FUNCTION";
      break;
    case ERROR_FILE_NOT_FOUND:
      *ppszErrIdent = "ERROR_FILE_NOT_FOUND";
      break;
    case ERROR_PATH_NOT_FOUND:
      *ppszErrIdent = "ERROR_PATH_NOT_FOUND";
      break;
    case ERROR_TOO_MANY_OPEN_FILES:
      *ppszErrIdent = "ERROR_TOO_MANY_OPEN_FILES";
      break;
    case ERROR_ACCESS_DENIED:
      *ppszErrIdent = "ERROR_ACCESS_DENIED";
      break;
    case ERROR_INVALID_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_HANDLE";
      break;
    case ERROR_ARENA_TRASHED:
      *ppszErrIdent = "ERROR_ARENA_TRASHED";
      break;
    case ERROR_NOT_ENOUGH_MEMORY:
      *ppszErrIdent = "ERROR_NOT_ENOUGH_MEMORY";
      break;
    case ERROR_INVALID_BLOCK:
      *ppszErrIdent = "ERROR_INVALID_BLOCK";
      break;
    case ERROR_BAD_ENVIRONMENT:
      *ppszErrIdent = "ERROR_BAD_ENVIRONMENT";
      break;
    case ERROR_BAD_FORMAT:
      *ppszErrIdent = "ERROR_BAD_FORMAT";
      break;
    case ERROR_INVALID_ACCESS:
      *ppszErrIdent = "ERROR_INVALID_ACCESS";
      break;
    case ERROR_INVALID_DATA:
      *ppszErrIdent = "ERROR_INVALID_DATA";
      break;
    case ERROR_OUTOFMEMORY:
      *ppszErrIdent = "ERROR_OUTOFMEMORY";
      break;
    case ERROR_INVALID_DRIVE:
      *ppszErrIdent = "ERROR_INVALID_DRIVE";
      break;
    case ERROR_CURRENT_DIRECTORY:
      *ppszErrIdent = "ERROR_CURRENT_DIRECTORY";
      break;
    case ERROR_NOT_SAME_DEVICE:
      *ppszErrIdent = "ERROR_NOT_SAME_DEVICE";
      break;
    case ERROR_NO_MORE_FILES:
      *ppszErrIdent = "ERROR_NO_MORE_FILES";
      break;
    case ERROR_WRITE_PROTECT:
      *ppszErrIdent = "ERROR_WRITE_PROTECT";
      break;
    case ERROR_BAD_UNIT:
      *ppszErrIdent = "ERROR_BAD_UNIT";
      break;
    case ERROR_NOT_READY:
      *ppszErrIdent = "ERROR_NOT_READY";
      break;
    case ERROR_BAD_COMMAND:
      *ppszErrIdent = "ERROR_BAD_COMMAND";
      break;
    case ERROR_CRC:
      *ppszErrIdent = "ERROR_CRC";
      break;
    case ERROR_BAD_LENGTH:
      *ppszErrIdent = "ERROR_BAD_LENGTH";
      break;
    case ERROR_SEEK:
      *ppszErrIdent = "ERROR_SEEK";
      break;
    case ERROR_NOT_DOS_DISK:
      *ppszErrIdent = "ERROR_NOT_DOS_DISK";
      break;
    case ERROR_SECTOR_NOT_FOUND:
      *ppszErrIdent = "ERROR_SECTOR_NOT_FOUND";
      break;
    case ERROR_OUT_OF_PAPER:
      *ppszErrIdent = "ERROR_OUT_OF_PAPER";
      break;
    case ERROR_WRITE_FAULT:
      *ppszErrIdent = "ERROR_WRITE_FAULT";
      break;
    case ERROR_READ_FAULT:
      *ppszErrIdent = "ERROR_READ_FAULT";
      break;
    case ERROR_GEN_FAILURE:
      *ppszErrIdent = "ERROR_GEN_FAILURE";
      break;
    case ERROR_SHARING_VIOLATION:
      *ppszErrIdent = "ERROR_SHARING_VIOLATION";
      break;
    case ERROR_LOCK_VIOLATION:
      *ppszErrIdent = "ERROR_LOCK_VIOLATION";
      break;
    case ERROR_WRONG_DISK:
      *ppszErrIdent = "ERROR_WRONG_DISK";
      break;
    case ERROR_SHARING_BUFFER_EXCEEDED:
      *ppszErrIdent = "ERROR_SHARING_BUFFER_EXCEEDED";
      break;
    case ERROR_HANDLE_EOF:
      *ppszErrIdent = "ERROR_HANDLE_EOF";
      break;
    case ERROR_HANDLE_DISK_FULL:
      *ppszErrIdent = "ERROR_HANDLE_DISK_FULL";
      break;
    case ERROR_NOT_SUPPORTED:
      *ppszErrIdent = "ERROR_NOT_SUPPORTED";
      break;
    case ERROR_REM_NOT_LIST:
      *ppszErrIdent = "ERROR_REM_NOT_LIST";
      break;
    case ERROR_DUP_NAME:
      *ppszErrIdent = "ERROR_DUP_NAME";
      break;
    case ERROR_BAD_NETPATH:
      *ppszErrIdent = "ERROR_BAD_NETPATH";
      break;
    case ERROR_NETWORK_BUSY:
      *ppszErrIdent = "ERROR_NETWORK_BUSY";
      break;
    case ERROR_DEV_NOT_EXIST:
      *ppszErrIdent = "ERROR_DEV_NOT_EXIST";
      break;
    case ERROR_TOO_MANY_CMDS:
      *ppszErrIdent = "ERROR_TOO_MANY_CMDS";
      break;
    case ERROR_ADAP_HDW_ERR:
      *ppszErrIdent = "ERROR_ADAP_HDW_ERR";
      break;
    case ERROR_BAD_NET_RESP:
      *ppszErrIdent = "ERROR_BAD_NET_RESP";
      break;
    case ERROR_UNEXP_NET_ERR:
      *ppszErrIdent = "ERROR_UNEXP_NET_ERR";
      break;
    case ERROR_BAD_REM_ADAP:
      *ppszErrIdent = "ERROR_BAD_REM_ADAP";
      break;
    case ERROR_PRINTQ_FULL:
      *ppszErrIdent = "ERROR_PRINTQ_FULL";
      break;
    case ERROR_NO_SPOOL_SPACE:
      *ppszErrIdent = "ERROR_NO_SPOOL_SPACE";
      break;
    case ERROR_PRINT_CANCELLED:
      *ppszErrIdent = "ERROR_PRINT_CANCELLED";
      break;
    case ERROR_NETNAME_DELETED:
      *ppszErrIdent = "ERROR_NETNAME_DELETED";
      break;
    case ERROR_NETWORK_ACCESS_DENIED:
      *ppszErrIdent = "ERROR_NETWORK_ACCESS_DENIED";
      break;
    case ERROR_BAD_DEV_TYPE:
      *ppszErrIdent = "ERROR_BAD_DEV_TYPE";
      break;
    case ERROR_BAD_NET_NAME:
      *ppszErrIdent = "ERROR_BAD_NET_NAME";
      break;
    case ERROR_TOO_MANY_NAMES:
      *ppszErrIdent = "ERROR_TOO_MANY_NAMES";
      break;
    case ERROR_TOO_MANY_SESS:
      *ppszErrIdent = "ERROR_TOO_MANY_SESS";
      break;
    case ERROR_SHARING_PAUSED:
      *ppszErrIdent = "ERROR_SHARING_PAUSED";
      break;
    case ERROR_REQ_NOT_ACCEP:
      *ppszErrIdent = "ERROR_REQ_NOT_ACCEP";
      break;
    case ERROR_REDIR_PAUSED:
      *ppszErrIdent = "ERROR_REDIR_PAUSED";
      break;
    case ERROR_FILE_EXISTS:
      *ppszErrIdent = "ERROR_FILE_EXISTS";
      break;
    case ERROR_CANNOT_MAKE:
      *ppszErrIdent = "ERROR_CANNOT_MAKE";
      break;
    case ERROR_FAIL_I24:
      *ppszErrIdent = "ERROR_FAIL_I24";
      break;
    case ERROR_OUT_OF_STRUCTURES:
      *ppszErrIdent = "ERROR_OUT_OF_STRUCTURES";
      break;
    case ERROR_ALREADY_ASSIGNED:
      *ppszErrIdent = "ERROR_ALREADY_ASSIGNED";
      break;
    case ERROR_INVALID_PASSWORD:
      *ppszErrIdent = "ERROR_INVALID_PASSWORD";
      break;
    case ERROR_INVALID_PARAMETER:
      *ppszErrIdent = "ERROR_INVALID_PARAMETER";
      break;
    case ERROR_NET_WRITE_FAULT:
      *ppszErrIdent = "ERROR_NET_WRITE_FAULT";
      break;
    case ERROR_NO_PROC_SLOTS:
      *ppszErrIdent = "ERROR_NO_PROC_SLOTS";
      break;
    case ERROR_TOO_MANY_SEMAPHORES:
      *ppszErrIdent = "ERROR_TOO_MANY_SEMAPHORES";
      break;
    case ERROR_EXCL_SEM_ALREADY_OWNED:
      *ppszErrIdent = "ERROR_EXCL_SEM_ALREADY_OWNED";
      break;
    case ERROR_SEM_IS_SET:
      *ppszErrIdent = "ERROR_SEM_IS_SET";
      break;
    case ERROR_TOO_MANY_SEM_REQUESTS:
      *ppszErrIdent = "ERROR_TOO_MANY_SEM_REQUESTS";
      break;
    case ERROR_INVALID_AT_INTERRUPT_TIME:
      *ppszErrIdent = "ERROR_INVALID_AT_INTERRUPT_TIME";
      break;
    case ERROR_SEM_OWNER_DIED:
      *ppszErrIdent = "ERROR_SEM_OWNER_DIED";
      break;
    case ERROR_SEM_USER_LIMIT:
      *ppszErrIdent = "ERROR_SEM_USER_LIMIT";
      break;
    case ERROR_DISK_CHANGE:
      *ppszErrIdent = "ERROR_DISK_CHANGE";
      break;
    case ERROR_DRIVE_LOCKED:
      *ppszErrIdent = "ERROR_DRIVE_LOCKED";
      break;
    case ERROR_BROKEN_PIPE:
      *ppszErrIdent = "ERROR_BROKEN_PIPE";
      break;
    case ERROR_OPEN_FAILED:
      *ppszErrIdent = "ERROR_OPEN_FAILED";
      break;
    case ERROR_BUFFER_OVERFLOW:
      *ppszErrIdent = "ERROR_BUFFER_OVERFLOW";
      break;
    case ERROR_DISK_FULL:
      *ppszErrIdent = "ERROR_DISK_FULL";
      break;
    case ERROR_NO_MORE_SEARCH_HANDLES:
      *ppszErrIdent = "ERROR_NO_MORE_SEARCH_HANDLES";
      break;
    case ERROR_INVALID_TARGET_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_TARGET_HANDLE";
      break;
    case ERROR_INVALID_CATEGORY:
      *ppszErrIdent = "ERROR_INVALID_CATEGORY";
      break;
    case ERROR_INVALID_VERIFY_SWITCH:
      *ppszErrIdent = "ERROR_INVALID_VERIFY_SWITCH";
      break;
    case ERROR_BAD_DRIVER_LEVEL:
      *ppszErrIdent = "ERROR_BAD_DRIVER_LEVEL";
      break;
    case ERROR_CALL_NOT_IMPLEMENTED:
      *ppszErrIdent = "ERROR_CALL_NOT_IMPLEMENTED";
      break;
    case ERROR_SEM_TIMEOUT:
      *ppszErrIdent = "ERROR_SEM_TIMEOUT";
      break;
    case ERROR_INSUFFICIENT_BUFFER:
      *ppszErrIdent = "ERROR_INSUFFICIENT_BUFFER";
      break;
    case ERROR_INVALID_NAME:
      *ppszErrIdent = "ERROR_INVALID_NAME";
      break;
    case ERROR_INVALID_LEVEL:
      *ppszErrIdent = "ERROR_INVALID_LEVEL";
      break;
    case ERROR_NO_VOLUME_LABEL:
      *ppszErrIdent = "ERROR_NO_VOLUME_LABEL";
      break;
    case ERROR_MOD_NOT_FOUND:
      *ppszErrIdent = "ERROR_MOD_NOT_FOUND";
      break;
    case ERROR_PROC_NOT_FOUND:
      *ppszErrIdent = "ERROR_PROC_NOT_FOUND";
      break;
    case ERROR_WAIT_NO_CHILDREN:
      *ppszErrIdent = "ERROR_WAIT_NO_CHILDREN";
      break;
    case ERROR_CHILD_NOT_COMPLETE:
      *ppszErrIdent = "ERROR_CHILD_NOT_COMPLETE";
      break;
    case ERROR_DIRECT_ACCESS_HANDLE:
      *ppszErrIdent = "ERROR_DIRECT_ACCESS_HANDLE";
      break;
    case ERROR_NEGATIVE_SEEK:
      *ppszErrIdent = "ERROR_NEGATIVE_SEEK";
      break;
    case ERROR_SEEK_ON_DEVICE:
      *ppszErrIdent = "ERROR_SEEK_ON_DEVICE";
      break;
    case ERROR_IS_JOIN_TARGET:
      *ppszErrIdent = "ERROR_IS_JOIN_TARGET";
      break;
    case ERROR_IS_JOINED:
      *ppszErrIdent = "ERROR_IS_JOINED";
      break;
    case ERROR_IS_SUBSTED:
      *ppszErrIdent = "ERROR_IS_SUBSTED";
      break;
    case ERROR_NOT_JOINED:
      *ppszErrIdent = "ERROR_NOT_JOINED";
      break;
    case ERROR_NOT_SUBSTED:
      *ppszErrIdent = "ERROR_NOT_SUBSTED";
      break;
    case ERROR_JOIN_TO_JOIN:
      *ppszErrIdent = "ERROR_JOIN_TO_JOIN";
      break;
    case ERROR_SUBST_TO_SUBST:
      *ppszErrIdent = "ERROR_SUBST_TO_SUBST";
      break;
    case ERROR_JOIN_TO_SUBST:
      *ppszErrIdent = "ERROR_JOIN_TO_SUBST";
      break;
    case ERROR_SUBST_TO_JOIN:
      *ppszErrIdent = "ERROR_SUBST_TO_JOIN";
      break;
    case ERROR_BUSY_DRIVE:
      *ppszErrIdent = "ERROR_BUSY_DRIVE";
      break;
    case ERROR_SAME_DRIVE:
      *ppszErrIdent = "ERROR_SAME_DRIVE";
      break;
    case ERROR_DIR_NOT_ROOT:
      *ppszErrIdent = "ERROR_DIR_NOT_ROOT";
      break;
    case ERROR_DIR_NOT_EMPTY:
      *ppszErrIdent = "ERROR_DIR_NOT_EMPTY";
      break;
    case ERROR_IS_SUBST_PATH:
      *ppszErrIdent = "ERROR_IS_SUBST_PATH";
      break;
    case ERROR_IS_JOIN_PATH:
      *ppszErrIdent = "ERROR_IS_JOIN_PATH";
      break;
    case ERROR_PATH_BUSY:
      *ppszErrIdent = "ERROR_PATH_BUSY";
      break;
    case ERROR_IS_SUBST_TARGET:
      *ppszErrIdent = "ERROR_IS_SUBST_TARGET";
      break;
    case ERROR_SYSTEM_TRACE:
      *ppszErrIdent = "ERROR_SYSTEM_TRACE";
      break;
    case ERROR_INVALID_EVENT_COUNT:
      *ppszErrIdent = "ERROR_INVALID_EVENT_COUNT";
      break;
    case ERROR_TOO_MANY_MUXWAITERS:
      *ppszErrIdent = "ERROR_TOO_MANY_MUXWAITERS";
      break;
    case ERROR_INVALID_LIST_FORMAT:
      *ppszErrIdent = "ERROR_INVALID_LIST_FORMAT";
      break;
    case ERROR_LABEL_TOO_LONG:
      *ppszErrIdent = "ERROR_LABEL_TOO_LONG";
      break;
    case ERROR_TOO_MANY_TCBS:
      *ppszErrIdent = "ERROR_TOO_MANY_TCBS";
      break;
    case ERROR_SIGNAL_REFUSED:
      *ppszErrIdent = "ERROR_SIGNAL_REFUSED";
      break;
    case ERROR_DISCARDED:
      *ppszErrIdent = "ERROR_DISCARDED";
      break;
    case ERROR_NOT_LOCKED:
      *ppszErrIdent = "ERROR_NOT_LOCKED";
      break;
    case ERROR_BAD_THREADID_ADDR:
      *ppszErrIdent = "ERROR_BAD_THREADID_ADDR";
      break;
    case ERROR_BAD_ARGUMENTS:
      *ppszErrIdent = "ERROR_BAD_ARGUMENTS";
      break;
    case ERROR_BAD_PATHNAME:
      *ppszErrIdent = "ERROR_BAD_PATHNAME";
      break;
    case ERROR_SIGNAL_PENDING:
      *ppszErrIdent = "ERROR_SIGNAL_PENDING";
      break;
    case ERROR_MAX_THRDS_REACHED:
      *ppszErrIdent = "ERROR_MAX_THRDS_REACHED";
      break;
    case ERROR_LOCK_FAILED:
      *ppszErrIdent = "ERROR_LOCK_FAILED";
      break;
    case ERROR_BUSY:
      *ppszErrIdent = "ERROR_BUSY";
      break;
    case ERROR_CANCEL_VIOLATION:
      *ppszErrIdent = "ERROR_CANCEL_VIOLATION";
      break;
    case ERROR_ATOMIC_LOCKS_NOT_SUPPORTED:
      *ppszErrIdent = "ERROR_ATOMIC_LOCKS_NOT_SUPPORTED";
      break;
    case ERROR_INVALID_SEGMENT_NUMBER:
      *ppszErrIdent = "ERROR_INVALID_SEGMENT_NUMBER";
      break;
    case ERROR_INVALID_ORDINAL:
      *ppszErrIdent = "ERROR_INVALID_ORDINAL";
      break;
    case ERROR_ALREADY_EXISTS:
      *ppszErrIdent = "ERROR_ALREADY_EXISTS";
      break;
    case ERROR_INVALID_FLAG_NUMBER:
      *ppszErrIdent = "ERROR_INVALID_FLAG_NUMBER";
      break;
    case ERROR_SEM_NOT_FOUND:
      *ppszErrIdent = "ERROR_SEM_NOT_FOUND";
      break;
    case ERROR_INVALID_STARTING_CODESEG:
      *ppszErrIdent = "ERROR_INVALID_STARTING_CODESEG";
      break;
    case ERROR_INVALID_STACKSEG:
      *ppszErrIdent = "ERROR_INVALID_STACKSEG";
      break;
    case ERROR_INVALID_MODULETYPE:
      *ppszErrIdent = "ERROR_INVALID_MODULETYPE";
      break;
    case ERROR_INVALID_EXE_SIGNATURE:
      *ppszErrIdent = "ERROR_INVALID_EXE_SIGNATURE";
      break;
    case ERROR_EXE_MARKED_INVALID:
      *ppszErrIdent = "ERROR_EXE_MARKED_INVALID";
      break;
    case ERROR_BAD_EXE_FORMAT:
      *ppszErrIdent = "ERROR_BAD_EXE_FORMAT";
      break;
    case ERROR_ITERATED_DATA_EXCEEDS_64k:
      *ppszErrIdent = "ERROR_ITERATED_DATA_EXCEEDS_64k";
      break;
    case ERROR_INVALID_MINALLOCSIZE:
      *ppszErrIdent = "ERROR_INVALID_MINALLOCSIZE";
      break;
    case ERROR_DYNLINK_FROM_INVALID_RING:
      *ppszErrIdent = "ERROR_DYNLINK_FROM_INVALID_RING";
      break;
    case ERROR_IOPL_NOT_ENABLED:
      *ppszErrIdent = "ERROR_IOPL_NOT_ENABLED";
      break;
    case ERROR_INVALID_SEGDPL:
      *ppszErrIdent = "ERROR_INVALID_SEGDPL";
      break;
    case ERROR_AUTODATASEG_EXCEEDS_64k:
      *ppszErrIdent = "ERROR_AUTODATASEG_EXCEEDS_64k";
      break;
    case ERROR_RING2SEG_MUST_BE_MOVABLE:
      *ppszErrIdent = "ERROR_RING2SEG_MUST_BE_MOVABLE";
      break;
    case ERROR_RELOC_CHAIN_XEEDS_SEGLIM:
      *ppszErrIdent = "ERROR_RELOC_CHAIN_XEEDS_SEGLIM";
      break;
    case ERROR_INFLOOP_IN_RELOC_CHAIN:
      *ppszErrIdent = "ERROR_INFLOOP_IN_RELOC_CHAIN";
      break;
    case ERROR_ENVVAR_NOT_FOUND:
      *ppszErrIdent = "ERROR_ENVVAR_NOT_FOUND";
      break;
    case ERROR_NO_SIGNAL_SENT:
      *ppszErrIdent = "ERROR_NO_SIGNAL_SENT";
      break;
    case ERROR_FILENAME_EXCED_RANGE:
      *ppszErrIdent = "ERROR_FILENAME_EXCED_RANGE";
      break;
    case ERROR_RING2_STACK_IN_USE:
      *ppszErrIdent = "ERROR_RING2_STACK_IN_USE";
      break;
    case ERROR_META_EXPANSION_TOO_LONG:
      *ppszErrIdent = "ERROR_META_EXPANSION_TOO_LONG";
      break;
    case ERROR_INVALID_SIGNAL_NUMBER:
      *ppszErrIdent = "ERROR_INVALID_SIGNAL_NUMBER";
      break;
    case ERROR_THREAD_1_INACTIVE:
      *ppszErrIdent = "ERROR_THREAD_1_INACTIVE";
      break;
    case ERROR_LOCKED:
      *ppszErrIdent = "ERROR_LOCKED";
      break;
    case ERROR_TOO_MANY_MODULES:
      *ppszErrIdent = "ERROR_TOO_MANY_MODULES";
      break;
    case ERROR_NESTING_NOT_ALLOWED:
      *ppszErrIdent = "ERROR_NESTING_NOT_ALLOWED";
      break;
    case ERROR_BAD_PIPE:
      *ppszErrIdent = "ERROR_BAD_PIPE";
      break;
    case ERROR_PIPE_BUSY:
      *ppszErrIdent = "ERROR_PIPE_BUSY";
      break;
    case ERROR_NO_DATA:
      *ppszErrIdent = "ERROR_NO_DATA";
      break;
    case ERROR_PIPE_NOT_CONNECTED:
      *ppszErrIdent = "ERROR_PIPE_NOT_CONNECTED";
      break;
    case ERROR_MORE_DATA:
      *ppszErrIdent = "ERROR_MORE_DATA";
      break;
    case ERROR_VC_DISCONNECTED:
      *ppszErrIdent = "ERROR_VC_DISCONNECTED";
      break;
    case ERROR_INVALID_EA_NAME:
      *ppszErrIdent = "ERROR_INVALID_EA_NAME";
      break;
    case ERROR_EA_LIST_INCONSISTENT:
      *ppszErrIdent = "ERROR_EA_LIST_INCONSISTENT";
      break;
    case ERROR_NO_MORE_ITEMS:
      *ppszErrIdent = "ERROR_NO_MORE_ITEMS";
      break;
    case ERROR_CANNOT_COPY:
      *ppszErrIdent = "ERROR_CANNOT_COPY";
      break;
    case ERROR_DIRECTORY:
      *ppszErrIdent = "ERROR_DIRECTORY";
      break;
    case ERROR_EAS_DIDNT_FIT:
      *ppszErrIdent = "ERROR_EAS_DIDNT_FIT";
      break;
    case ERROR_EA_FILE_CORRUPT:
      *ppszErrIdent = "ERROR_EA_FILE_CORRUPT";
      break;
    case ERROR_EA_TABLE_FULL:
      *ppszErrIdent = "ERROR_EA_TABLE_FULL";
      break;
    case ERROR_INVALID_EA_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_EA_HANDLE";
      break;
    case ERROR_EAS_NOT_SUPPORTED:
      *ppszErrIdent = "ERROR_EAS_NOT_SUPPORTED";
      break;
    case ERROR_NOT_OWNER:
      *ppszErrIdent = "ERROR_NOT_OWNER";
      break;
    case ERROR_TOO_MANY_POSTS:
      *ppszErrIdent = "ERROR_TOO_MANY_POSTS";
      break;
    case ERROR_MR_MID_NOT_FOUND:
      *ppszErrIdent = "ERROR_MR_MID_NOT_FOUND";
      break;
    case ERROR_INVALID_ADDRESS:
      *ppszErrIdent = "ERROR_INVALID_ADDRESS";
      break;
    case ERROR_ARITHMETIC_OVERFLOW:
      *ppszErrIdent = "ERROR_ARITHMETIC_OVERFLOW";
      break;
    case ERROR_PIPE_CONNECTED:
      *ppszErrIdent = "ERROR_PIPE_CONNECTED";
      break;
    case ERROR_PIPE_LISTENING:
      *ppszErrIdent = "ERROR_PIPE_LISTENING";
      break;
    case ERROR_EA_ACCESS_DENIED:
      *ppszErrIdent = "ERROR_EA_ACCESS_DENIED";
      break;
    case ERROR_OPERATION_ABORTED:
      *ppszErrIdent = "ERROR_OPERATION_ABORTED";
      break;
    case ERROR_IO_INCOMPLETE:
      *ppszErrIdent = "ERROR_IO_INCOMPLETE";
      break;
    case ERROR_IO_PENDING:
      *ppszErrIdent = "ERROR_IO_PENDING";
      break;
    case ERROR_NOACCESS:
      *ppszErrIdent = "ERROR_NOACCESS";
      break;
    case ERROR_SWAPERROR:
      *ppszErrIdent = "ERROR_SWAPERROR";
      break;
    case ERROR_STACK_OVERFLOW:
      *ppszErrIdent = "ERROR_STACK_OVERFLOW";
      break;
    case ERROR_INVALID_MESSAGE:
      *ppszErrIdent = "ERROR_INVALID_MESSAGE";
      break;
    case ERROR_CAN_NOT_COMPLETE:
      *ppszErrIdent = "ERROR_CAN_NOT_COMPLETE";
      break;
    case ERROR_INVALID_FLAGS:
      *ppszErrIdent = "ERROR_INVALID_FLAGS";
      break;
    case ERROR_UNRECOGNIZED_VOLUME:
      *ppszErrIdent = "ERROR_UNRECOGNIZED_VOLUME";
      break;
    case ERROR_FILE_INVALID:
      *ppszErrIdent = "ERROR_FILE_INVALID";
      break;
    case ERROR_FULLSCREEN_MODE:
      *ppszErrIdent = "ERROR_FULLSCREEN_MODE";
      break;
    case ERROR_NO_TOKEN:
      *ppszErrIdent = "ERROR_NO_TOKEN";
      break;
    case ERROR_BADDB:
      *ppszErrIdent = "ERROR_BADDB";
      break;
    case ERROR_BADKEY:
      *ppszErrIdent = "ERROR_BADKEY";
      break;
    case ERROR_CANTOPEN:
      *ppszErrIdent = "ERROR_CANTOPEN";
      break;
    case ERROR_CANTREAD:
      *ppszErrIdent = "ERROR_CANTREAD";
      break;
    case ERROR_CANTWRITE:
      *ppszErrIdent = "ERROR_CANTWRITE";
      break;
    case ERROR_REGISTRY_RECOVERED:
      *ppszErrIdent = "ERROR_REGISTRY_RECOVERED";
      break;
    case ERROR_REGISTRY_CORRUPT:
      *ppszErrIdent = "ERROR_REGISTRY_CORRUPT";
      break;
    case ERROR_REGISTRY_IO_FAILED:
      *ppszErrIdent = "ERROR_REGISTRY_IO_FAILED";
      break;
    case ERROR_NOT_REGISTRY_FILE:
      *ppszErrIdent = "ERROR_NOT_REGISTRY_FILE";
      break;
    case ERROR_KEY_DELETED:
      *ppszErrIdent = "ERROR_KEY_DELETED";
      break;
    case ERROR_NO_LOG_SPACE:
      *ppszErrIdent = "ERROR_NO_LOG_SPACE";
      break;
    case ERROR_KEY_HAS_CHILDREN:
      *ppszErrIdent = "ERROR_KEY_HAS_CHILDREN";
      break;
    case ERROR_CHILD_MUST_BE_VOLATILE:
      *ppszErrIdent = "ERROR_CHILD_MUST_BE_VOLATILE";
      break;
    case ERROR_NOTIFY_ENUM_DIR:
      *ppszErrIdent = "ERROR_NOTIFY_ENUM_DIR";
      break;
    case ERROR_DEPENDENT_SERVICES_RUNNING:
      *ppszErrIdent = "ERROR_DEPENDENT_SERVICES_RUNNING";
      break;
    case ERROR_INVALID_SERVICE_CONTROL:
      *ppszErrIdent = "ERROR_INVALID_SERVICE_CONTROL";
      break;
    case ERROR_SERVICE_REQUEST_TIMEOUT:
      *ppszErrIdent = "ERROR_SERVICE_REQUEST_TIMEOUT";
      break;
    case ERROR_SERVICE_NO_THREAD:
      *ppszErrIdent = "ERROR_SERVICE_NO_THREAD";
      break;
    case ERROR_SERVICE_DATABASE_LOCKED:
      *ppszErrIdent = "ERROR_SERVICE_DATABASE_LOCKED";
      break;
    case ERROR_SERVICE_ALREADY_RUNNING:
      *ppszErrIdent = "ERROR_SERVICE_ALREADY_RUNNING";
      break;
    case ERROR_INVALID_SERVICE_ACCOUNT:
      *ppszErrIdent = "ERROR_INVALID_SERVICE_ACCOUNT";
      break;
    case ERROR_SERVICE_DISABLED:
      *ppszErrIdent = "ERROR_SERVICE_DISABLED";
      break;
    case ERROR_CIRCULAR_DEPENDENCY:
      *ppszErrIdent = "ERROR_CIRCULAR_DEPENDENCY";
      break;
    case ERROR_SERVICE_DOES_NOT_EXIST:
      *ppszErrIdent = "ERROR_SERVICE_DOES_NOT_EXIST";
      break;
    case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
      *ppszErrIdent = "ERROR_SERVICE_CANNOT_ACCEPT_CTRL";
      break;
    case ERROR_SERVICE_NOT_ACTIVE:
      *ppszErrIdent = "ERROR_SERVICE_NOT_ACTIVE";
      break;
    case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
      *ppszErrIdent = "ERROR_FAILED_SERVICE_CONTROLLER_CONNECT";
      break;
    case ERROR_EXCEPTION_IN_SERVICE:
      *ppszErrIdent = "ERROR_EXCEPTION_IN_SERVICE";
      break;
    case ERROR_DATABASE_DOES_NOT_EXIST:
      *ppszErrIdent = "ERROR_DATABASE_DOES_NOT_EXIST";
      break;
    case ERROR_SERVICE_SPECIFIC_ERROR:
      *ppszErrIdent = "ERROR_SERVICE_SPECIFIC_ERROR";
      break;
    case ERROR_PROCESS_ABORTED:
      *ppszErrIdent = "ERROR_PROCESS_ABORTED";
      break;
    case ERROR_SERVICE_DEPENDENCY_FAIL:
      *ppszErrIdent = "ERROR_SERVICE_DEPENDENCY_FAIL";
      break;
    case ERROR_SERVICE_LOGON_FAILED:
      *ppszErrIdent = "ERROR_SERVICE_LOGON_FAILED";
      break;
    case ERROR_SERVICE_START_HANG:
      *ppszErrIdent = "ERROR_SERVICE_START_HANG";
      break;
    case ERROR_INVALID_SERVICE_LOCK:
      *ppszErrIdent = "ERROR_INVALID_SERVICE_LOCK";
      break;
    case ERROR_SERVICE_MARKED_FOR_DELETE:
      *ppszErrIdent = "ERROR_SERVICE_MARKED_FOR_DELETE";
      break;
    case ERROR_SERVICE_EXISTS:
      *ppszErrIdent = "ERROR_SERVICE_EXISTS";
      break;
    case ERROR_ALREADY_RUNNING_LKG:
      *ppszErrIdent = "ERROR_ALREADY_RUNNING_LKG";
      break;
    case ERROR_SERVICE_DEPENDENCY_DELETED:
      *ppszErrIdent = "ERROR_SERVICE_DEPENDENCY_DELETED";
      break;
    case ERROR_BOOT_ALREADY_ACCEPTED:
      *ppszErrIdent = "ERROR_BOOT_ALREADY_ACCEPTED";
      break;
    case ERROR_SERVICE_NEVER_STARTED:
      *ppszErrIdent = "ERROR_SERVICE_NEVER_STARTED";
      break;
    case ERROR_DUPLICATE_SERVICE_NAME:
      *ppszErrIdent = "ERROR_DUPLICATE_SERVICE_NAME";
      break;
    case ERROR_END_OF_MEDIA:
      *ppszErrIdent = "ERROR_END_OF_MEDIA";
      break;
    case ERROR_FILEMARK_DETECTED:
      *ppszErrIdent = "ERROR_FILEMARK_DETECTED";
      break;
    case ERROR_BEGINNING_OF_MEDIA:
      *ppszErrIdent = "ERROR_BEGINNING_OF_MEDIA";
      break;
    case ERROR_SETMARK_DETECTED:
      *ppszErrIdent = "ERROR_SETMARK_DETECTED";
      break;
    case ERROR_NO_DATA_DETECTED:
      *ppszErrIdent = "ERROR_NO_DATA_DETECTED";
      break;
    case ERROR_PARTITION_FAILURE:
      *ppszErrIdent = "ERROR_PARTITION_FAILURE";
      break;
    case ERROR_INVALID_BLOCK_LENGTH:
      *ppszErrIdent = "ERROR_INVALID_BLOCK_LENGTH";
      break;
    case ERROR_DEVICE_NOT_PARTITIONED:
      *ppszErrIdent = "ERROR_DEVICE_NOT_PARTITIONED";
      break;
    case ERROR_UNABLE_TO_LOCK_MEDIA:
      *ppszErrIdent = "ERROR_UNABLE_TO_LOCK_MEDIA";
      break;
    case ERROR_UNABLE_TO_UNLOAD_MEDIA:
      *ppszErrIdent = "ERROR_UNABLE_TO_UNLOAD_MEDIA";
      break;
    case ERROR_MEDIA_CHANGED:
      *ppszErrIdent = "ERROR_MEDIA_CHANGED";
      break;
    case ERROR_BUS_RESET:
      *ppszErrIdent = "ERROR_BUS_RESET";
      break;
    case ERROR_NO_MEDIA_IN_DRIVE:
      *ppszErrIdent = "ERROR_NO_MEDIA_IN_DRIVE";
      break;
    case ERROR_NO_UNICODE_TRANSLATION:
      *ppszErrIdent = "ERROR_NO_UNICODE_TRANSLATION";
      break;
    case ERROR_DLL_INIT_FAILED:
      *ppszErrIdent = "ERROR_DLL_INIT_FAILED";
      break;
    case ERROR_SHUTDOWN_IN_PROGRESS:
      *ppszErrIdent = "ERROR_SHUTDOWN_IN_PROGRESS";
      break;
    case ERROR_NO_SHUTDOWN_IN_PROGRESS:
      *ppszErrIdent = "ERROR_NO_SHUTDOWN_IN_PROGRESS";
      break;
    case ERROR_IO_DEVICE:
      *ppszErrIdent = "ERROR_IO_DEVICE";
      break;
    case ERROR_SERIAL_NO_DEVICE:
      *ppszErrIdent = "ERROR_SERIAL_NO_DEVICE";
      break;
    case ERROR_IRQ_BUSY:
      *ppszErrIdent = "ERROR_IRQ_BUSY";
      break;
    case ERROR_MORE_WRITES:
      *ppszErrIdent = "ERROR_MORE_WRITES";
      break;
    case ERROR_COUNTER_TIMEOUT:
      *ppszErrIdent = "ERROR_COUNTER_TIMEOUT";
      break;
    case ERROR_FLOPPY_ID_MARK_NOT_FOUND:
      *ppszErrIdent = "ERROR_FLOPPY_ID_MARK_NOT_FOUND";
      break;
    case ERROR_FLOPPY_WRONG_CYLINDER:
      *ppszErrIdent = "ERROR_FLOPPY_WRONG_CYLINDER";
      break;
    case ERROR_FLOPPY_UNKNOWN_ERROR:
      *ppszErrIdent = "ERROR_FLOPPY_UNKNOWN_ERROR";
      break;
    case ERROR_FLOPPY_BAD_REGISTERS:
      *ppszErrIdent = "ERROR_FLOPPY_BAD_REGISTERS";
      break;
    case ERROR_DISK_RECALIBRATE_FAILED:
      *ppszErrIdent = "ERROR_DISK_RECALIBRATE_FAILED";
      break;
    case ERROR_DISK_OPERATION_FAILED:
      *ppszErrIdent = "ERROR_DISK_OPERATION_FAILED";
      break;
    case ERROR_DISK_RESET_FAILED:
      *ppszErrIdent = "ERROR_DISK_RESET_FAILED";
      break;
    case ERROR_EOM_OVERFLOW:
      *ppszErrIdent = "ERROR_EOM_OVERFLOW";
      break;
    case ERROR_NOT_ENOUGH_SERVER_MEMORY:
      *ppszErrIdent = "ERROR_NOT_ENOUGH_SERVER_MEMORY";
      break;
    case ERROR_POSSIBLE_DEADLOCK:
      *ppszErrIdent = "ERROR_POSSIBLE_DEADLOCK";
      break;
    case ERROR_MAPPED_ALIGNMENT:
      *ppszErrIdent = "ERROR_MAPPED_ALIGNMENT";
      break;
    case ERROR_SET_POWER_STATE_VETOED:
      *ppszErrIdent = "ERROR_SET_POWER_STATE_VETOED";
      break;
    case ERROR_SET_POWER_STATE_FAILED:
      *ppszErrIdent = "ERROR_SET_POWER_STATE_FAILED";
      break;
    case ERROR_OLD_WIN_VERSION:
      *ppszErrIdent = "ERROR_OLD_WIN_VERSION";
      break;
    case ERROR_APP_WRONG_OS   :
      *ppszErrIdent = "ERROR_APP_WRONG_OS   ";
      break;
    case ERROR_SINGLE_INSTANCE_APP:
      *ppszErrIdent = "ERROR_SINGLE_INSTANCE_APP";
      break;
    case ERROR_RMODE_APP:
      *ppszErrIdent = "ERROR_RMODE_APP";
      break;
    case ERROR_INVALID_DLL:
      *ppszErrIdent = "ERROR_INVALID_DLL";
      break;
    case ERROR_NO_ASSOCIATION:
      *ppszErrIdent = "ERROR_NO_ASSOCIATION";
      break;
    case ERROR_DDE_FAIL:
      *ppszErrIdent = "ERROR_DDE_FAIL";
      break;
    case ERROR_DLL_NOT_FOUND:
      *ppszErrIdent = "ERROR_DLL_NOT_FOUND";
      break;
    case ERROR_BAD_DEVICE:
      *ppszErrIdent = "ERROR_BAD_DEVICE";
      break;
    case ERROR_CONNECTION_UNAVAIL:
      *ppszErrIdent = "ERROR_CONNECTION_UNAVAIL";
      break;
    case ERROR_DEVICE_ALREADY_REMEMBERED:
      *ppszErrIdent = "ERROR_DEVICE_ALREADY_REMEMBERED";
      break;
    case ERROR_NO_NET_OR_BAD_PATH:
      *ppszErrIdent = "ERROR_NO_NET_OR_BAD_PATH";
      break;
    case ERROR_BAD_PROVIDER:
      *ppszErrIdent = "ERROR_BAD_PROVIDER";
      break;
    case ERROR_CANNOT_OPEN_PROFILE:
      *ppszErrIdent = "ERROR_CANNOT_OPEN_PROFILE";
      break;
    case ERROR_BAD_PROFILE:
      *ppszErrIdent = "ERROR_BAD_PROFILE";
      break;
    case ERROR_NOT_CONTAINER:
      *ppszErrIdent = "ERROR_NOT_CONTAINER";
      break;
    case ERROR_EXTENDED_ERROR:
      *ppszErrIdent = "ERROR_EXTENDED_ERROR";
      break;
    case ERROR_INVALID_GROUPNAME:
      *ppszErrIdent = "ERROR_INVALID_GROUPNAME";
      break;
    case ERROR_INVALID_COMPUTERNAME:
      *ppszErrIdent = "ERROR_INVALID_COMPUTERNAME";
      break;
    case ERROR_INVALID_EVENTNAME:
      *ppszErrIdent = "ERROR_INVALID_EVENTNAME";
      break;
    case ERROR_INVALID_DOMAINNAME:
      *ppszErrIdent = "ERROR_INVALID_DOMAINNAME";
      break;
    case ERROR_INVALID_SERVICENAME:
      *ppszErrIdent = "ERROR_INVALID_SERVICENAME";
      break;
    case ERROR_INVALID_NETNAME:
      *ppszErrIdent = "ERROR_INVALID_NETNAME";
      break;
    case ERROR_INVALID_SHARENAME:
      *ppszErrIdent = "ERROR_INVALID_SHARENAME";
      break;
    case ERROR_INVALID_PASSWORDNAME:
      *ppszErrIdent = "ERROR_INVALID_PASSWORDNAME";
      break;
    case ERROR_INVALID_MESSAGENAME:
      *ppszErrIdent = "ERROR_INVALID_MESSAGENAME";
      break;
    case ERROR_INVALID_MESSAGEDEST:
      *ppszErrIdent = "ERROR_INVALID_MESSAGEDEST";
      break;
    case ERROR_SESSION_CREDENTIAL_CONFLICT:
      *ppszErrIdent = "ERROR_SESSION_CREDENTIAL_CONFLICT";
      break;
    case ERROR_REMOTE_SESSION_LIMIT_EXCEEDED:
      *ppszErrIdent = "ERROR_REMOTE_SESSION_LIMIT_EXCEEDED";
      break;
    case ERROR_DUP_DOMAINNAME:
      *ppszErrIdent = "ERROR_DUP_DOMAINNAME";
      break;
    case ERROR_ALREADY_INITIALIZED:
      *ppszErrIdent = "ERROR_ALREADY_INITIALIZED";
      break;
    case ERROR_NO_MORE_DEVICES:
      *ppszErrIdent = "ERROR_NO_MORE_DEVICES";
      break;
    case ERROR_NOT_ALL_ASSIGNED:
      *ppszErrIdent = "ERROR_NOT_ALL_ASSIGNED";
      break;
    case ERROR_SOME_NOT_MAPPED:
      *ppszErrIdent = "ERROR_SOME_NOT_MAPPED";
      break;
    case ERROR_NO_QUOTAS_FOR_ACCOUNT:
      *ppszErrIdent = "ERROR_NO_QUOTAS_FOR_ACCOUNT";
      break;
    case ERROR_LOCAL_USER_SESSION_KEY:
      *ppszErrIdent = "ERROR_LOCAL_USER_SESSION_KEY";
      break;
    case ERROR_NULL_LM_PASSWORD:
      *ppszErrIdent = "ERROR_NULL_LM_PASSWORD";
      break;
    case ERROR_UNKNOWN_REVISION:
      *ppszErrIdent = "ERROR_UNKNOWN_REVISION";
      break;
    case ERROR_REVISION_MISMATCH:
      *ppszErrIdent = "ERROR_REVISION_MISMATCH";
      break;
    case ERROR_INVALID_OWNER:
      *ppszErrIdent = "ERROR_INVALID_OWNER";
      break;
    case ERROR_INVALID_PRIMARY_GROUP:
      *ppszErrIdent = "ERROR_INVALID_PRIMARY_GROUP";
      break;
    case ERROR_NO_IMPERSONATION_TOKEN:
      *ppszErrIdent = "ERROR_NO_IMPERSONATION_TOKEN";
      break;
    case ERROR_CANT_DISABLE_MANDATORY:
      *ppszErrIdent = "ERROR_CANT_DISABLE_MANDATORY";
      break;
    case ERROR_NO_LOGON_SERVERS:
      *ppszErrIdent = "ERROR_NO_LOGON_SERVERS";
      break;
    case ERROR_NO_SUCH_LOGON_SESSION:
      *ppszErrIdent = "ERROR_NO_SUCH_LOGON_SESSION";
      break;
    case ERROR_NO_SUCH_PRIVILEGE:
      *ppszErrIdent = "ERROR_NO_SUCH_PRIVILEGE";
      break;
    case ERROR_PRIVILEGE_NOT_HELD:
      *ppszErrIdent = "ERROR_PRIVILEGE_NOT_HELD";
      break;
    case ERROR_INVALID_ACCOUNT_NAME:
      *ppszErrIdent = "ERROR_INVALID_ACCOUNT_NAME";
      break;
    case ERROR_USER_EXISTS:
      *ppszErrIdent = "ERROR_USER_EXISTS";
      break;
    case ERROR_NO_SUCH_USER:
      *ppszErrIdent = "ERROR_NO_SUCH_USER";
      break;
    case ERROR_GROUP_EXISTS:
      *ppszErrIdent = "ERROR_GROUP_EXISTS";
      break;
    case ERROR_NO_SUCH_GROUP:
      *ppszErrIdent = "ERROR_NO_SUCH_GROUP";
      break;
    case ERROR_MEMBER_IN_GROUP:
      *ppszErrIdent = "ERROR_MEMBER_IN_GROUP";
      break;
    case ERROR_MEMBER_NOT_IN_GROUP:
      *ppszErrIdent = "ERROR_MEMBER_NOT_IN_GROUP";
      break;
    case ERROR_LAST_ADMIN:
      *ppszErrIdent = "ERROR_LAST_ADMIN";
      break;
    case ERROR_WRONG_PASSWORD:
      *ppszErrIdent = "ERROR_WRONG_PASSWORD";
      break;
    case ERROR_ILL_FORMED_PASSWORD:
      *ppszErrIdent = "ERROR_ILL_FORMED_PASSWORD";
      break;
    case ERROR_PASSWORD_RESTRICTION:
      *ppszErrIdent = "ERROR_PASSWORD_RESTRICTION";
      break;
    case ERROR_LOGON_FAILURE:
      *ppszErrIdent = "ERROR_LOGON_FAILURE";
      break;
    case ERROR_ACCOUNT_RESTRICTION:
      *ppszErrIdent = "ERROR_ACCOUNT_RESTRICTION";
      break;
    case ERROR_INVALID_LOGON_HOURS:
      *ppszErrIdent = "ERROR_INVALID_LOGON_HOURS";
      break;
    case ERROR_INVALID_WORKSTATION:
      *ppszErrIdent = "ERROR_INVALID_WORKSTATION";
      break;
    case ERROR_PASSWORD_EXPIRED:
      *ppszErrIdent = "ERROR_PASSWORD_EXPIRED";
      break;
    case ERROR_ACCOUNT_DISABLED:
      *ppszErrIdent = "ERROR_ACCOUNT_DISABLED";
      break;
    case ERROR_NONE_MAPPED:
      *ppszErrIdent = "ERROR_NONE_MAPPED";
      break;
    case ERROR_TOO_MANY_LUIDS_REQUESTED:
      *ppszErrIdent = "ERROR_TOO_MANY_LUIDS_REQUESTED";
      break;
    case ERROR_LUIDS_EXHAUSTED:
      *ppszErrIdent = "ERROR_LUIDS_EXHAUSTED";
      break;
    case ERROR_INVALID_SUB_AUTHORITY:
      *ppszErrIdent = "ERROR_INVALID_SUB_AUTHORITY";
      break;
    case ERROR_INVALID_ACL:
      *ppszErrIdent = "ERROR_INVALID_ACL";
      break;
    case ERROR_INVALID_SID:
      *ppszErrIdent = "ERROR_INVALID_SID";
      break;
    case ERROR_INVALID_SECURITY_DESCR:
      *ppszErrIdent = "ERROR_INVALID_SECURITY_DESCR";
      break;
    case ERROR_BAD_INHERITANCE_ACL:
      *ppszErrIdent = "ERROR_BAD_INHERITANCE_ACL";
      break;
    case ERROR_SERVER_DISABLED:
      *ppszErrIdent = "ERROR_SERVER_DISABLED";
      break;
    case ERROR_SERVER_NOT_DISABLED:
      *ppszErrIdent = "ERROR_SERVER_NOT_DISABLED";
      break;
    case ERROR_INVALID_ID_AUTHORITY:
      *ppszErrIdent = "ERROR_INVALID_ID_AUTHORITY";
      break;
    case ERROR_ALLOTTED_SPACE_EXCEEDED:
      *ppszErrIdent = "ERROR_ALLOTTED_SPACE_EXCEEDED";
      break;
    case ERROR_INVALID_GROUP_ATTRIBUTES:
      *ppszErrIdent = "ERROR_INVALID_GROUP_ATTRIBUTES";
      break;
    case ERROR_BAD_IMPERSONATION_LEVEL:
      *ppszErrIdent = "ERROR_BAD_IMPERSONATION_LEVEL";
      break;
    case ERROR_CANT_OPEN_ANONYMOUS:
      *ppszErrIdent = "ERROR_CANT_OPEN_ANONYMOUS";
      break;
    case ERROR_BAD_VALIDATION_CLASS:
      *ppszErrIdent = "ERROR_BAD_VALIDATION_CLASS";
      break;
    case ERROR_BAD_TOKEN_TYPE:
      *ppszErrIdent = "ERROR_BAD_TOKEN_TYPE";
      break;
    case ERROR_NO_SECURITY_ON_OBJECT:
      *ppszErrIdent = "ERROR_NO_SECURITY_ON_OBJECT";
      break;
    case ERROR_CANT_ACCESS_DOMAIN_INFO:
      *ppszErrIdent = "ERROR_CANT_ACCESS_DOMAIN_INFO";
      break;
    case ERROR_INVALID_SERVER_STATE:
      *ppszErrIdent = "ERROR_INVALID_SERVER_STATE";
      break;
    case ERROR_INVALID_DOMAIN_STATE:
      *ppszErrIdent = "ERROR_INVALID_DOMAIN_STATE";
      break;
    case ERROR_INVALID_DOMAIN_ROLE:
      *ppszErrIdent = "ERROR_INVALID_DOMAIN_ROLE";
      break;
    case ERROR_NO_SUCH_DOMAIN:
      *ppszErrIdent = "ERROR_NO_SUCH_DOMAIN";
      break;
    case ERROR_DOMAIN_EXISTS:
      *ppszErrIdent = "ERROR_DOMAIN_EXISTS";
      break;
    case ERROR_DOMAIN_LIMIT_EXCEEDED:
      *ppszErrIdent = "ERROR_DOMAIN_LIMIT_EXCEEDED";
      break;
    case ERROR_INTERNAL_DB_CORRUPTION:
      *ppszErrIdent = "ERROR_INTERNAL_DB_CORRUPTION";
      break;
    case ERROR_INTERNAL_ERROR:
      *ppszErrIdent = "ERROR_INTERNAL_ERROR";
      break;
    case ERROR_GENERIC_NOT_MAPPED:
      *ppszErrIdent = "ERROR_GENERIC_NOT_MAPPED";
      break;
    case ERROR_BAD_DESCRIPTOR_FORMAT:
      *ppszErrIdent = "ERROR_BAD_DESCRIPTOR_FORMAT";
      break;
    case ERROR_NOT_LOGON_PROCESS:
      *ppszErrIdent = "ERROR_NOT_LOGON_PROCESS";
      break;
    case ERROR_LOGON_SESSION_EXISTS:
      *ppszErrIdent = "ERROR_LOGON_SESSION_EXISTS";
      break;
    case ERROR_NO_SUCH_PACKAGE:
      *ppszErrIdent = "ERROR_NO_SUCH_PACKAGE";
      break;
    case ERROR_BAD_LOGON_SESSION_STATE:
      *ppszErrIdent = "ERROR_BAD_LOGON_SESSION_STATE";
      break;
    case ERROR_LOGON_SESSION_COLLISION:
      *ppszErrIdent = "ERROR_LOGON_SESSION_COLLISION";
      break;
    case ERROR_INVALID_LOGON_TYPE:
      *ppszErrIdent = "ERROR_INVALID_LOGON_TYPE";
      break;
    case ERROR_CANNOT_IMPERSONATE:
      *ppszErrIdent = "ERROR_CANNOT_IMPERSONATE";
      break;
    case ERROR_RXACT_INVALID_STATE:
      *ppszErrIdent = "ERROR_RXACT_INVALID_STATE";
      break;
    case ERROR_RXACT_COMMIT_FAILURE:
      *ppszErrIdent = "ERROR_RXACT_COMMIT_FAILURE";
      break;
    case ERROR_SPECIAL_ACCOUNT:
      *ppszErrIdent = "ERROR_SPECIAL_ACCOUNT";
      break;
    case ERROR_SPECIAL_GROUP:
      *ppszErrIdent = "ERROR_SPECIAL_GROUP";
      break;
    case ERROR_SPECIAL_USER:
      *ppszErrIdent = "ERROR_SPECIAL_USER";
      break;
    case ERROR_MEMBERS_PRIMARY_GROUP:
      *ppszErrIdent = "ERROR_MEMBERS_PRIMARY_GROUP";
      break;
    case ERROR_TOKEN_ALREADY_IN_USE:
      *ppszErrIdent = "ERROR_TOKEN_ALREADY_IN_USE";
      break;
    case ERROR_NO_SUCH_ALIAS:
      *ppszErrIdent = "ERROR_NO_SUCH_ALIAS";
      break;
    case ERROR_MEMBER_NOT_IN_ALIAS:
      *ppszErrIdent = "ERROR_MEMBER_NOT_IN_ALIAS";
      break;
    case ERROR_MEMBER_IN_ALIAS:
      *ppszErrIdent = "ERROR_MEMBER_IN_ALIAS";
      break;
    case ERROR_ALIAS_EXISTS:
      *ppszErrIdent = "ERROR_ALIAS_EXISTS";
      break;
    case ERROR_LOGON_NOT_GRANTED:
      *ppszErrIdent = "ERROR_LOGON_NOT_GRANTED";
      break;
    case ERROR_TOO_MANY_SECRETS:
      *ppszErrIdent = "ERROR_TOO_MANY_SECRETS";
      break;
    case ERROR_SECRET_TOO_LONG:
      *ppszErrIdent = "ERROR_SECRET_TOO_LONG";
      break;
    case ERROR_INTERNAL_DB_ERROR:
      *ppszErrIdent = "ERROR_INTERNAL_DB_ERROR";
      break;
    case ERROR_TOO_MANY_CONTEXT_IDS:
      *ppszErrIdent = "ERROR_TOO_MANY_CONTEXT_IDS";
      break;
    case ERROR_LOGON_TYPE_NOT_GRANTED:
      *ppszErrIdent = "ERROR_LOGON_TYPE_NOT_GRANTED";
      break;
    case ERROR_NT_CROSS_ENCRYPTION_REQUIRED:
      *ppszErrIdent = "ERROR_NT_CROSS_ENCRYPTION_REQUIRED";
      break;
    case ERROR_NO_SUCH_MEMBER:
      *ppszErrIdent = "ERROR_NO_SUCH_MEMBER";
      break;
    case ERROR_INVALID_MEMBER:
      *ppszErrIdent = "ERROR_INVALID_MEMBER";
      break;
    case ERROR_TOO_MANY_SIDS:
      *ppszErrIdent = "ERROR_TOO_MANY_SIDS";
      break;
    case ERROR_LM_CROSS_ENCRYPTION_REQUIRED:
      *ppszErrIdent = "ERROR_LM_CROSS_ENCRYPTION_REQUIRED";
      break;
    case ERROR_NO_INHERITANCE:
      *ppszErrIdent = "ERROR_NO_INHERITANCE";
      break;
    case ERROR_FILE_CORRUPT:
      *ppszErrIdent = "ERROR_FILE_CORRUPT";
      break;
    case ERROR_DISK_CORRUPT:
      *ppszErrIdent = "ERROR_DISK_CORRUPT";
      break;
    case ERROR_NO_USER_SESSION_KEY:
      *ppszErrIdent = "ERROR_NO_USER_SESSION_KEY";
      break;
    case ERROR_LICENSE_QUOTA_EXCEEDED:
      *ppszErrIdent = "ERROR_LICENSE_QUOTA_EXCEEDED";
      break;
    case ERROR_INVALID_WINDOW_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_WINDOW_HANDLE";
      break;
    case ERROR_INVALID_MENU_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_MENU_HANDLE";
      break;
    case ERROR_INVALID_CURSOR_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_CURSOR_HANDLE";
      break;
    case ERROR_INVALID_ACCEL_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_ACCEL_HANDLE";
      break;
    case ERROR_INVALID_HOOK_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_HOOK_HANDLE";
      break;
    case ERROR_INVALID_DWP_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_DWP_HANDLE";
      break;
    case ERROR_TLW_WITH_WSCHILD:
      *ppszErrIdent = "ERROR_TLW_WITH_WSCHILD";
      break;
    case ERROR_CANNOT_FIND_WND_CLASS:
      *ppszErrIdent = "ERROR_CANNOT_FIND_WND_CLASS";
      break;
    case ERROR_WINDOW_OF_OTHER_THREAD:
      *ppszErrIdent = "ERROR_WINDOW_OF_OTHER_THREAD";
      break;
    case ERROR_HOTKEY_ALREADY_REGISTERED:
      *ppszErrIdent = "ERROR_HOTKEY_ALREADY_REGISTERED";
      break;
    case ERROR_CLASS_ALREADY_EXISTS:
      *ppszErrIdent = "ERROR_CLASS_ALREADY_EXISTS";
      break;
    case ERROR_CLASS_DOES_NOT_EXIST:
      *ppszErrIdent = "ERROR_CLASS_DOES_NOT_EXIST";
      break;
    case ERROR_CLASS_HAS_WINDOWS:
      *ppszErrIdent = "ERROR_CLASS_HAS_WINDOWS";
      break;
    case ERROR_INVALID_INDEX:
      *ppszErrIdent = "ERROR_INVALID_INDEX";
      break;
    case ERROR_INVALID_ICON_HANDLE:
      *ppszErrIdent = "ERROR_INVALID_ICON_HANDLE";
      break;
    case ERROR_PRIVATE_DIALOG_INDEX:
      *ppszErrIdent = "ERROR_PRIVATE_DIALOG_INDEX";
      break;
    case ERROR_LISTBOX_ID_NOT_FOUND:
      *ppszErrIdent = "ERROR_LISTBOX_ID_NOT_FOUND";
      break;
    case ERROR_NO_WILDCARD_CHARACTERS:
      *ppszErrIdent = "ERROR_NO_WILDCARD_CHARACTERS";
      break;
    case ERROR_CLIPBOARD_NOT_OPEN:
      *ppszErrIdent = "ERROR_CLIPBOARD_NOT_OPEN";
      break;
    case ERROR_HOTKEY_NOT_REGISTERED:
      *ppszErrIdent = "ERROR_HOTKEY_NOT_REGISTERED";
      break;
    case ERROR_WINDOW_NOT_DIALOG:
      *ppszErrIdent = "ERROR_WINDOW_NOT_DIALOG";
      break;
    case ERROR_CONTROL_ID_NOT_FOUND:
      *ppszErrIdent = "ERROR_CONTROL_ID_NOT_FOUND";
      break;
    case ERROR_INVALID_COMBOBOX_MESSAGE:
      *ppszErrIdent = "ERROR_INVALID_COMBOBOX_MESSAGE";
      break;
    case ERROR_WINDOW_NOT_COMBOBOX:
      *ppszErrIdent = "ERROR_WINDOW_NOT_COMBOBOX";
      break;
    case ERROR_INVALID_EDIT_HEIGHT:
      *ppszErrIdent = "ERROR_INVALID_EDIT_HEIGHT";
      break;
    case ERROR_DC_NOT_FOUND:
      *ppszErrIdent = "ERROR_DC_NOT_FOUND";
      break;
    case ERROR_INVALID_HOOK_FILTER:
      *ppszErrIdent = "ERROR_INVALID_HOOK_FILTER";
      break;
    case ERROR_INVALID_FILTER_PROC:
      *ppszErrIdent = "ERROR_INVALID_FILTER_PROC";
      break;
    case ERROR_HOOK_NEEDS_HMOD:
      *ppszErrIdent = "ERROR_HOOK_NEEDS_HMOD";
      break;
    case ERROR_GLOBAL_ONLY_HOOK:
      *ppszErrIdent = "ERROR_GLOBAL_ONLY_HOOK";
      break;
    case ERROR_JOURNAL_HOOK_SET:
      *ppszErrIdent = "ERROR_JOURNAL_HOOK_SET";
      break;
    case ERROR_HOOK_NOT_INSTALLED:
      *ppszErrIdent = "ERROR_HOOK_NOT_INSTALLED";
      break;
    case ERROR_INVALID_LB_MESSAGE:
      *ppszErrIdent = "ERROR_INVALID_LB_MESSAGE";
      break;
    case ERROR_SETCOUNT_ON_BAD_LB:
      *ppszErrIdent = "ERROR_SETCOUNT_ON_BAD_LB";
      break;
    case ERROR_LB_WITHOUT_TABSTOPS:
      *ppszErrIdent = "ERROR_LB_WITHOUT_TABSTOPS";
      break;
    case ERROR_DESTROY_OBJECT_OF_OTHER_THREAD:
      *ppszErrIdent = "ERROR_DESTROY_OBJECT_OF_OTHER_THREAD";
      break;
    case ERROR_CHILD_WINDOW_MENU:
      *ppszErrIdent = "ERROR_CHILD_WINDOW_MENU";
      break;
    case ERROR_NO_SYSTEM_MENU:
      *ppszErrIdent = "ERROR_NO_SYSTEM_MENU";
      break;
    case ERROR_INVALID_MSGBOX_STYLE:
      *ppszErrIdent = "ERROR_INVALID_MSGBOX_STYLE";
      break;
    case ERROR_INVALID_SPI_VALUE:
      *ppszErrIdent = "ERROR_INVALID_SPI_VALUE";
      break;
    case ERROR_SCREEN_ALREADY_LOCKED:
      *ppszErrIdent = "ERROR_SCREEN_ALREADY_LOCKED";
      break;
//    case ERROR_HWNDS_HAVE_DIFFERENT_PARENT:
//      *ppszErrIdent = "ERROR_HWNDS_HAVE_DIFFERENT_PARENT";
//      break;
    case ERROR_NOT_CHILD_WINDOW:
      *ppszErrIdent = "ERROR_NOT_CHILD_WINDOW";
      break;
    case ERROR_INVALID_GW_COMMAND:
      *ppszErrIdent = "ERROR_INVALID_GW_COMMAND";
      break;
    case ERROR_INVALID_THREAD_ID:
      *ppszErrIdent = "ERROR_INVALID_THREAD_ID";
      break;
    case ERROR_NON_MDICHILD_WINDOW:
      *ppszErrIdent = "ERROR_NON_MDICHILD_WINDOW";
      break;
    case ERROR_POPUP_ALREADY_ACTIVE:
      *ppszErrIdent = "ERROR_POPUP_ALREADY_ACTIVE";
      break;
    case ERROR_NO_SCROLLBARS:
      *ppszErrIdent = "ERROR_NO_SCROLLBARS";
      break;
    case ERROR_INVALID_SCROLLBAR_RANGE:
      *ppszErrIdent = "ERROR_INVALID_SCROLLBAR_RANGE";
      break;
    case ERROR_INVALID_SHOWWIN_COMMAND:
      *ppszErrIdent = "ERROR_INVALID_SHOWWIN_COMMAND";
      break;
    case ERROR_EVENTLOG_FILE_CORRUPT:
      *ppszErrIdent = "ERROR_EVENTLOG_FILE_CORRUPT";
      break;
    case ERROR_EVENTLOG_CANT_START:
      *ppszErrIdent = "ERROR_EVENTLOG_CANT_START";
      break;
    case ERROR_LOG_FILE_FULL:
      *ppszErrIdent = "ERROR_LOG_FILE_FULL";
      break;
    case ERROR_EVENTLOG_FILE_CHANGED:
      *ppszErrIdent = "ERROR_EVENTLOG_FILE_CHANGED";
      break;
    case RPC_S_INVALID_STRING_BINDING:
      *ppszErrIdent = "RPC_S_INVALID_STRING_BINDING";
      break;
    case RPC_S_WRONG_KIND_OF_BINDING:
      *ppszErrIdent = "RPC_S_WRONG_KIND_OF_BINDING";
      break;
    case RPC_S_INVALID_BINDING:
      *ppszErrIdent = "RPC_S_INVALID_BINDING";
      break;
    case RPC_S_PROTSEQ_NOT_SUPPORTED:
      *ppszErrIdent = "RPC_S_PROTSEQ_NOT_SUPPORTED";
      break;
    case RPC_S_INVALID_RPC_PROTSEQ:
      *ppszErrIdent = "RPC_S_INVALID_RPC_PROTSEQ";
      break;
    case RPC_S_INVALID_STRING_UUID:
      *ppszErrIdent = "RPC_S_INVALID_STRING_UUID";
      break;
    case RPC_S_INVALID_ENDPOINT_FORMAT:
      *ppszErrIdent = "RPC_S_INVALID_ENDPOINT_FORMAT";
      break;
    case RPC_S_INVALID_NET_ADDR:
      *ppszErrIdent = "RPC_S_INVALID_NET_ADDR";
      break;
    case RPC_S_NO_ENDPOINT_FOUND:
      *ppszErrIdent = "RPC_S_NO_ENDPOINT_FOUND";
      break;
    case RPC_S_INVALID_TIMEOUT:
      *ppszErrIdent = "RPC_S_INVALID_TIMEOUT";
      break;
    case RPC_S_OBJECT_NOT_FOUND:
      *ppszErrIdent = "RPC_S_OBJECT_NOT_FOUND";
      break;
    case RPC_S_ALREADY_REGISTERED:
      *ppszErrIdent = "RPC_S_ALREADY_REGISTERED";
      break;
    case RPC_S_TYPE_ALREADY_REGISTERED:
      *ppszErrIdent = "RPC_S_TYPE_ALREADY_REGISTERED";
      break;
    case RPC_S_ALREADY_LISTENING:
      *ppszErrIdent = "RPC_S_ALREADY_LISTENING";
      break;
    case RPC_S_NO_PROTSEQS_REGISTERED:
      *ppszErrIdent = "RPC_S_NO_PROTSEQS_REGISTERED";
      break;
    case RPC_S_NOT_LISTENING:
      *ppszErrIdent = "RPC_S_NOT_LISTENING";
      break;
    case RPC_S_UNKNOWN_MGR_TYPE:
      *ppszErrIdent = "RPC_S_UNKNOWN_MGR_TYPE";
      break;
    case RPC_S_UNKNOWN_IF:
      *ppszErrIdent = "RPC_S_UNKNOWN_IF";
      break;
    case RPC_S_NO_BINDINGS:
      *ppszErrIdent = "RPC_S_NO_BINDINGS";
      break;
    case RPC_S_NO_PROTSEQS:
      *ppszErrIdent = "RPC_S_NO_PROTSEQS";
      break;
    case RPC_S_CANT_CREATE_ENDPOINT:
      *ppszErrIdent = "RPC_S_CANT_CREATE_ENDPOINT";
      break;
    case RPC_S_OUT_OF_RESOURCES:
      *ppszErrIdent = "RPC_S_OUT_OF_RESOURCES";
      break;
    case RPC_S_SERVER_UNAVAILABLE:
      *ppszErrIdent = "RPC_S_SERVER_UNAVAILABLE";
      break;
    case RPC_S_SERVER_TOO_BUSY:
      *ppszErrIdent = "RPC_S_SERVER_TOO_BUSY";
      break;
    case RPC_S_INVALID_NETWORK_OPTIONS:
      *ppszErrIdent = "RPC_S_INVALID_NETWORK_OPTIONS";
      break;
    case RPC_S_NO_CALL_ACTIVE:
      *ppszErrIdent = "RPC_S_NO_CALL_ACTIVE";
      break;
    case RPC_S_CALL_FAILED:
      *ppszErrIdent = "RPC_S_CALL_FAILED";
      break;
    case RPC_S_CALL_FAILED_DNE:
      *ppszErrIdent = "RPC_S_CALL_FAILED_DNE";
      break;
    case RPC_S_PROTOCOL_ERROR:
      *ppszErrIdent = "RPC_S_PROTOCOL_ERROR";
      break;
    case RPC_S_UNSUPPORTED_TRANS_SYN:
      *ppszErrIdent = "RPC_S_UNSUPPORTED_TRANS_SYN";
      break;
//    case RPC_S_SERVER_OUT_OF_MEMORY:
//      *ppszErrIdent = "RPC_S_SERVER_OUT_OF_MEMORY";
//      break;
    case RPC_S_UNSUPPORTED_TYPE:
      *ppszErrIdent = "RPC_S_UNSUPPORTED_TYPE";
      break;
    case RPC_S_INVALID_TAG:
      *ppszErrIdent = "RPC_S_INVALID_TAG";
      break;
    case RPC_S_INVALID_BOUND:
      *ppszErrIdent = "RPC_S_INVALID_BOUND";
      break;
    case RPC_S_NO_ENTRY_NAME:
      *ppszErrIdent = "RPC_S_NO_ENTRY_NAME";
      break;
    case RPC_S_INVALID_NAME_SYNTAX:
      *ppszErrIdent = "RPC_S_INVALID_NAME_SYNTAX";
      break;
    case RPC_S_UNSUPPORTED_NAME_SYNTAX:
      *ppszErrIdent = "RPC_S_UNSUPPORTED_NAME_SYNTAX";
      break;
    case RPC_S_UUID_NO_ADDRESS:
      *ppszErrIdent = "RPC_S_UUID_NO_ADDRESS";
      break;
    case RPC_S_DUPLICATE_ENDPOINT:
      *ppszErrIdent = "RPC_S_DUPLICATE_ENDPOINT";
      break;
    case RPC_S_UNKNOWN_AUTHN_TYPE:
      *ppszErrIdent = "RPC_S_UNKNOWN_AUTHN_TYPE";
      break;
    case RPC_S_MAX_CALLS_TOO_SMALL:
      *ppszErrIdent = "RPC_S_MAX_CALLS_TOO_SMALL";
      break;
    case RPC_S_STRING_TOO_LONG:
      *ppszErrIdent = "RPC_S_STRING_TOO_LONG";
      break;
    case RPC_S_PROTSEQ_NOT_FOUND:
      *ppszErrIdent = "RPC_S_PROTSEQ_NOT_FOUND";
      break;
    case RPC_S_PROCNUM_OUT_OF_RANGE:
      *ppszErrIdent = "RPC_S_PROCNUM_OUT_OF_RANGE";
      break;
    case RPC_S_BINDING_HAS_NO_AUTH:
      *ppszErrIdent = "RPC_S_BINDING_HAS_NO_AUTH";
      break;
    case RPC_S_UNKNOWN_AUTHN_SERVICE:
      *ppszErrIdent = "RPC_S_UNKNOWN_AUTHN_SERVICE";
      break;
    case RPC_S_UNKNOWN_AUTHN_LEVEL:
      *ppszErrIdent = "RPC_S_UNKNOWN_AUTHN_LEVEL";
      break;
    case RPC_S_INVALID_AUTH_IDENTITY:
      *ppszErrIdent = "RPC_S_INVALID_AUTH_IDENTITY";
      break;
    case RPC_S_UNKNOWN_AUTHZ_SERVICE:
      *ppszErrIdent = "RPC_S_UNKNOWN_AUTHZ_SERVICE";
      break;
    case EPT_S_INVALID_ENTRY:
      *ppszErrIdent = "EPT_S_INVALID_ENTRY";
      break;
    case EPT_S_CANT_PERFORM_OP:
      *ppszErrIdent = "EPT_S_CANT_PERFORM_OP";
      break;
    case EPT_S_NOT_REGISTERED:
      *ppszErrIdent = "EPT_S_NOT_REGISTERED";
      break;
    case RPC_S_INCOMPLETE_NAME:
      *ppszErrIdent = "RPC_S_INCOMPLETE_NAME";
      break;
    case RPC_S_INVALID_VERS_OPTION:
      *ppszErrIdent = "RPC_S_INVALID_VERS_OPTION";
      break;
    case RPC_S_NO_MORE_MEMBERS:
      *ppszErrIdent = "RPC_S_NO_MORE_MEMBERS";
      break;
    case RPC_S_NOT_ALL_OBJS_UNEXPORTED:
      *ppszErrIdent = "RPC_S_NOT_ALL_OBJS_UNEXPORTED";
      break;
    case RPC_S_INTERFACE_NOT_FOUND:
      *ppszErrIdent = "RPC_S_INTERFACE_NOT_FOUND";
      break;
    case RPC_S_ENTRY_ALREADY_EXISTS:
      *ppszErrIdent = "RPC_S_ENTRY_ALREADY_EXISTS";
      break;
    case RPC_S_ENTRY_NOT_FOUND:
      *ppszErrIdent = "RPC_S_ENTRY_NOT_FOUND";
      break;
    case RPC_S_NAME_SERVICE_UNAVAILABLE:
      *ppszErrIdent = "RPC_S_NAME_SERVICE_UNAVAILABLE";
      break;
    case RPC_S_CANNOT_SUPPORT:
      *ppszErrIdent = "RPC_S_CANNOT_SUPPORT";
      break;
    case RPC_S_NO_CONTEXT_AVAILABLE:
      *ppszErrIdent = "RPC_S_NO_CONTEXT_AVAILABLE";
      break;
    case RPC_S_INTERNAL_ERROR:
      *ppszErrIdent = "RPC_S_INTERNAL_ERROR";
      break;
    case RPC_S_ZERO_DIVIDE:
      *ppszErrIdent = "RPC_S_ZERO_DIVIDE";
      break;
    case RPC_S_ADDRESS_ERROR:
      *ppszErrIdent = "RPC_S_ADDRESS_ERROR";
      break;
    case RPC_S_FP_DIV_ZERO:
      *ppszErrIdent = "RPC_S_FP_DIV_ZERO";
      break;
    case RPC_S_FP_UNDERFLOW:
      *ppszErrIdent = "RPC_S_FP_UNDERFLOW";
      break;
    case RPC_S_FP_OVERFLOW:
      *ppszErrIdent = "RPC_S_FP_OVERFLOW";
      break;
    case RPC_X_NO_MORE_ENTRIES:
      *ppszErrIdent = "RPC_X_NO_MORE_ENTRIES";
      break;
    case RPC_X_SS_CHAR_TRANS_OPEN_FAIL:
      *ppszErrIdent = "RPC_X_SS_CHAR_TRANS_OPEN_FAIL";
      break;
    case RPC_X_SS_CHAR_TRANS_SHORT_FILE:
      *ppszErrIdent = "RPC_X_SS_CHAR_TRANS_SHORT_FILE";
      break;
    case RPC_X_SS_IN_NULL_CONTEXT:
      *ppszErrIdent = "RPC_X_SS_IN_NULL_CONTEXT";
      break;
//    case RPC_X_SS_CONTEXT_MISMATCH:
//      *ppszErrIdent = "RPC_X_SS_CONTEXT_MISMATCH";
//      break;
    case RPC_X_SS_CONTEXT_DAMAGED:
      *ppszErrIdent = "RPC_X_SS_CONTEXT_DAMAGED";
      break;
    case RPC_X_SS_HANDLES_MISMATCH:
      *ppszErrIdent = "RPC_X_SS_HANDLES_MISMATCH";
      break;
    case RPC_X_SS_CANNOT_GET_CALL_HANDLE:
      *ppszErrIdent = "RPC_X_SS_CANNOT_GET_CALL_HANDLE";
      break;
//    case RPC_X_NULL_REF_POINTER:
//      *ppszErrIdent = "RPC_X_NULL_REF_POINTER";
//      break;
    case RPC_X_ENUM_VALUE_OUT_OF_RANGE:
      *ppszErrIdent = "RPC_X_ENUM_VALUE_OUT_OF_RANGE";
      break;
    case RPC_X_BYTE_COUNT_TOO_SMALL:
      *ppszErrIdent = "RPC_X_BYTE_COUNT_TOO_SMALL";
      break;
    case RPC_X_BAD_STUB_DATA:
      *ppszErrIdent = "RPC_X_BAD_STUB_DATA";
      break;
    case ERROR_INVALID_USER_BUFFER:
      *ppszErrIdent = "ERROR_INVALID_USER_BUFFER";
      break;
    case ERROR_UNRECOGNIZED_MEDIA:
      *ppszErrIdent = "ERROR_UNRECOGNIZED_MEDIA";
      break;
    case ERROR_NO_TRUST_LSA_SECRET:
      *ppszErrIdent = "ERROR_NO_TRUST_LSA_SECRET";
      break;
    case ERROR_NO_TRUST_SAM_ACCOUNT:
      *ppszErrIdent = "ERROR_NO_TRUST_SAM_ACCOUNT";
      break;
    case ERROR_TRUSTED_DOMAIN_FAILURE:
      *ppszErrIdent = "ERROR_TRUSTED_DOMAIN_FAILURE";
      break;
    case ERROR_TRUSTED_RELATIONSHIP_FAILURE:
      *ppszErrIdent = "ERROR_TRUSTED_RELATIONSHIP_FAILURE";
      break;
    case ERROR_TRUST_FAILURE:
      *ppszErrIdent = "ERROR_TRUST_FAILURE";
      break;
    case RPC_S_CALL_IN_PROGRESS:
      *ppszErrIdent = "RPC_S_CALL_IN_PROGRESS";
      break;
    case ERROR_NETLOGON_NOT_STARTED:
      *ppszErrIdent = "ERROR_NETLOGON_NOT_STARTED";
      break;
    case ERROR_ACCOUNT_EXPIRED:
      *ppszErrIdent = "ERROR_ACCOUNT_EXPIRED";
      break;
    case ERROR_REDIRECTOR_HAS_OPEN_HANDLES:
      *ppszErrIdent = "ERROR_REDIRECTOR_HAS_OPEN_HANDLES";
      break;
    case ERROR_PRINTER_DRIVER_ALREADY_INSTALLED:
      *ppszErrIdent = "ERROR_PRINTER_DRIVER_ALREADY_INSTALLED";
      break;
    case ERROR_UNKNOWN_PORT:
      *ppszErrIdent = "ERROR_UNKNOWN_PORT";
      break;
    case ERROR_UNKNOWN_PRINTER_DRIVER:
      *ppszErrIdent = "ERROR_UNKNOWN_PRINTER_DRIVER";
      break;
    case ERROR_UNKNOWN_PRINTPROCESSOR:
      *ppszErrIdent = "ERROR_UNKNOWN_PRINTPROCESSOR";
      break;
    case ERROR_INVALID_SEPARATOR_FILE:
      *ppszErrIdent = "ERROR_INVALID_SEPARATOR_FILE";
      break;
    case ERROR_INVALID_PRIORITY:
      *ppszErrIdent = "ERROR_INVALID_PRIORITY";
      break;
    case ERROR_INVALID_PRINTER_NAME:
      *ppszErrIdent = "ERROR_INVALID_PRINTER_NAME";
      break;
    case ERROR_PRINTER_ALREADY_EXISTS:
      *ppszErrIdent = "ERROR_PRINTER_ALREADY_EXISTS";
      break;
    case ERROR_INVALID_PRINTER_COMMAND:
      *ppszErrIdent = "ERROR_INVALID_PRINTER_COMMAND";
      break;
    case ERROR_INVALID_DATATYPE:
      *ppszErrIdent = "ERROR_INVALID_DATATYPE";
      break;
    case ERROR_INVALID_ENVIRONMENT:
      *ppszErrIdent = "ERROR_INVALID_ENVIRONMENT";
      break;
    case RPC_S_NO_MORE_BINDINGS:
      *ppszErrIdent = "RPC_S_NO_MORE_BINDINGS";
      break;
    case ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT:
      *ppszErrIdent = "ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT";
      break;
    case ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT:
      *ppszErrIdent = "ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT";
      break;
    case ERROR_NOLOGON_SERVER_TRUST_ACCOUNT:
      *ppszErrIdent = "ERROR_NOLOGON_SERVER_TRUST_ACCOUNT";
      break;
    case ERROR_DOMAIN_TRUST_INCONSISTENT:
      *ppszErrIdent = "ERROR_DOMAIN_TRUST_INCONSISTENT";
      break;
    case ERROR_SERVER_HAS_OPEN_HANDLES:
      *ppszErrIdent = "ERROR_SERVER_HAS_OPEN_HANDLES";
      break;
    case ERROR_RESOURCE_DATA_NOT_FOUND:
      *ppszErrIdent = "ERROR_RESOURCE_DATA_NOT_FOUND";
      break;
    case ERROR_RESOURCE_TYPE_NOT_FOUND:
      *ppszErrIdent = "ERROR_RESOURCE_TYPE_NOT_FOUND";
      break;
    case ERROR_RESOURCE_NAME_NOT_FOUND:
      *ppszErrIdent = "ERROR_RESOURCE_NAME_NOT_FOUND";
      break;
    case ERROR_RESOURCE_LANG_NOT_FOUND:
      *ppszErrIdent = "ERROR_RESOURCE_LANG_NOT_FOUND";
      break;
    case ERROR_NOT_ENOUGH_QUOTA:
      *ppszErrIdent = "ERROR_NOT_ENOUGH_QUOTA";
      break;
    case RPC_S_NO_INTERFACES:
      *ppszErrIdent = "RPC_S_NO_INTERFACES";
      break;
    case RPC_S_CALL_CANCELLED:
      *ppszErrIdent = "RPC_S_CALL_CANCELLED";
      break;
    case RPC_S_BINDING_INCOMPLETE:
      *ppszErrIdent = "RPC_S_BINDING_INCOMPLETE";
      break;
    case RPC_S_COMM_FAILURE:
      *ppszErrIdent = "RPC_S_COMM_FAILURE";
      break;
    case RPC_S_UNSUPPORTED_AUTHN_LEVEL:
      *ppszErrIdent = "RPC_S_UNSUPPORTED_AUTHN_LEVEL";
      break;
    case RPC_S_NO_PRINC_NAME:
      *ppszErrIdent = "RPC_S_NO_PRINC_NAME";
      break;
    case RPC_S_NOT_RPC_ERROR:
      *ppszErrIdent = "RPC_S_NOT_RPC_ERROR";
      break;
    case RPC_S_UUID_LOCAL_ONLY:
      *ppszErrIdent = "RPC_S_UUID_LOCAL_ONLY";
      break;
    case RPC_S_SEC_PKG_ERROR:
      *ppszErrIdent = "RPC_S_SEC_PKG_ERROR";
      break;
    case RPC_S_NOT_CANCELLED:
      *ppszErrIdent = "RPC_S_NOT_CANCELLED";
      break;
    case RPC_X_INVALID_ES_ACTION:
      *ppszErrIdent = "RPC_X_INVALID_ES_ACTION";
      break;
    case RPC_X_WRONG_ES_VERSION:
      *ppszErrIdent = "RPC_X_WRONG_ES_VERSION";
      break;
    case RPC_X_WRONG_STUB_VERSION:
      *ppszErrIdent = "RPC_X_WRONG_STUB_VERSION";
      break;
    case RPC_S_GROUP_MEMBER_NOT_FOUND:
      *ppszErrIdent = "RPC_S_GROUP_MEMBER_NOT_FOUND";
      break;
    case EPT_S_CANT_CREATE:
      *ppszErrIdent = "EPT_S_CANT_CREATE";
      break;
    case RPC_S_INVALID_OBJECT:
      *ppszErrIdent = "RPC_S_INVALID_OBJECT";
      break;
    case ERROR_INVALID_TIME:
      *ppszErrIdent = "ERROR_INVALID_TIME";
      break;
    case ERROR_INVALID_FORM_NAME:
      *ppszErrIdent = "ERROR_INVALID_FORM_NAME";
      break;
    case ERROR_INVALID_FORM_SIZE:
      *ppszErrIdent = "ERROR_INVALID_FORM_SIZE";
      break;
    case ERROR_ALREADY_WAITING:
      *ppszErrIdent = "ERROR_ALREADY_WAITING";
      break;
    case ERROR_PRINTER_DELETED:
      *ppszErrIdent = "ERROR_PRINTER_DELETED";
      break;
    case ERROR_INVALID_PRINTER_STATE:
      *ppszErrIdent = "ERROR_INVALID_PRINTER_STATE";
      break;
    case ERROR_PASSWORD_MUST_CHANGE:
      *ppszErrIdent = "ERROR_PASSWORD_MUST_CHANGE";
      break;
    case ERROR_DOMAIN_CONTROLLER_NOT_FOUND :
      *ppszErrIdent = "ERROR_DOMAIN_CONTROLLER_NOT_FOUND ";
      break;
    case ERROR_ACCOUNT_LOCKED_OUT:
      *ppszErrIdent = "ERROR_ACCOUNT_LOCKED_OUT";
      break;
    case ERROR_INVALID_PIXEL_FORMAT:
      *ppszErrIdent = "ERROR_INVALID_PIXEL_FORMAT";
      break;
    case ERROR_BAD_DRIVER :
      *ppszErrIdent = "ERROR_BAD_DRIVER ";
      break;
    case ERROR_INVALID_WINDOW_STYLE:
      *ppszErrIdent = "ERROR_INVALID_WINDOW_STYLE";
      break;
    case ERROR_METAFILE_NOT_SUPPORTED:
      *ppszErrIdent = "ERROR_METAFILE_NOT_SUPPORTED";
      break;
    case ERROR_TRANSFORM_NOT_SUPPORTED:
      *ppszErrIdent = "ERROR_TRANSFORM_NOT_SUPPORTED";
      break;
    case ERROR_CLIPPING_NOT_SUPPORTED:
      *ppszErrIdent = "ERROR_CLIPPING_NOT_SUPPORTED";
      break;
    case ERROR_NO_NETWORK:
      *ppszErrIdent = "ERROR_NO_NETWORK";
      break;
    case ERROR_BAD_USERNAME:
      *ppszErrIdent = "ERROR_BAD_USERNAME";
      break;
    case ERROR_NOT_CONNECTED:
      *ppszErrIdent = "ERROR_NOT_CONNECTED";
      break;
    case ERROR_OPEN_FILES:
      *ppszErrIdent = "ERROR_OPEN_FILES";
      break;
    case ERROR_DEVICE_IN_USE:
      *ppszErrIdent = "ERROR_DEVICE_IN_USE";
      break;
    case ERROR_UNKNOWN_PRINT_MONITOR:
      *ppszErrIdent = "ERROR_UNKNOWN_PRINT_MONITOR";
      break;
    case ERROR_PRINTER_DRIVER_IN_USE:
      *ppszErrIdent = "ERROR_PRINTER_DRIVER_IN_USE";
      break;
    case ERROR_SPOOL_FILE_NOT_FOUND:
      *ppszErrIdent = "ERROR_SPOOL_FILE_NOT_FOUND";
      break;
    case ERROR_SPL_NO_STARTDOC :
      *ppszErrIdent = "ERROR_SPL_NO_STARTDOC ";
      break;
    case ERROR_SPL_NO_ADDJOB:
      *ppszErrIdent = "ERROR_SPL_NO_ADDJOB";
      break;
    case ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED :
      *ppszErrIdent = "ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED ";
      break;
    case ERROR_PRINT_MONITOR_ALREADY_INSTALLED :
      *ppszErrIdent = "ERROR_PRINT_MONITOR_ALREADY_INSTALLED ";
      break;
    case ERROR_WINS_INTERNAL:
      *ppszErrIdent = "ERROR_WINS_INTERNAL";
      break;
    case ERROR_CAN_NOT_DEL_LOCAL_WINS:
      *ppszErrIdent = "ERROR_CAN_NOT_DEL_LOCAL_WINS";
      break;
    case ERROR_STATIC_INIT:
      *ppszErrIdent = "ERROR_STATIC_INIT";
      break;
    case ERROR_INC_BACKUP:
      *ppszErrIdent = "ERROR_INC_BACKUP";
      break;
    case ERROR_FULL_BACKUP :
      *ppszErrIdent = "ERROR_FULL_BACKUP ";
      break;
    case ERROR_REC_NON_EXISTENT:
      *ppszErrIdent = "ERROR_REC_NON_EXISTENT";
      break;
    case ERROR_RPL_NOT_ALLOWED:
      *ppszErrIdent = "ERROR_RPL_NOT_ALLOWED";
      break;
    case ERROR_NO_BROWSER_SERVERS_FOUND:
      *ppszErrIdent = "ERROR_NO_BROWSER_SERVERS_FOUND";
      break;
    default:
      *ppszErrIdent = "Unknown Identifier";
    }
  }

/*
 * =============================== END ========================================
 */