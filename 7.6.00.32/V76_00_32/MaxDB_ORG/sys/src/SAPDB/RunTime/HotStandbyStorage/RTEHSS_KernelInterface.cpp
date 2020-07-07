/*!
  @file           RTEHSS_KernelInterface.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          RTEHSS Implementation kernel interface

    This interface tries to make usage of HotStandbyStorage routines easier, but collecting
    them into a singleton.

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

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_KernelInterface.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_ApiDll.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_HSS_Messages.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/System/RTESys_CommandPipe.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/HotStandbyStorage/RTEHSS_StandbyNodeList.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp" // only used as interface definition
#include "geo002.h"
#include "heo01.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/* at least one master and one standby */
#define RTEHSS_PAN_DEFAULT_MASTER_NODE  KERNELPARAM_HS_NODE_ "001"
#define RTEHSS_MINIMUM_NEEDED_NODES 2 
#define RTEHSS_CONTEXT "RTEHSS",__FILE__,__LINE__ 
#define RTEHSS_DEFAULT_MAPPING_SIZE 4096
#define RTEHSS_MAX_REPLY_BUFFER_SIZE (64*1024)

#if defined(WIN32)
#define RTEHSS_COMMAND_OUTPUT_PIPE "\\\\.\\pipe\\HS_%s_%s"
#define RTEHSS_COMMAND_OUTPUT_PIPE_LENGTH 13 /* strlen("\\\\.\\pipe\\HS_") */
#define RTEHSS_COMMAND_ARGUMENT_PIPE "\\\\.\\pipe\\ARG_%s_%s"
#define RTEHSS_COMMAND_ARGUMENT_PIPE_LENGTH 14 /* strlen("\\\\.\\pipe\\ARG_") */
#define RTEHSS_DEFAULT_KERNEL_TO_KERNEL_CALL "dbmcli.exe " SAPDB_OPT_PIPE " "
#else
#include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#define RTEHSS_DEFAULT_KERNEL_TO_KERNEL_CALL "dbmcli " SAPDB_OPT_PIPE " %d"
#endif

//---------------------------------------------
#include "heo51.h" // for vgetpid only
#include "RunTime/Tasking/RTETask_IWorkerJob.hpp"

/*!
  a reply handler is called to investigate to reply buffer content filled by answer to 
  server to server call.
 */
typedef SAPDB_Bool (*RTEHSS_ReplyHandler)(SAPDB_Char const *command,
                                       SAPDB_Char const *reply,
                                       void             *replyContext,
                                       SAPDBErr_MessageList &errList);

extern "C"
{
    /*!
      @brief password decrypt routine
      @param clearname [out] decrypted name buffer
      @param crypt [in] the crypt input
     */
    void s02decrypt (
            tsp00_Name          & clearname,
            const tsp00_CryptPw & crypt);
};

/*!
  @class RTEHSS_Helper
  @brief Hhis class is simply a collection of helper routines
 */
class RTEHSS_Helper
{
public:
    /*!
      @brief allocate memory and return it set to zero
      @param what [in] descriptive text for memory region
      @param bytes [in] number of bytes requested
      @param errList [out] errorlist filled if memory not allocated
      @return 0 if call failed, pointer to allocated memory else
     */
    static void *        AllocateAndClear( SAPDB_Char const *what,
                                        SAPDB_ULong bytes,
                                        SAPDBErr_MessageList &errList);

    /*!
      @brief deallocate memory allocated by a previous AllocateAndClear call
      @param ptr [in] memory to free
     */
    static void          Deallocate( void *ptr );

    /*!
      @brief allocate and fill an area with the data volumes currently defined for database

      If mirrored data volumes are found they are returned too. The returned number of volumes
      is the total of data and mirrored data volumes.
      @param numberOfDataVolumes [out] number of volumes found
      @param errList [out] errorlist filled if memory not allocated
      @return 0 if call failed, pointer to allocated array else
     */
    static SAPDB_Char ** AllocateAndFillDataVolumeArray(SAPDB_Int4 &numberOfDataVolumes, 
                                                            SAPDBErr_MessageList &errList);

    /*!
      @brief allocate and fill an area with the log volumes currently defined for database

      If mirrored log volumes are found they are returned too. The returned number of volumes
      is the total of log and mirrored log volumes.
      @param numberOfLogAndMirroredLogVolumes [out] number of volumes found
      @param errList [out] errorlist filled if memory not allocated
      @return 0 if call failed, pointer to allocated array else
     */
    static SAPDB_Char ** AllocateAndFillLogVolumeArray(SAPDB_Int4 &numberOfLogAndMirroredLogVolumes, 
                                                            SAPDBErr_MessageList &errList);

    /*!
      @brief fill binary mapping buffer from volume array
      @param mappingBuffer [out] buffer for binary representation
      @param numberOfVolumes [in] the number of volumes in volumeArray
      @param volumeArray [in] the array with the volume names
      @param errList [out] errorlist filled if memory not allocated
      @return false if call failed, true else
     */
    static SAPDB_Bool    GetMappingForVolumeArray( RTEHSS_Buffer &mappingBuffer,
                                                   SAPDB_Int4 numberOfVolumes,
                                                   SAPDB_Char ** volumeArray,
                                                   SAPDBErr_MessageList & errList);

    /*!
      @brief fill hex digit string mapping from binary representation

      @param mapping [out] hex digit representation buffer to fill
      @param mappingBuffer [in] buffer with binary representation
      @param errList [out] errorlist filled if memory not allocated
      @return false if call failed, true else
     */
    static SAPDB_Bool    ConvertToHex( RTEHSS_KernelInterface::Mapping &mapping,
                                       RTEHSS_Buffer const &mappingBuffer,
                                       SAPDBErr_MessageList &errList);

    /*!
      @brief fill binary representation from hex digit string mapping

      @param mappingBuffer [out] buffer with binary representation
      @param mapping [in] hex digit representation buffer to fill
      @param errList [out] errorlist filled if memory not allocated
      @return false if call failed, true else
     */
    static SAPDB_Bool    ConvertFromHex( RTEHSS_Buffer &mappingBuffer,
                                         RTEHSS_KernelInterface::Mapping const &mapping,
                                         SAPDBErr_MessageList &errList);

    /*!
      @brief convert hex digit into number

      If valid hexdigit (0-9a-fA-F) the corresponding value is returned otherwise 0.

      '0' - '9' equals 0 - 9,  'a'/'A' - 'f'/'F' equals 10 - 15...

      @param hexDigit [in] digit to convert
      @return number represented by the digit or 0 if no hex digit
     */
    static inline SAPDB_Byte ToBin( SAPDB_Char hexDigit )
    {
        switch( hexDigit )
        {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'a': case 'A':
            return 10;
        case 'b': case 'B':
            return 11;
        case 'c': case 'C':
            return 12;
        case 'd': case 'D':
            return 13;
        case 'e': case 'E':
            return 14;
        case 'f': case 'F':
            return 15;
        default:
            return 0;
        }
    }

    /*!
      @brief send the call to given node

      If valid hexdigit (0-9a-fA-F) the corresponding value is returned otherwise 0.

      '0' - '9' equals 0 - 9,  'a'/'A' - 'f'/'F' equals 10 - 15...

      @param nodename [in] the node to call
      @param command [in] the command line
      @param arg0 [in] optional command line argument 0 (use 0 if not used)
      @param arg1 [in] optional command line argument 1 (use 0 if not used)
      @param replyHandler [in] the command reply handler
      @param replyContext [in] the command reply context
      @param errList [out] error list filled if error occured
      @return true if call succeeded, false else
     */
    static SAPDB_Bool    SendCall( SAPDB_Char const     * nodename,
                                   SAPDB_Char const     * command,
                                   char const           * arg0,
                                   char const           * arg1,
                                   RTEHSS_ReplyHandler    replyHandler,
                                   void                 * replyContext,
                                   SAPDBErr_MessageList & errList);
};

/*!
  @class Default_ServerToServerCall
  @brief default implementation for server to server call
 */
class Default_ServerToServerCall: public RTEHSS_IServerToServerCall
{
public:
    /*!
      @brief ctor

      empty
     */
    Default_ServerToServerCall()
    : m_CommandLine(0)
    , m_CryptLength(0)
#if defined(WIN32)
    { m_CryptKey = (SAPDB_UInt2)((KGS->ulCurrTime)%60000); }
#else
    { m_CryptKey = (SAPDB_UInt2)((KGS->start_time)%60000); }
#endif
    /*!
      @brief initialization

      Allow the client call to initialize itself (i.e. prepare authorization, check pathes etc..)

      Called only once.

      @param errList [out] filled in case of problems
      @return true client call is usable, false failed...
     */
    virtual SAPDB_Bool Initialize(SAPDBErr_MessageList &errList)
    {
        SAPDB_Bool result;
        RTEConf_Parameter::String whichControlUser;
        result = RTEConf_ParameterAccess::Instance()->GetString( (SAPDB_UTF8 const *)PAN_CONTROLID,
                                                                 whichControlUser,
                                                                 errList);
        if ( !result )
        {
            return false;
        }

        RTEConf_Parameter::CryptInfo whichControlCrpytedPassword;
        result = RTEConf_ParameterAccess::Instance()->GetCryptInfo((SAPDB_UTF8 const *)PAN_CONTROLPW,
                                                               whichControlCrpytedPassword,
                                                               errList);
        if ( !result )
        {
            return false;
        }

        tsp00_Pathc   dbmcliPath;
        tsp01_RteError rteError;

#if defined(WIN32)
        if ( !sqlGetIndependentPgmPath( dbmcliPath,
                                        TERM_WITH_DELIMITER_EO01,
                                        &rteError ) )
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_BAD_CLIENT_COMMAND_STRING,
                                            "Failed to get independent pgm path" );
            return false;
        }
#else
        if ( !sqlGetIndependentBinPath( dbmcliPath,
                                        TERM_WITH_DELIMITER_EO01,
                                        &rteError ) )
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_BAD_CLIENT_COMMAND_STRING,
                                            "Failed to get independent bin path" );
            return false;
        }
#endif

        m_CommandLine = (SAPDB_Char *) RTEHSS_Helper::AllocateAndClear("CommandLine",
               strlen(dbmcliPath.asCharp()) 
             + strlen(RTEHSS_DEFAULT_KERNEL_TO_KERNEL_CALL)
             + 1, errList );
        if ( !m_CommandLine )
        {
            return false;
        }

        strcpy( m_CommandLine, dbmcliPath.asCharp() );
        strcat( m_CommandLine, RTEHSS_DEFAULT_KERNEL_TO_KERNEL_CALL );

        tsp00_Namec   whichControlPassword;
        tsp00_Name    pascalPassword;
        tsp00_CryptPw cryptedPw;

        SAPDB_MemCopyNoCheck( &cryptedPw[0], &whichControlCrpytedPassword[0], sizeof(RTEConf_Parameter::CryptInfo));

        s02decrypt( pascalPassword, cryptedPw);
        SAPDB_MemFillNoCheck( &cryptedPw[0], 0, sizeof(RTEConf_Parameter::CryptInfo));

        whichControlPassword.p2c(pascalPassword);
        SAPDB_MemFillNoCheck(pascalPassword.asCharp(), 0, sizeof(tsp00_Name)); // reset trace on stack...

        if ( !Crypt( (SAPDB_Char *)whichControlUser
                   , whichControlPassword.asCharp()
                   , errList ) )
        {
            RTEHSS_Helper::Deallocate(m_CommandLine);
            m_CommandLine = 0;
            return false;
        }
        return true;
    }


    /*!
      @brief Prepare argument vector and command line

      @param nodeName [in] the node name used to build argument vector
      @param dbName   [in] the database name used to build argument vector
      @param command  [in] the command used to build argument vector
      @param arg0     [in] the first argument used to build argument vector (may be 0)
      @param arg1     [in] the second argument used to build argument vector (may be 0)
      @param commandLineFormatString [out] the command line format string to use
      @param argumentCount [out] the total arguments needed
      @param arguments     [out] the argument vector
      @param outputPipeName [out] optional output pipe name
      @param argumentPipeName [out] optional argument pipe name
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
                                  , SAPDBErr_MessageList &errList);

    /*!
      @brief release resources allocated during last 'PrepareCall'

      @param commandLineFormatString [in] the command line format string to use
      @param argumentCount [in] the total arguments set
      @param arguments     [in] the argument vector
      @param outputPipeName [in] optional output pipe name
      @param argumentPipeName [in] optional arguement pipe name
      @return none
     */
    virtual void FreeCallResources( SAPDB_Char *  commandLineFormatString
                                  , SAPDB_Int4    argumentCount
                                  , SAPDB_Char ** arguments
                                  , SAPDB_Char *  outputPipeName
                                  , SAPDB_Char *  argumentPipeName );

private:
    SAPDB_Char * m_CommandLine;     ///< command line to use (identical for all commands
    SAPDB_UInt2 * m_AuthorizeCopy;   ///< crypted copy of authorization
    SAPDB_UInt2   m_CryptKey;        ///< crypt salt key
    SAPDB_Int4   m_CryptLength;     ///< length of crypt information
    SAPDB_Bool Crypt(SAPDB_Char *username, SAPDB_Char *password, SAPDBErr_MessageList &errList ); ///< simple crypt for authorize copy
    SAPDB_Char *Uncrypt( SAPDBErr_MessageList &errList ); ///< simple uncrypt for authorize copy
};

