/*!
  @file           RTEHSS_ApiCopy.c
  @author         JoergM
  @ingroup        Runtime
  @brief          RTEHSS API Dll Implementation using remote copy instead of special storage system

  A mapping simply consists of zero terminated file names.
  The mirror establish call immediatly returns after checking mapping and accessibility.
  The mirror split call does the remote copy call.
  The log device call is a dummy.
  The isSynchronized call is a dummy.
  Restrictions that apply to the RTEHSS_ApiCopy are

  On NT data volumes must have relative pathes 'below' the current directory.
  On NT log volumes must have absolute pathes.

  For all implementations the current directory must be the rundirectory of the database.

  For NT a share with naming convertion 'HSS_id' must be creatable. 'id' should be the database
  instance identifier.

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo60.h"
#include "geo007_1.h"

#if defined(WIN32)
#define WINNT 1
#define UNICODE 1
#define _UNICODE 1
#include <windows.h>
#include <lmshare.h>
#include <lm.h>
#else
#include "RunTime/RTE_IniFileHandling.h" /* nocheck */
#include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#endif /* WIN32 */

#include "RunTime/HotStandbyStorage/RTEHSS_Api.h"
#include "RunTime/RTE_Types.h"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/System/RTESys_IO.h"
#include "RunTime/System/RTESys_CommandPipe.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_string.h"

#include <stdio.h>


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*!
  @brief Copy buffer size
 */
#define RTEHSS_COPY_VOLUME_BUFFER_SIZE (64*1024)


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/*!
   The maximum message length for diagnostic messages
 */
#define RTEHSS_MAX_MESSAGE_LENGTH 4096

/*!
   The local Dll state structure
 */
static struct HSS_ApiCopy
{
	SAPDB_Int4        inUse;                                       /*!< DLL is in use */
	RTEHSS_ErrorCode  lastError;                                   /*!< last error occured using DLL */
	SAPDB_Int4        verbosity;                                   /*!< current verbosity level */
    void            (*reportCallback)(RTEHSS_ReportType type,
                                      const SAPDB_Char *errorMsg); /*!< current diagnostic report callback */
	SAPDB_Char        msgBuffer[RTEHSS_MAX_MESSAGE_LENGTH+1];      /*!< a static message buffer */
	SAPDB_Char        id[RTEHSS_IDENTIFIER_MAX_LENGTH+1];          /*!< copy of the identifier given in init call */
    const SAPDB_Char *currentFunction;                             /*!< currently called function */
#if !defined(WIN32)
    RTE_Path          commonConfigPath;                            /*!< Common configuration directory indepData/config */
    SAPDB_Int4        commonConfigPathLength;                      /*!< Length of common configuration directory path indepData/config */
#endif /* !WIN32 */
    RTE_Path          workingDirectory;                            /*!< Working directory while kernel is running */
} ApiCopy = { 0 };

/*!
  Dummy handle used to return that api is already in use
 */
static SAPDB_Int4 ApiCopyInUse = 1;

/*!
  The internal structure of the mapping information

  The mapping consists of this header, followed by the hostname and the sharename and the individual volume names
 */
typedef struct CopyMappingHeader_
{
    SAPDB_Int2 totalSize;        /*!< total size in bytes of mapping including this header */
    SAPDB_Int2 numberOfEntries;  /*!< number of volume entries in mapping */
} CopyMappingHeader;

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/
/*!
   @brief        - DLL Local - report callback for any type of message
   @param        type [in] the report type
   @param        msgFormat [in] the 'printf' like msgFormat followed by its arguments
 */
static void ReportCallback(
    RTEHSS_ReportType type,
    SAPDB_Char *msgFormat,
    ...);

/*!
   @brief        - DLL Local - report callback for error message
   @param        msgFormat [in] the 'printf' like msgFormat followed by its arguments
 */
static void ReportError(
    SAPDB_Char *msgFormat,
    ...);

/*!
   @brief        - DLL Local - setting of last occured error
   @param        lastError [in] the last error that occured
   @return       always false
 */
static SAPDB_Bool LocalSetLastError(
    RTEHSS_ErrorCode lastError );

/*!
   @brief        - DLL Local - Add mapping to mapping buffer
   @param        mappingBuffer [in] the target mapping buffer
   @param        name          [in] the name to add
 */
static void AddToMappingBuffer(
    RTEHSS_Buffer *mappingBuffer,
    SAPDB_Char const *name
);

/*!
   @brief        - DLL Local - Verify given mappings
   @param        sourceMapping  [in] identifier of the source mapping information
   @param        targetMapping  [in] identifier of the target mapping information
   @param        pSourceHostname  [out] source hostname
   @param        pTargetHostname  [out] target hostname
   @param        pSourceFirstVolume  [out] source first volume
   @param        pTargetFirstVolume  [out] target first volume
   @param        pNumberOfMappingEntries [out] number of mapping entries
   @return       true on success, false on failure
 */
static SAPDB_Bool
VerifyMapping(RTEHSS_Buffer const * sourceMapping,
              RTEHSS_Buffer const * targetMapping,
              SAPDB_Char const * *pSourceHostname,
              SAPDB_Char const * *pTargetHostname,
              SAPDB_Char const * *pSourceFirstVolume,
              SAPDB_Char const * *pTargetFirstVolume,
              SAPDB_Int4       * pNumberOfMappingEntries );

/*!
   @brief        - DLL Local - Check given mapping
   @param        what           [in] identifier of the mapping information
   @param        mappingBuffer  [in] the given mapping buffer
   @param        hostname       [out] pointer to the hostname
   @param        firstVolume    [out] pointer to the first volume
   @return       true on success, false on failure
 */
static CopyMappingHeader const *
CheckMapping(const char *what, 
             RTEHSS_Buffer const * mappingBuffer,
             SAPDB_Char const * *hostname,
             SAPDB_Char const * *firstVolume );

/*!
   @brief        - DLL Local - Build remote path
   @param        hostname       [in] pointer to the hostname
   @param        volume         [in] pointer to the volume or NULL if only share name wanted
   @param        remotePath     [out] filled with remote path
   @return       true on success, false on failure
 */
static SAPDB_Bool
BuildRemoteAccessPath( SAPDB_Char const * hostname,
                       SAPDB_Char const * volume,
                       RTE_Path           remotePath);

#if defined(WIN32)
/*!
   @brief        - DLL Local - WIN32 only - Build share name for HSS shared rundirectory
   @param        shareName     [out] filled with share name
   @return       true on success, false on failure
 */
static SAPDB_Bool
BuildShareName(RTE_Path shareName);
#endif /* defined(WIN32) */

/*!
   @brief        - DLL Local - Check remote path
   @param        remotePath     [in] filled with remote path
   @param        readOnly       [in] flag indicating check for readonly access
   @return       true on success, false on failure
 */
static SAPDB_Bool
CheckRemoteAccess( RTE_Path   remotePath,
                   SAPDB_Bool readOnly );

/*!
   @brief        - DLL Local - Do the volume copy
   @param        sourceRemotePath     [in] copy source path
   @param        targetRemotePath     [in] copy target path
   @return       true on success, false on failure
 */
static SAPDB_Bool
DoVolumeCopy(RTE_Path sourceRemotePath,
             RTE_Path targetRemotePath);

/*!
   @brief        - DLL Local - WIN32 only - Validate/Create the volume share
   @param        volumePath [in] volumePath to validate
   @return       true on success, false on failure
 */
static SAPDB_Bool
ValidateOrCreateShare(SAPDB_Char const * volumePath);

/*!
  @brief         build an error message containing the given routine and file name and write it using MSGD()

 */
static void ShowLastOsError( const SAPDB_Char *    Routine,
                             const SAPDB_Char *    Filename );

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
   @brief          Initialization call

  Initialize the storage access API using the given session identification.
  The storage returns its onw identification, which is exchanged between master and standby
  nodes.
   @param          databaseId [in] database instance unique identification of session
   @param          verbosity [in] logical or of RTEHSS_Verbosity enumeration values
   @param          reportCallback [in] a function to report single error message lines
   @param          pHandle [out] pointer to handle
   @return   true if call succeeded, false if not (use RTEHSS_GetLastError
              to get more information).
 */
