/*!
  @file           RTEHSS_KernelInterface.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          HotStandbyStorage Kernel Interface
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



#ifndef RTEHSS_KERNELINTERFACE_HPP
#define RTEHSS_KERNELINTERFACE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "RunTime/RTE_Types.h"
#include "RunTime/HotStandbyStorage/RTEHSS_Api.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class    RTEHSS_KernelInterface
  @brief    Access class for HotStandbyStorage

  This interface simplifies the access the HotStandbyStorage functionality.
  The class retrieves all node information from configuration. If no node
  is configured (so called normal configuration) all calls fail, except for
  Instance() and the routines

      CheckForValidHotStandbyConfiguration(configuration, errList)
 
  which returns true and RTEHSS_NormalConfiguration.

  The kernel code must report the 'master' standby role found in the log volume header before
  any write access to the log device. Exception is the 'normal configuration' where this role
  is setup automatically...
 */

class RTEHSS_KernelInterface : public SAPDB_Singleton
{
public:
    /*!
       @brief    Return the signalton instance
       @return   the reference to the RTEHSS_KernelInterface
     */
    static RTEHSS_KernelInterface & Instance();

    /*! HotStandby 'role' enumeration. Kernel code reads log volume header to identify its role and
        uses SetRoleToMaster() or SetRoleToStandby() to copy this knowledge into the RTEHSS_KernelInterface singleton.
     */
    enum Role { RoleUnknown = 0,
                RoleMaster = 1,
                RoleStandby = 2 
    };

    typedef SAPDB_UInt4 LogPosition; ///< numerical type used to store log position

    /*!
      @brief          System view HOTSTANDBYCOMPONENT content
      
      Each hotstandby component has its specific information in this system view. 
      If role is master, the most entries are never set. If role changed from
      standby to master via TAKEOVER, the most entries are used.
     */
    struct Statistic
    {
        /// @brief Initializer
        Statistic()
            : ownRole(RoleUnknown)
            , ownDelayTime(0)
            , timeOfStandbyState(0)
            , timeOfInitStandby(0)
            , timeOfRestartStandby(0)
            , timeOfSynchronize(0)
            , timeOfTakeover(0)
            , timeOfPrepareStandby(0)
            , timeOfRegisterStandby(0)
            , synchronizeReceiveCount(0)
            , lastValidLogOffsetReceived(0)
            , firstNeededLogOffsetReported(0)
        {
            masterNodeName[0] = 0; 
        }

        ///! @name The HOTSTANDBYCOMPONENT details
        /// role in hotstandby configuration (set by SetRoleToMaster or SetRoleToStandby)
        Role         ownRole;               
        /// Uppercase master local node name (set by SetRoleToMaster or SetRoleToStandby)
        RTE_Nodename masterNodeName;        
        /// delay time configured (read from configuration)
        SAPDB_Int4   ownDelayTime;
        /// time STANDBY state was reached (set by SetKernelStateToStandby)
        SAPDB_UInt8  timeOfStandbyState;

        ///! @name The time stamps of the standby node specific commands
        /// time INIT STANDBY was received (set by GotInitStandby)
        SAPDB_UInt8  timeOfInitStandby;     
        /// time RESTART STANDBY was received (set by GotRestartStandby)
        SAPDB_UInt8  timeOfRestartStandby;  
        /// time TAKEOVER was received (set by GotTakeover)
        SAPDB_UInt8  timeOfTakeover;        
        /// time PREPARE STANDBY was send to master (set by SendPrepareStandby)
        SAPDB_UInt8  timeOfPrepareStandby;
        /// time REGISTER STANDBY was send to master (set by SendRegisterStandby)
        SAPDB_UInt8  timeOfRegisterStandby;

        ///! @name The SYNCHRONIZE details (set by GotSynchronize)  
        /// time last SYNCHRONIZE was received from master
        SAPDB_UInt8  timeOfSynchronize;
        /// number of successfull synchronize receives
        SAPDB_Int4   synchronizeReceiveCount;      
        /// last valid log offset received by master
        LogPosition  lastValidLogOffsetReceived;   
        /// log offset needed reported to master to recover from own last savepoint
        LogPosition  firstNeededLogOffsetReported;
    };