/*!
  @class RTEHSS_OtherKernelCall
  @brief class implementing calls to other kernels
 */
class RTEHSS_OtherKernelCall : public SAPDB_Singleton
{
public:
    /*!
      @brief get the instance referece for this singleton

      If called for the very first time, the singleton is initialized by this member function.
      @return the single instance reference
     */
    static RTEHSS_OtherKernelCall & Instance();

    /*!
      @brief setup the common part of server to server call

      For each server to server call, a common command line is used (including authorization). To camouflage it,
      it is stored inverted.

      @param serverCall [in] the server to server call interface
      @param errList [out] error message if call failed
      @return true if call succeeded, false else (errList filled)
     */
    SAPDB_Bool Initialize( RTEHSS_IServerToServerCall &serverCall,
                           SAPDBErr_MessageList &errList);

    /*!
      @brief construct the command line and send the command

      For each command a command pipe is opened and a reply buffer is allocated. After constructing the command line,
      the command pipe is opened, the reply is retrieved and after that the replyHandler is called to interpret the 
      reply.
      @param nodename [in] the node to send to ( own host will be rejected )
      @param command [in] the command format string
      @param arg0 [in] first arguemtn for command or NULL if no first argument
      @param arg1 [in] second argument for command or NULL if no second argument
      @param replyHandler [in] the handler that interprets the reply
      @param replyContext [in] the context hook passed to the replyHandler
      @param errList [out] error message if call failed
      @return true if call succeeded, false else (errList filled)
     */
    SAPDB_Bool SendCall( SAPDB_Char const     * nodename,
                         SAPDB_Char const     * command,
                         char const           * arg0,
                         char const           * arg1,
                         RTEHSS_ReplyHandler    replyHandler,
                         void                 * replyContext,
                         SAPDBErr_MessageList & errList);

#if !defined(WIN32)
    /*!
      @brief fill reply buffer with answer

      Reply buffer is allocated for each command individually and is extended by this function if too small
      @param cmdOutput [in] the file handle for the result pipe
      @param replyBuffer [out] the reply buffer filled
      @param replyBufferLengthInBytes [in] the preallocated size
      @param errList [out] error message if call failed
      @return true if call succeeded, false else (errList filled)
     */
    SAPDB_Bool FillReplyBuffer( RTE_FileHandle & cmdOutput,
                                SAPDB_Char *   & replyBuffer,
                                SAPDB_Int4       replyBufferLengthInBytes,
                                SAPDBErr_MessageList &errList );
#else /* WIN32 only */
    /*!
      @brief fill reply buffer with answer from named pipe

      Reply buffer is allocated for each command individually and is extended by this function if too small.
      The process is constantly monitored during reply handling to abort unwanted waiting if process is already dead.

      @param processHandle [in] the handle for the started process
      @param pipeHandle [in] the handle for the result named pipe
      @param replyBuffer [out] the reply buffer filled
      @param replyBufferLengthInBytes [in] the preallocated size
      @param errList [out] error message if call failed
      @return true if call succeeded, false else (errList filled)
     */
    SAPDB_Bool FillReplyBufferFromNamedPipe( HANDLE           processHandle,
                                             HANDLE           pipeHandle,
                                             SAPDB_Char *   & replyBuffer,
                                             SAPDB_Int4       replyBufferLengthInBytes,
                                             SAPDBErr_MessageList &errList );
#endif
private:
    /*!
      @brief singleton constructor (private)
     */
    RTEHSS_OtherKernelCall();

    /*!
      @brief Allocated and fill command string plus parameters for kernel to kernel call

      @param nodeName [in] the node the command is send to
      @param dbName [in] the database the command is send to
      @param command [in] the command itself
      @param arg0 [in] optional first argument or NULL
      @param arg1 [in] optional second argument or NULL
      @param commandLineFormatString [out] the command line to use
      @param cmdArgc [out] command argument count
      @param cmdArgv [out] command argument vector
      @param outputPipeName [out] the name of the output pipe if any
      @param argumentPipeName [out] the name of the argument pipe if any
      @param errList [out] filled with error, if error found
      @return true if call succeeded, false if falied(errList filled)
     */
    SAPDB_Bool PrepareCommandLine( SAPDB_Char const * nodeName
                                 , SAPDB_Char const * dbName
                                 , SAPDB_Char const * command
                                 , SAPDB_Char const * arg0
                                 , SAPDB_Char const * arg1
                                 , SAPDB_Char      *    & commandLineFormatString
                                 , SAPDB_Int4           & cmdArgc
                                 , SAPDB_Char      **   & cmdArgv
                                 , SAPDB_Char      *    & outputPipeName
                                 , SAPDB_Char      *    & argumentPipeName
                                 , SAPDBErr_MessageList & errList )
    {
        return ( m_ServerCall->PrepareCall( nodeName
                                          , dbName
                                          , command
                                          , arg0
                                          , arg1
                                          , commandLineFormatString
                                          , cmdArgc
                                          , cmdArgv
                                          , outputPipeName
                                          , argumentPipeName
                                          , errList) );
    }

    /*!
      @brief Release resources allocated for command line
      @param commandLineFormatString [in] command line
      @param cmdArgc [in] command argument count
      @param cmdArgv [in] command argument vector
      @param outputPipeName [in] the name of the output pipe if any
      @param argumentPipeName [in] the name of the argument pipe if any
      @return none
     */
    void FreeCommandLine( SAPDB_Char      *  commandLineFormatString
                        , SAPDB_Int4         cmdArgc
                        , SAPDB_Char      ** cmdArgv
                        , SAPDB_Char      *  outputPipeName
                        , SAPDB_Char      *  argumentPipeName )
    {
        return ( m_ServerCall->FreeCallResources( commandLineFormatString
                                                , cmdArgc
                                                , cmdArgv
                                                , outputPipeName
                                                , argumentPipeName ) );
    }

    static RTEHSS_OtherKernelCall * m_Instance;        ///< Singleton hook, initialized by Instance() member function
    SAPDB_Bool        m_IsInitialized;                 ///< set to true by calling RTEHSS_InitializeServerToServerCall()...
    SAPDB_Bool        m_InitializeResult;              ///< cached result of RTEHSS_InitializeServerToServerCall()...
    SAPDBErr_MessageList m_InitializeErrList;          ///< cached result of RTEHSS_InitializeServerToServerCall()...
    RTEHSS_IServerToServerCall *m_ServerCall;          ///< the server to server call class
};

//--------------------------------

/*!
  @class SendCallJob
  @brief implenets worker job for send calls
 */
class SendCallJob : public RTETask_IWorkerJob
{
public:
    SendCallJob( SAPDB_Char const *calledNodeName,
                 SAPDB_Char const *command,
                 char const * arg0,
                 char const * arg1,
                 RTEHSS_ReplyHandler replyHandler,
                 void *replyContext,
                 SAPDBErr_MessageList &errList)

                : m_CalledNodeName(calledNodeName)
                , m_Command(command)
                , m_Arg0(arg0)
                , m_Arg1(arg1)
                , m_ReplyHandler(replyHandler)
                , m_ReplyContext(replyContext)
                , m_ErrList(errList)
                , m_Result(true)
    {
        vgetpid(m_OwnId);
    }

    virtual RTE_TaskId const GetTaskId() const { return m_OwnId; }

    virtual void DoJob()
    {
        m_Result = RTEHSS_OtherKernelCall::Instance().SendCall(m_CalledNodeName,
                                                               m_Command,
                                                               m_Arg0,
                                                               m_Arg1,
                                                               m_ReplyHandler,
                                                               m_ReplyContext,
                                                               m_ErrList);
    }

    SAPDB_Bool GetResult() { return m_Result; }
private:
    RTE_TaskId m_OwnId;
    SAPDB_Char const *m_CalledNodeName;
    SAPDB_Char const *m_Command;
    char const * m_Arg0;
    char const * m_Arg1;
    RTEHSS_ReplyHandler m_ReplyHandler;
    void *m_ReplyContext;
    SAPDBErr_MessageList &m_ErrList;
    SAPDB_Bool m_Result;
};

class Sql4ByteHexString
{
public:
    Sql4ByteHexString(SAPDB_UInt4 number)
    {
        SAPDB_Byte *pByte = (SAPDB_Byte *)&number;
        m_hexString[0] = 'x';
        m_hexString[1] = '\'';
        m_hexString[2] = "0123456789ABCDEF"[(pByte[0]&0x0F0)>>4];
        m_hexString[3] = "0123456789ABCDEF"[(pByte[0]&0x0F)];
        m_hexString[4] = "0123456789ABCDEF"[(pByte[1]&0x0F0)>>4];
        m_hexString[5] = "0123456789ABCDEF"[(pByte[1]&0x0F)];
        m_hexString[6] = "0123456789ABCDEF"[(pByte[2]&0x0F0)>>4];
        m_hexString[7] = "0123456789ABCDEF"[(pByte[2]&0x0F)];
        m_hexString[8] = "0123456789ABCDEF"[(pByte[3]&0x0F0)>>4];
        m_hexString[9] = "0123456789ABCDEF"[(pByte[3]&0x0F)];
        m_hexString[10] = '\'';
        m_hexString[11] = 0;
    }

    SAPDB_Char *asCharp() { return &m_hexString[0]; }
private:
    SAPDB_Char m_hexString[sizeof(SAPDB_Int4)*2 + 4];
};

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
// RTEHSS_ReplyHandler expection an OK
static SAPDB_Bool
RTEHSS_ExpectOk( SAPDB_Char const *command,
                 SAPDB_Char const *reply,
                 void *,
                 SAPDBErr_MessageList &errList);

// RTEHSS_ReplyHandler expection an OK and a number on the next line
static SAPDB_Bool
RTEHSS_ExpectOkAndUInt4( SAPDB_Char const *command,
                         SAPDB_Char const *reply,
                         void *replyContext,
                         SAPDBErr_MessageList &errList);

// RTEHSS_ReplyHandler expection an OK and a mapping info as hex digits on the next line
static SAPDB_Bool
RTEHSS_ExpectOkAndMappingInfo( SAPDB_Char const *command,
                               SAPDB_Char const *reply,
                               void *replyContext,
                               SAPDBErr_MessageList &errList);

RTEHSS_OtherKernelCall * RTEHSS_OtherKernelCall::m_Instance = (RTEHSS_OtherKernelCall *)0;

RTEHSS_OtherKernelCall & RTEHSS_OtherKernelCall::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEHSS_OtherKernelCall));
  
    if ( !RTEHSS_OtherKernelCall::m_Instance )
    {
        RTEHSS_OtherKernelCall::m_Instance = new (Space) RTEHSS_OtherKernelCall();
    }
    return *m_Instance;
}

RTEHSS_KernelInterface * RTEHSS_KernelInterface::m_Instance = (RTEHSS_KernelInterface *)0;

RTEHSS_KernelInterface & RTEHSS_KernelInterface::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEHSS_KernelInterface));
  
    if ( !RTEHSS_KernelInterface::m_Instance )
    {
        RTEHSS_KernelInterface::m_Instance = new (Space) RTEHSS_KernelInterface();
    }
    return *m_Instance;
}

//---------------------------------------------

RTEHSS_KernelInterface::RTEHSS_KernelInterface() 
: m_IsInitialized(false),
  m_IsDefaultMaster(false),
  m_Statistic()
{
}

extern "C" {

void myRTEHSS_ReportCallback(RTEHSS_ReportType type,
                             SAPDB_Char const *msg)
{
    SAPDBErr_MessageList messageList;

    switch(type)
    {
    case RTEHSS_ERROR:
        messageList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_API_MESSAGE,
                                        msg
                                       );
        break;
    case RTEHSS_WARNING:
        messageList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEWARN_HSS_API_MESSAGE,
                                        msg
                                       );
        break;
    case RTEHSS_INFO:messageList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEINFO_HSS_API_MESSAGE,
                                        msg
                                       );
        break;
    default:
        messageList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_BAD_API_MESSAGE,
                                        SAPDB_ToString(type),
                                        msg
                                       );
        break;
    }
    RTE_Message(messageList);
}


};

