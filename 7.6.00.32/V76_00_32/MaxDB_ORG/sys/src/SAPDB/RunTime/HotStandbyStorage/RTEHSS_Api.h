/*!
  @file           RTEHSS_Api.h
  @author         JoergM
  @ingroup        Runtime
  @brief          LowLevel HotStandbyStorage API

  This is used inside the runtime coding and for test code only. The kernel interface is found
  in RTEHSS_KernelInterface.hpp

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



#ifndef RTEHSS_API_H
#define RTEHSS_API_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! 
   Limitation of number of characters for sessionId parameter used in RTEHSS_Init call 
 */
#define RTEHSS_IDENTIFIER_MAX_LENGTH 8

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  Enumerates classification of reported messages.

  RTEHSS_ERROR is used for all errors
    (non recoverable errors, like wrong parameter, device not found etc.)
  RTEHSS_WARNING is used for all warnings
    (recoverable error situations, i.e. temporary unavailability of a device...)
  RTEHSS_INFO is used for informational messages
    (i.e. call parameter reports)
  Used in RTEHSS_Init call.
 */
typedef enum RTEHSS_ReportType_ 
{
    RTEHSS_ERROR=0, 
    RTEHSS_WARNING=1, 
    RTEHSS_INFO=2
} RTEHSS_ReportType;

/*!
  Enumerates selection of verbosity. 

  Errors are always reported. Default is set to report warnings
  and configuration changes. Used in RTEHSS_Init and RTEHSS_SetVerbosity call.
  The enumeration values or specified, so that a logical OR operation is allowed!
 */
typedef enum RTEHSS_Verbosity_ 
{
    RTEHSS_VERBOSE_WARNINGS=1,
    RTEHSS_VERBOSE_CONFIGURATION_CHANGES=2,
    RTEHSS_VERBOSE_DEFAULT=3,
    RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT=4, 
    RTEHSS_VERBOSE_DEVELOPER_INFO=8,
    RTEHSS_VERBOSE_DEVELOPER_INFO_VERBOSE=16,
    RTEHSS_VERBOSE_ALL=31
} RTEHSS_Verbosity;

/*!
  Enumerates possible errors codes returned by RTEHSS_GetLastError call
 */
typedef enum RTEHSS_ErrorCode_ {
    RTEHSS_ERR_OK=0,              /* Indicates no error detected */
    RTEHSS_ERR_IN_USE=1,          /* Second initializaztion call rejected */
    RTEHSS_ERR_BAD_HANDLE=2,      /* Bad handle given */
    RTEHSS_ERR_PARAMETER=3,       /* Invalid parameter set */
    RTEHSS_ERR_NOT_READY=4,       /* Storage system not ready to use */
    RTEHSS_ERR_UNKNOWN_DEVICE=5,  /* Device given was is unknown to storage system */
    RTEHSS_ERR_CONFIGURATION=6,   /* Storage system configuration problem */
    RTEHSS_ERR_MEMORY=7,          /* Out of memory inside API call */
    RTEHSS_ERR_BUFFER_EXCEEDED=8, /* RTEHSS_Buffer supplied not sufficient */
    RTEHSS_ERR_UNEXPECTED=9       /* An unexpected internal error occured */ 
} RTEHSS_ErrorCode;

/*!
  Enumerates classification of waiting types

  RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED is used is waiting for mirror established
  RTEHSS_CHECK_FOR_SPLIT_COMPLETED is used if waiting for split completed
  Used in RTEHSS_CheckForSynchronized call
 */
typedef enum RTEHSS_CheckType_ 
{
    RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED=0, 
    RTEHSS_CHECK_FOR_SPLIT_COMPLETED=1
} RTEHSS_CheckType;

/*!
    RTEHSS Common memory handling:
    All RTEHSS_Functions get the needed memory from outside. If a buffer is provided its 
    length is given 'bytesGiven' and the number of bytes used or needed is returned
    'bytesUsedOrNeeded'. If buffer space is not sufficient, the API function will return
    'false' and indicate with setting 'needMore'-Flag to true, that more space is needed.
    The caller can decide whether  to issue the call again with expanded space or not.
 */