SAPDB_Bool
RTEHSS_Init(
	SAPDB_Char const * databaseId,
    SAPDB_Int4         verbosity,
    void (*reportCallback)(RTEHSS_ReportType type, SAPDB_Char const *msg),
	void **            pHandle )
{
    tsp00_VfReturn_Param returnValue;

    if ( 0 != ApiCopy.inUse )
	{
        ApiCopy.lastError = RTEHSS_ERR_IN_USE;
        *pHandle = (void *)&ApiCopyInUse;
		return false;
	}
	ApiCopy.inUse = 1;
    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_Init";
    ApiCopy.reportCallback = reportCallback;
	ApiCopy.lastError = RTEHSS_ERR_OK;
    ApiCopy.verbosity = verbosity;
	ApiCopy.msgBuffer[0] = 0;

#if !defined(WIN32)
    {
        tsp00_ErrTextc errText;
        if ( !RTE_GetCommonConfigPath ( ApiCopy.commonConfigPath,
                                        true,
                                        errText ) )
        {
            if ( reportCallback ) 
                ReportCallback( RTEHSS_ERROR, "Failed to get independend config directory:%s", errText);
            return false;
        }
        ApiCopy.commonConfigPathLength = strlen(ApiCopy.commonConfigPath);
    }
#endif

    RTESys_IOGetWorkingDirectory(ApiCopy.workingDirectory, &returnValue);
    if ( vf_ok != returnValue )
    {
        if ( reportCallback ) 
            ReportCallback( RTEHSS_ERROR, "Failed to get current working directory");
        return false;
    }

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        if ( reportCallback ) 
            ReportCallback( RTEHSS_INFO, "Given databaseId %s", databaseId );
        if ( reportCallback ) 
            ReportCallback( RTEHSS_INFO, "Given verbosity %d", verbosity );
        if ( reportCallback ) 
            ReportCallback( RTEHSS_INFO, "Given reportCallback (in use...)");
    }

    if ( !databaseId )
	{
		ApiCopy.id[0] = 0;
	}
	else
	{
		SAPDB_memset(ApiCopy.id, 0, sizeof(ApiCopy.id));
        if ( strlen(databaseId) > RTEHSS_IDENTIFIER_MAX_LENGTH )
        {
            ReportCallback( RTEHSS_WARNING, "Session identifier '%s' truncated", databaseId);
        }
		strncpy(ApiCopy.id, databaseId, RTEHSS_IDENTIFIER_MAX_LENGTH);
	}

	*pHandle = (void *)&ApiCopy;

	return true;
}

/*!
   @brief    Deinitialization call
   @param          pHandle [inout] pointer to handle returned form RTEHSS_Init call
   @return         true if call succeeded, false if not (use RTEHSS_GetLastError
              to get more information). If true was returned handle is invalidated
 */
SAPDB_Bool
RTEHSS_Deinit( void ** pHandle )
{
	if ( !pHandle || ((*pHandle) != (void *)&ApiCopy) )
	{
        ReportError("RTEHSS_Deinit called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_Deinit";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
    }

    *pHandle = 0;
    return true;
}

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
   @return   true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
SAPDB_Bool
RTEHSS_SetVerbosity(void * const handle, 
                    SAPDB_Int4   newVerbosity,
                    SAPDB_Int4 * oldVerbosity)
{
	if ( handle != (void * const)&ApiCopy )
	{
        ReportError("RTEHSS_SetVerbosity called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_SetVerbosity";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
        ReportCallback( RTEHSS_INFO, "Got new verbosity %d", newVerbosity);
    }

    if ( oldVerbosity ) *oldVerbosity = ApiCopy.verbosity;
    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "%s old verbosity %d", 
            oldVerbosity ? "Returned" : "Would have returned", ApiCopy.verbosity);
    }
    ApiCopy.verbosity = newVerbosity;
    return true;
}

/*!
   @brief    Get the last error reported

  Can be called after an HSSAPI returned false. It will return the error code
  that describes the reason for the last call returng false. If the last call
  returned true, the routine will return 'RTEHSS_ERR_OK'.
  If called twice, the second call will retrieve the same information.
  For detailed error messages, the report callback is used.

   @return   The result of the last RTEHSS_API call
 */
RTEHSS_ErrorCode 
RTEHSS_GetLastError()
{
	return ApiCopy.lastError;
}

/*!
   @brief    Convert last error code into readable string
   @param    errCode [in] the error code to convert
   @return   A string describing the error code
 */
const SAPDB_Char *
RTEHSS_Perror(RTEHSS_ErrorCode errCode)
{
    switch(errCode)
    {
    case RTEHSS_ERR_OK:
        return "No error detected";
    case RTEHSS_ERR_IN_USE:
        return "Second initialization call rejected";
    case RTEHSS_ERR_BAD_HANDLE:
        return "Bad handle given";
    case RTEHSS_ERR_PARAMETER:
        return "Invalid parameter set";
    case RTEHSS_ERR_NOT_READY:
        return "Storage system not ready to use";
    case RTEHSS_ERR_UNKNOWN_DEVICE:
        return "Device given was is unknown to storage system";
    case RTEHSS_ERR_CONFIGURATION:
        return "Storage system configuration problem";
    case RTEHSS_ERR_MEMORY:
        return "Out of memory inside API call";
    case RTEHSS_ERR_BUFFER_EXCEEDED:
        return "RTEHSS_Buffer supplied not sufficient";
    case RTEHSS_ERR_UNEXPECTED:
        return "An unexpected internal error occured";
    default:
        break;
    }
    return "unknown error code";
}

/*!
   @brief    Used to get the device mapping information needed to establish
a group of devices.

  This call is almost identical in behaviour with RTEHSS_GetMapping, but allows to fake 
  a target mapping using the target hostname as parameter.
  Not a common interface for RTEHSS_Api.h, but only locally defined in test programs testing RTEHSS_ApiCopy.c

   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          numberOfVolumes [in] number of volumes to map
   @param          targetHostname [in] host which should be source of mapping
   @param          volumeArray [in] array of zero terminated pathes to volumes to be mapped
   @param          mappingBuffer [inout] filled with internal mapping representation
   @return   true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
SAPDB_Bool 
RTEHSS_GetTargetMapping(
	void * const	    handle,
    SAPDB_Int4          numberOfVolumes,
	SAPDB_Char const  * targetHostname,
	SAPDB_Char       ** volumeArray,
	RTEHSS_Buffer     * mappingBuffer)
{
    SAPDB_Int4 iVolume;
    SAPDB_Int4 mappingLength;

	if ( handle != (const void *)&ApiCopy )
	{
        ReportError("RTEHSS_GetFakeMapping called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_GetFakeMapping";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
        ReportCallback( RTEHSS_INFO, "Got %d volumes", numberOfVolumes);
    }

    if ( numberOfVolumes <= 0 )
    {
        ReportError("Got invalid number of volumes:%d", numberOfVolumes);
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( !targetHostname )
    {
        ReportError("Got NULL mapping hostname");
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( !volumeArray )
    {
        ReportError("Got NULL volume array");
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( !mappingBuffer )
    {
        ReportError("Got NULL mapping buffer");
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    mappingLength = (SAPDB_Int4)(sizeof(CopyMappingHeader) + strlen(targetHostname) + 1);

    for ( iVolume = 0; iVolume < numberOfVolumes; iVolume++ )
    {
        SAPDB_Int4 volumeNameLength;
        
        if ( !volumeArray[iVolume] )
        {
            ReportError("Got NULL pointer for device entry %d", iVolume );
		    return LocalSetLastError(RTEHSS_ERR_PARAMETER);
        }

        volumeNameLength = (SAPDB_Int4)strlen(volumeArray[iVolume]);

        if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
        {
           ReportCallback( RTEHSS_INFO, "Volume[%d] '%s'", iVolume, volumeArray[iVolume] );
        }

        if ( !ValidateOrCreateShare(volumeArray[iVolume]) )
        {
            ReportError("Failed to create or validate share for volume %s entry %d ", 
                volumeArray[iVolume], iVolume );
		    return LocalSetLastError(RTEHSS_ERR_PARAMETER);
        }

        mappingLength += (volumeNameLength + 1);
    }

    mappingBuffer->bytesUsedOrNeeded = mappingLength;

    if ( !mappingBuffer->buffer )
    {
        ReportError("Got no mapping buffer space need space %d bytes",
            mappingLength );
		return LocalSetLastError(RTEHSS_ERR_BUFFER_EXCEEDED);
    }

    if ( mappingBuffer->bufferGiven < mappingLength )
    {
        ReportError("Got mapping buffer space length %d need %d",
            mappingBuffer->bufferGiven, mappingLength );
		return LocalSetLastError(RTEHSS_ERR_BUFFER_EXCEEDED);
    }

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Given mapping buffer space for %d bytes, needed %d bytes",
            mappingBuffer->bufferGiven,
            mappingLength );
    }

    SAPDB_memset(mappingBuffer->buffer, 0, sizeof(CopyMappingHeader)); /* zero filled header */
    ((CopyMappingHeader *)mappingBuffer->buffer)->totalSize = sizeof(CopyMappingHeader);
    ((CopyMappingHeader *)mappingBuffer->buffer)->numberOfEntries = 0;
    {
        AddToMappingBuffer(mappingBuffer, targetHostname);
        for ( iVolume = 0; iVolume < numberOfVolumes; iVolume++ )
        {
            AddToMappingBuffer(mappingBuffer, volumeArray[iVolume]);
        }
    }

    return true;
}
 