//---------------------------------------------
// Read "HS_MAX_NODES" from configuration and after that
// read HS_NODE_001.... and HS_DELAY_TIME_001... into allocatoed arrays
// Now the local HostName is searched in the allocated node name array.
// If no match is found, configuration is invalid too!!!
// Last check is to check and eliminate duplicates in the array.
//
SAPDB_Bool
RTEHSS_KernelInterface::Initialize (
    SAPDBErr_MessageList & returnErrList )
{
    SAPDBErr_MessageList errList;

    if ( !m_IsInitialized )
    {
        SAPDB_Bool  result;

        // get own delay time and verify configuration
        SAPDB_Bool  isAnyNodeConfigured;
        RTEConf_Parameter::Integer ownDelayTime;

        result = RTEConf_ParameterAccess::Instance()->GetHssConfiguration( isAnyNodeConfigured,
                                                                            ownDelayTime,
                                                                            errList);
        if ( result )
        {
            if ( !isAnyNodeConfigured )
            {
                errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                RTEINFO_HSS_NO_NODE_CONFIGURED );  // no hss configuration
                result = false;
            }
            else
            {
                if ( m_Statistic.ownDelayTime < 0 ) // invalid delay time defaults to no delay 
                {
                    m_Statistic.ownDelayTime = 0;
                }
                else
                {
                    m_Statistic.ownDelayTime = (SAPDB_Int4)ownDelayTime;
                }
            }
        }

#if defined(WIN32)
        SAPDB_Char const *szServerDB = RTE_KGS::Instance().GetKGS()->szServerDB;
#else
        SAPDB_Char const *szServerDB = RTE_KGS::Instance().GetKGS()->serverdb;
#endif
        if ( result )
        {
            RTEHSS_ApiDll & api = RTEHSS_ApiDll::Instance();
            // initialize RTEHSS ApiDll
            result = api.Init( szServerDB,
                               getenv("RTEHSS_VERBOSE") ? RTEHSS_VERBOSE_ALL : RTEHSS_VERBOSE_DEFAULT,
                               myRTEHSS_ReportCallback,
                               errList );
        }

        // initialize Kernel To Kernel Call comand line format string
        if ( result )
        {
            static Default_ServerToServerCall dbmcliCaller;
            result = RTEHSS_OtherKernelCall::Instance().Initialize( dbmcliCaller
                                                                  , errList);
        }

        if ( result )
        {
            SAPDB_Char const * ownNodeName = RTE_ISystem::Instance().GetLocalNodeName();
            if ( strlen(ownNodeName) >= sizeof(RTE_Nodename) )
            {
                errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                RTEERR_HSS_LOCAL_NODE_TOO_LONG, // ownNodeName too long!!!
                                                ownNodeName );
                result = false;
            }
        }

        // set flag of default master node
        if ( result )
        {
            RTEConf_Parameter::String whichNodename;
            result = RTEConf_ParameterAccess::Instance()->GetString( RTEHSS_PAN_DEFAULT_MASTER_NODE,
                                                                     whichNodename,
                                                                     errList);
            if ( !result )
            {
                RTE_Message(errList);

                if ( RTEERR_CONF_NOT_FOUND_ID == errList.ID() )
                {
                    errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                    RTEERR_HSS_DEFAULT_MASTER_NODE_MISSING ); // no hss configuration
                }
            }
            else
            {
                m_IsDefaultMaster = ( 0 == strcmp(RTE_KGS::Instance().GetStatistic().localNodeName, (SAPDB_Char *)whichNodename) );
            }
        }

        // Create mapping for data volumes
        if ( result )
        {
            m_OwnDataVolumesBinaryMapping.buffer = 0;
            m_OwnDataVolumesMapping.mappingString = 0;

            result = GetOwnDataVolumeMapping(errList);
        }

        // Create mapping for log volumes
        if ( result )
        {
            m_OwnLogVolumesBinaryMapping.buffer = 0;
            m_OwnLogVolumesMapping.mappingString = 0;

            result = GetOwnLogVolumeMapping(errList);
        }

        if ( !result )
        {
            KGS->isHotStandbyConfiguration = false;
        }
        else
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEINFO_HSS_VALID_CONFIGURATION,
                                            RTE_KGS::Instance().GetStatistic().localNodeName,
                                            SAPDB_ToString(m_Statistic.ownDelayTime)
                                          );
            RTE_Message(errList);
            KGS->isHotStandbyConfiguration = true;
        }

        m_IsInitialized = result;
    } // End of initialization code

    if ( !m_IsInitialized )
    {
        returnErrList = errList;
    }
    return m_IsInitialized;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::CheckForValidHotStandbyConfiguration( Configuration & found,
                                                 SAPDBErr_MessageList & errList )
{
    SAPDBErr_MessageList localErrList;
    SAPDB_Bool callResult = Initialize(localErrList);

    if ( callResult )
    {
        found = ConfigurationHotStandby;
    }
    else
    {
        if ( RTEINFO_HSS_NO_NODE_CONFIGURED_ID == localErrList.ID() ) // PTS ? UH 2004-01-21 use localErrlist instead of errList
        {
            found = ConfigurationNormal;
            callResult = true;
        }
        else
        {
            found = ConfigurationInvalid;
            errList = localErrList;
        }
    }
    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SetKernelStateToStandby( SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        RTE_KGS::Instance().SetDBState(RTE_KGS::RTEKGS_StateStandby);
    }
    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::GetOwnNodeName (
    RTE_Nodename & ownNodeName,
    SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        strcpy(ownNodeName, RTE_KGS::Instance().GetStatistic().localNodeName);
    }

    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::GetOfficialNodeName (
    RTE_Nodename & officialNodeName,
    SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        strcpy(officialNodeName, RTE_KGS::Instance().GetStatistic().officialNodeName);
    }

    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::GetOwnDelayTime (
    SAPDB_Int4           & delayTime,
    SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        delayTime = m_Statistic.ownDelayTime;
    }

    return callResult;
}

/*
   @brief    Return the answer to the question if caller is default master

   The default master is simply the node which local node name is identical with node
   name entry HS_NODE_001.
   @param    callerIsDefaultMaster [out] flag set to true if caller is default master
   @param    errList [out] set to error if call failed
   @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::IsDefaultMaster ( SAPDB_Bool           & callerIsDefaultMaster,
                                          SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        callerIsDefaultMaster = m_IsDefaultMaster;
    }

    return callResult;
}

/*
   @brief    Return current recommended log volume open mode

   This call is called by I/O runtime routines to find out, if log volumes have to opened
   read only. Note that in a 'normal configuration' this call will return an error!
   It internally checks the current hot standby role. Calling this function in a not hot
   standby configuration is handled as an error.

   @param    logReadOnly [out] flag set to true if log volumes should be opened read only
   @param    errList [out] set to error if call failed
   @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::IsLogReadOnly( SAPDB_Bool & logReadOnly,
                                       SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        logReadOnly = (RoleMaster != m_Statistic.ownRole);
    }

    return callResult;
}

/*
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
SAPDB_Bool
RTEHSS_KernelInterface::SetRoleToMaster ( SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        strcpy(m_Statistic.masterNodeName, RTE_KGS::Instance().GetStatistic().localNodeName);

        callResult = RTEHSS_ApiDll::Instance().SetLogReadOnlyStatus( m_OwnLogVolumesBinaryMapping,
                                                                     false,
                                                                     errList);
        if ( callResult )
        {
            m_Statistic.ownRole = RoleMaster;
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEINFO_HSS_SET_ROLE_MASTER
                                          );
            RTE_Message(errList);
        }
    }

    return callResult;
}

//---------------------------------------------

/*
   @brief    Prepare for becoming standby

   This sets up internal role of a standby node. The caller must be sure the node has standby role. 
   Only standby nodes (after inspected the log volume) will issue this call. The role may be changed afterwards
   by a SetMasterRole() call.

   @param    masterNodeName [in] the node name of the master
   @param    errList [out] set to error if call failed
   @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::SetRoleToStandby ( RTE_Nodename const   & masterNodeName,
                                           SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        strcpy(m_Statistic.masterNodeName, masterNodeName);

        callResult = RTEHSS_ApiDll::Instance().SetLogReadOnlyStatus( m_OwnLogVolumesBinaryMapping,
                                                                     true,
                                                                     errList);
        if ( callResult )
        {
            m_Statistic.ownRole = RoleStandby;
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEINFO_HSS_SET_ROLE_STANDBY,
                                            m_Statistic.masterNodeName
                                          );
            RTE_Message(errList);
        }
    }

    return callResult;
}

/*
  @brief          Notify that a INIT STANDBY command was received for the node

  Updates m_Statistic.timeOfInitStandby
 */
void RTEHSS_KernelInterface::GotInitStandby()
{
    m_Statistic.timeOfInitStandby = RTESys_MicroSecondTime();
}

/*
  @brief          Notify that a RESTART STANDBY command was received for the node

  Updates m_Statistic.timeOfRestartStandby
 */
void RTEHSS_KernelInterface::GotRestartStandby()
{
    m_Statistic.timeOfRestartStandby = RTESys_MicroSecondTime();
}

/*
  @brief          Notify that a TAKEOVER command was received for the node

  Updates m_Statistic.timeOfTakeover
 */
void RTEHSS_KernelInterface::GotTakeover()
{
    m_Statistic.timeOfTakeover = RTESys_MicroSecondTime();
}

/*
  @brief          Notify that a TAKEOVER command was received for the node

  Updates m_Statistic.timeOfTakeover
 */
void RTEHSS_KernelInterface::GotSynchronize(LogPosition lastValidLogOffsetReceived,
                                            LogPosition firstNeededLogOffsetReported)
{
    m_Statistic.timeOfSynchronize = RTESys_MicroSecondTime();
    m_Statistic.synchronizeReceiveCount++;
    m_Statistic.lastValidLogOffsetReceived = lastValidLogOffsetReceived;
    m_Statistic.firstNeededLogOffsetReported = firstNeededLogOffsetReported;
}