typedef struct RTEHSS_Buffer_ {
	void *buffer;                 /*!< pointer to the buffer space */
	SAPDB_Int4 bufferGiven;       /*!< number of bytes in buffer */
	SAPDB_Int4 bytesUsedOrNeeded; /*!< number of bytes used in buffer [in] or needed in buffer [out] */
	SAPDB_Bool needMore;          /*!< flag to indicate more bytes are needed */
} RTEHSS_Buffer;

/*!
   Error report callback routine
 */
typedef void (*RTEHSS_ReportCallback)(RTEHSS_ReportType, SAPDB_Char const *msg);

/*!
   @brief    Initialization call

  The identifier is a zero terminated string which is unique for each database on a
  machine. Normally this is the name of the database instance. Maximum length of 
  identifier is RTEHSS_IDENTIFIER_MAX_LENGTH + 1 (including zero termination)
  The storage returns its own identification, which is exchanged between master and
  standby nodes.
  All output of any call will be produced using the report callback.
   @param          sessionId [in] identifier unique for each database instance on machine
   @param          verbosity [in] result of logical OR operation 
                                  of RTEHSS_Verbosity enumeration values
   @param          reportCallback [in] an optional callback for reporting error,warning or infos
   @param          pHandle [out] pointer to handle
   @return         true if call succeeded, false if not (use RTEHSS_GetLastError
              to get more information).
 */
SAPDB_Bool
RTEHSS_Init(
	SAPDB_Char const *    sessionId,
    SAPDB_Int4            verbosity,
    RTEHSS_ReportCallback reportCallback,
	void **               pHandle );

/*!
   @brief    Deinitialization call
   @param          pHandle [inout] pointer to handle returned form RTEHSS_Init call
   @return         true if call succeeded, false if not (use RTEHSS_GetLastError
              to get more information). If true was returned, handle is invalidated.
 */

SAPDB_Bool
RTEHSS_Deinit( void ** pHandle );

/*!
   @brief    Set the level of verbosity

  Errors are always reported. All other informations are optional.
  The maximum verbosity is set with RTEHSS_VERBOSE_ALL. A logical
  combination of all RTEHSS_Verbosity selections is possible.
  The default verbosity level is
  RTEHSS_VERBOSE_WARNINGS|RTEHSS_VERBOSE_CONFIGURATION_CHANGES
  That means all warnings and configuration changes are reported if a report
  callback was given.
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          newVerbosity [in] logical or of RTEHSS_Verbosity enumeration values
   @param          oldVerbosity [out] if not NULL the former verbosity is returned
   @return         true if call, false if not (use RTEHSS_GetLastError to get more information).
 */

SAPDB_Bool
RTEHSS_SetVerbosity(void * const handle, 
                    SAPDB_Int4   newVerbosity,
                    SAPDB_Int4 * oldVerbosity);

/*!
   @brief    Get the last error reported

  Can be called after a HSSAPI call was called. It will return the error code
  that describes the reason for the last call return false. If the last call
  returned true, the routine will return 'RTEHSS_ERR_OK'.
  If called twice and no other HSSAPI call was issued, the second call will 
  retrieve the same information.
  For detailed error messages, the report callback is used.
   @return         The result of the last HSSAPI call
 */
RTEHSS_ErrorCode
RTEHSS_GetLastError();

/*!
   @brief    Convert last error code into readable string
   @param    errcode [in] the error code to convert
   @return   A string describing the error code
 */
const SAPDB_Char *
RTEHSS_Perror(RTEHSS_ErrorCode errcode);