    /// Mapping information string encoded as hex digit string for transfer as string parameter
    struct Mapping
    {
        SAPDB_Int4   lengthInBytes; ///< number of character excluding terminating zero
        SAPDB_Char * mappingString; ///< pointer to zero byte terminated hex character string
    };

    /*! HotStandby 'configuration' enumeration. Kernel parameter file defines either a invalid configuration, a valid
        normal configuration or a valid hot standby configuration. A valid hot standby configuration 
        (ConfigurationHotStandby) is given, if the local node name is found as kernel NODE_nnn parameter, the 
        default master is defined (NODE_001) and the mapping of data and log volumes was successfully created. A valid
        normal configuration (ConfigurationNormal) is found, if configuration file defined no nodes and had no
        other inconsistency.
     */
    enum Configuration { ConfigurationInvalid = 0,
                         ConfigurationNormal = 1,
                         ConfigurationHotStandby = 2
    };

    /*!
       @brief    Check if a valid HotStandby configuration is found

       This call does return the result of the check. If configuration is invalid or inconsistent
       it returns false and fills 'errList'. Otherwise 'isHotStandbyConfiguration' either returns
       the result of the check. True means HotStandby configuration, false 'normal'. The decision
       is based on searching NODE_nnn parameters for 'local node' entry. After found, the mapping
       of data volumes is done. If all that succeeded, the configuration is validated...

       Note that a 'normal' configuration is valid too, but not a hot standby configuration.

       @param    found [out] what kind of configuration was found
       @param    errList [out] set to error if call failed
       @return   true if call suceeded (valid configuration found), false if call failed.
     */
    SAPDB_Bool CheckForValidHotStandbyConfiguration( Configuration & found,
                                                     SAPDBErr_MessageList & errList );

    /*!
       @brief    Return content for system view HOTSTANDBYCOMPONENT

       @return   reference of actual statistic record
     */
    Statistic const & GetStatistic() const { return m_Statistic; }

    /*!
       @brief    Return own node name

       @param    ownNodeName [out] filled with own node name (all uppercase)
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
     SAPDB_Bool GetOwnNodeName ( RTE_Nodename & ownNodeName,
                                 SAPDBErr_MessageList & errList );

    /*!
       @brief    Return official node name

       @param    officialNodeName [out] filled with official node name (all uppercase)
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
     SAPDB_Bool GetOfficialNodeName ( RTE_Nodename & officialNodeName,
                                      SAPDBErr_MessageList & errList );

    /*!
      @brief    Return own role as string

      Internally used for readable output of own role

      @return   the string representation of own role
     */
    SAPDB_Char const *GetOwnRoleAsString()
    {
        SAPDB_Char const *roleName;

        switch( m_Statistic.ownRole )
        {
        case RoleUnknown:
            roleName = "UNKNOWN";
            break;

        case RoleMaster:
            roleName = "MASTER";
            break;

        case RoleStandby:
            roleName = "STANDBY";
            break;

        default:
            roleName = "UNKNOWN";
            break;
        }
        return roleName;
    }