/*
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
SAPDB_Bool
RTEHSS_KernelInterface::GetOwnMappingInfo ( Mapping              & ownMapping,
                                            SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        ownMapping = m_OwnDataVolumesMapping;
    }

#ifdef TEST_CODE
    {
        RTE_Nodename server = "PINTEL02";
        RTEHSS_KernelInterface::Mapping serverMapping;
        SAPDBErr_MessageList testErrList;
        if ( !RTEHSS_KernelInterface::Instance().SendInfoStandby(server,serverMapping, testErrList) )
        {
            RTE_Message(testErrList);
        }
        else
        {
            testErrList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                RTEERR_HSS_API_MESSAGE,
                                                "Got DEVICE INFO from PINTEL02" );
            RTE_Message(testErrList);

            testErrList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                RTEERR_HSS_API_MESSAGE,
                                                serverMapping.mappingString );
            RTE_Message(testErrList);

            if ( !RTEHSS_KernelInterface::Instance().FreeMemory(serverMapping.mappingString,
                                                            testErrList) )
            {
                RTE_Message(testErrList);
            }
        }
    }
#endif

    return callResult;
}

/*
  @brief          Notify that the data volume configuration has changed

  Updates m_OwnDataVolumesMapping, m_OwnDataVolumesBinaryMapping,

  Should be called only if data volume configuration did change...

  @param          errList [out] set to error if call failed
  @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::UpdateOwnDataVolumeMapping( SAPDBErr_MessageList & errList )
{
    SAPDB_Bool    result = RTEHSS_KernelInterface::Instance().Initialize(errList);

    if ( !result )
    {
        // if no HotStandbyConfiguration, update is not needed...
        return true;
    }

    return GetOwnDataVolumeMapping(errList);
}

/*
  @brief          Notify that the data volume configuration has changed

  Updates m_OwnDataVolumesMapping, m_OwnDataVolumesBinaryMapping,

  Should be called only if data volume configuration did change...

  @param          errList [out] set to error if call failed
  @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::GetOwnDataVolumeMapping( SAPDBErr_MessageList & errList )
{
    SAPDBErr_MessageList ignoredList;
    SAPDB_Bool    result = true;
    SAPDB_Int4    numberOfDataVolumes;
    SAPDB_Char ** dataVolumeArray = RTEHSS_Helper::AllocateAndFillDataVolumeArray( numberOfDataVolumes,
                                                                                   errList );
    Mapping       newOwnDataVolumesMapping;          ///< externally used (hex digit string)
    RTEHSS_Buffer newOwnDataVolumesBinaryMapping;    ///< internally used (binary representation)

    if ( !dataVolumeArray )
    {
        result = false;
    }
    else
    {
        if ( !RTEHSS_Helper::GetMappingForVolumeArray( newOwnDataVolumesBinaryMapping, 
                                                       numberOfDataVolumes,
                                                       dataVolumeArray,
                                                       errList) )
        {
            result = false;
        }
        else if ( !RTEHSS_Helper::ConvertToHex( newOwnDataVolumesMapping,
                                                newOwnDataVolumesBinaryMapping,
                                                errList) )
        {
            RTEHSS_Helper::Deallocate(newOwnDataVolumesBinaryMapping.buffer);
            result = false;
        }

        RTEHSS_Helper::Deallocate(dataVolumeArray);
    }

    if ( result )
    {
        // if somebody complains about not 'synchronizing' here, ask the higher kernel layer,
        // why they do not synchronize... It could only collide if a new standby node wants to
        // update its configuration wile an add volume happens.

        void *oldBuffer = m_OwnDataVolumesBinaryMapping.buffer;

        m_OwnDataVolumesBinaryMapping.buffer            = newOwnDataVolumesBinaryMapping.buffer;
        m_OwnDataVolumesBinaryMapping.bufferGiven       = newOwnDataVolumesBinaryMapping.bufferGiven;
        m_OwnDataVolumesBinaryMapping.bytesUsedOrNeeded = newOwnDataVolumesBinaryMapping.bytesUsedOrNeeded;
        m_OwnDataVolumesBinaryMapping.needMore          = newOwnDataVolumesBinaryMapping.needMore;

        if ( oldBuffer )
        {
            RTEHSS_Helper::Deallocate(oldBuffer);
        }

        void *oldMappingString = m_OwnDataVolumesMapping.mappingString;

        m_OwnDataVolumesMapping.mappingString = newOwnDataVolumesMapping.mappingString;
        m_OwnDataVolumesMapping.lengthInBytes = newOwnDataVolumesMapping.lengthInBytes;

        if ( oldMappingString )
        {
            RTEHSS_Helper::Deallocate(oldMappingString);
        }
    }

    return result;
}

/*
  @brief          Notify that the data volume configuration has changed

  Updates m_OwnLogVolumesMapping, m_OwnLogVolumesBinaryMapping,

  Should be called only if logging volume configuration did change...

  @param          errList [out] set to error if call failed
  @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::UpdateOwnLogVolumeMapping( SAPDBErr_MessageList & errList )
{
    SAPDB_Bool    result = RTEHSS_KernelInterface::Instance().Initialize(errList);

    if ( !result )
    {
        // if no HotStandbyConfiguration, update is not needed...
        return true;
    }

    return GetOwnLogVolumeMapping(errList);
}

/*
  @brief          Notify that the data volume configuration has changed

  Updates m_OwnLogVolumesMapping, m_OwnLogVolumesBinaryMapping,

  Should be called only if logging volume configuration did change...

  @param          errList [out] set to error if call failed
  @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::GetOwnLogVolumeMapping( SAPDBErr_MessageList & errList )
{
    SAPDBErr_MessageList ignoredList;
    SAPDB_Bool    result = true;
    SAPDB_Int4    numberOfLogVolumes;
    SAPDB_Char ** logVolumeArray = RTEHSS_Helper::AllocateAndFillLogVolumeArray( numberOfLogVolumes,
                                                                                 errList );
    Mapping       newOwnLogVolumesMapping;           ///< externally used (hex digit string)
    RTEHSS_Buffer newOwnLogVolumesBinaryMapping;     ///< internally used (binary representation)
    if ( !logVolumeArray )
    {
        result = false;
    }
    else
    {
        if ( !RTEHSS_Helper::GetMappingForVolumeArray( newOwnLogVolumesBinaryMapping, 
                                                       numberOfLogVolumes,
                                                       logVolumeArray,
                                                       errList) )
        {
            result = false;
        }
        else if ( !RTEHSS_Helper::ConvertToHex( newOwnLogVolumesMapping,
                                                newOwnLogVolumesBinaryMapping,
                                                errList) )
        {
            RTEHSS_Helper::Deallocate(newOwnLogVolumesBinaryMapping.buffer);
            result = false;
        }

        RTEHSS_Helper::Deallocate(logVolumeArray);
    }

    if ( result )
    {
        // if somebody complains about not 'synchronizing' here, ask the higher kernel layer,
        // why they do not synchronize... It could only collide if a new standby node wants to
        // update its configuration wile an add volume happens.

        void *oldBuffer = m_OwnLogVolumesBinaryMapping.buffer;

        m_OwnLogVolumesBinaryMapping.buffer            = newOwnLogVolumesBinaryMapping.buffer;
        m_OwnLogVolumesBinaryMapping.bufferGiven       = newOwnLogVolumesBinaryMapping.bufferGiven;
        m_OwnLogVolumesBinaryMapping.bytesUsedOrNeeded = newOwnLogVolumesBinaryMapping.bytesUsedOrNeeded;
        m_OwnLogVolumesBinaryMapping.needMore          = newOwnLogVolumesBinaryMapping.needMore;

        if ( oldBuffer )
        {
            RTEHSS_Helper::Deallocate(oldBuffer);
        }

        void *oldMappingString = m_OwnLogVolumesMapping.mappingString;

        m_OwnLogVolumesMapping.mappingString = newOwnLogVolumesMapping.mappingString;
        m_OwnLogVolumesMapping.lengthInBytes = newOwnLogVolumesMapping.lengthInBytes;

        if ( oldMappingString )
        {
            RTEHSS_Helper::Deallocate(oldMappingString);
        }
    }

    return result;
}

/*
   @brief Establish the mirror using given mapping info

   @param    otherMapping [in] length + hex digits string for mapping of all DATA-volumes on remote node
   @param    errList [out] set to error if call failed
   @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::EstablishMirror( Mapping const & otherMapping,
                                         SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        if ( RoleMaster  != m_Statistic.ownRole 
          && RoleStandby != m_Statistic.ownRole )
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_ROLE_UNKNOWN );
            RTE_Message(errList);
            callResult = false;
        }
        else
        {
            // split the mirror that is expected to be established by standby node
            if ( callResult )
            {
                RTEHSS_ApiDll & api = RTEHSS_ApiDll::Instance();
                RTEHSS_Buffer otherBinaryMapping;

                callResult = RTEHSS_Helper::ConvertFromHex( otherBinaryMapping,
                                                            otherMapping,
                                                            errList);
                if ( callResult )
                {
                    if ( RoleMaster == m_Statistic.ownRole )
                    {
                        callResult = api.MirrorData(m_OwnDataVolumesBinaryMapping, // source = master = own role
                                                    otherBinaryMapping,            // target = standby
                                                    errList);
                    }
                    else
                    {
                        callResult = api.MirrorData(otherBinaryMapping,            // source = master
                                                    m_OwnDataVolumesBinaryMapping, // target = standby = own role
                                                    errList);
                    }
                    RTEHSS_Helper::Deallocate(otherBinaryMapping.buffer);
                }
            }
        }
    }

    return callResult;
}

/*
   @brief Split the previously established mirror using given mapping info

   This routine triggers splitting the previouly established mirror
   of the data volumes. It is expected before this call is used,
   a savepoint is written and all I/O activitiy on data volumes are stalled.
   @param    otherMapping [in] length + hex digits string for mapping of all DATA-volumes on remote node
   @param    errList [out] set to error if call failed
   @return   true if call suceeded, false if call failed
 */
SAPDB_Bool
RTEHSS_KernelInterface::SplitMirror ( Mapping const & otherMapping,
                                      SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        if ( RoleMaster  != m_Statistic.ownRole 
          && RoleStandby != m_Statistic.ownRole )
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_ROLE_UNKNOWN );
            RTE_Message(errList);
            callResult = false;
        }
        else
        {
            // split the mirror that is expected to be established by standby node
            if ( callResult )
            {
                RTEHSS_ApiDll & api = RTEHSS_ApiDll::Instance();
                RTEHSS_Buffer otherBinaryMapping;

                callResult = RTEHSS_Helper::ConvertFromHex( otherBinaryMapping,
                                                            otherMapping,
                                                            errList);
                if ( callResult )
                {
                    if ( RoleMaster == m_Statistic.ownRole )
                    {
                        callResult = api.MirrorSplit( m_OwnDataVolumesBinaryMapping, // source = master = own role
                                                      otherBinaryMapping, // target = standby
                                                      errList);
                    }
                    else
                    {
                        callResult = api.MirrorSplit( otherBinaryMapping, // source = master
                                                      m_OwnDataVolumesBinaryMapping, // target = standby = own role
                                                      errList);
                    }
                    RTEHSS_Helper::Deallocate(otherBinaryMapping.buffer);
                }
            }
        }
    }

    return callResult;
}

/*
   @brief    Get extended synchronization storage state information

   Synchronization means either a mirror is synchronized or a split
   has completed all outstanding I/O. If storage system is synchronized,
   isSynchronized is returned as true. The statusMessage returned is usefull
   information for the caller.
   @param          isSynchronized [out] set to true if synchronized, false if not
   @param          statusMessage [out] a textual message usable for status output
   @param          errList [out] set to error if call failed
   @return   true if call suceeded, false if call failed

 */
SAPDB_Bool
RTEHSS_KernelInterface::GetStorageState( Mapping const        & otherMapping,
                                         SAPDB_Bool           & isSynchronized,
                                         SAPDB_Char const *   & statusMessage,
                                         SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        // for further study....
        isSynchronized = true;
        statusMessage = "Synchronized";
    }

    return callResult;
}

/*!
  @brief   Trim reply from external command, to allow display in knldiag without corrupting this file

  Especially new lines are dangerous.... Leading spaces are trimmed away and any groups of whitespaces
  are replaced by a single space char. The last space is removed at last.

  @param reply [inout] the reply to trim
 */
static void TrimReplyForKnldiag(SAPDB_Char const *reply)
{
    SAPDB_Char const *pReply = reply;
    SAPDB_Char *pTrim = const_cast<SAPDB_Char *>(reply);

    // skip leading white spaces
    while ( pReply && isspace(*pReply) )
    {
        ++pReply;
    }

    // compress spaces and convert newline to spaces
    SAPDB_Bool gotSpace = false;
    while ( pReply && *pReply )
    {
        SAPDB_Char const singleChar = *pReply++;

        if ( isspace(singleChar) )
        {
            if ( !gotSpace )
            {
                *pTrim++ = ' ';
            }
            gotSpace = true;
        }
        else
        {
            gotSpace = false;
            *pTrim++ = singleChar;
        }
    }

     // reomve trailing space if it exists
    if ( pTrim != const_cast<SAPDB_Char *>(reply) )
    {
        if ( *--pTrim == ' ' )
        {
            *pTrim = 0;
        }
    }
}

//-------------------------- SEND MEMBER FUNCTION ----------------------
// Result interpretation function
// Three different possible results OK , OK + 'ITEMNAME=' Integer, OK + 'ITEMNAME=' String

/*!
  @brief RTEHSS_ReplyHandler for expectation of an OK and a mapping info as hex digits on the next line
  @param command [in] used command string
  @param reply [in] the reply string
  @param replyContext [in] context for the call
  @param errList [out] errorlist to be filled if parsing failes
  @return true if call succeeded, false else (errList filled)
 */
static SAPDB_Bool
RTEHSS_ExpectOkAndMappingInfo( SAPDB_Char const *command,
                               SAPDB_Char const *reply,
                               void *replyContext,
                               SAPDBErr_MessageList &errList)
{
    SAPDB_Bool        callResult = true;
    SAPDB_Char const *pReply = reply;

    while ( *pReply && isspace(*pReply) )
        ++pReply;

    if ( pReply[0] != 'O'
      || pReply[1] != 'K' )
    {
        TrimReplyForKnldiag(reply);
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_UNEXPECTED_REPLY,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(reply) );
        callResult = false;
    }

    if ( callResult )
    {
        // reply information is on next line
        while ( *pReply && *pReply != '\n' )
            ++pReply;

        if ( *pReply )
        {
            while ( *pReply && isspace(*pReply) )
                ++pReply;
            while ( *pReply && ('=' != *pReply) )
                ++pReply;
            if ( '=' == *pReply )
                ++pReply;
            while ( *pReply && isspace(*pReply) )
                ++pReply;
            if ( 'x' == *pReply )
            {
                ++pReply;
                if ( '\'' == *pReply )
                {
                   ++pReply;
                }
            }
        }

        if ( !*pReply )
        {
            TrimReplyForKnldiag(reply);
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_UNEXPECTED_REPLY,
                                            SAPDB_ToString(command),
                                            SAPDB_ToString(reply) );
            callResult = false;
        }
    }

    RTEHSS_KernelInterface::Mapping *pMappingInfo;

    if ( callResult )
    {
        pMappingInfo = (RTEHSS_KernelInterface::Mapping *)replyContext;

        if ( 0 == pMappingInfo )
        {
            TrimReplyForKnldiag(reply);
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_MISSING_REPLY_CONTEXT,
                                            SAPDB_ToString(command),
                                            SAPDB_ToString(reply) );
            callResult = false;
        }

    }

    if ( callResult )
    {
        SAPDB_Char const *startOfMapping = pReply;
        SAPDB_Char const *endOfMapping = pReply;

        while ( *endOfMapping
             && *endOfMapping != '\n' )
            ++endOfMapping;
        while ( *endOfMapping 
            && startOfMapping != endOfMapping
            && isspace(*endOfMapping) )
            --endOfMapping;
        if ( *endOfMapping == '\'' )
            --endOfMapping;

        pMappingInfo->lengthInBytes = (endOfMapping - startOfMapping) + 1;
        pMappingInfo->mappingString = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear( "MappingInfo",
                                                                       pMappingInfo->lengthInBytes + 1,
                                                                       errList );
        if ( !pMappingInfo->mappingString )
        {
            callResult = false;
        }
        else
        {
            SAPDB_MemCopyNoCheck(pMappingInfo->mappingString, startOfMapping, pMappingInfo->lengthInBytes);
        }
    }

    return callResult;
}

/*!
  @brief Interpret output expecting 'OK' and integer reply
  @param command [in] used command string
  @param reply [in] the reply string
  @param replyContext [in] context for the call
  @param errList [out] errorlist to be filled if parsing failes
  @return true if call succeeded, false else (errList filled)
 */