/*!
   @brief    Used to get the device mapping information needed to establish a group of devices.

  This call is issued once for every device to be mapped.
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          numberOfVolumes [in] number of volumes to map (LOG and DATA are mapped separate)
   @param          volumeArray [in] array of zero terminated paths to volume to be mapped
   @param          mappingBuffer [inout] filled with internal mapping representation
   @return         true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
SAPDB_Bool 
RTEHSS_GetMapping(
	void * const    handle,
    SAPDB_Int4      numberOfVolumes,
	SAPDB_Char   ** volumeArray,
	RTEHSS_Buffer * mappingBuffer);
 
/*!
   @brief    Create a mirror for later data split using the mapping information of master and standby server

  Used by master database server to create the mirror group using the DATA-volume mapping information from 
  master and slave. After the mirror is established, it is splitted immediately to allow both sides to work
  on its DATA-volumes independently.
  Since the master has to wait with the next savepoint for this call to complete, it should complete in a 
  reasonable amount of time. During execution of the RTEHSS_DataSplit() call, no data is written to the data
  volumes neither on master database nor on standby database volumes.

   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          sourceMapping [in] mapping as returned by calls to RTEHSS_GetMapping for DATA-volume array
   @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping for DATA-volume array
   @return         true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
SAPDB_Bool 
RTEHSS_MirrorData (
	void * const	      handle,
	RTEHSS_Buffer const * sourceMapping,
	RTEHSS_Buffer const * targetMapping );

/*!
   @brief    Execute a data split using the mapping information of master and standby server

  Used by master database server to create the mirror group using the DATA-volume mapping 
  information from master and slave. After the mirror is established, it is split immediately
  to allow both sides to work on its DATA-volumes independently.
  Since the master has to wait with the next savepoint for this call to complete, it should 
  complete in a reasonable amount of time. During execution of the RTEHSS_MirrorSplit() call,
  no data is written to the data volumes neither on master database nor on standby database 
  volumes.
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          sourceMapping [in] mapping as returned by calls to RTEHSS_GetMapping for DATA-volume array
   @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping for DATA-volume array
   @return         true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
SAPDB_Bool 
RTEHSS_MirrorSplit (
	void * const	      handle,
	RTEHSS_Buffer const * sourceMapping,
	RTEHSS_Buffer const * targetMapping );

/*!
  @brief    Inform the standby about completion of the data split operation

  Used by standby database server to reenable usage of the mirrored DATA-volumes on the standby
  site. The target mapping information is given. This can be used, i.e. to recreate volume groups
  or mount a filesystem needed to access the DATA-volumes on the standby.

  @param          handle [in] internal handle returned by RTEHSS_Init
  @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping for DATA-volume array
  @return         true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
SAPDB_Bool 
RTEHSS_MirrorSplitDone (
	void * const	      handle,
	RTEHSS_Buffer const * targetMapping );

/*!
   @brief    Find out, if data volumes have reached requested synchronization state

  Synchronization state is reached if either mirror is established (what==RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED)
  or until splitting is completed (what==RTEHSS_CHECK_FOR_SPLIT_COMPLETED). If the state is not yet reached, 
  an informational message is send using the standard report callback (see RTEHSS_Init).
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          what [in] indicate what the routine should be checking
   @param          pDone [out] true if synchronized, false otherwise
   @return         true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
SAPDB_Bool
RTEHSS_CheckIfSynchronized( void * const     handle, 
                            RTEHSS_CheckType what, 
                            SAPDB_Bool *     pDone);

/*!
   @brief    Set the given log volume read only status.

  This is a pure protectional call. It is not absolutely necessary to have the LOG-volumes read only for the
  standby database server. But if the storage system provides such a feature, that only the master has read/write
  access and can prevent the standby database server from write access, it is usefull to call it. Otherwise this
  can be a dummy.
  Both master and standby will issue this call.
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          localMapping [in] mapping as returned by calls to RTEHSS_GetMapping for LOG-volume array
   @param          readOnly [in] if false, set to read/write else set to read only.
   @return         true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
 SAPDB_Bool
RTEHSS_SetLogReadOnlyStatus (
    void * const          handle,
	RTEHSS_Buffer const * localMapping,
	SAPDB_Bool		      readOnly);

#if defined(__cplusplus)
}
#endif

#endif  /* RTEHSS_API_H */
