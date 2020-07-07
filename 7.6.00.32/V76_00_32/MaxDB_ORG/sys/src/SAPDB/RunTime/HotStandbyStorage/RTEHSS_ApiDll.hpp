/*!
  @file           RTEHSS_ApiDll.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Class implementation for the singleton describing access to RTEHSS API

  All HotStandbyStorage access is contained in an on demand loaded ApiDLL. The DLL is selected by a configuration parameter.
  This class allows the access to the DLL functionality via a singleton access.

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


#ifndef RTEHSS_APIDLL_HPP
#define RTEHSS_APIDLL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/System/RTESys_SharedLib.h"
#include "RunTime/HotStandbyStorage/RTEHSS_Api.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
   @class          RTEHSS_ApiDll
   @brief          Singleton class for accessing RTEHSS_API derived from RTESys_SharedLib

 */

class RTEHSS_ApiDll: public RTESys_SharedLib
{
public:
    /*!
       @brief    Initialization call

  The identifier is a zero terminated string which is unique for each database an a
  machine. Normally this is the name of the database instance. Maximum length of 
  identifier is RTEHSS_IDENTIFIER_MAX_LENGTH + 1 (including zero termination)
  The storage returns its onw identification, which is exchanged between master and
  standby nodes.
  All output of any call will be produced using the report callback.
       @return         a reference to the API instance

     */

    static RTEHSS_ApiDll & Instance();

    /*!
       @brief    Initialization call


  The identifier is a zero terminated string which is unique for each database an a
  machine. Normally this is the name of the database instance. Maximum length of 
  identifier is RTEHSS_IDENTIFIER_MAX_LENGTH + 1 (including zero termination)
  The storage returns its onw identification, which is exchanged between master and
  standby nodes.
  All output of any call will be produced using the report callback.
       @param          sessionId [in] identifier unique for each database instance on machine
       @param          verbosity [in] logical or of RTEHSS_Verbosity enumeration values
       @param          reportCallback [in] an optional callback for reporting error,warning or infos
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */

     SAPDB_Bool Init(
        SAPDB_Char const *     sessionId,
        SAPDB_Int4             verbosity,
        RTEHSS_ReportCallback  reportCallback,
        SAPDBErr_MessageList & msgList );

//  Normally i keep private and public separated, but due to the strong connection and
//  schematic use of the member variables, i broke this rule here...
private:
    SAPDB_Bool m_InitLoaded; ///< if set call is loaded

public:
    /*!
       @brief    Deinitialization call
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */

    SAPDB_Bool Deinit( SAPDBErr_MessageList & msgList );
private:
    SAPDB_Bool m_DeinitLoaded; ///< if set call is loaded

public:
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
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */

    SAPDB_Bool SetVerbosity(SAPDB_Int4             newVerbosity,
                            SAPDB_Int4           & oldVerbosity,
                            SAPDBErr_MessageList & msgList );

private:
    SAPDB_Bool m_SetVerbosityLoaded; ///< if set call is loaded

public:
    /*!
      @brief    Get the last error reported (private)

      Can be called after an HSSAPI call was called. It will return the error code
      that describes the reason for the last call return false. If the last call
      returned true, the routine will return 'RTEHSS_ERR_OK'.
      If called twice and no other HSSAPI call was issued, the second call will 
      retrieve the same information.

      @return         The last error code reported
     */

    RTEHSS_ErrorCode GetLastError( );
private:
    SAPDB_Bool m_GetLastErrorLoaded; ///< if set call is loaded

    /*!
       @brief    Convert last error code into readable string (private)
       @param    errcode [in] the error code to convert
       @return   A string describing the error code

     */

    const SAPDB_Char * Perror( RTEHSS_ErrorCode errcode );
    SAPDB_Bool m_PerrorLoaded; ///< if set call is loaded

public:
    /*!
       @brief    Used to get the device mapping information needed to establish
a group of devices.


  This call is issued once for every device to be mapped.
       @param          numberOfVolumes [in] number of volumes to map
       @param          volumeArray [in] array of zero terminated paths to volume to be mapped
       @param          mappingBuffer [inout] filled with internal mapping representation
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */

    SAPDB_Bool GetMapping(
        SAPDB_Int4      numberOfVolumes,
	    SAPDB_Char   ** volumeArray,
	    RTEHSS_Buffer & mappingBuffer,
        SAPDBErr_MessageList &msgList );
private:
    SAPDB_Bool m_GetMappingLoaded; ///< if set call is loaded

public:
    /*!
       @brief    Create a mirror group for later data split using the mapping information of master and backup server


  Used by master database server to create the mirror group using the DATA-volume mapping information from master and slave. After the mirror is established, it is splitted immediately to allow
  both sides to work on its DATA-volumes independendly.
  Since the master has to wait with the next savepoint for this call to complete, it should complete in a reasonable amount of time. During execution of the RTEHSS_DataSplit() call, no data is written to the data volumes neither on master database nor on backup database volumes.
       @param          sourceMapping [in] mapping as returned by calls to RTEHSS_GetMapping
       @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */
 