static SAPDB_Bool
RTEHSS_ExpectOkAndUInt4( SAPDB_Char const *command,
                         SAPDB_Char const *reply,
                         void *replyContext,
                         SAPDBErr_MessageList &errList)
{
    SAPDB_Bool        callResult = true;
    SAPDB_Char const *pReply = reply;

    while ( *pReply && isspace(*pReply) )
        ++pReply;

    if ( pReply[0] != 'O'
      || pReply[1] != 'K' )
    {
        TrimReplyForKnldiag(reply);
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_UNEXPECTED_REPLY,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(reply) );
        callResult = false;
    }

    if ( callResult )
    {
        // reply information is on next line
        while ( *pReply && *pReply != '\n' )
            ++pReply;

        if ( *pReply )
        {
            while ( *pReply && isspace(*pReply) )
                ++pReply;
            while ( *pReply && ('=' != *pReply) )
                ++pReply;
            if ( '=' == *pReply )
                ++pReply;
            while ( *pReply && isspace(*pReply) )
                ++pReply;
            if ( 'x' == *pReply )
            {
                ++pReply;
                if ( '\'' == *pReply )
                {
                   ++pReply;
                }
            }
        }

        if ( !*pReply )
        {
            TrimReplyForKnldiag(reply);
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_UNEXPECTED_REPLY,
                                            SAPDB_ToString(command),
                                            SAPDB_ToString(reply) );
            callResult = false;
        }
    }

    if ( callResult )
    {
        SAPDB_UInt4 *pNumber = (SAPDB_UInt4 *)replyContext;

        if ( 0 == pNumber )
        {
            TrimReplyForKnldiag(reply);
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_MISSING_REPLY_CONTEXT,
                                            SAPDB_ToString(command),
                                            SAPDB_ToString(reply) );
            callResult = false;
        }
        else
        {
            union {
                SAPDB_UInt4 u4;
                SAPDB_Byte  c4[4];
            } u;
            
            for ( SAPDB_Int iByte = 0; iByte < sizeof(SAPDB_UInt4); ++iByte )
            {
                u.c4[iByte] = ( RTEHSS_Helper::ToBin(pReply[iByte*2]) << 4 ) + RTEHSS_Helper::ToBin(pReply[(iByte*2)+1]);
            }

            *pNumber = u.u4;
        }
    }

    return callResult;
}


/*!
  @brief Interpret output expecting 'OK'
  @param command [in] used command string
  @param reply [in] the reply string
  @param replyContext [in] context for the call
  @param errList [out] errorlist to be filled if parsing failes
  @return true if call succeeded, false else (errList filled)
 */
static SAPDB_Bool
RTEHSS_ExpectOk(SAPDB_Char const *command,
                SAPDB_Char const *reply,
                void *replyContext, // reply context not needed
                SAPDBErr_MessageList &errList)
{
    SAPDB_Char const *pReply = reply;

    while ( isspace(*pReply) )
        ++pReply;

    if ( pReply[0] != 'O'
      || pReply[1] != 'K' )
    {
        TrimReplyForKnldiag(reply);
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_UNEXPECTED_REPLY,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(reply) );
        return false;
    }

    return true;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SendInfoStandby ( RTE_Nodename const   & otherNode,
                                          Mapping &              otherMapping,
                                          SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    // check if masterNodeName is already known
    if ( callResult )
    {
        // get mapping from given node
        callResult = RTEHSS_Helper::SendCall( otherNode,
                                              "INFO STANDBY",
                                              "",
                                              "",
                                              RTEHSS_ExpectOkAndMappingInfo,
                                              &otherMapping,
                                              errList);
    }
    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SendBackupHistoryInfo ( RTE_Nodename const   & otherNode,
                                                SAPDB_Char const    * backupHistoryInfo,
                                                SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    // check if masterNodeName is already known
    if ( callResult )
    {
        // get mapping from given node
        callResult = RTEHSS_Helper::SendCall( otherNode,
                                              "WRITE HISTORY",
                                              backupHistoryInfo,
                                              "",
                                              RTEHSS_ExpectOk,
                                              0,
                                              errList);
    }
    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SendBackupMediumInfo ( RTE_Nodename const   & otherNode,
                                               SAPDB_Char const    * backupMediumInfo,
                                               SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    // check if masterNodeName is already known
    if ( callResult )
    {
        // get mapping from given node
        callResult = RTEHSS_Helper::SendCall( otherNode,
                                              "WRITE MEDIUM",
                                              backupMediumInfo,
                                              "",
                                              RTEHSS_ExpectOk,
                                              0,
                                              errList);
    }
    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::FreeMemory ( void * memoryStart,
                                     SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    // check if masterNodeName is already known
    if ( callResult )
    {
        // return memory previously allocated for mapping or status message
        RTEHSS_Helper::Deallocate( memoryStart );
    }
    return callResult;
}
//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SendPrepareStandby ( SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    // check if masterNodeName is already known
    if ( callResult )
    {
        if ( RoleStandby != m_Statistic.ownRole )
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_MUST_HAVE_STANDBY_ROLE,
                                            "SendPrepareStandby" );
            callResult = false;
        }
    }

    if ( callResult )
    {
        SAPDB_Char sqlStringParameter[sizeof(RTE_Nodename)+2];

        // sql needs single quotes fro strings
        SAPDB_sprintf(sqlStringParameter, sizeof(sqlStringParameter),
                      "'%s'", RTE_KGS::Instance().GetStatistic().localNodeName );
        callResult = RTEHSS_Helper::SendCall( m_Statistic.masterNodeName,
                                              "PREPARE STANDBY",
                                              sqlStringParameter,
                                              "",
                                              RTEHSS_ExpectOk,
                                              0,
                                              errList);
        if ( callResult )
        {
            RTEHSS_ApiDll & api = RTEHSS_ApiDll::Instance();
            // allow standby do prepare attach of data volumes
            callResult = api.MirrorSplitDone( m_OwnDataVolumesBinaryMapping, // standby = own role
                                              errList);
            if ( callResult )
            {
                m_Statistic.timeOfPrepareStandby = RTESys_MicroSecondTime();
            }
        }
    }
    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SendRegisterStandby( LogPosition            lastSavepointOffset, 
                                             LogPosition          & lastValidLogOffset,
                                             SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        if ( !m_Statistic.masterNodeName )
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_MUST_HAVE_STANDBY_ROLE,
                                            "SendRegisterStandby" );
            callResult = false;
        }
    }
    if ( callResult )
    {
        SAPDB_Char sqlStringParameter[sizeof(RTE_Nodename)+2];

        // sql needs single quotes fro strings
        SAPDB_sprintf(sqlStringParameter, sizeof(sqlStringParameter),
                      "'%s'", RTE_KGS::Instance().GetStatistic().localNodeName );
        Sql4ByteHexString hexString(lastSavepointOffset);

        LogPosition replyNumber;
        callResult = RTEHSS_Helper::SendCall( m_Statistic.masterNodeName,
                                              "REGISTER STANDBY",
                                              sqlStringParameter,
                                              hexString.asCharp(),
                                              RTEHSS_ExpectOkAndUInt4,
                                              &replyNumber,
                                              errList);
        if ( callResult )
        {
            m_Statistic.timeOfRegisterStandby = RTESys_MicroSecondTime();
            m_Statistic.firstNeededLogOffsetReported = lastSavepointOffset;
            m_Statistic.lastValidLogOffsetReceived = replyNumber;
            m_Statistic.synchronizeReceiveCount = 0;
            lastValidLogOffset = replyNumber;
        }
    }

    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SendInsertStandby( RTE_Nodename const   & nodeName,
                                           RTE_Nodename const   & newStandbyNodeName,
                                           SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        SAPDB_Char sqlStringParameter[sizeof(RTE_Nodename)+2];

        // sql needs single quotes fro strings
        SAPDB_sprintf(sqlStringParameter, sizeof(sqlStringParameter),
                      "'%s'", newStandbyNodeName );
        callResult = RTEHSS_Helper::SendCall( nodeName,
                                              "INSERT STANDBY",
                                              sqlStringParameter,
                                              "",
                                              RTEHSS_ExpectOk,
                                              0,
                                              errList);
    }

    return callResult;
}

//---------------------------------------------

SAPDB_Bool
RTEHSS_KernelInterface::SendRemoveStandby( RTE_Nodename const   & nodeName,
                                           RTE_Nodename const   & oldStandbyNodeName,
                                           SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        SAPDB_Char sqlStringParameter[sizeof(RTE_Nodename)+2];

        // sql needs single quotes fro strings
        SAPDB_sprintf(sqlStringParameter, sizeof(sqlStringParameter),
                      "'%s'", oldStandbyNodeName );
        callResult = RTEHSS_Helper::SendCall( nodeName,
                                              "REMOVE STANDBY",
                                              sqlStringParameter,
                                              "",
                                              RTEHSS_ExpectOk,
                                              0,
                                              errList);
    }

    return callResult;
}

//---------------------------------------------

/*
  @brief send synchronize command
  @param standbyNodeName [in] the standby node
  @param lastValidOffset [in] last valid log offset
  @param firstStillNeededOffset [out] first still needed offset
  @param errList [out] filled in case of problem
  @return true if call succeeded, false else
 */
SAPDB_Bool
RTEHSS_KernelInterface::SendSynchronize( RTE_Nodename const   & standbyNodeName,
                                         LogPosition            lastValidOffset,
                                         LogPosition          & firstStillNeededOffset,
                                         SAPDBErr_MessageList & errList )
{
    SAPDB_Bool callResult = Initialize(errList);

    if ( callResult )
    {
        if ( RoleMaster != m_Statistic.ownRole )
        {
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_MUST_HAVE_MASTER_ROLE,
                                            "SendSynchronize" );
            callResult = false;
        }
    }

    LogPosition number = 0;

    if ( callResult )
    {
        Sql4ByteHexString hexString(lastValidOffset);
        callResult = RTEHSS_Helper::SendCall( standbyNodeName,
                                              "SYNCHRONIZE",
                                              hexString.asCharp(),
                                              "",
                                              RTEHSS_ExpectOkAndUInt4,
                                              &number,
                                              errList);
        if ( callResult )
        {
            firstStillNeededOffset = (LogPosition)number;
        }
    }

    SAPDB_Int4 nodeIndex;
    if ( RTEHSS_StandbyNodeList::Instance().FindNode( standbyNodeName, nodeIndex ) )
    {
        RTEHSS_StandbyNodeList::Instance().AddSynchronizeCallResult( nodeIndex,
                                                                     callResult,
                                                                     lastValidOffset,
                                                                     number );
    }
    return callResult;
}

/*
  @brief server to server call
  This call may be issued before initializing RTEHSS_KernelInterface if the 'dbmcli' command initialization
  has to be overwritten. The first 'Initialize' wins.
  @param serverCall [in] the calling interface
  @param errList [out] filled in case of problems
  @return true if call succeeded, false else
 */
SAPDB_Bool RTEHSS_SetupServerToServerCall( RTEHSS_IServerToServerCall &serverCall
                                         , SAPDBErr_MessageList &errList)
{
    return RTEHSS_OtherKernelCall::Instance().Initialize( serverCall
                                                        , errList);
}

//---------------------------------------------
//   RTEHSS_Helper class
//---------------------------------------------

void * RTEHSS_Helper::AllocateAndClear( SAPDB_Char const *what,
                                       SAPDB_ULong bytes,
                                       SAPDBErr_MessageList &errList)
{
    SAPDBERR_ASSERT_STATE(0 != what);

    void *ptr = RTEMem_RteAllocator::Instance().Allocate(bytes);
    if ( !ptr )
    {
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_MALLOC_FAILED_FOR,
                                        what );
        return 0;
    }

    SAPDB_MemFillNoCheck(ptr, 0, bytes);
    return ptr;
}

//---------------------------------------------

void RTEHSS_Helper::Deallocate( void *ptr )
{
    RTEMem_RteAllocator::Instance().Deallocate(ptr);
}

//---------------------------------------------

SAPDB_Char ** RTEHSS_Helper::AllocateAndFillDataVolumeArray( SAPDB_Int4 &numberOfDataVolumes, 
                                                            SAPDBErr_MessageList &errList)
{
    SAPDB_UInt4 iVolume;
    SAPDB_Int4 numberOfVolumes;

    numberOfVolumes = 0;
    for ( iVolume = 0; 
          iVolume < RTE_KGS::Instance().GetXParam()->DevspDescr [ RTE_VolumeTypeData ]. ulNumDevs;
          ++iVolume )
    {
        if ( DEVSPACE_UNUSED != RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeData ].Devspaces [ iVolume ].lDevspaceType )
        {
            ++numberOfVolumes;
        }
    }

    SAPDB_Char **dataVolumeArray = (SAPDB_Char **)RTEHSS_Helper::AllocateAndClear( "dataVolumeArray",
                                                            numberOfVolumes * sizeof(SAPDB_Char *),
                                                            errList);
    if ( !dataVolumeArray )
    {
        return (SAPDB_Char **)0;
    }

    for ( numberOfVolumes = 0, iVolume = 0; 
          iVolume < RTE_KGS::Instance().GetXParam()->DevspDescr [ RTE_VolumeTypeData ]. ulNumDevs;
          ++iVolume )
    {
        if ( DEVSPACE_UNUSED != RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeData ].Devspaces [ iVolume ].lDevspaceType )
        {
            dataVolumeArray[numberOfVolumes] = &(RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeData ].Devspaces [ iVolume ].szDevspaceName [0]);
            ++numberOfVolumes;
        }
    }

    numberOfDataVolumes = numberOfVolumes;

    return dataVolumeArray;
}

//---------------------------------------------