    /*!
       @brief    Return the delay time for own node

       This call will return the internal stored delay time retrieved from configuration file.
       @param          delayTime [out] delay time configured for that node name
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
     SAPDB_Bool GetOwnDelayTime ( SAPDB_Int4           & delayTime,
                                  SAPDBErr_MessageList & errList );

    /*!
       @brief    Return the answer to the question if caller is default master

       The default master is simply the node which local node name is identical with node
       name entry HS_NODE_001.
       @param    callerIsDefaultMaster [out] flag set to true if caller is default master
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
     SAPDB_Bool IsDefaultMaster ( SAPDB_Bool           & callerIsDefaultMaster,
                                  SAPDBErr_MessageList & errList );

    /*!
       @brief    Return current recommended log volume open mode

       This call is called by I/O runtime routines to find out, if log volumes have to opened
       read only. Note that in a 'normal configuration' this call will return an error!
       It internally checks the current hot standby role. Calling this function in a not hot
       standby configuration is handled as an error.

       @param    logReadOnly [out] flag set to true if log volumes should be opened read only
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
     SAPDB_Bool IsLogReadOnly( SAPDB_Bool & logReadOnly,
                               SAPDBErr_MessageList & errList );

    /*!
       @brief    Prepare for becoming master

       This sets up internal state of a master node. The caller must be sure he is master.
       He can make sure only by inspecting the LOG-volume header information. If he finds
       himself he is master. If LOG-volume is not initialized, he is only master if the call
       IsDefaultMaster() returns true.
       The routine tries to set LOG-volumes readonly for all standby database server
       and read-write only for the master.If the storage system provides such a feature, 
       it can prevent the standby database server from accidental write access.
       This may call deeper into the storage system cutting of ability for readWrite
       access to LOG-volume completely by disabling the corresponding write channels!
       Both master (during startup) and standby (during TAKEOVER) will issue this call.

       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
     SAPDB_Bool SetRoleToMaster ( SAPDBErr_MessageList & errList );

    /*!
       @brief    Prepare for becoming standby

       This sets up internal role of a standby node. The caller must be sure the node has standby role. 
       Only standby nodes (after inspected the log volume) will issue this call. The role may be changed afterwards
       by a SetMasterRole() call.

       @param    masterNodeName [in] the node name of the master
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
     SAPDB_Bool SetRoleToStandby ( RTE_Nodename const   & masterNodeName,
                                   SAPDBErr_MessageList & errList );

    /*!
      @brief          Notify that a INIT STANDBY command was received for the node

      Updates m_Statistic.timeOfInitStandby
     */
    void GotInitStandby();

    /*!
      @brief          Notify that a RESTART STANDBY command was received for the node

      Updates m_Statistic.timeOfRestartStandby
     */
    void GotRestartStandby();

    /*!
      @brief          Notify that a TAKEOVER command was received for the node

      Updates m_Statistic.timeOfTakeover
     */
    void GotTakeover();

    /*!
      @brief          Notify that a SYNCHRONIZE command was received for the node

      Updates m_Statistic.timeOfSynchronize, synchronizeReceiveCount,
      lastValidLogOffsetReceived and firstNeededLogOffsetReported
     */
    void GotSynchronize(LogPosition lastValidLogOffsetReceived,
                        LogPosition firstNeededLogOffsetReported);

    /*!
       @brief    Set the internal kernel state to 'STANDBY'
       
       This call is issued by the kernel code, if the 'STANDBY' state is reached. That means the 
       'Register Standby' command successfully returned and the kernel is waiting for synchronize
       commands. After 'takeover' a call 'vrestart' can be used to upgrade into 'ONLINE' state.

       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed (errList filled)
     */
    SAPDB_Bool  SetKernelStateToStandby( SAPDBErr_MessageList & errList );

    /*!
       @brief    Get the own mapping info string

      Returns a reference to own mapping info string. This string contains all
      information needed to allow establishing a mirror of all data volumes.
      Mapping info is string build of Hexdigits only [0-9A-F].
      The own mapping info never must be returned. This call is needed since the answer
      is returned for MAPPING command must be placed in 'AUFTRAGS SEGMENT'.
       @param          ownMapping [out] length + hex digits string for mapping of all DATA-volumes
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool GetOwnMappingInfo( Mapping              & ownMapping,
                                  SAPDBErr_MessageList & errList );

    /*!
      @brief          Notify that the data volume configuration has changed

      Updates m_OwnDataVolumesMapping, m_OwnDataVolumesBinaryMapping,

      Should be called only if data volume configuration did change...

      @param          errList [out] set to error if call failed
      @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool UpdateOwnDataVolumeMapping( SAPDBErr_MessageList & errList );

    /*!
      @brief          Notify that the data volume configuration has changed

      Updates m_OwnLogVolumesMapping, m_OwnLogVolumesBinaryMapping,

      Should be called only if logging volume configuration did change...

      @param          errList [out] set to error if call failed
      @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool UpdateOwnLogVolumeMapping( SAPDBErr_MessageList & errList );

    /*!
       @brief Establish the mirror using given mapping info

       @param    otherMapping [in] length + hex digits string for mapping of all DATA-volumes on remote node
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool EstablishMirror( Mapping const & otherMapping,
                                SAPDBErr_MessageList & errList );

    /*!
       @brief Split the previously established mirror using given mapping info

       This routine triggers splitting the previouly established mirror
       of the data volumes. It is expected before this call is used,
       a savepoint is written and all I/O activitiy on data volumes are stalled.
       @param    otherMapping [in] length + hex digits string for mapping of all DATA-volumes on remote node
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool SplitMirror( Mapping const & otherMapping,
                            SAPDBErr_MessageList & errList );

    /*!
       @brief    Get extended synchronization storage state information

       Synchronization means either a mirror is synchronized or a split
       has completed all outstanding I/O. If storage system is synchronized,
       isSynchronized is returned as true. The statusMessage returned is hopefully usefull
       information for the caller to be used as message output.

       If call succeeded caller is responsible to free memory returned as statusMessage if no longer needed. a call to
       FreeMemory(statusMessage, errList) must be used for this purpose.

       @param          otherMapping [in] length + hex digits string for mapping of all DATA-volumes on remote node
       @param          isSynchronized [out] set to true if synchronized, false if not
       @param          statusMessage [out] a textual message usable for status output (must be returned by FreeMemory call)
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed

     */
    SAPDB_Bool GetStorageState( Mapping const        & otherMapping,
                                SAPDB_Bool           & isSynchronized,
                                SAPDB_Char const *   & statusMessage,
                                SAPDBErr_MessageList & errList );
	    

