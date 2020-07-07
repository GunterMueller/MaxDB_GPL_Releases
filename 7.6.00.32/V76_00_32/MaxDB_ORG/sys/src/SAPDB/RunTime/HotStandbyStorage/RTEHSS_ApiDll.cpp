/*!
  @file           RTEHSS_ApiDll.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          RTEHSS API Dll Class

  This is the class implementation for the singleton describing access to RTEHSS API
  The member function implementation follows always the same schema. 
  A specific member variable (m_MemberFuncLoaded) is tested and if it not set, the procedure call pointer
  is loaded into a specific union (uMemberFunc). This union is used then to call the DLL function.

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

#include "RunTime/HotStandbyStorage/RTEHSS_ApiDll.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "geo002.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/*
  Union definition to convert received procedure call pointers into type correct calls
  without need of cast operation. Each union is named after the member function where
  it is used.
 */
extern "C" {

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uInit
    {
        ///! function pointer for RTEHSS_Init call
        SAPDB_Bool (*Init)(
	        SAPDB_Char const * sessionId,
            SAPDB_Int4         verbosity,
            void (*reportCallback)(RTEHSS_ReportType type, SAPDB_Char const *msg),
	        void **            pHandle );

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uDeinit
    {
        ///! function pointer for RTEHSS_Deinit call
        SAPDB_Bool (*Deinit)(
	        void **            pHandle );

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uSetVerbosity 
    {
        ///! function pointer for RTEHSS_SetVerbosity call
        SAPDB_Bool (*SetVerbosity)(
            void * const handle, 
            SAPDB_Int4   newVerbosity,
            SAPDB_Int4 * oldVerbosity);

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uGetLastError 
    {
        ///! function pointer for RTEHSS_GetLastError call
        RTEHSS_ErrorCode (*GetLastError)();

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uPerror 
    {
        ///! function pointer for RTEHSS_Perror call
        const SAPDB_Char * (*Perror)(RTEHSS_ErrorCode errcode);

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uGetMapping 
    {
        ///! function pointer for RTEHSS_GetMapping call
        SAPDB_Bool (*GetMapping)(
	        void * const    handle,
            SAPDB_Int4      numberOfVolumes,
	        SAPDB_Char   ** volumeArray,
	        RTEHSS_Buffer * mappingBuffer);

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uMirrorData 
    {
        ///! function pointer for RTEHSS_MirrorData call
        SAPDB_Bool (*MirrorData)(
	        void * const    handle,
	        RTEHSS_Buffer const * sourceMapping,
	        RTEHSS_Buffer const * targetMapping );

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uMirrorSplit 
    {
        ///! function pointer for RTEHSS_MirrorSplit call
        SAPDB_Bool (*MirrorSplit)(
	        void * const    handle,
	        RTEHSS_Buffer const * sourceMapping,
	        RTEHSS_Buffer const * targetMapping );

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uMirrorSplitDone
    {
        ///! function pointer for RTEHSS_MirrorSplit call
        SAPDB_Bool (*MirrorSplitDone)(
	        void * const    handle,
	        RTEHSS_Buffer const * targetMapping );

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uCheckIfSynchronized 
    {
        ///! function pointer for RTEHSS_CheckIfSynchronized call
        SAPDB_Bool (*CheckIfSynchronized)(
	        void * const    handle,
	        RTEHSS_CheckType what, 
	        SAPDB_Bool *     pDone );

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

    /*!
      Union definitions to convert received procedure call pointers into type correct calls
      without need of cast operation. Each union is named after the member function where
      it is used.
     */
    union uSetLogReadOnlyStatus 
    {
        ///! function pointer for RTEHSS_SetLogReadOnlyStatus call
        SAPDB_Bool (*SetLogReadOnlyStatus)(
	        void * const          handle,
	        RTEHSS_Buffer const * localMapping,
	        SAPDB_Bool	          readOnly );

        ///! return procedure address
        RTE_SharedLibProcAddress procAddress;
    };

};

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

RTEHSS_ApiDll * RTEHSS_ApiDll::m_Instance = NULL;

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
   @brief    Initialization call

  The identifier is a zero terminated string which is unique for each database an a
  machine. Normally this is the name of the database instance. Maximum length of 
  identifier is RTEHSS_IDENTIFIER_MAX_LENGHT + 1 (including zero termination)
  The storage returns its onw identification, which is exchanged between master and
  standby nodes.
  All output of any call will be produced using the report callback.

  @return        true if call succeeded, false if not (use RTEHSS_GetLastError
              to get more information).

 */

RTEHSS_ApiDll & RTEHSS_ApiDll::Instance()
{
    SAPDB_Bool constructorOk;
    SAPDBErr_MessageList messageList;
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEHSS_ApiDll));

    if ( !RTEHSS_ApiDll::m_Instance )
    {
        RTEConf_Parameter::String whichLibName;
        constructorOk = RTEConf_ParameterAccess::Instance()->GetString( (SAPDB_UTF8 const *)PAN_HS_STORAGE_DLL,
                                                                 whichLibName,
                                                                 messageList);
        if ( constructorOk )
        {
            RTEHSS_ApiDll::m_Instance = new (Space) RTEHSS_ApiDll( (SAPDB_Char const *)whichLibName,
                                                                   constructorOk,
                                                                   messageList);
        }
        else
        {
            RTEHSS_ApiDll::m_Instance = new (Space) RTEHSS_ApiDll( "libhss",
                                                                   constructorOk,
                                                                   messageList);
        }

#ifdef DEBUG_RTE
        if ( !constructorOk )
        {
            RTE_Message(messageList);
        }
#endif
    }

    SAPDBERR_ASSERT_STATE(RTEHSS_ApiDll::m_Instance);

    return *RTEHSS_ApiDll::m_Instance;
}

/*!
   @brief    Initialization call


  The identifier is a zero terminated string which is unique for each database an a
  machine. Normally this is the name of the database instance. Maximum length of 
  identifier is RTEHSS_IDENTIFIER_MAX_LENGHT + 1 (including zero termination)
  The storage returns its onw identification, which is exchanged between master and
  standby nodes.
  All output of any call will be produced using the report callback.
   @param          sessionId [in] identifier unique for each database instance on machine
   @param          verbosity [in] logical or of RTEHSS_Verbosity enumeration values
   @param          reportCallback [in] an optional callback for reporting error,warning or infos
   @param          handle [out] pointer to handle
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).

 */
SAPDB_Bool
RTEHSS_ApiDll::Init(
	SAPDB_Char const *     sessionId,
    SAPDB_Int4             verbosity,
    RTEHSS_ReportCallback  reportCallback,
    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_Init", msgList);
    }

    static union uInit u;

    if ( !m_InitLoaded 
      && !(m_InitLoaded = this->GetProcAddress("RTEHSS_Init", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.Init(sessionId, verbosity, reportCallback, &m_ApiHandle) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_Init", msgList);
    }
    return true;
}

/*!
   @brief    Deinitialization call
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).

 */
SAPDB_Bool
RTEHSS_ApiDll::Deinit( SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_Deinit", msgList);
    }

    static union uDeinit u;

    if ( !m_DeinitLoaded 
      && !(m_DeinitLoaded = this->GetProcAddress("RTEHSS_Deinit", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.Deinit(&m_ApiHandle) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_Deinit", msgList);
    }
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
   @param          newVerbosity [in] logical or of RTEHSS_Verbosity enumeration values
   @param          oldVerbosity [out] if not NULL the former verbosity is returned
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).

 */
SAPDB_Bool
RTEHSS_ApiDll::SetVerbosity(
                    SAPDB_Int4   newVerbosity,
                    SAPDB_Int4 & oldVerbosity,
                    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_SetVerbosity", msgList);
    }

    static union uSetVerbosity u;

    if ( !m_SetVerbosityLoaded 
      && !(m_SetVerbosityLoaded = this->GetProcAddress("RTEHSS_SetVerbosity", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.SetVerbosity(m_ApiHandle, newVerbosity, &oldVerbosity) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_SetVerbosity", msgList);
    }
    return true;
}

/*!
   @brief    Get the last error reported


  Can be called after an HSSAPI call was called. It will return the error code
  that describes the reason for the last call return false. If the last call
  returned true, the routine will return 'RTEHSS_ERR_OK'.
  If called twice and no other HSSAPI call was issued, the second call will 
  retrieve the same information.
  For detailed error messages, the report callback is used.
   @return        The result of the last HSSAPI call

 */
RTEHSS_ErrorCode
RTEHSS_ApiDll::GetLastError()
{
    SAPDBErr_MessageList msgList;
    static union uGetLastError u;

    if ( !m_GetLastErrorLoaded
      && !(m_GetLastErrorLoaded = this->GetProcAddress("RTEHSS_GetLastError", u.procAddress, msgList)) )
    {
        return RTEHSS_ERR_UNEXPECTED;
    }

    return u.GetLastError();
}

/*!
   @brief    Convert last error code into readable string
   @argument       errcode [in] the error code to convert
   @return        A string describing the error code

 */

const SAPDB_Char *
RTEHSS_ApiDll::Perror(RTEHSS_ErrorCode errcode)
{
    SAPDBErr_MessageList msgList;
    static union uPerror u;

    if ( !m_PerrorLoaded 
      && !(m_PerrorLoaded = this->GetProcAddress("RTEHSS_Perror", u.procAddress, msgList)) )
    {
        return "translation failed";
    }

    return u.Perror(errcode);
}


/*!
   @brief    Used to get the device mapping information needed to establish a group of devices.


  This call is issued once for every device to be mapped.
   @param          numberOfVolumes [in] number of volumes to map
   @param          volumeArray [in] array of zero terminated paths to volume to be mapped
   @param          mappingBuffer [inout] filled with internal mapping representation
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).

 */
SAPDB_Bool 
RTEHSS_ApiDll::GetMapping(
    SAPDB_Int4             numberOfVolumes,
	SAPDB_Char   **        volumeArray,
	RTEHSS_Buffer        & mappingBuffer,
    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_GetMapping", msgList);
    }

    static union uGetMapping u;

    if ( !m_GetMappingLoaded 
      && !(m_GetMappingLoaded = this->GetProcAddress("RTEHSS_GetMapping", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.GetMapping(m_ApiHandle, numberOfVolumes, volumeArray, &mappingBuffer) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_GetMapping", msgList);
    }
    else
    {
        /* If mapping returned empty buffer, fill a single '0' as dummy mapping... */
        if ( (mappingBuffer.bufferGiven > 0) 
          && (0 == mappingBuffer.bytesUsedOrNeeded) )
        {
            mappingBuffer.bytesUsedOrNeeded = 1;
            ((char *)mappingBuffer.buffer)[0] = 0;
        }
    }
    return true;
}

/*!
   @brief    Create a mirror group for later data split using the mapping information of master and backup server


  Used by master database server to create the mirror group using the DATA-volume mapping information from master and slave. After the mirror is established, it is splitted immediately to allow
  both sides to work on its DATA-volumes independendly.
  Since the master has to wait with the next savepoint for this call to complete, it should complete in a reasonable amount of time. During execution of the RTEHSS_DataSplit() call, no data is written to the data volumes neither on master database nor on backup database volumes.
   @param          sourceMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).
 */
 SAPDB_Bool
RTEHSS_ApiDll::MirrorData (
	RTEHSS_Buffer const  & sourceMapping,
	RTEHSS_Buffer const  & targetMapping,
    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_MirrorData", msgList);
    }

    static union uMirrorData u;

    if ( !m_MirrorDataLoaded 
      && !(m_MirrorDataLoaded = this->GetProcAddress("RTEHSS_MirrorData", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.MirrorData(m_ApiHandle, &sourceMapping, &targetMapping) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_MirrorData", msgList);
    }
    return true;
}

/*!
   @brief    Execute a data split using the mapping information of master and backup server

  Used by master database server to create the mirror group using the DATA-volume mapping 
  information from master and slave. After the mirror is established, it is splitted immediately
  to allow both sides to work on its DATA-volumes independendly.
  Since the master has to wait with the next savepoint for this call to complete, it should 
  complete in a reasonable amount of time. During execution of the RTEHSS_MirrorSplit() call,
  no data is written to the data volumes neither on master database nor on backup database 
  volumes.
   @param          sourceMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).
 */ 
SAPDB_Bool 
RTEHSS_ApiDll::MirrorSplit (
	RTEHSS_Buffer const  & sourceMapping,
	RTEHSS_Buffer const  & targetMapping,
    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_MirrorSplit", msgList);
    }

    static union uMirrorSplit u;

    if ( !m_MirrorSplitLoaded 
      && !(m_MirrorSplitLoaded = this->GetProcAddress("RTEHSS_MirrorSplit", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.MirrorSplit(m_ApiHandle, &sourceMapping, &targetMapping) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_MirrorSplit", msgList);
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
RTEHSS_ApiDll::MirrorSplitDone (
	RTEHSS_Buffer const  & targetMapping,
    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_MirrorSplitDone", msgList);
    }

    static union uMirrorSplitDone u;

    if ( !m_MirrorSplitDoneLoaded 
      && !(m_MirrorSplitDoneLoaded = this->GetProcAddress("RTEHSS_MirrorSplitDone", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.MirrorSplitDone(m_ApiHandle, &targetMapping) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_MirrorSplitDone", msgList);
    }
    return true;
}

/*!
   @brief    Find out, if data volumes have reached requested synchronization state

  Synchronization state is reached if either mirror is established (what==RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED)
  or until splitting is completed (what==RTEHSS_CHECK_FOR_SPLIT_COMPLETED). If the state is not yet reached, 
  an informational message is send using the standard report callback (see RTEHSS_Init).
   @param          what [in] indicate what the routine should be checking
   @param          done [out] true if synchronized, false otherwise
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).
 */
SAPDB_Bool
RTEHSS_ApiDll::CheckIfSynchronized( 
    RTEHSS_CheckType       what, 
    SAPDB_Bool           & done,
    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_CheckIfSynchronized", msgList);
    }

    static union uCheckIfSynchronized u;

    if ( !m_CheckIfSynchronizedLoaded 
      && !(m_CheckIfSynchronizedLoaded = this->GetProcAddress("RTEHSS_CheckIfSynchronized", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.CheckIfSynchronized(m_ApiHandle, what, &done) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_CheckIfSynchronized", msgList);
    }
    return true;
}

/*!
   @brief    Set the given log volume read only status.

  This is a pure protectional call. It is not absolutely necessary to have the
  LOG-volumes readonly for the backup database server. But if the storage system provides 
  such a feature, that only the master has read/write access and can prevent the backup database
  server from write access, it is usefull to call it. Otherwise this can be a dummy.
  Both master and backup will issue this call.
   @param          localMapping [in] mapping as returned by calls to RTEHSS_GetMapping
   @param          readOnly [in] if false, set to read/write else set to read only.
   @param          errList [out] set to error if call failed
   @return        true if call, false if not (errList is used to store information).
 */
SAPDB_Bool
RTEHSS_ApiDll::SetLogReadOnlyStatus (
	RTEHSS_Buffer const &  localMapping,
	SAPDB_Bool	           readOnly,
    SAPDBErr_MessageList & msgList )
{
    if ( m_DllNotUsable )
    {
        return ReturnUnasuabilityMessage("RTEHSS_SetLogReadOnlyStatus", msgList);
    }

    static union uSetLogReadOnlyStatus u;

    if ( !m_SetLogReadOnlyStatusLoaded 
      && !(m_SetLogReadOnlyStatusLoaded = this->GetProcAddress("RTEHSS_SetLogReadOnlyStatus", u.procAddress, msgList)) )
    {
        return false;
    }

    if ( !u.SetLogReadOnlyStatus(m_ApiHandle, &localMapping, readOnly) )
    {
        return ReturnApiCallErrorMessage("RTEHSS_SetLogReadOnlyStatus", msgList);
    }
    return true;
}

/*===========================================================================*
 *  Local CODE                                                               *
 *===========================================================================*/

/*!
   @brief    in case of unuasable api, return the message containing the reason plus the calling member

   The member 'm_NotUsableMessage' is copied and another messagee identifying the calling member function is added.

   @param    member [in] the calling member
   @param    msgList [out] filled with error message
   @return   always false
 */
SAPDB_Bool
RTEHSS_ApiDll::ReturnUnasuabilityMessage( SAPDB_Char const *member
                                        , SAPDBErr_MessageList &msgList )
{
    msgList = m_NotUsableMessage;
    msgList.AppendNewMessage( SAPDBErr_MessageList( "RTEHSS",
                                                    __FILE__,
                                                    __LINE__,
                                                    RTEERR_HSS_API_DLL_NOT_USABLE,
                                                    member ) );
    return false;
}

/*!
       @brief    in case of api call error, return the message containing the reason including the calling member

       @param    member [in] the calling member
       @param    msgList [out] filled with error message
       @return   always false
 */
SAPDB_Bool
RTEHSS_ApiDll::ReturnApiCallErrorMessage( SAPDB_Char const *member
                                        , SAPDBErr_MessageList &msgList )
{
    RTEHSS_ErrorCode apiResult;

    apiResult = this->GetLastError();

    msgList = SAPDBErr_MessageList( "RTEHSS",
                                    __FILE__,
                                    __LINE__,
                                    RTEERR_HSS_APICALL_FAILED,
                                    member,
                                    SAPDB_ToString((SAPDB_Int4)apiResult),
                                    this->Perror(apiResult) );
    return false;
}

/*!
   @brief    construct the singleton passing responsibility to underlying class...
 */
RTEHSS_ApiDll::RTEHSS_ApiDll( SAPDB_Char const     * sharedLibName,
                              SAPDB_Bool           & ok,
                              SAPDBErr_MessageList & errList)
: m_InitLoaded(0),
  m_DeinitLoaded(0),
  m_SetVerbosityLoaded(0),
  m_GetLastErrorLoaded(0),
  m_PerrorLoaded(0),
  m_GetMappingLoaded(0),
  m_MirrorDataLoaded(0),
  m_MirrorSplitLoaded(0),
  m_MirrorSplitDoneLoaded(0),
  m_CheckIfSynchronizedLoaded(0),
  m_SetLogReadOnlyStatusLoaded(0),
  m_ApiHandle(0),
  RTESys_SharedLib( sharedLibName,
                    RTE_LoadSymbolsLocal,
                    ok,
                    errList )
{
    m_DllNotUsable = !ok;
    if ( m_DllNotUsable )
    {
        m_NotUsableMessage = errList;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/