SAPDB_Char ** RTEHSS_Helper::AllocateAndFillLogVolumeArray( SAPDB_Int4 &numberOfLogAndMirroredLogVolumes, 
                                                            SAPDBErr_MessageList &errList)
{
    SAPDB_UInt4 iVolume;
    SAPDB_Int4 numberOfVolumes;

    numberOfVolumes = 0;
    for ( iVolume = 0; 
          iVolume < RTE_KGS::Instance().GetXParam()->DevspDescr [ RTE_VolumeTypeLog ]. ulNumDevs;
          ++iVolume )
    {
        if ( DEVSPACE_UNUSED != RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeLog ].Devspaces [ iVolume ].lDevspaceType )
        {
            ++numberOfVolumes;
        }
        if ( RTE_KGS::Instance().GetXParam()->fLogMirrored )
        {
            if ( DEVSPACE_UNUSED != RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeMirrorLog ].Devspaces [ iVolume ].lDevspaceType )
            {
                ++numberOfVolumes;
            }
        }
    }

    SAPDB_Char **logVolumeArray = (SAPDB_Char **)RTEHSS_Helper::AllocateAndClear( "logVolumeArray",
                                                            numberOfVolumes * sizeof(SAPDB_Char *),
                                                            errList);
    if ( !logVolumeArray )
    {
        return (SAPDB_Char **)0;
    }

    numberOfVolumes = 0;
    for ( iVolume = 0; 
          iVolume < RTE_KGS::Instance().GetXParam()->DevspDescr [ RTE_VolumeTypeLog ]. ulNumDevs;
          ++iVolume )
    {
        if ( DEVSPACE_UNUSED != RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeLog ].Devspaces [ iVolume ].lDevspaceType )
        {
            logVolumeArray[numberOfVolumes] = &(RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeLog ].Devspaces [ iVolume ].szDevspaceName [0]);
            ++numberOfVolumes;
        }
        if ( RTE_KGS::Instance().GetXParam()->fLogMirrored )
        {
            if ( DEVSPACE_UNUSED != RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeMirrorLog ].Devspaces [ iVolume ].lDevspaceType )
            {
                logVolumeArray[numberOfVolumes] = &(RTE_KGS::Instance().GetXParam()->DevspDescr[ RTE_VolumeTypeMirrorLog ].Devspaces [ iVolume ].szDevspaceName [0]);
                ++numberOfVolumes;
            }
        }
    }

    numberOfLogAndMirroredLogVolumes = numberOfVolumes;

    return logVolumeArray;
}

//---------------------------------------------

SAPDB_Bool RTEHSS_Helper::ConvertToHex( RTEHSS_KernelInterface::Mapping &mapping,
                                        RTEHSS_Buffer const &mappingBuffer,
                                        SAPDBErr_MessageList &errList)
{
    SAPDB_Char *mappingString;
    mappingString = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear( "MappingString",
                                                                   (2 * mappingBuffer.bytesUsedOrNeeded) + 1,
                                                                   errList );
    if ( !mappingString )
    {
        return false;
    }

    SAPDB_Int4    mappingBytes;
    for ( mappingBytes = 0; mappingBytes < mappingBuffer.bytesUsedOrNeeded; mappingBytes++ )
    {
        SAPDB_Byte p = ((SAPDB_Byte *)mappingBuffer.buffer)[mappingBytes];
        mappingString[2*mappingBytes]   = "0123456789ABCDEF"[(p&0x0F0)>>4];
        mappingString[2*mappingBytes+1] = "0123456789ABCDEF"[(p&0x0F)];
    }
    mappingString[2*mappingBytes] = 0;

    mapping.lengthInBytes = 2 * mappingBuffer.bytesUsedOrNeeded;
    mapping.mappingString = mappingString;
    return true;
}

//---------------------------------------------

SAPDB_Bool RTEHSS_Helper::ConvertFromHex( RTEHSS_Buffer &mappingBuffer,
                                          RTEHSS_KernelInterface::Mapping const &mapping,
                                          SAPDBErr_MessageList &errList)
{
    SAPDB_Byte *buffer;
    buffer = (SAPDB_Byte *)RTEHSS_Helper::AllocateAndClear( "BinaryRepresentation",
                                                                   mapping.lengthInBytes/2,
                                                                   errList );
    if ( !buffer )
    {
        return false;
    }

    SAPDB_Char *mappingString = mapping.mappingString;
    SAPDB_Int4  mappingBytes;

    for ( mappingBytes = 0; mappingBytes < (mapping.lengthInBytes/2); mappingBytes++ )
    {
        buffer[mappingBytes] = ( ToBin(mappingString[(2 * mappingBytes)]) << 4)
                                      + ToBin(mappingString[(2 * mappingBytes) + 1]);
    }

    mappingBuffer.buffer      = buffer;
    mappingBuffer.bufferGiven = mappingBuffer.bytesUsedOrNeeded = mappingBytes;
	mappingBuffer.needMore    = false;

    return true;
}

//---------------------------------------------

SAPDB_Bool RTEHSS_Helper::GetMappingForVolumeArray(RTEHSS_Buffer &mappingBuffer,
                                                          SAPDB_Int4 numberOfVolumes,
                                                          SAPDB_Char ** volumeArray,
                                                          SAPDBErr_MessageList & errList)
{
    SAPDBERR_ASSERT_STATE(0 != volumeArray);
    SAPDBERR_ASSERT_STATE(0 != numberOfVolumes);

    SAPDB_Bool gotMapping = false;

    RTEHSS_ApiDll & api = RTEHSS_ApiDll::Instance();

    SAPDB_MemFillNoCheck(&mappingBuffer, 0, sizeof(RTEHSS_Buffer));

    mappingBuffer.bytesUsedOrNeeded = RTEHSS_DEFAULT_MAPPING_SIZE;

    do 
    {
    	mappingBuffer.bufferGiven = mappingBuffer.bytesUsedOrNeeded;
    	mappingBuffer.bytesUsedOrNeeded = 0;
	    mappingBuffer.needMore = false;

        if ( mappingBuffer.buffer )
        {
            RTEHSS_Helper::Deallocate(mappingBuffer.buffer);
        }
        mappingBuffer.buffer = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear( "MappingBuffer",
                                                        mappingBuffer.bufferGiven,
                                                        errList);
        if ( !mappingBuffer.buffer )
        {
            break;
        }

        gotMapping = api.GetMapping( numberOfVolumes,
                                     volumeArray,
                                     mappingBuffer,
                                     errList );

        if ( !gotMapping 
          && api.GetLastError() != RTEHSS_ERR_BUFFER_EXCEEDED )
        {
            break;
        }
    } while ( !gotMapping );

    return gotMapping;
} // RTEHSS_Helper::GetMappingForVolumeArray

//---------------------------------------------

SAPDB_Bool RTEHSS_Helper::SendCall( SAPDB_Char const *calledNodeName,
                                    SAPDB_Char const *command,
                                    char const * arg0,
                                    char const * arg1,
                                    RTEHSS_ReplyHandler replyHandler,
                                    void *replyContext,
                                    SAPDBErr_MessageList &errList)
{
    SendCallJob theJob(calledNodeName,
                       command,
                       arg0,
                       arg1,
                       replyHandler,
                       replyContext,
                       errList);

    RTETask_CallWorker(theJob);

    return theJob.GetResult();
} // RTEHSS_Helper::SendCall

//---------------------------------------------
//   RTEHSS_OtherKernelCall
//---------------------------------------------

RTEHSS_OtherKernelCall::RTEHSS_OtherKernelCall()
: m_IsInitialized(false)
, m_InitializeResult(false)
, m_ServerCall(0)
{
} // RTEHSS_OtherKernelCall::RTEHSS_OtherKernelCall

//---------------------------------------------

/*
  @brief Helper function for allocating and copying single argument
  @param name [in] name of parameter (used in errlist only)
  @param source [in] the string to copy (must be zero termination non null string
  @param errList [out] filled in case of error
  @return 0 if problem, pointer to copy else
 */
static SAPDB_Char *CopyArgument( SAPDB_Char const *name
                               , SAPDB_Char const *source
                               , SAPDBErr_MessageList &errList)
{
    SAPDB_Int4 length = (SAPDB_Int4)strlen(source);
    SAPDB_Char *copy = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear(name, length + 1, errList);
    if ( !copy )
    {
        return 0;
    }
    SAPDB_MemCopyNoCheck(copy, source, length+1);
    return copy;
}

//---------------------

static inline SAPDB_Char const *SearchSpace(SAPDB_Char const *search)
{
    if ( search )
    {
        while ( *search )
        {
            if ( ' ' == *search )
            {
                return search;
            }
            ++search;
        }
    }
    return 0;
}

/*
   Call arguments for dbmcli
   
   Prepare the argument vector needed for a dbmcli call to execute 'command'
   
   Windows only: two extra arguments for output pipe name
    "-o"
  , "\\.\pipe\HS_dbName_nodeName" ,

    "-n"
  , nodeName
  , "-d"
  , dbName
  , "db_execute"
  , command
[ , arg0
 [, arg1 ] ]
  , 0

 */
SAPDB_Bool Default_ServerToServerCall::PrepareCall( SAPDB_Char const *nodeName
                                                  , SAPDB_Char const *dbName
                                                  , SAPDB_Char const *command
                                                  , SAPDB_Char const *arg0
                                                  , SAPDB_Char const *arg1
                                                  , SAPDB_Char *  &commandLineFormatString
                                                  , SAPDB_Int4    &argumentCount
                                                  , SAPDB_Char ** &arguments
                                                  , SAPDB_Char *  &outputPipeName
                                                  , SAPDB_Char *  &argumentPipeName
                                                  , SAPDBErr_MessageList &errList)
{
#if defined(WIN32)
    /* dbmcli output redirection does not work, since kernel has no console window */
    /* Redirection is overwritten then by application which is enforced to open its */
    /* own console window... */
    SAPDB_Char pipeName[RTEHSS_COMMAND_OUTPUT_PIPE_LENGTH+sizeof(tsp00_DbNamec)+sizeof(RTE_Nodename)];
    SAPDB_sprintf( pipeName, sizeof(pipeName), RTEHSS_COMMAND_OUTPUT_PIPE, dbName, nodeName );

    //---------------------------

    SAPDB_Char argPipeName[RTEHSS_COMMAND_ARGUMENT_PIPE_LENGTH+sizeof(tsp00_DbNamec)+sizeof(RTE_Nodename)];
    SAPDB_sprintf( argPipeName, sizeof(argPipeName), RTEHSS_COMMAND_ARGUMENT_PIPE, dbName, nodeName );
#else
    SAPDB_Char pipeName[1];
    pipeName[0] = 0; /* not used at all */
    SAPDB_Char argPipeName[1];
    argPipeName[0] = 0; /* set in calling routine... */
#endif

    //---------------------------

    commandLineFormatString = (SAPDB_Char *) RTEHSS_Helper::AllocateAndClear("CommandLine",
           strlen(m_CommandLine) + strlen(&argPipeName[0]) + 1, errList );
    if ( !commandLineFormatString )
    {
        return false;
    }

    strcpy( commandLineFormatString, m_CommandLine );
    argumentPipeName = commandLineFormatString + strlen(m_CommandLine);
    strcpy( argumentPipeName, &argPipeName[0] );

#if defined(WIN32)
    /* two extra arguments for output pipe name -o \\.\pipe\HS_dbName_nodeName */
    argumentCount = 10;
#else
    argumentCount = 8;
#endif

    /* commands may be separated by spaces... */
    SAPDB_Char const *partialCommand = command;
    SAPDB_Char const *foundSpace = SearchSpace(partialCommand);
    while ( foundSpace )
    {
        ++argumentCount;
        partialCommand = foundSpace + 1;
        foundSpace = SearchSpace(partialCommand);
    }

    if ( arg0 && arg0[0] )
    {
        ++argumentCount;
        if ( arg1 && arg1[0] )
        {
            ++argumentCount;
        }
    }

    arguments = (SAPDB_Char **)RTEHSS_Helper::AllocateAndClear("CommandArguments", sizeof(SAPDB_Char *) * (argumentCount+1), errList);
    if ( !arguments )
    {
        return false;
    }

    SAPDB_Int4 iArg = 0;
#if defined(WIN32)
    arguments[  iArg] = (SAPDB_Char *)"-o";
    arguments[++iArg] = CopyArgument("CommandPipeName", &pipeName[0], errList);
    if ( !arguments[iArg] ) 
    {
        FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
    }

    arguments[++iArg] = (SAPDB_Char *)"-n";
#else
    arguments[  iArg] = (SAPDB_Char *)"-n";
#endif

    arguments[++iArg] = CopyArgument("CommandNodeName", nodeName, errList);
    if ( !arguments[iArg] ) 
    {
        FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
    }

    arguments[++iArg] = (SAPDB_Char *)"-d";
    arguments[++iArg] = CopyArgument("CommandDbName", dbName, errList);
    if ( !arguments[iArg] ) 
    {
        FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
    }

    arguments[++iArg] = (SAPDB_Char *)"-u";
    arguments[++iArg] = Uncrypt(errList);
    if ( !arguments[iArg] ) 
    {
        FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
    }

    arguments[++iArg] = (SAPDB_Char *)"db_execute";

    /* commands may be separated by spaces. This will split them into individual commands... */
    partialCommand = command;
    foundSpace = SearchSpace(partialCommand);
    while ( foundSpace )
    {
        arguments[++iArg] = CopyArgument("CommandString", partialCommand, errList);
        if ( !arguments[iArg] )
        {
            FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
        }
        (arguments[iArg])[foundSpace - partialCommand] = 0;
        partialCommand = foundSpace + 1;
        foundSpace = SearchSpace(partialCommand);
    }
    arguments[++iArg] = CopyArgument("CommandString", partialCommand, errList);
    if ( !arguments[iArg] )
    {
        FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
    }

    if ( arg0 && arg0[0] )
    {
        arguments[++iArg] = CopyArgument("CommandArg0", arg0, errList);
        if ( !arguments[iArg] )
        {
            FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
        }

        if ( arg1 && arg1[0] )
        {
            arguments[++iArg] = CopyArgument("CommandArg0", arg1, errList);
            if ( !arguments[iArg] )
            {
                FreeCallResources( 0, iArg, arguments, 0, 0 );  return false;
            }
        }
    }

#if defined(WIN32)
    outputPipeName = arguments[1];
#else
    outputPipeName = 0;
#endif

    return true;
}