/*!
   @brief    Used to get the device mapping information needed to establish
a group of devices.


  This call is issued once for every device array to be mapped.
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          numberOfVolumes [in] number of volumes to map
   @param          volumeArray [in] array of zero terminated pathes to volumes to be mapped
   @param          mappingBuffer [inout] filled with internal mapping representation
   @return   true if call, false if not (use RTEHSS_GetLastError to get more information).

 */
SAPDB_Bool 
RTEHSS_GetMapping(
	void * const	    handle,
    SAPDB_Int4          numberOfVolumes,
	SAPDB_Char       ** volumeArray,
	RTEHSS_Buffer     * mappingBuffer)
{
    SAPDB_Int4 iVolume;
    SAPDB_Int4 mappingLength;
    SAPDB_Char const *localHostname;

	if ( handle != (const void *)&ApiCopy )
	{
        ReportError("RTEHSS_GetMapping called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_GetMapping";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
        ReportCallback( RTEHSS_INFO, "Got %d volumes", numberOfVolumes);
    }

    if ( numberOfVolumes <= 0 )
    {
        ReportError("Got invalid number of volumes:%d", numberOfVolumes);
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( !volumeArray )
    {
        ReportError("Got NULL volume array");
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( !mappingBuffer )
    {
        ReportError("Got NULL mapping buffer");
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    localHostname = RTESys_Hostname();

    mappingLength = (SAPDB_Int4)(sizeof(CopyMappingHeader) + strlen(localHostname) + 1);

    for ( iVolume = 0; iVolume < numberOfVolumes; iVolume++ )
    {
        SAPDB_Int4 volumeNameLength;
        
        if ( !volumeArray[iVolume] )
        {
            ReportError("Got NULL pointer for device entry %d", iVolume );
		    return LocalSetLastError(RTEHSS_ERR_PARAMETER);
        }

        volumeNameLength = (SAPDB_Int4)strlen(volumeArray[iVolume]);

        if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
        {
           ReportCallback( RTEHSS_INFO, "Volume[%d] '%s'", iVolume, volumeArray[iVolume] );
        }

        if ( !ValidateOrCreateShare(volumeArray[iVolume]) )
        {
            ReportError("Failed to create or validate share for volume %s entry %d ", 
                volumeArray[iVolume], iVolume );
		    return LocalSetLastError(RTEHSS_ERR_PARAMETER);
        }

        mappingLength += (volumeNameLength + 1);
    }

    mappingBuffer->bytesUsedOrNeeded = mappingLength;

    if ( !mappingBuffer->buffer )
    {
        ReportError("Got no mapping buffer space need space %d bytes",
            mappingLength );
		return LocalSetLastError(RTEHSS_ERR_BUFFER_EXCEEDED);
    }

    if ( mappingBuffer->bufferGiven < mappingLength )
    {
        ReportError("Got mapping buffer space length %d need %d",
            mappingBuffer->bufferGiven, mappingLength );
		return LocalSetLastError(RTEHSS_ERR_BUFFER_EXCEEDED);
    }

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Given mapping buffer space for %d bytes, needed %d bytes",
            mappingBuffer->bufferGiven,
            mappingLength );
    }

    SAPDB_memset(mappingBuffer->buffer, 0, sizeof(CopyMappingHeader)); /* zero filled header */
    ((CopyMappingHeader *)mappingBuffer->buffer)->totalSize = sizeof(CopyMappingHeader);
    ((CopyMappingHeader *)mappingBuffer->buffer)->numberOfEntries = 0;

    AddToMappingBuffer(mappingBuffer, localHostname);
    for ( iVolume = 0; iVolume < numberOfVolumes; iVolume++ )
    {
        AddToMappingBuffer(mappingBuffer, volumeArray[iVolume]);
    }

    return true;
}

/*!
   @brief    Establish a mirror

  Used by a database server to create the mirror group using the DATA-volume mapping 
  information from master and slave. After the mirror is established, it is splitted immediately
  to allow both sides to work on its DATA-volumes independendly.
  Since the master has to wait with the next savepoint for this call to complete, it should
  complete it a reasonable amount of time. During execution of the RTEHSS_DataSplit() call,
  no data is written to the data volumes neither on master database nor on backup database volumes.

   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          sourceMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @return         false if handle is valid, true in all other cases (no access check...)
 */
SAPDB_Bool 
RTEHSS_MirrorData (
	void * const	      handle,
	RTEHSS_Buffer const * sourceMapping,
	RTEHSS_Buffer const * targetMapping )
{
    SAPDB_Char const * sourceHostname;
    SAPDB_Char const * targetHostname;
    SAPDB_Char const * sourceFirstVolume;
    SAPDB_Char const * targetFirstVolume;
    SAPDB_Int4         numberOfMappingEntries;

	if ( handle != (const void *)&ApiCopy )
	{
        ReportError("RTEHSS_MirrorData called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_MirrorData";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
    }

    if ( !VerifyMapping( sourceMapping,
                         targetMapping,
                         &sourceHostname,
                         &targetHostname,
                         &sourceFirstVolume,
                         &targetFirstVolume,
                         &numberOfMappingEntries ) )
    {
        ReportError("RTEHSS_MirrorData called with bad mapping information");
		return false;
    }
    else
    {
        if ( 0 != (ApiCopy.verbosity & RTEHSS_VERBOSE_DEVELOPER_INFO) )
        {
            ReportCallback( RTEHSS_INFO, "source host %s first volume %s", sourceHostname, sourceFirstVolume );
            ReportCallback( RTEHSS_INFO, "target host %s first volume %s", targetHostname, targetFirstVolume );
        }
    }

    {
        SAPDB_Int4        iVolume;
        SAPDB_Char const *checkVolume;
        RTE_Path          remotePath;
        
        for ( checkVolume = sourceFirstVolume, iVolume = 0;
              iVolume < numberOfMappingEntries;
              ++iVolume, checkVolume += strlen(checkVolume) + 1 )
        {
            if ( !BuildRemoteAccessPath(sourceHostname, checkVolume, remotePath) )
            {
                ReportError("RTEHSS_MirrorData failed to build remote path for source host %s volume %s",
                    sourceHostname, checkVolume);
		        return false;
            }

            if ( !CheckRemoteAccess(remotePath, true) )
            {
                ReportError("RTEHSS_MirrorData failed to get read only access to remote path %s",
                    remotePath );
		        return false;
            }
        }

        for ( checkVolume = targetFirstVolume, iVolume = 0;
              iVolume < numberOfMappingEntries;
              ++iVolume, checkVolume += strlen(checkVolume) + 1 )
        {
            if ( !BuildRemoteAccessPath(targetHostname, checkVolume, remotePath) )
            {
                ReportError("RTEHSS_MirrorData failed to build remote path for target host %s volume %s",
                    targetHostname, checkVolume);
		        return false;
            }

            if ( !CheckRemoteAccess(remotePath, false) )
            {
                ReportError("RTEHSS_MirrorData failed to get read and write access to remote path %s",
                    remotePath );
		        return false;
            }
        }
    }

    return true;
}

/*!
   @brief    Perform a data split using the mapping information of master and backup server

  Used by a database server to create the mirror group using the DATA-volume mapping 
  information from master and slave. After the mirror is established, it is splitted immediately
  to allow both sides to work on its DATA-volumes independendly.
  Since the master has to wait with the next savepoint for this call to complete, it should
  complete it a reasonable amount of time. During execution of the RTEHSS_DataSplit() call,
  no data is written to the data volumes neither on master database nor on backup database volumes.

   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          sourceMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @return   true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
 
SAPDB_Bool 
RTEHSS_MirrorSplit (
	void * const	      handle,
	RTEHSS_Buffer const * sourceMapping,
	RTEHSS_Buffer const * targetMapping )
{
    SAPDB_Char const * sourceHostname;
    SAPDB_Char const * targetHostname;
    SAPDB_Char const * sourceFirstVolume;
    SAPDB_Char const * targetFirstVolume;
    SAPDB_Int4         numberOfMappingEntries;

	if ( handle != (const void *)&ApiCopy )
	{
        ReportError("RTEHSS_MirrorSplit called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_MirrorSplit";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
    }

    if ( !VerifyMapping( sourceMapping,
                         targetMapping,
                         &sourceHostname,
                         &targetHostname,
                         &sourceFirstVolume,
                         &targetFirstVolume,
                         &numberOfMappingEntries ) )
    {
        ReportError("RTEHSS_MirrorSplit called with bad mapping information");
		return false;
    }

    {
        SAPDB_Int4        iVolume;
        SAPDB_Char const *checkSourceVolume;
        SAPDB_Char const *checkTargetVolume;
        RTE_Path          sourceRemotePath;
        RTE_Path          targetRemotePath;
        
        for ( checkSourceVolume = sourceFirstVolume, iVolume = 0;
              iVolume < numberOfMappingEntries;
              ++iVolume, checkSourceVolume += strlen(checkSourceVolume) + 1 )
        {
            if ( !BuildRemoteAccessPath(sourceHostname, checkSourceVolume, sourceRemotePath) )
            {
                ReportError("RTEHSS_MirrorSplit failed to build remote path for source host %s volume %s",
                    sourceHostname, checkSourceVolume);
		        return false;
            }

            if ( !CheckRemoteAccess(sourceRemotePath, true) )
            {
                ReportError("RTEHSS_MirrorSplit failed to get read only access to remote path %s",
                    sourceRemotePath );
		        return false;
            }
        }

        for ( checkTargetVolume = targetFirstVolume, iVolume = 0;
              iVolume < numberOfMappingEntries;
              ++iVolume, checkTargetVolume += strlen(checkTargetVolume) + 1 )
        {
            if ( !BuildRemoteAccessPath(targetHostname, checkTargetVolume, targetRemotePath) )
            {
                ReportError("RTEHSS_MirrorSplit failed to build remote path for target host %s volume %s",
                    targetHostname, checkTargetVolume);
		        return false;
            }

            if ( !CheckRemoteAccess(targetRemotePath, false) )
            {
                ReportError("RTEHSS_MirrorSplit failed to get read and write access to remote path %s",
                    targetRemotePath );
		        return false;
            }
        }

        for ( checkSourceVolume = sourceFirstVolume, checkTargetVolume = targetFirstVolume, iVolume = 0;
              iVolume < numberOfMappingEntries;
              ++iVolume, checkSourceVolume += strlen(checkSourceVolume) + 1, checkTargetVolume += strlen(checkTargetVolume) + 1 )
        {
            if ( !BuildRemoteAccessPath(sourceHostname, checkSourceVolume, sourceRemotePath) )
            {
                ReportError("RTEHSS_MirrorSplit failed in 2nd build remote path for source host %s volume %s",
                    sourceHostname, checkSourceVolume);
		        return false;
            }

            if ( !BuildRemoteAccessPath(targetHostname, checkTargetVolume, targetRemotePath) )
            {
                ReportError("RTEHSS_MirrorSplit failed in 2nd build remote path for target host %s volume %s",
                    targetHostname, checkTargetVolume);
		        return false;
            }

            if ( !DoVolumeCopy(sourceRemotePath, targetRemotePath) )
            {
                ReportError("RTEHSS_MirrorSplit volume copy failed %s -> %s",
                    sourceRemotePath, targetRemotePath);
		        return false;
            }
        }
    }

    return true;
}

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
	RTEHSS_Buffer const * targetMapping )
{
    return true;
}

/*!
   @brief    Find out, if data volumes have reached requested synchronization state

  Synchronization state is reached if either mirror is established (what==RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED)
  or until splitting is completed (what==RTEHSS_CHECK_FOR_SPLIT_COMPLETED). If the state is not yet reached, 
  an informational message is send using the standard report callback (see RTEHSS_Init).
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          what [in] indicate what the routine should be checking
   @param          pDone [out] true if synchronized, false otherwise
   @return   true if call, false if not (use RTEHSS_GetLastError to get more information).
 */
 
SAPDB_Bool
RTEHSS_CheckIfSynchronized( void * const     handle, 
                            RTEHSS_CheckType what, 
                            SAPDB_Bool *     pDone)
{
	if ( handle != (const void *)&ApiCopy )
	{
        ReportError("RTEHSS_CheckIfSynchronized called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_CheckIfSynchronized";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
        ReportCallback( RTEHSS_INFO, "Check type %s", 
            what == RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED ? "RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED" :
               what == RTEHSS_CHECK_FOR_SPLIT_COMPLETED ? "RTEHSS_CHECK_FOR_SPLIT_COMPLETED" : "???" );
    }

    *pDone = true;

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Report synchronize state: %s", *pDone ? "done" : "busy");
    }

    return true;
}

/*!
   @brief    Set the given log volume read only status.

  This is a pure protectional call. It is not absolutely necessary to have the LOG-volumes readonly 
  for the backup database server. But if the storage system provides such a feature, that only the 
  master has read/write access and can prevent the backup database server from write access, it is
  usefull to call it. Otherwise this can be a dummy.
  Input to this call is the mapping information for each LOG volume device used.
  If i.e. LOG volume consists of 2 devices, numberOfMappings is 2.
  For EMC the action consists of protecting all ports and after reopening access to
  the port owned by this maschine.
  Both master and backup will issue this call.
   @param          handle [in] internal handle returned by RTEHSS_Init
   @param          localMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          readOnly [in] if false, set to read/write else set to read only.
   @return   true if call, false if not (use RTEHSS_GetLastError to get more information).

 */
 
SAPDB_Bool
RTEHSS_SetLogReadOnlyStatus (
    void * const          handle,
	RTEHSS_Buffer const * localMapping,
	SAPDB_Bool	          readOnly)
{
	if ( handle != (const void *)&ApiCopy )
	{
        ReportError("RTEHSS_SetLogReadOnlyStatus called with bad API handle");
		return LocalSetLastError(RTEHSS_ERR_BAD_HANDLE);
	}

    ApiCopy.currentFunction = "RTEHSS_API(COPY):RTEHSS_SetLogReadOnlyStatus";

    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
    {
        ReportCallback( RTEHSS_INFO, "Got valid handle");
        if ( readOnly )
        {
            ReportCallback( RTEHSS_INFO, "Would set log access to read only");
        }
        else
        {
            ReportCallback( RTEHSS_INFO, "Would set log access to read and write");
        }
    }

    return true;
}

/*===========================================================================*
 *  Local CODE                                                               *
 *===========================================================================*/

/*-------------------------------------------------------
  Function: ReportCallback
  Description: show typed message using the specified report callback
  Arguments: type [in] RTEHSS_INFO,RTEHSS_WARNING or RTEHSS_ERROR
             msgFormat  [in] zero terminated message string
 */
static void ReportCallback( RTEHSS_ReportType type,
                            SAPDB_Char *msgFormat,
                            ... )
{
    if ( ApiCopy.reportCallback )
    {
        va_list args ;
          if ( ApiCopy.currentFunction )
          {
            (*ApiCopy.reportCallback)(type, ApiCopy.currentFunction);
            ApiCopy.currentFunction = 0;
          }
          va_start ( args, msgFormat ) ;
          sp77vsprintf ( ApiCopy.msgBuffer, RTEHSS_MAX_MESSAGE_LENGTH, msgFormat, args );
        va_end ( args ) ;

        (*ApiCopy.reportCallback)(type, ApiCopy.msgBuffer);
    }
}

/*-------------------------------------------------------
  Function: ReportError
  Description: show error message using the specified report callback
  Arguments: msgFormat  [in] zero terminated message string followed by arguments
 */
static void ReportError( SAPDB_Char *msgFormat,
                         ...)
{
    if ( ApiCopy.reportCallback )
    {
        va_list args ;
        if ( ApiCopy.currentFunction )
        {
            (*ApiCopy.reportCallback)(RTEHSS_ERROR, ApiCopy.currentFunction);
            ApiCopy.currentFunction = 0;
        }
        va_start ( args, msgFormat ) ;
        sp77vsprintf ( ApiCopy.msgBuffer, RTEHSS_MAX_MESSAGE_LENGTH, msgFormat, args ) ;
        va_end ( args ) ;

        (*ApiCopy.reportCallback)(RTEHSS_ERROR, ApiCopy.msgBuffer);
    }
}

/*-------------------------------------------------------
  Function: LocalSetLastError
  Description: Copy last error into internal state structure
  Arguments: errCode [in] Error code
  Return value: always false
 */
static SAPDB_Bool LocalSetLastError( RTEHSS_ErrorCode errCode )
{
	ApiCopy.lastError = errCode;
    return false;
}

/*-------------------------------------------------------
  Function: AddToMappingBuffer
  Description: Copy the mapping to given mapping buffer

  Arguments: mappingBuffer   [inout]  the buffer to fill with the mapping information
             name            [in]     the volume name or hostname
 */
static void AddToMappingBuffer(
  RTEHSS_Buffer *mappingBuffer,
  SAPDB_Char const *name
)
{
	CopyMappingHeader *pMapping = (CopyMappingHeader *)mappingBuffer->buffer;
	SAPDB_Int4      spaceNeeded = (SAPDB_Int4)strlen(name) + 1;

    SAPDB_memcpy( ((SAPDB_Char *)mappingBuffer->buffer) + pMapping->totalSize, 
            name, 
            spaceNeeded);
    pMapping->totalSize += spaceNeeded;

    ++(pMapping->numberOfEntries);
}

/*-------------------------------------------------------
  Function: CheckMapping
  Description: Check the given mapping buffer for consistency

  Arguments: what          [in]  identification of mapping buffer
             mappingBuffer [in]  the mapping buffer
             hostname      [out] pointer to the hostname
             firstVolume   [out] pointer to the first volume

  Return value: pointer to CopyMappingHeader or 0 if invalid mapping
 */
static CopyMappingHeader const *
CheckMapping(const char *what, 
             RTEHSS_Buffer const * mappingBuffer,
             SAPDB_Char const * *hostname,
             SAPDB_Char const * *firstVolume )
{
    CopyMappingHeader const * pMapping = 0;
    if ( !mappingBuffer )
    {
        ReportError("CheckMapping(%s) got NULL mapping", what);
		LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }
    else if ( !mappingBuffer->buffer )
    {
        ReportError("CheckMapping(%s) got NULL mapping buffer space", what);
		LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }
    else 
    {
        pMapping = (CopyMappingHeader const *)mappingBuffer->buffer;

        if ( pMapping->numberOfEntries <= 0 )
        {
            ReportError("CheckMapping(%s) got bad number of mapping entries:%d", what, pMapping->numberOfEntries );
		    LocalSetLastError(RTEHSS_ERR_PARAMETER);
            pMapping = 0;
        }
        else
        {
            int mappingIndex;
            SAPDB_Char *pEntry;
            SAPDB_Char *pNextEntry;

            if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
            {
                ReportCallback( RTEHSS_INFO, "Got %s map total %d bytes %d entries",
                    what, pMapping->totalSize, pMapping->numberOfEntries );
            }

            if ( mappingBuffer->bufferGiven < pMapping->totalSize )
            {
                ReportError("CheckMapping(%s) got incomplete mapping", what);
		        LocalSetLastError(RTEHSS_ERR_PARAMETER);
                pMapping = 0;
            }
            else
            {
                pEntry = ((SAPDB_Char *)pMapping) + sizeof(CopyMappingHeader);

                for ( mappingIndex = 0; mappingIndex < pMapping->numberOfEntries; ++mappingIndex )
                {
                    if ( (((char *)pEntry)-((char *)pMapping)) > pMapping->totalSize )
                    {
                        ReportError("CheckMapping(%s) got bad offset %d for entry %d", 
                                    what,
                                    (int)(((char *)pEntry)-((char *)pMapping)),
                                    mappingIndex);
		                LocalSetLastError(RTEHSS_ERR_PARAMETER);
                        pMapping = 0;
                        break;
                    }

                    pNextEntry = pEntry + strlen(pEntry) + 1;

                    if ( (((char *)pNextEntry)-((char *)pMapping)) > pMapping->totalSize 
                       || (0 != *(pNextEntry-1) ) )
                    {
                        ReportError("CheckMapping(%s) found no zero termination for entry %d",
                                    what,
                                    mappingIndex);
		                LocalSetLastError(RTEHSS_ERR_PARAMETER);
                        pMapping = 0;
                        break;
                    }

                    if ( (ApiCopy.verbosity & RTEHSS_VERBOSE_CALL_PARAMETER_AND_RESULT) != 0 )
                    {
                        ReportCallback( RTEHSS_INFO, 
                                        "[%d] mapentry %s",
                                        mappingIndex, 
                                        pEntry);
                    }

                    if ( mappingIndex == 0 )
                    {
                        *hostname = pEntry;
                        *firstVolume = pNextEntry;
                    }

                    pEntry = pNextEntry;
                } /* for */
            }
        }
    }

    return pMapping;
}

/*------------------------------------------------------- */

static SAPDB_Bool
VerifyMapping(RTEHSS_Buffer const * sourceMapping,
              RTEHSS_Buffer const * targetMapping,
              SAPDB_Char const * *pSourceHostname,
              SAPDB_Char const * *pTargetHostname,
              SAPDB_Char const * *pSourceFirstVolume,
              SAPDB_Char const * *pTargetFirstVolume,
              SAPDB_Int4       * pNumberOfMappingEntries )
{
    CopyMappingHeader const *sourceCopyMapping;
    CopyMappingHeader const *targetCopyMapping;
    SAPDB_Char const * myHostname = (SAPDB_Char const *)RTESys_Hostname();

    sourceCopyMapping = CheckMapping("Source data volumes", sourceMapping, pSourceHostname, pSourceFirstVolume);
    if ( !sourceCopyMapping )
    {
        ReportError("VerifyMapping called with bad source mapping");
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    targetCopyMapping = CheckMapping("Target data volumes", targetMapping, pTargetHostname, pTargetFirstVolume);
    if ( !targetCopyMapping )
    {
        ReportError("VerifyMapping called with bad target mapping");
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( sourceCopyMapping->numberOfEntries != targetCopyMapping->numberOfEntries )
    {
        ReportError("VerifyMapping with different number of mapping entries (%d != %d)",
            sourceCopyMapping->numberOfEntries, targetCopyMapping->numberOfEntries);
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( sourceCopyMapping->numberOfEntries < 2 )
    {
        ReportError("VerifyMapping with not enough mapping entries (%d)",
            sourceCopyMapping->numberOfEntries );
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    *pNumberOfMappingEntries = (sourceCopyMapping->numberOfEntries - 1);

    if ( 0 == strcmp(*pSourceHostname, *pTargetHostname) )
    {
        ReportError("VerifyMapping source and target hosts identical '%s'",
            *pTargetHostname );
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    if ( 0 == strcmp(*pSourceHostname, myHostname) )
    {
        if ( 0 != (ApiCopy.verbosity & RTEHSS_VERBOSE_DEVELOPER_INFO) )
        {
            ReportCallback( RTEHSS_INFO, "local hostname %s matches source hostname", myHostname );
        }
    }
    else if ( 0 == strcmp(*pTargetHostname, myHostname) )
    {
        if ( 0 != (ApiCopy.verbosity & RTEHSS_VERBOSE_DEVELOPER_INFO) )
        {
            ReportCallback( RTEHSS_INFO, "local hostname %s matches target hostname", myHostname );
        }
    }
    else
    {
        ReportError("VerifyMapping no local mapping given local host '%s' given hosts %s and %s",
            myHostname, *pSourceHostname, *pTargetHostname );
		return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    {
        SAPDB_Int4 sourceVolumeSpaceLength = sourceMapping->bytesUsedOrNeeded -
            (SAPDB_Int4)(*pSourceFirstVolume - (SAPDB_Char *)sourceCopyMapping);
        SAPDB_Int4 targetVolumeSpaceLength = targetMapping->bytesUsedOrNeeded -
            (SAPDB_Int4)(*pTargetFirstVolume - (SAPDB_Char *)targetCopyMapping);

        if ( sourceVolumeSpaceLength != targetVolumeSpaceLength )
        {
            ReportError("VerifyMapping with different mapping entry size (%d != %d)",
                sourceVolumeSpaceLength, targetVolumeSpaceLength);
            return LocalSetLastError(RTEHSS_ERR_PARAMETER);
        }

        if ( 0 != memcmp( *pSourceFirstVolume, *pTargetFirstVolume, sourceVolumeSpaceLength) )
        {
            ReportError("VerifyMapping with different volume entries" );
            return LocalSetLastError(RTEHSS_ERR_PARAMETER);
        }

    }

    if ( 0 != (ApiCopy.verbosity & RTEHSS_VERBOSE_DEVELOPER_INFO) )
    {
        ReportCallback( RTEHSS_INFO, "mapping accepted" );
    }

    return true;
}

#if defined(WIN32)
/*---------------------------------------------*/

static SAPDB_Bool
BuildShareName(RTE_Path shareName)
{
    if ( (strlen("HSS_") + strlen(ApiCopy.id)) > sizeof(RTE_Path) )
    {
        ReportError("BuildShareName failed HSS_%s too long",
            ApiCopy.id );
        return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    strcpy(shareName, "HSS_");
    strcat(shareName, ApiCopy.id);
    return true;
}
#endif /* WIN32 */

/*---------------------------------------------*/

static SAPDB_Bool
BuildRemoteAccessPath( SAPDB_Char const * hostname,
                       SAPDB_Char const * volume,
                       RTE_Path           remotePath)
{
#if defined(WIN32)
    RTE_Path shareName;

    if ( !BuildShareName(shareName) )
    {
        ReportError("BuildRemoteAccessPath failed to build share name" );
        return false;
    }

    if ( (strlen("\\\\") + strlen(hostname) + strlen("\\") + strlen(shareName) + strlen("\\") + strlen(volume) + 1) > sizeof(RTE_Path) )
    {
        ReportError("BuildRemoteAccessPath failed (path too long) "
            "\\\\%s\\%s\\%s", hostname, shareName, volume );
        return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }
    SAPDB_sprintf(&remotePath[0], sizeof(RTE_Path), 
            "\\\\%s\\%s\\%s", hostname, shareName, volume);
#else
    if ( (ApiCopy.commonConfigPathLength + strlen("HSS_") + strlen(hostname) + strlen("_") + strlen(ApiCopy.id) + strlen("/") + strlen(volume) + 1) > sizeof(RTE_Path) )
    {
        ReportError("BuildRemoteAccessPath failed (path too long) "
            "%sHSS_%s_%s/%s", ApiCopy.commonConfigPath, hostname, ApiCopy.id, volume );
        return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }
    SAPDB_sprintf(&remotePath[0], sizeof(RTE_Path),
            "%sHSS_%s_%s/%s", ApiCopy.commonConfigPath, hostname, ApiCopy.id, volume );
#endif
    return true;
}

/*---------------------------------------------*/

static SAPDB_Bool
CheckRemoteAccess( RTE_Path   remotePath,
                   SAPDB_Bool readOnly )
{
    RTE_FileHandle hFile;
    tsp00_VfReturn_Param returnStatus;

    RTESys_IOOpen( &hFile,
                   remotePath,
                   readOnly ? RTESys_IOReadOnlyShared : RTESys_IOReadWriteShared,
                   true,
                   DEFAULT_SECURITY_RTESYS_IO,
                   &returnStatus);
    if ( vf_ok != returnStatus )
    {
        ReportError("CheckRemoteAccess open %s for '%s' failed [%d]",
            remotePath, readOnly ? "read only" : "read write", (int)RTESys_GetLastError() );
        return LocalSetLastError(RTEHSS_ERR_PARAMETER);

    }

    RTESys_IOClose(hFile, &returnStatus);

    return true;
}

/*------------------------------------------*/

static SAPDB_Bool
DoVolumeCopy(RTE_Path sourceRemotePath,
             RTE_Path targetRemotePath)
{
    tRTESys_IOPosition BytesWritten;
    tRTESys_IOPosition BytesRead;
    tRTESys_IOPosition skipOffset;
    tRTESys_IOPosition newPosition;
    long Buffer[RTEHSS_COPY_VOLUME_BUFFER_SIZE];

    tRTESys_IOHandle OriginalFd;
    tRTESys_IOHandle DestinationFd;
    tsp00_VfReturn_Param IgnoredError; /* close errors of temporary files are ignored here */
    tsp00_VfReturn_Param returnStatus;

    RTESys_IOOpen( &OriginalFd, sourceRemotePath, RTESys_IOReadOnlyShared, false, DEFAULT_SECURITY_RTESYS_IO, &returnStatus );
    if ( vf_ok == returnStatus )
    {
        RTESys_IOOpen( &DestinationFd, targetRemotePath, RTESys_IOWriteOnlyShared, false, DEFAULT_SECURITY_RTESYS_IO,  &returnStatus );
        if ( vf_ok != returnStatus )
        {
            ShowLastOsError("Open(Destination)", targetRemotePath );
            RTESys_IOClose( OriginalFd, &IgnoredError );
            return false;
        }
    }

#if defined(WIN32)

#define PRODUCT_ID_POS                  8
#define PRODUCT_ID_STRING               "SAPDB "
#define DEVSPACE_ID_POS                 (PRODUCT_ID_POS + sizeof (PRODUCT_ID_STRING))
#define DEVSPACE_ID_STRING              "DEVSPACE"
#define DEVSPACE_MAXSIZE_POS            256

    SAPDB_memset (Buffer, '\0', DEVSPACE_MAXSIZE_POS);

    ((char *)&Buffer[0])[2] = (CHAR) (( sizeof(tsp00_Page) % (64*1024) ) / 256);
    ((char *)&Buffer[0])[3] = (CHAR) (( sizeof(tsp00_Page) % (64*1024) ) % 256);
    ((char *)&Buffer[0])[4] = (CHAR) (( sizeof(tsp00_Page) / (64*1024) ) / 256);
    ((char *)&Buffer[0])[5] = (CHAR) (( sizeof(tsp00_Page) / (64*1024) ) % 256);

    SAPDB_memcpy ( ((char *)&Buffer[0]) + PRODUCT_ID_POS,
             PRODUCT_ID_STRING,
             sizeof (PRODUCT_ID_STRING) - 1);

    SAPDB_memcpy ( ((char *)&Buffer[0]) + DEVSPACE_ID_POS,
             DEVSPACE_ID_STRING,
             sizeof (DEVSPACE_ID_STRING) - 1);

    skipOffset = DEVSPACE_MAXSIZE_POS;

    RTESys_IOSeek ( OriginalFd,
                    skipOffset,
                    RTESys_IOSeekSet,
                    &newPosition,
                    &returnStatus );
    if ( vf_ok != returnStatus || newPosition != skipOffset )
    {
        ShowLastOsError("Skip(Source)", sourceRemotePath );
        RTESys_IOClose( DestinationFd, &IgnoredError );
        RTESys_IOClose( OriginalFd, &IgnoredError );
        return false;
    }

#else

    skipOffset = 0;

    RTESys_IOSeek ( OriginalFd,
                    skipOffset,
                    RTESys_IOSeekSet,
                    &newPosition,
                    &returnStatus );
    if ( vf_ok != returnStatus || newPosition != skipOffset )
    {
        ShowLastOsError("Skip(Source)", sourceRemotePath );
        RTESys_IOClose( DestinationFd, &IgnoredError );
        RTESys_IOClose( OriginalFd, &IgnoredError );
        return false;
    }

#endif

    RTESys_IORead( OriginalFd, ((char *)&Buffer[0])+skipOffset, RTEHSS_COPY_VOLUME_BUFFER_SIZE-skipOffset, &BytesRead, &returnStatus);
    if ( vf_ok != returnStatus )
    {
        ShowLastOsError("Read(Original)", sourceRemotePath );
        RTESys_IOClose( DestinationFd, &IgnoredError );
        RTESys_IOClose( OriginalFd, &IgnoredError );
        return false;
    }

    RTESys_IOWrite( DestinationFd, Buffer, RTEHSS_COPY_VOLUME_BUFFER_SIZE, &BytesWritten, &returnStatus );
    if ( vf_ok != returnStatus )
    {
        ShowLastOsError("Write(Destination)", targetRemotePath );
        RTESys_IOClose( DestinationFd, &IgnoredError );
        RTESys_IOClose( OriginalFd, &IgnoredError );
        return false;
    }

    do {
      RTESys_IORead( OriginalFd, Buffer, RTEHSS_COPY_VOLUME_BUFFER_SIZE, &BytesRead, &returnStatus );
      if ( vf_ok == returnStatus )
      {
        RTESys_IOWrite( DestinationFd, Buffer, BytesRead, &BytesWritten, &returnStatus );
        if ( vf_ok != returnStatus )
        {
          ShowLastOsError("Write(Destination)", targetRemotePath );
        }
      }
      else if ( vf_eof != returnStatus ) /* 1110223 */
      {
        ShowLastOsError("Read(Original)", sourceRemotePath );
      }
    } while ( vf_ok == returnStatus && 0 != BytesRead );
    RTESys_IOClose( OriginalFd, &IgnoredError );
    RTESys_IOClose( DestinationFd, &IgnoredError );

    return ( returnStatus == vf_eof && BytesRead == 0 );
}

/*------------------------------------------*/

/*
  A given volume path is checked to be relative and the corresponding share is created if needed
 */
static SAPDB_Bool
ValidateOrCreateShare(SAPDB_Char const * volumePath)
{
#if defined(WIN32)
    wchar_t unicodeHostname[sizeof(RTE_Nodename)];  
    wchar_t unicodeShareName[sizeof(RTE_Path)];
    wchar_t unicodeRunDirectory[sizeof(RTE_Path)];
    NET_API_STATUS resShareApi;
    DWORD devType = 0;
    RTE_Path shareName;
    RTE_Path runDirectory;
    SAPDB_Char const * myHostname;
    union {
        LPBYTE ptr;
        SHARE_INFO_2 *pInfo;
    } bufferPtr;

    /*
      Absolute path checking 
       a) path introducer '/' 
       b) path introducer '\\' 
       c) Disk identifier "[A-Za-z]:"
     */
    if ( '\\' == *volumePath 
       || '/' == *volumePath 
       || ( isalpha(*volumePath) && (':' == *(volumePath+1)) ) )
    {
        ReportCallback( RTEHSS_INFO, "Absolute path for volume '%s' does not force to create share", volumePath );
        return true;
    }

    /*
      Releative paths above current directory also rejected
     */
    if ( '.' == *volumePath && '.' == *(volumePath+1) )
    {
        ReportError("ValidateOrCreateShare failed: Relative path '%s' impossible to access via shared rundirectory",
            volumePath );
        return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    myHostname = (SAPDB_Char const *)RTESys_Hostname();
    SAPDB_sprintfUnicode(SAPDB_EncodingUCS2Native, unicodeHostname, sizeof(unicodeHostname), "\\\\%s", myHostname);

    if ( !BuildShareName(shareName) )
    {
        ReportError("ValidateOrCreateShare failed to get share name");
        return false;
    }
    SAPDB_sprintfUnicode(SAPDB_EncodingUCS2Native, unicodeShareName, sizeof(unicodeShareName), "%s", shareName);

    /*
      Rundirectory is always identical with current working directory...
     */
    strcpy(runDirectory, ApiCopy.workingDirectory);
    SAPDB_sprintfUnicode(SAPDB_EncodingUCS2Native, unicodeRunDirectory, sizeof(unicodeRunDirectory), "%s", runDirectory);

    /*
      Call the NetShareGetInfo function to verify for already existing share
     */
    resShareApi = NetShareGetInfo(unicodeHostname, unicodeShareName, 2, &bufferPtr.ptr);
    if(resShareApi==0)
    {
        SAPDB_Char const * shareTypeName;

        if ( STYPE_DISKTREE != bufferPtr.pInfo->shi2_type )
        {
            switch( bufferPtr.pInfo->shi2_type )
            {
            case STYPE_DISKTREE: shareTypeName = "DiskTree";
                break;
            case STYPE_PRINTQ: shareTypeName = "Printer";
                break;
            case STYPE_DEVICE: shareTypeName = "CommunicationDevice";
                break;
            case STYPE_IPC: shareTypeName = "InterprocessCommunication";
                break;
            case STYPE_SPECIAL: shareTypeName = "Special";
                break;
            default: shareTypeName = "Unknown";
                break;
            }
            ReportError("ValidateOrCreateShare failed: share type (%d)'%s' illegal for shared rundirectory",
                bufferPtr.pInfo->shi2_type, shareTypeName );

            NetApiBufferFree(bufferPtr.ptr);
            return LocalSetLastError(RTEHSS_ERR_CONFIGURATION);
        }

        if ( 0 != _wcsicmp(unicodeRunDirectory, bufferPtr.pInfo->shi2_path ) )
        {
            NetApiBufferFree(bufferPtr.ptr);

            resShareApi = NetShareDel(unicodeHostname, unicodeShareName, 0);
            if ( 0 == resShareApi )
            {
                if ( 0 != (ApiCopy.verbosity & RTEHSS_VERBOSE_CONFIGURATION_CHANGES) )
                {
                    ReportCallback( RTEHSS_INFO, "Deleted share '%s' (path differed)", shareName );
                }
            }
            else
            {
                ReportError("ValidateOrCreateShare failed to delete share '%s' [%d]",
                    shareName, resShareApi );
                return LocalSetLastError(RTEHSS_ERR_CONFIGURATION);
            }
        }
        else
        {
            if ( 0 != (ApiCopy.verbosity & RTEHSS_VERBOSE_DEVELOPER_INFO) )
            {
                ReportCallback( RTEHSS_INFO, "Reused share '%s' (path identical)", shareName );
            }
            return true;
        }
    }

    /*
      Share does not or does no longer exists... Create it using the rundirectory
     */
    {
        DWORD parmError;
        SHARE_INFO_2 shareInfo;

        shareInfo.shi2_netname = unicodeShareName;
        shareInfo.shi2_type = STYPE_DISKTREE;
        shareInfo.shi2_remark = L"automatically added by SAPDB for HSS_TEST";
        shareInfo.shi2_permissions = ACCESS_ALL;
        shareInfo.shi2_max_uses = -1;
        shareInfo.shi2_current_uses = 0;
        shareInfo.shi2_path = unicodeRunDirectory;
        shareInfo.shi2_passwd = L"";
        
        resShareApi = NetShareAdd(unicodeHostname, 2, (LPBYTE)&shareInfo, &parmError);
        //
        // If the function succeeds, inform the user.
        //
        if(0 == resShareApi)
        {
            if ( 0 != (ApiCopy.verbosity & RTEHSS_VERBOSE_CONFIGURATION_CHANGES) )
            {
                ReportCallback( RTEHSS_INFO, "Added share '%s' for '%s'", shareName, runDirectory );
            }
        }
        else
        {
            ReportError("ValidateOrCreateShare failed to create share '%s' for '%s' [%d]",
                shareName, runDirectory, resShareApi );
            return LocalSetLastError(RTEHSS_ERR_CONFIGURATION);
        }
    }

#else

    struct stat volumeStatBuffer;
    struct stat linkStatBuffer;
    RTE_Path shareName;
    RTE_Path sharePath;
    SAPDB_Char const * myHostname;
    tsp00_VfReturn_Param returnValue;

    /*
      Absolute path checking by path introducer '/' 
     */
    if ( '/' == *volumePath )
    {
        ReportError("ValidateOrCreateShare failed: Absolute path '%s' impossible to access for two different hosts",
                        volumePath );
        return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    /*
      Releative paths above directory also rejected
     */
    if ( '.' == *volumePath && '.' == *(volumePath+1) )
    {
        ReportError("ValidateOrCreateShare failed: Relative path '%s' impossible to access via shared run directory",
            volumePath );
        return LocalSetLastError(RTEHSS_ERR_PARAMETER);
    }

    myHostname = (SAPDB_Char const *)RTESys_Hostname();

    if ( (ApiCopy.commonConfigPathLength + strlen("HSS_") + strlen(myHostname) + strlen("_") + strlen(ApiCopy.id) + strlen("/") + strlen(volumePath) + 1) > sizeof(RTE_Path) )
    {
        ReportError("ValidateOrCreateShare failed to get share name (%sHSS_%s_%s/%s) too long", 
                        shareName, myHostname, ApiCopy.id, volumePath);
        return false;
    }
    else
    {
        strcpy(shareName, ApiCopy.commonConfigPath);
        strcat(shareName, "HSS_");
        strcat(shareName, myHostname);
        strcat(shareName, "_");
        strcat(shareName, ApiCopy.id);
    }

    memset(sharePath, 0, sizeof(RTE_Path));
    if ( readlink(shareName, sharePath, sizeof(RTE_Path)-1) < 0 )
    {
        if ( errno == ENOENT )
        {
            if ( 0 == symlink(ApiCopy.workingDirectory, shareName) )
            {
                return true;
            }
            ReportError("ValidateOrCreateShare cannot create read volume directory symlink %s:%d",
                        shareName, errno);
        }
        else
        {
            ReportError("ValidateOrCreateShare cannot read volume directory symlink %s:%d",
                        shareName, errno);
        }
        return LocalSetLastError(RTEHSS_ERR_CONFIGURATION);
    }

    if ( 0 != RTE_save_stat(sharePath, &linkStatBuffer) )
    {
        ReportError("ValidateOrCreateShare cannot stat volume directory symlink %s(%):%d",
                        sharePath, shareName, errno);
        return LocalSetLastError(RTEHSS_ERR_CONFIGURATION);
    }

    if ( 0 != RTE_save_stat(ApiCopy.workingDirectory, &volumeStatBuffer) )
    {
        ReportError("ValidateOrCreateShare cannot stat rundirectoy directory %s:%d",
                        ApiCopy.workingDirectory, errno);
        return LocalSetLastError(RTEHSS_ERR_CONFIGURATION);
    }

    if ( linkStatBuffer.st_dev != volumeStatBuffer.st_dev
      || linkStatBuffer.st_ino != volumeStatBuffer.st_ino )
    {
        ReportError("ValidateOrCreateShare symlink %s(%s) does not point to rundirectoy %s",
                        sharePath, shareName, ApiCopy.workingDirectory);
        return LocalSetLastError(RTEHSS_ERR_CONFIGURATION);
    }
#endif
    return true;
}

/*------------------------------------------*/

#if defined(WIN32)

void sql60_msg_diag ( long  lMsgID,
                      unsigned long ulEventType,
                      const char * pszLabel,
                      const char * pszFormatStr, ...)
{
    if ( ApiCopy.reportCallback )
    {
        RTEHSS_ReportType type;
        va_list args ;

        switch( ulEventType )
        {
        case 0:
        case 1:
            type = RTEHSS_ERROR;
            break;
        case 2:
            type = RTEHSS_WARNING;
            break;
        case 3:
            type = RTEHSS_INFO;
            break;
        default:
            type = RTEHSS_ERROR;
            break;
        }

        if ( ApiCopy.currentFunction )
        {
            (*ApiCopy.reportCallback)(type, ApiCopy.currentFunction);
            ApiCopy.currentFunction = 0;
        }
        va_start ( args, pszFormatStr ) ;
            sp77vsprintf ( ApiCopy.msgBuffer, RTEHSS_MAX_MESSAGE_LENGTH, pszFormatStr, args );
        va_end ( args ) ;

        (*ApiCopy.reportCallback)(type, ApiCopy.msgBuffer);
    }
}

#else

void	sql60c_msg_8    ( int         lMsgID ,
                          int         ulEventType ,
                          const char *pszLabel ,
                          const char *pszFormatStr ,
                          ...                           )
{
    if ( ApiCopy.reportCallback )
    {
        RTEHSS_ReportType type;
        va_list args ;

        switch( ulEventType )
        {
        case 0:
        case 1:
            type = RTEHSS_ERROR;
            break;
        case 2:
            type = RTEHSS_WARNING;
            break;
        case 3:
            type = RTEHSS_INFO;
            break;
        default:
            type = RTEHSS_ERROR;
            break;
        }

        if ( ApiCopy.currentFunction )
        {
            (*ApiCopy.reportCallback)(type, ApiCopy.currentFunction);
            ApiCopy.currentFunction = 0;
        }

        va_start ( args, pszFormatStr ) ;
            sp77vsprintf ( ApiCopy.msgBuffer, RTEHSS_MAX_MESSAGE_LENGTH, pszFormatStr, args );
        va_end ( args ) ;

        (*ApiCopy.reportCallback)(type, ApiCopy.msgBuffer);
    }
    return;
}

#endif

static void ShowLastOsError( const SAPDB_Char *    Routine,
                             const SAPDB_Char *    Filename )
{
    char                  LastErrorMessage[256];
    RTESys_ErrorMessage   SysErrorMessage;
    RTE_SystemRc lastError = RTESys_GetLastError();

    sp77sprintf( &LastErrorMessage[0], sizeof(LastErrorMessage), "[%d] %s", (int)lastError, RTESys_StrError(lastError, SysErrorMessage) );
    MSGD((  ERR_HSS_OS_ERROR, 
            Routine, 
            Filename == NULL ? "" : Filename, 
            &LastErrorMessage[0] ));
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