    // Member functions that send commands to other nodes and wait for response...

    /*!
       @brief    Prepare the standby mode establishing

       This routine will retrieve mapping info from other node.

       If call succeeded caller is responsible to free memory returned as Mapping if no longer needed. A call to
       FreeMemory(otherMapping.mappingString, errList) must be used for this purpose.

       @param    otherNode [in] node name to ask for mapping information
       @param    otherMapping [out] length + hex digits string for mapping of all DATA-volumes on remote node
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool SendInfoStandby( RTE_Nodename const   & otherNode,
                                Mapping              & otherMapping,
                                SAPDBErr_MessageList & errList );

    /*!
       @brief    Free the mapping info or status message retrieved by a previuos SendInfoStandby or GetStorageState call

       This call is needed to return the storage consumed by the result of a previous SendInfoStandby or GetStorageState call
       @param    memoryStart [in] memory start
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool FreeMemory( void * memoryStart,
                           SAPDBErr_MessageList & errList );

    /*!
       @brief    Prepare the standby mode establishing

       This routine will first retrieve mapping info from master node.
       After that it will establish the mirror using the retrieved mapping
       and its own mapping. After that the PREPARE STANDBY command is
       send to the master and the call blocks until the answer is received.
       This call is issued by standby nodes only during INIT STANDBY command execution.

       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool SendPrepareStandby( SAPDBErr_MessageList & errList );

    /*!
       @brief    Send command to register standby node into internal list of given nodes

       Send from standby candidate to master instance. If the master cannot register the standby node
       due to its savepoint offset information, the external client must recover...
       @param    lastSavepointOffset [in] savepoint offset information from DATA-volumes of standby node
       @param    lastValidLogOffset [out] last valid offset in LOG-volumes
       @param    errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool SendRegisterStandby ( LogPosition            lastSavepointOffset, 
                                     LogPosition          & lastValidLogOffset,
                                     SAPDBErr_MessageList & errList );

    /*!
       @brief    Send command to insert standby node into internal list of given node
       @param          nodeName [in] name of node to send command to
       @param          newStandbyNodeName [in] name of node now in standby mode
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed

     */
    SAPDB_Bool SendInsertStandby ( RTE_Nodename const   & nodeName,
                                   RTE_Nodename const   & newStandbyNodeName,
                                   SAPDBErr_MessageList & errList );

    /*!
       @brief    Send command to remove standby node into internal list of given node
       @param          nodeName [in] name of node to send command to
       @param          oldStandbyNodeName [in] name of node now in standby mode
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed

     */
    SAPDB_Bool SendRemoveStandby ( RTE_Nodename const   & nodeName,
                                   RTE_Nodename const   & oldStandbyNodeName,
                                   SAPDBErr_MessageList & errList );