//----------------------------

/*
  @brief deallocate argument vector
  @param argumentCount [in] number of filled argument strings
  @param arguments [in] the argument vector
  @errList [out] filled in case of problem
  @return true if call succeeded, false if problem
 */
void Default_ServerToServerCall::FreeCallResources( SAPDB_Char * commandLineFormatString
                                                  , SAPDB_Int4   argumentCount
                                                  , SAPDB_Char **arguments
                                                  , SAPDB_Char *
                                                  , SAPDB_Char * )
{
    if ( commandLineFormatString )
    {
        RTEHSS_Helper::Deallocate(commandLineFormatString);
    }

    if ( arguments )
    {
        SAPDB_Int4 iArg = 0;
#if defined(WIN32)
        if ( argumentCount > 1 )
        {
            RTEHSS_Helper::Deallocate(arguments[1]);
            iArg = 2; /* on windows two extra arguments are used */
        }
#endif
        if ( argumentCount > (iArg+1) )
        {
            RTEHSS_Helper::Deallocate(arguments[iArg+1]);
        }

        if ( argumentCount > (iArg+3) )
        {
            RTEHSS_Helper::Deallocate(arguments[iArg+3]);
        }

        if ( argumentCount > (iArg+5) )
        {
            SAPDB_MemFillNoCheck(arguments[iArg+5], 0, strlen(arguments[iArg+5]));
            RTEHSS_Helper::Deallocate(arguments[iArg+5]);
        }

        if ( argumentCount > (iArg+7) )
        {
            RTEHSS_Helper::Deallocate(arguments[iArg+7]);
        }

        if ( argumentCount > (iArg+8) )
        {
            RTEHSS_Helper::Deallocate(arguments[iArg+8]);
        }

        if ( argumentCount > (iArg+9) )
        {
            RTEHSS_Helper::Deallocate(arguments[iArg+9]);
        }
    }

    RTEHSS_Helper::Deallocate(arguments);
}

//----------------------------

SAPDB_Bool Default_ServerToServerCall::Crypt( SAPDB_Char *username
                                            , SAPDB_Char *password
                                            , SAPDBErr_MessageList &errList )
{
    m_CryptLength = ( (SAPDB_Int4)strlen(username) + 1 /* , */ 
                      + (SAPDB_Int4)strlen(password)
                        );
    m_AuthorizeCopy = (SAPDB_UInt2 *)RTEHSS_Helper::AllocateAndClear("AuthorizeCopy", sizeof(SAPDB_UInt2) * m_CryptLength, errList );
    if ( !m_AuthorizeCopy )
    {
        return false;
    }

    SAPDB_Int iCrypt;
    SAPDB_Int iCopy;
    SAPDB_UInt2 cryptKey = m_CryptKey;

    for ( iCopy = iCrypt = 0; username[iCopy] != 0; ++iCopy, ++iCrypt )
    {
        // inverted strings will not so easy found in core dumps...
        m_AuthorizeCopy[iCrypt] = cryptKey ^ username[iCopy];
        cryptKey *= username[iCopy];
    }

    m_AuthorizeCopy[iCrypt++] = cryptKey ^ ',';
    cryptKey *= ',';

    for ( iCopy = 0; password[iCopy] != 0; ++iCopy, ++iCrypt )
    {
        // inverted strings will not so easy found in core dumps...
        m_AuthorizeCopy[iCrypt] = cryptKey ^ password[iCopy];
        cryptKey *= password[iCopy];
    }
    SAPDB_MemFillNoCheck(password, 0, iCopy);
    return true;
}

//----------------------------

SAPDB_Char * Default_ServerToServerCall::Uncrypt( SAPDBErr_MessageList &errList )
{
    SAPDB_Char *copy = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear("AuthCopy", m_CryptLength + 1, errList);
    if ( !copy )
    {
        return 0;
    }

    SAPDB_UInt2 cryptKey = m_CryptKey;
    SAPDB_Int4 iCrypt;
    for ( iCrypt = 0; iCrypt < m_CryptLength; ++iCrypt )
    {
        copy[iCrypt] = (SAPDB_Char)(m_AuthorizeCopy[iCrypt] ^ cryptKey);
        cryptKey *= copy[iCrypt];
    }
    copy[iCrypt] = 0;
    return copy;
}

//---------------------------

static void WriteArgumentsToPipe(RTE_FileHandle pipeHandle, int givenArgc, char **givenArgv)
{
    int nIndex = 0;
    char szParameter[SAPDB_PARAMETER_PIPE_SIZE];
    char *argument;
    int   argLength;

#if defined(WIN32)
    DWORD bytesWritten;

    ConnectNamedPipe(pipeHandle, 0);
#endif

    for ( nIndex = 0; nIndex < givenArgc; nIndex++ )
    {
        argument = givenArgv[nIndex];
        argLength = (int)strlen(argument);
        while ( argLength >= (SAPDB_PARAMETER_PIPE_SIZE-1) )
        {
            szParameter[0] = SAPDB_PART_OF_LONG_PARAMETER;
            SAPDB_MemCopyNoCheck(&szParameter[1], argument, SAPDB_PARAMETER_PIPE_SIZE-1);
            argLength -= (SAPDB_PARAMETER_PIPE_SIZE-1);
#if defined(WIN32)
            WriteFile(pipeHandle, szParameter, SAPDB_PARAMETER_PIPE_SIZE, &bytesWritten, 0);
#else
            RTE_save_write( pipeHandle, szParameter, SAPDB_PARAMETER_PIPE_SIZE);
#endif
            argument  += (SAPDB_PARAMETER_PIPE_SIZE-1);
        }

        szParameter[0] = SAPDB_LAST_PART_OF_PARAMETER;
        SAPDB_MemCopyNoCheck(&szParameter[1], argument, argLength);
        SAPDB_MemFillNoCheck(&szParameter[argLength+1], 0, SAPDB_PARAMETER_PIPE_SIZE - (argLength+1));
#if defined(WIN32)
        WriteFile(pipeHandle, szParameter, SAPDB_PARAMETER_PIPE_SIZE, &bytesWritten, 0);
#else
        RTE_save_write( pipeHandle, szParameter, SAPDB_PARAMETER_PIPE_SIZE);
#endif
    } // end for
    szParameter[0] = SAPDB_END_OF_PARAMETERS;
    SAPDB_MemFillNoCheck(&szParameter[1], 0, SAPDB_PARAMETER_PIPE_SIZE - 1);
#if defined(WIN32)
    WriteFile(pipeHandle, szParameter, SAPDB_PARAMETER_PIPE_SIZE, &bytesWritten, 0);
#else
    RTE_save_write( pipeHandle, szParameter, SAPDB_PARAMETER_PIPE_SIZE);
#endif
}

//----------------------------

#if !defined(WIN32)
static SAPDB_Char *GetCompleteCommandLine( SAPDB_Char *commandLine
                                         , int argumentsPipeDesc
                                         , SAPDBErr_MessageList &errList )
{
    SAPDB_Char *copy = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear("CompleteCommand", strlen(commandLine) + 30, errList);
    if ( !copy )
    {
        return 0;
    }
    SAPDB_sprintf(copy, strlen(commandLine) + 30, commandLine, argumentsPipeDesc);
    return copy;
}
#endif

//----------------------------

SAPDB_Bool
RTEHSS_OtherKernelCall::Initialize( RTEHSS_IServerToServerCall &serverCall
                                  , SAPDBErr_MessageList       &returnErrList )
{
    if ( !m_IsInitialized )
    {
        m_ServerCall = &serverCall;
        m_IsInitialized = m_ServerCall->Initialize(returnErrList);
    }

    return m_IsInitialized;
} // RTEHSS_OtherKernelCall::Initialize

//----------------------------

SAPDB_Bool
RTEHSS_OtherKernelCall::SendCall( SAPDB_Char const *calledNodeName,
                                  SAPDB_Char const *command,
                                  char const * arg0,
                                  char const * arg1,
                                  RTEHSS_ReplyHandler replyHandler,
                                  void *replyContext,
                                  SAPDBErr_MessageList &errList)
{
    SAPDBERR_ASSERT_STATE(0 != calledNodeName);
    SAPDBERR_ASSERT_STATE(0 != command);
    SAPDBERR_ASSERT_STATE(0 != replyHandler);
    SAPDBERR_ASSERT_STATE(m_IsInitialized);
    SAPDB_Int4   cmdArgc = 0;
    SAPDB_Char **cmdArgv = 0;
#if defined(WIN32)
    SAPDB_Char const *szServerDB = RTE_KGS::Instance().GetKGS()->szServerDB;
#else
    SAPDB_Char const *szServerDB = RTE_KGS::Instance().GetKGS()->serverdb;
#endif
    if ( !strcmp(RTE_ISystem::Instance().GetLocalNodeName(), calledNodeName) )
    {
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_CALLING_MYSELF,
                                        SAPDB_ToString(command) );
        return false;
    }

    SAPDB_Char *replyBuffer = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear("ReplyBuffer", RTEHSS_MAX_REPLY_BUFFER_SIZE, errList);
    if ( !replyBuffer )
    {
        return false;
    }

    SAPDB_Char *commandLine;
    SAPDB_Char *outputPipeName;
    SAPDB_Char *argumentPipeName;

    if ( !PrepareCommandLine( calledNodeName
                            , szServerDB
                            , command
                            , arg0
                            , arg1
                            , commandLine
                            , cmdArgc
                            , cmdArgv
                            , outputPipeName
                            , argumentPipeName
                            , errList) )
    {
        RTEHSS_Helper::Deallocate(replyBuffer);
        return false;
    }

#if defined(WIN32)
    /* dbmcli output redirection does not work, since kernel has no console window */
    /* Redirection is overwritten then by application which is enforced to open its */
    /* own console window... */

    RTE_FileHandle pipeHandle;
    RTE_FileHandle argPipeHandle;
    
    pipeHandle = CreateNamedPipe( outputPipeName,
                                  PIPE_ACCESS_INBOUND,
                                  PIPE_TYPE_BYTE     |
                                  PIPE_READMODE_BYTE |
                                  PIPE_NOWAIT,
                                  1,
                                  RTEHSS_MAX_REPLY_BUFFER_SIZE,
                                  RTEHSS_MAX_REPLY_BUFFER_SIZE,
                                  INFINITE,
                                  NULL );
    if ( INVALID_HANDLE_VALUE == pipeHandle )
    {
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_OPEN_OUTPUT_PIPE,
                                        outputPipeName,
                                        SAPDB_ToString(GetLastError()) );
        RTEHSS_Helper::Deallocate(replyBuffer);
        FreeCommandLine( commandLine
                       , cmdArgc
                       , cmdArgv
                       , outputPipeName
                       , argumentPipeName );
        return false;
    }

    (void)ConnectNamedPipe(pipeHandle, NULL); // More error handling takes place during pipe read

    //---------------------------

    argPipeHandle = CreateNamedPipe( argumentPipeName,
                                     PIPE_ACCESS_OUTBOUND,
                                     PIPE_TYPE_BYTE     |
                                     PIPE_READMODE_BYTE |
                                     PIPE_WAIT,
                                     1,
                                     SAPDB_PARAMETER_PIPE_SIZE,
                                     SAPDB_PARAMETER_PIPE_SIZE,
                                     INFINITE,
                                     NULL );
    if ( INVALID_HANDLE_VALUE == argPipeHandle )
    {
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_OPEN_OUTPUT_PIPE,
                                        argumentPipeName,
                                        SAPDB_ToString(GetLastError()) );
        CloseHandle(pipeHandle);
        RTEHSS_Helper::Deallocate(replyBuffer);
        FreeCommandLine( commandLine
                       , cmdArgc
                       , cmdArgv
                       , outputPipeName
                       , argumentPipeName );
        return false;
    }

    //---------------------------

    PROCESS_INFORMATION processInfo;
    STARTUPINFO         startInfo; 
    RTE_SystemRc rc;

    SAPDB_MemFillNoCheck( &processInfo, 0, sizeof(PROCESS_INFORMATION) );
    SAPDB_MemFillNoCheck( &startInfo, 0, sizeof(STARTUPINFO) );

    startInfo.cb = sizeof(STARTUPINFO); 
 
    // Create the child process. 
    if ( !CreateProcess(NULL, 
          (LPTSTR)commandLine,       /* command line */
          NULL,          /* process security attributes */
          NULL,          /* primary thread security attributes  */
          false,         /* handles are NOT inherited  */
          CREATE_NO_WINDOW,
          NULL,          /* use parent's environment  */
          NULL,          /* use parent's current directory  */
          &startInfo,    /* STARTUPINFO pointer */
          &processInfo)  /* receives PROCESS_INFORMATION */
       )
    {
        rc = GetLastError();
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_COMMAND_PIPE_OPEN,
                                        SAPDB_ToString(commandLine),
                                        SAPDB_ToString(rc)
                                      );
        CloseHandle(argPipeHandle);
        CloseHandle(pipeHandle);
        FreeCommandLine( commandLine
                       , cmdArgc
                       , cmdArgv
                       , outputPipeName
                       , argumentPipeName );
        RTEHSS_Helper::Deallocate(replyBuffer);
        return false;        
    }

    // Transfer arguments via pipe and close it afterwards

    WriteArgumentsToPipe( argPipeHandle, cmdArgc, cmdArgv );
    CloseHandle(argPipeHandle);

    // Fill reply buffer

    FreeCommandLine( commandLine
                   , cmdArgc
                   , cmdArgv
                   , outputPipeName
                   , argumentPipeName );
    if ( !FillReplyBufferFromNamedPipe( processInfo.hProcess,
                                        pipeHandle,
                                        replyBuffer, 
                                        RTEHSS_MAX_REPLY_BUFFER_SIZE, 
                                        errList) )
    {
        RTEHSS_Helper::Deallocate(replyBuffer);
        CloseHandle(processInfo.hProcess);
        CloseHandle(pipeHandle);
        return false;        
    }

    RTE_SystemRc result = RTE_SYSTEMRC_NO_ERROR;
    DWORD terminationStatus;
    
    do
    {
        if ( !GetExitCodeProcess(processInfo.hProcess, &terminationStatus) )
        {
            rc = GetLastError();
            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                            RTEERR_HSS_COMMAND_PIPE_CLOSE,
                                            SAPDB_ToString(command),
                                            SAPDB_ToString(rc)
                                           );
            RTEHSS_Helper::Deallocate(replyBuffer);
            CloseHandle(processInfo.hProcess);
            CloseHandle(pipeHandle);
            return false;
        }

        if ( STILL_ACTIVE == terminationStatus )
        {
            WaitForSingleObject(processInfo.hProcess, INFINITE);
        }

    } while ( STILL_ACTIVE == terminationStatus );
    CloseHandle(processInfo.hProcess);
    CloseHandle(pipeHandle);