    SAPDB_Bool MirrorData (
	    RTEHSS_Buffer const & sourceMapping,
	    RTEHSS_Buffer const & targetMapping,
        SAPDBErr_MessageList &msgList);
private:
    SAPDB_Bool m_MirrorDataLoaded; ///< if set call is loaded

public:
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
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */
 
    SAPDB_Bool MirrorSplit (
	    RTEHSS_Buffer const & sourceMapping,
	    RTEHSS_Buffer const & targetMapping,
        SAPDBErr_MessageList &msgList);
private:
    SAPDB_Bool m_MirrorSplitLoaded; ///< if set call is loaded

public:
    /*!
       @brief    Inform the standby about completion of the data split operation

       Used by standby database server to reenable usage of the mirrored DATA-volumes on the standby
       site. The target mapping information is given. This can be used, i.e. to recreate volume groups
       or mount a filesystem needed to access the DATA-volumes on the standby.

       @param          targetMapping [in] mapping as returned by calls to RTEHSS_GetMapping for DATA-volume array
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).
     */
    SAPDB_Bool MirrorSplitDone (
	    RTEHSS_Buffer const & targetMapping,
        SAPDBErr_MessageList &msgList );
private:
    SAPDB_Bool m_MirrorSplitDoneLoaded; ///< if set call is loaded

public:
    /*!
       @brief    Find out, if data volumes have reached requested synchronization state


  Synchronization state is reached if either mirror is established (what==RTEHSS_CHECK_FOR_MIRROR_ESTABLISHED)
  or until splitting is completed (what==RTEHSS_CHECK_FOR_SPLIT_COMPLETED). If the state is not yet reached, 
  an informational message is send using the standard report callback (see RTEHSS_Init).
       @param          what [in] indicate what the routine should be checking
       @param          done [out] true if synchronized, false otherwise
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */
 
    SAPDB_Bool CheckIfSynchronized( 
        RTEHSS_CheckType       what, 
        SAPDB_Bool           & done,
        SAPDBErr_MessageList & msgList);
private:
    SAPDB_Bool m_CheckIfSynchronizedLoaded; ///< if set call is loaded

public:
    /*!
       @brief    Set the given log volume read only status.


  This is a pure protectional call. It is not absolutely necessary to have the
  LOG-volumes readonly for the backup database server. But if the storage system provides 
  such a feature, that only the master has read/write access and can prevent the backup database
  server from write access, it is usefull to call it. Otherwise this can be a dummy.
  Both master and backup will issue this call.
       @param          localMapping [in] mapping as returned by calls to RTEHSS_GetMapping
       @param          readOnly [in] if false, set to read/write else set to read only.
       @param          msgList [out] set to error if call failed
       @return         true if call, false if not (msgList is used to store information).

     */
 
    SAPDB_Bool SetLogReadOnlyStatus (
	    RTEHSS_Buffer const  & localMapping,
	    SAPDB_Bool             readOnly,
        SAPDBErr_MessageList & msgList);
private:
    SAPDB_Bool m_SetLogReadOnlyStatusLoaded; ///< if set call is loaded

private:
    /*!
       @brief    in case of unuasable api, return the message containing the reason plus the calling member

       The member 'm_NotUsableMessage' is copied and another messagee identifying the calling member function is added.

       @param    member [in] the calling member
       @param    msgList [out] filled with error message
       @return   always false
     */
    SAPDB_Bool ReturnUnasuabilityMessage( SAPDB_Char const *member 
                                        , SAPDBErr_MessageList &msgList );

/*!
       @brief    in case of api call error, return the message containing the reason including the calling member

       @param    member [in] the calling member
       @param    msgList [out] filled with error message
       @return   always false
     */
    SAPDB_Bool ReturnApiCallErrorMessage( SAPDB_Char const *member 
                                        , SAPDBErr_MessageList &msgList );
    /*!
       @brief    construct the singleton passing responsibility to underlying class...
       @param          sharedLibName [in] storage access shared library name
       @param          ok [out] show if constructor was successfull or not
       @param          msgList [inout] filled with error message if ok is false
     */
    RTEHSS_ApiDll( SAPDB_Char const *sharedLibName,
                   SAPDB_Bool &ok,
                   SAPDBErr_MessageList &msgList);

    SAPDB_Bool m_DllNotUsable;        ///< set to true if constructor fails to load DLL
    SAPDBErr_MessageList  m_NotUsableMessage; ///< contains message why DLL is not usable
    static RTEHSS_ApiDll *m_Instance; ///< singleton instance hook
    void *m_ApiHandle;                ///< stored api handle after successfull Init() call
}; /* class RTEHSS_ApiDll */

#endif /* RTEHSS_APIDLL_HPP */