    /*!
       @brief    Send synchronize command to exchange LOG-volume synchronization information

       Send from master instance to standby instance.
       @param          standbyNodeName [in] node to send command to
       @param          lastValidLogOffset [in] last valid LOG-volume offset position to transfer
       @param          firstStillNeededOffset [out] first still needed LOG-volume offset of standby node
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed

     */
     SAPDB_Bool SendSynchronize( RTE_Nodename const  & standbyNodeName,
                                LogPosition            lastValidLogOffset,
                                LogPosition          & firstStillNeededOffset,
                                SAPDBErr_MessageList & errList );

    /*!
       @brief    Send backup history info line

       Send from master instance to standby instance.
       @param          standbyNodeName [in] node to send command to
       @param          backupHistoryInfo [in] the info line (if needed quoted externally, not in this routine!)
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed

     */
     SAPDB_Bool SendBackupHistoryInfo( RTE_Nodename const  & standbyNodeName,
                                       SAPDB_Char const    * backupHistoryInfo,
                                       SAPDBErr_MessageList & errList );

    /*!
       @brief    Send backup medium information line

       Send from master instance to standby instance.
       @param          standbyNodeName [in] node to send command to
       @param          backupMediumInfo [in] the info line (if needed quoted externally, not in this routine!)
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed

     */
     SAPDB_Bool SendBackupMediumInfo( RTE_Nodename const  & standbyNodeName,
                                      SAPDB_Char const    * backupMediumInfo,
                                      SAPDBErr_MessageList & errList );

private:
    /*!
       @brief    The constructor

       Issued only once for creation of singleton
     */
    RTEHSS_KernelInterface();

    /*!
       @brief    The runtime internal initialize call

       Issued separated from constructor to allow singleton creation error to be reported
       @param          errList [out] set to error if call failed
       @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool Initialize(SAPDBErr_MessageList &errList);
    static RTEHSS_KernelInterface * m_Instance;              ///< singleton instance pointer

    /*!
      @brief          Notify that the data volume configuration has changed

      Updates m_OwnDataVolumesMapping, m_OwnDataVolumesBinaryMapping,

      Should be called only if data volume configuration did change...

      @param          errList [out] set to error if call failed
      @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool GetOwnDataVolumeMapping( SAPDBErr_MessageList & errList );

    /*!
      @brief          Notify that the data volume configuration has changed

      Updates m_OwnLogVolumesMapping, m_OwnLogVolumesBinaryMapping,

      Should be called only if logging volume configuration did change...

      @param          errList [out] set to error if call failed
      @return   true if call suceeded, false if call failed
     */
    SAPDB_Bool GetOwnLogVolumeMapping( SAPDBErr_MessageList & errList );

    SAPDB_Bool             m_IsInitialized;                  ///< false while Initialize() was never called
    SAPDB_Bool             m_IsDefaultMaster;                ///< flag to indicate node is default master node
    Mapping                m_OwnDataVolumesMapping;          ///< externally used (hex digit string)
    RTEHSS_Buffer          m_OwnDataVolumesBinaryMapping;    ///< internally used (binary representation)
    Mapping                m_OwnLogVolumesMapping;           ///< externally used (hex digit string)
    RTEHSS_Buffer          m_OwnLogVolumesBinaryMapping;    ///< internally used (binary representation)
    Statistic              m_Statistic;                      ///< hotstandby component system view content
}; 

/*!
  Following definitions are used by RunTime environment internally to transfer information needed
  to 'overwrite' the default call for server from server communication.

  Helper routines allow to specify the 'client' program and its argument passing convention

  The clientProgram is the name of the client program without the path to it. It is always searched 
  in <indepPrograms>/bin !

  The client program is expected to use 'Tools_PipeCall::ReadArguments()' to get the correct
  argument line.

  All Parameters are passed via 'pipe' !

  Parameters are transferred individual. The expected separation character is always a space.
  Example: To specify a node two argument are needed -n and the node itself. The 
  corresponding 'nodeOptionFormat' would be "-n %s"
  
  i.e. for 'dbmcli' the correct values are
  
    "dbmcli" as clientProgram
    "-u dbx,dbx" as authorization (dbx,dbx is not neccessary correct...
    "-d %s" as the database selection option 'databaseOption' dbname filled after known
    "-n %s" as the node option 'nodeOptionFormat' node filled after known
    "db_execute %s %s %s" as the execute call option. Three arguments are given (command,arg0,arg1)

 */