#else
    SAPDB_Int argumentPipes[2];
    if ( RTE_save_pipe(argumentPipes) != 0 )
    {
        int rc = errno;
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_COMMAND_PIPE_OPEN,
                                        SAPDB_ToString(commandLine),
                                        SAPDB_ToString(rc)
                                      );
        FreeCommandLine( commandLine
                       , cmdArgc
                       , cmdArgv
                       , outputPipeName
                       , argumentPipeName );
        RTEHSS_Helper::Deallocate(replyBuffer);
        return false;        
    }

    SAPDB_Char *completeCommandLine = GetCompleteCommandLine( commandLine
                                                            , argumentPipes[0]
                                                            , errList );
    if ( !completeCommandLine )
    {
        FreeCommandLine( commandLine
                       , cmdArgc
                       , cmdArgv
                       , outputPipeName
                       , argumentPipeName );
        RTEHSS_Helper::Deallocate(replyBuffer);
        return false;
    }

    RTESys_CommandOutputPipeContext cmdContext;
    RTE_SystemRc rc;
    rc = RTESys_OpenCommandOutputPipe(cmdContext, completeCommandLine);
    if ( RTE_SYSTEMRC_NO_ERROR != rc )
    {
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_COMMAND_PIPE_OPEN,
                                        SAPDB_ToString(completeCommandLine),
                                        SAPDB_ToString(rc)
                                       );
        RTEHSS_Helper::Deallocate(completeCommandLine);
        FreeCommandLine( commandLine
                       , cmdArgc
                       , cmdArgv
                       , outputPipeName
                       , argumentPipeName );
        RTEHSS_Helper::Deallocate(replyBuffer);
        return false;        
    }
    RTEHSS_Helper::Deallocate(completeCommandLine);

    // Transfer arguments via pipe and close it afterwards

    close(argumentPipes[0]);
    WriteArgumentsToPipe( argumentPipes[1], cmdArgc, cmdArgv );
    close(argumentPipes[1]);

    // Work on reply of command

    FreeCommandLine( commandLine
                   , cmdArgc
                   , cmdArgv
                   , outputPipeName
                   , argumentPipeName );
    if ( !FillReplyBuffer( cmdContext.cmdOutput,
                           replyBuffer, 
                           RTEHSS_MAX_REPLY_BUFFER_SIZE,
                           errList) )
    {
        RTEHSS_Helper::Deallocate(replyBuffer);
        return false;        
    }

    SAPDB_Int4 exitStatus;
    rc = RTESys_CloseCommandOutputPipe(cmdContext, exitStatus);
    if ( RTE_SYSTEMRC_NO_ERROR != rc )
    {
        errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                        RTEERR_HSS_COMMAND_PIPE_CLOSE,
                                        SAPDB_ToString(command),
                                        SAPDB_ToString(rc)
                                       );
        RTEHSS_Helper::Deallocate(replyBuffer);
        return false;        
    }
#endif

    SAPDB_Bool callResult = (*replyHandler)(command, replyBuffer, replyContext, errList);

    RTEHSS_Helper::Deallocate(replyBuffer);
    return callResult;
} // RTEHSS_OtherKernelCall::SendCall

//---------------------------------------------
#if !defined(WIN32)
SAPDB_Bool RTEHSS_OtherKernelCall::FillReplyBuffer(
                            RTE_FileHandle & cmdOutput,
                            SAPDB_Char *   & replyBuffer,
                            SAPDB_Int4       replyBufferLengthInBytes,
                            SAPDBErr_MessageList &errList )
{
    SAPDB_Bool callResult = true;
    SAPDB_Char * nextInput;
    tsp00_VfReturn_Param returnValue;
    RTE_FileOffset bytesReadFromPipe;
    RTE_FileOffset remainingBufferSpace = replyBufferLengthInBytes;

    SAPDB_MemFillNoCheck(replyBuffer, 0, replyBufferLengthInBytes);
    nextInput = replyBuffer;

    do
    {
        bytesReadFromPipe = 0;
        RTESys_IORead( cmdOutput, nextInput, remainingBufferSpace, bytesReadFromPipe, returnValue);
        if ( vf_ok == returnValue && bytesReadFromPipe > 0 )
        {
            remainingBufferSpace -= bytesReadFromPipe;
            nextInput += bytesReadFromPipe;

            SAPDBERR_ASSERT_STATE(remainingBufferSpace > 0);

            if ( 0 == remainingBufferSpace ) // reallocate extended buffer...
            {
                SAPDB_Char *extendedReplyBuffer = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear("ExtendedReplyBuffer", 
                                                            replyBufferLengthInBytes + RTEHSS_MAX_REPLY_BUFFER_SIZE, 
                                                            errList);
                if ( !extendedReplyBuffer )
                {
                    callResult = false;
                    break;
                }

                SAPDB_MemCopyNoCheck(extendedReplyBuffer, replyBuffer, replyBufferLengthInBytes);
                nextInput = (extendedReplyBuffer + (nextInput - replyBuffer));

                RTEHSS_Helper::Deallocate(replyBuffer);

                replyBufferLengthInBytes += RTEHSS_MAX_REPLY_BUFFER_SIZE;
                replyBuffer = extendedReplyBuffer;
                remainingBufferSpace += RTEHSS_MAX_REPLY_BUFFER_SIZE;
            }
        }
    } while ( vf_ok == returnValue );

    return callResult;
} // RTEHSS_OtherKernelCall::FillReplyBuffer

#else /* WIN32 does not support redirection... */

//---------------------------------------------

SAPDB_Bool RTEHSS_OtherKernelCall::FillReplyBufferFromNamedPipe( HANDLE processHandle,
                                                                 HANDLE pipeHandle,
                                                                 SAPDB_Char *   & replyBuffer,
                                                                 SAPDB_Int4       replyBufferLengthInBytes,
                                                                 SAPDBErr_MessageList &errList )
{
    SAPDB_Bool callResult = true;
    SAPDB_Char * nextInput;
    tsp00_VfReturn_Param returnValue = vf_ok;
    RTE_FileOffset bytesReadFromPipe;
    RTE_FileOffset remainingBufferSpace = replyBufferLengthInBytes;

    SAPDB_MemFillNoCheck(replyBuffer, 0, replyBufferLengthInBytes);
    nextInput = replyBuffer;


    do
    {
        bytesReadFromPipe = 0;
        // Asynchronous opened Output pipe synchronous read....
#define RTEHSS_PIPE_READ_TIMEOUT 100
        {
            SAPDBErr_MessageList localErrList;
            SAPDB_Bool readPipeDone = false;
            while ( !readPipeDone ) 
            {
                switch( WaitForSingleObject(pipeHandle, RTEHSS_PIPE_READ_TIMEOUT) )
                {
                    case WAIT_OBJECT_0:
                        localErrList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                             RTEERR_HSS_OUTPUT_PIPE_READ,
                                                             "Pipe handle signals"
                                                           );
                        RTE_Message(localErrList);
                        break;

                    case WAIT_ABANDONED:
                        readPipeDone = true;
                        localErrList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                             RTEERR_HSS_OUTPUT_PIPE_READ,
                                                             "Wait for handle abandoned"
                                                           );
                        RTE_Message(localErrList);
                        returnValue = vf_notok;
                        break;

                    case WAIT_TIMEOUT:
                        break;

                    default:
                        break;
                }

                DWORD winBytesReadFromPipe;
                if ( !ReadFile(pipeHandle, nextInput, (DWORD)remainingBufferSpace, &winBytesReadFromPipe, NULL) )
                {
                    DWORD rc = GetLastError();

                    if ( ERROR_PIPE_LISTENING != rc && ERROR_NO_DATA != rc )
                    {
                        readPipeDone = true;
                        if ( ERROR_BROKEN_PIPE == rc || ERROR_HANDLE_EOF == rc )
                        {
                            returnValue = vf_eof;
                        }
                        else
                        {
                            localErrList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                                 RTEERR_HSS_OUTPUT_PIPE_READ,
                                                                 SAPDB_ToString(rc)
                                                               );
                            RTE_Message(localErrList);
                            returnValue = vf_notok;
                        }
                    }
                    else
                    {
                        DWORD terminationStatus;
                        if ( !GetExitCodeProcess(processHandle, &terminationStatus) )
                        {
                            rc = GetLastError();
                            errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                            RTEERR_HSS_COMMAND_PIPE_CLOSE,
                                                            "Cannot get exit status from kernel command",
                                                            SAPDB_ToString(rc)
                                                          );
                            callResult = false;
                            break;
                        }

                        if ( STILL_ACTIVE != terminationStatus )
                        {
                            if ( ERROR_NO_DATA == rc ) // PTS 1127600
                            {
                                readPipeDone = true;
                                returnValue = vf_eof;
                            }
                            else
                            {
                                errList = SAPDBErr_MessageList( RTEHSS_CONTEXT,
                                                                RTEERR_HSS_COMMAND_PIPE_CLOSE,
                                                                "Kernel command no longer active",
                                                                SAPDB_ToString(terminationStatus)
                                                              );
                                callResult = false;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    bytesReadFromPipe = winBytesReadFromPipe;
                    readPipeDone = true;
                }
            }
        }

        if ( vf_ok == returnValue && bytesReadFromPipe > 0 )
        {
            remainingBufferSpace -= bytesReadFromPipe;
            nextInput += bytesReadFromPipe;

            SAPDBERR_ASSERT_STATE(remainingBufferSpace > 0);

            if ( 0 == remainingBufferSpace ) // reallocate extended buffer...
            {
                SAPDB_Char *extendedReplyBuffer = (SAPDB_Char *)RTEHSS_Helper::AllocateAndClear("ExtendedReplyBuffer", 
                                                            replyBufferLengthInBytes + RTEHSS_MAX_REPLY_BUFFER_SIZE, 
                                                            errList);
                if ( !extendedReplyBuffer )
                {
                    callResult = false;
                    break;
                }

                SAPDB_MemCopyNoCheck(extendedReplyBuffer, replyBuffer, replyBufferLengthInBytes);
                nextInput = (extendedReplyBuffer + (nextInput - replyBuffer));

                RTEHSS_Helper::Deallocate(replyBuffer);

                replyBufferLengthInBytes += RTEHSS_MAX_REPLY_BUFFER_SIZE;
                replyBuffer = extendedReplyBuffer;
                remainingBufferSpace += RTEHSS_MAX_REPLY_BUFFER_SIZE;
            }
        }
    } while ( vf_ok == returnValue );

    return callResult;
} // RTEHSS_OtherKernelCall::FillReplyBufferFromNamedPipe
#endif

/* destroy this with new I/O handling... */
extern "C" tsp00_Bool 
RTEHSS_UpdateLogVolumeMapping()
{
    SAPDBErr_MessageList ignoredList;
    return RTEHSS_KernelInterface::Instance().UpdateOwnLogVolumeMapping(ignoredList);
}

extern "C" tsp00_Bool 
RTEHSS_UpdateDataVolumeMapping()
{
    SAPDBErr_MessageList ignoredList;
    return RTEHSS_KernelInterface::Instance().UpdateOwnDataVolumeMapping(ignoredList);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