/*!
  @class RTEHSS_IServerToServerCall
  @brief Class that allows to prepare server to server call command line and argument vector

  The client application is expected to use option -SDB_PARAMSVIAPIPE

  The pipe name used for argument transfer is used on Windows only. On unix a anonymous
  pipe is used, which is created on demand. For that reason the commandLineFormatString
  on Unix contains a '%d' to allow to fill in the right pipe descriptor.

  The output pipe name returned is used on Windows only. On unix the 'stdout' is redirected.
 */
class RTEHSS_IServerToServerCall
{
public:
    /*!
      @brief initialization

      Allow the client call to initialize itself (i.e. prepare authorization, check pathes etc..)

      Called only once.

      @param errList [out] filled in case of problems
      @return true client call is usable, false failed...
     */
    virtual SAPDB_Bool Initialize(SAPDBErr_MessageList &errList) = 0;

    /*!
      @brief Prepare argument vector and command line

      To the commandLineFormatString returned is added the '-SDB_PARAMSVIAPIPE option'.
      The output on Windows is reditected to a named pipe while on UNIX redirection uses
      an anonymous pipe.
      The argumentPipeName is added on Win32 while on UNIX the filedescriuptor number
      of an anonymous pipe is used.
      Therefore on UNIX 'outputPipeName and argumentPipeName are ignored. On Windows the
      naming convention '//./pipe/HS_dbName_nodeName' and '//./pipe/ARG_dbName_nodeName'
      are normally used.
      
      @param nodeName [in] the node name used to build argument vector
      @param dbName   [in] the database name used to build argument vector
      @param command  [in] the command used to build argument vector
      @param arg0     [in] the first argument used to build argument vector (may be 0)
      @param arg1     [in] the second argument used to build argument vector (may be 0)
      @param commandLineFormatString [out] the command line format string to use
      @param argumentCount [out] the total arguments needed
      @param arguments     [out] the argument vector
      @param outputPipeName [out] optional output pipe name (Win32 only)
      @param argumentPipeName [out] optional argument pipe name (Win32 only)
      @param errList [out] filled in case of problems
      @return true client is prepared, false failed...
     */
    virtual SAPDB_Bool PrepareCall( SAPDB_Char const *nodeName
                                  , SAPDB_Char const *dbName
                                  , SAPDB_Char const *command
                                  , SAPDB_Char const *arg0
                                  , SAPDB_Char const *arg1
                                  , SAPDB_Char *  &commandLineFormatString
                                  , SAPDB_Int4    &argumentCount
                                  , SAPDB_Char ** &arguments
                                  , SAPDB_Char *  &outputPipeName
                                  , SAPDB_Char *  &argumentPipeName
                                  , SAPDBErr_MessageList &errList) = 0;

    /*!
      @brief release resources allocated during last 'PrepareCall'

      @param commandLineFormatString [in] the command line format string to use
      @param argumentCount [in] the number of arrgument set in argument vector
      @param arguments     [in] the argument vector
      @param outputPipeName [in] optional output pipe name (Win32 only)
      @param argumentPipeName [in] optional arguement pipe name (Win32 only)
      @return none
     */
    virtual void FreeCallResources( SAPDB_Char *  commandLineFormatString
                                  , SAPDB_Int4    argumentCount
                                  , SAPDB_Char ** arguments
                                  , SAPDB_Char *  outputPipeName
                                  , SAPDB_Char *  argumentPipeName ) = 0;
}; /*! @endclass RTEHSS_IServerToServerCall */

/*!
  @brief Routine to overwrite existing client call interface
  @param serverCall [in] the client call object
  @param errList [out] filled in case of problems
  @return true if new client call is accepted, false if not
 */
SAPDB_Bool RTEHSS_SetupServerToServerCall( RTEHSS_IServerToServerCall &serverCall
                                         , SAPDBErr_MessageList &errList);

#endif /* RTEHSS_KERNELINTERFACE_HPP */
