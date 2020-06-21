/*!
  \file    Loader_Loader.h
  \author  SteffenS
  \brief   Defines the main Loader.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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
#ifndef LOADER_LOADER_HPP
#define LOADER_LOADER_HPP

&ifdef SDK
#include "SQLDBC.h" /* no check */
&else
#include "Interfaces/SQLDBC/SQLDBC.h"
&endif

#include "Loader_Types.hpp" /* no check */


using namespace SQLDBC;

#ifdef WIN32
#define LOADER_DLLEXPORT __declspec(dllexport)
#else
#define LOADER_DLLEXPORT
#endif


class Loader_ClientRuntime;
class Loader_PageBuilder;
class Loader_Connection;
class Loader_FastloadCommand;
class Loader_ErrorMsgHandler;
class Loader_ConnectProperties;

/*!
    @brief THE Loader class
 */
class Loader
{
public:
    /*!
       @brief constructor
     */
    LOADER_DLLEXPORT
        Loader();

    /*!
       @brief destructor
     */
    LOADER_DLLEXPORT
        ~Loader();

    /*!
        @brief create a Loader connection object
     */
    LOADER_DLLEXPORT
        Loader_Connection*  createConnection() const;

    LOADER_DLLEXPORT
        Loader_Connection*  createConnection(SQLDBC_Connection* pSQLDBCConnection) const;

    /*!
        @brief Close a Loader_Connection
     */
    LOADER_DLLEXPORT
        void                releaseConnection(Loader_Connection* pLoaderConnection) const;

    /*!
       @brief Returns the version of used SDBLoader.
   
       This is the version of the used SDBLoader.
       @return A character string in the format 'SDBLoader M.M.C    Build 002-000-000-000' containing a complete
       version string of the (shared) linked SDBLoader.
     */
    LOADER_DLLEXPORT
        const char*         getLoaderVersion();

    /*LOADER_DLLEXPORT
        void                setTraceLevel(LOADER_TraceLevel level);*/


private:
    //Loader_ClientRuntime* m_pLoaderRuntime;
    SQLDBC_IRuntime*      m_pLoaderRuntime;
    SQLDBC_Environment*   m_pSQLDBCEnvironment;
};



class Loader_ConnectProperties
{
public:
    /**
     * @brief Creates an empty set of options. A default allocator is
     * used.
     */
    LOADER_DLLEXPORT
        Loader_ConnectProperties();

    /**
     * @brief The copy constructor.
     * @param copy The options be copied.
     */
    LOADER_DLLEXPORT
        Loader_ConnectProperties(const Loader_ConnectProperties& copy);

    /**
     * @brief The destructor.
     */
    LOADER_DLLEXPORT
        ~Loader_ConnectProperties();

    /**
     * @brief Sets an option.
     * @param key The option to be set (ASCII string)
     * @param value The option value to be set (ASCII string)
     */
    LOADER_DLLEXPORT
        void setProperty(const LOADER_Char* key, const LOADER_Char* value);

    /**
     * @brief Retrieves an option.
     * @param key The option key to be requested
     * @param defaultvalue The value to be returned. If the option is not found the default value is returned.
     * @return The value found in the options set, or the default value specified.
     */
    LOADER_DLLEXPORT
        const char *getProperty(const LOADER_Char* key, const LOADER_Char* defaultvalue=0) const;

private:
    friend class Loader_Connection;
    friend class Loader_Loader;
    SQLDBC_ConnectProperties *m_prop;
};


/*!
    @brief Loader connection class: connects Loader to DB
 */
class Loader_Connection
{
public:
    /*! 
       @brief Establishing connection
     */
    LOADER_DLLEXPORT
        LOADER_Retcode              connect(const LOADER_Char *servernode, 
                                            const LOADER_Char *serverdb,
                                            const LOADER_Char *username,
                                            const LOADER_Char *password);

    LOADER_DLLEXPORT
        LOADER_Retcode              connect(const LOADER_Char *servernode, 
                                            const LOADER_Char *serverdb,
                                            const LOADER_Char *username,
                                            const LOADER_Char *password,
                                            const Loader_ConnectProperties* properties);

    LOADER_DLLEXPORT
        LOADER_Retcode              close();

    LOADER_DLLEXPORT
        LOADER_Retcode              disconnect();

    LOADER_DLLEXPORT
        LOADER_Bool                 isConnected() const;

    LOADER_DLLEXPORT
        void                        setAutoCommit(LOADER_Bool autocommit);

    LOADER_DLLEXPORT
        LOADER_Bool                 getAutoCommit() const;

    LOADER_DLLEXPORT
        void                        setSQLMode(LOADER_SQLMode sqlmode);

    LOADER_DLLEXPORT
        LOADER_Int4                 getKernelVersion() const;

    LOADER_DLLEXPORT
        LOADER_DateTimeFormat       getDateTimeFormat() const;
 
    LOADER_DLLEXPORT
        LOADER_Retcode              rollback();

    LOADER_DLLEXPORT
        const LOADER_Char*          getErrorText() const;

    LOADER_DLLEXPORT
        LOADER_Int4                 getErrorCode();


    /*!
      @brief Creates a FASTLOAD command object
     */
    LOADER_DLLEXPORT
        Loader_FastloadCommand*     createFastloadCommand();

    LOADER_DLLEXPORT
        void  releaseFastloadCommand(Loader_FastloadCommand* pFastloadCommand);


private:
    friend class Loader;
    friend class Loader_FastloadCommand;

    /*!
       @brief hidden constructor: should not be called directly by any application
     */
//    Loader_Connection(SQLDBC_Environment* SQLDBC_Environment, Loader_ClientRuntime* LoaderRuntime);
    Loader_Connection(SQLDBC_Environment* SQLDBC_Environment, SQLDBC_IRuntime* LoaderRuntime);
    
    Loader_Connection(SQLDBC_Environment*    SQLDBC_Environment,
                      SQLDBC_IRuntime*       LoaderRuntime,
                      SQLDBC_Connection*     pSQLDBCConnection);

    /*!
       @brief hidden destructor
     */
    ~Loader_Connection();

    void            initializeMembers();

    LOADER_Retcode  setSessionInformation();




    SQLDBC_Environment*         m_pSQLDBCEnv;
//    Loader_ClientRuntime*       m_pLoaderRuntime;
    SQLDBC_IRuntime*            m_pLoaderRuntime;
    SQLDBC_Connection*          m_pSQLDBCConnection;

    Loader_FastloadCommand*     m_pFLCmdObj;

    LOADER_Char*                m_pHostName;
    LOADER_Char*                m_pDBName;
    LOADER_Char*                m_UserName;
    SQLDBC_ConnectProperties    m_ConnectProperties;
    LOADER_Bool                 m_UseEstablishedConnection;

    LOADER_Int8                 m_ConnectionID;
    void*                       m_PacketList[2];       // we need always 2 packet pointers
    LOADER_Int4                 m_PacketCount;
    LOADER_UInt4                m_PacketSize;
    LOADER_UInt4                m_PageSize;
    LOADER_SQLMode              m_SQLMode;
    LOADER_Char*                m_pszUserGroupDefault;
    LOADER_Char*                m_SYSDBADefault;
    LOADER_Bool                 m_DBIsUnicode;
    LOADER_Int4                 m_KernelVersion;
    LOADER_DateTimeFormat       m_DateTimeFormat;

    Loader_ErrorMsgHandler*     m_pError;               // the error object
};
// class Loader_Connection



/*!
    @brief THE class that does the work: FASTLOAD
 */
class Loader_FastloadCommand
{
public:

    LOADER_DLLEXPORT
        LOADER_Retcode      initialize(const LOADER_Char* pszTableName, LOADER_Char** pszColumnNames = NULL, LOADER_UInt4 lColumnNameCount = 0,
                                       const LOADER_StringEncoding  encoding = LOADER_STRINGENCODING_ASCII);

    LOADER_DLLEXPORT
        LOADER_Retcode      getColumnNames(LOADER_Char** ppszNameBuffer,
                                           const LOADER_UInt4 lBufferElementLength,
                                           const LOADER_StringEncoding encoding = LOADER_STRINGENCODING_ASCII);

    LOADER_DLLEXPORT
        LOADER_Retcode      getColumnCount(LOADER_UInt4& lColumnCount);

    LOADER_DLLEXPORT
        LOADER_UInt4        getCountofRecordsOnPage();

    LOADER_DLLEXPORT
        LOADER_UInt4        getPreferredBatchSize();


    LOADER_DLLEXPORT
        LOADER_Retcode      setBindingType(LOADER_UInt4 Size, LOADER_Bool bFixedLength = LOADER_FALSE);

    LOADER_DLLEXPORT
        LOADER_Retcode      SetBatchSize(LOADER_UInt4 ArraySize);

    LOADER_DLLEXPORT
        const LOADER_Int4*  getRowStatus() const;

    LOADER_DLLEXPORT
        LOADER_Retcode      bindParameter(const LOADER_UInt2 ParameterNumber,
                                          const LOADER_UInt2 ParameterType,
                                          void*              paramAddr,
                                          LOADER_Length*     LengthIndicator,
                                          LOADER_Length      Size);

    LOADER_DLLEXPORT
        LOADER_Retcode      clearParameters();

    LOADER_DLLEXPORT
        LOADER_Retcode      Execute();

    LOADER_DLLEXPORT
        LOADER_Retcode      End(LOADER_Bool bRollback = LOADER_FALSE);

    LOADER_DLLEXPORT
        const LOADER_Char*  getErrorText() const;

    LOADER_DLLEXPORT
        LOADER_Int4         getErrorCode();

    void  deleteFastloadCommand()
    {
        delete this;
    }

private:            // functions
    LOADER_Int4             getColumnNamesOfTable(LOADER_Char**&     pszColumnNames, LOADER_UInt4& lColumnCount);
    LOADER_Int4             executeSQLStatement(LOADER_Int4          lPacketNo,
                                                const LOADER_Char*   pszCmd,
                                                LOADER_Bool          bWithInfo = LOADER_FALSE);
    LOADER_Int4             executeSQLStatementWData(LOADER_Int4         lPacketNo,
                                                     const LOADER_Char*  pszCmd,
                                                     void*               pPartInfo,
                                                     LOADER_Bool         bWithInfo = LOADER_FALSE);
    LOADER_Int4             getColumnInfosForFastload(LOADER_Char** ppszColumnNames,
                                                      LOADER_UInt4 lColumnNameCount,
                                                      const LOADER_StringEncoding encoding = LOADER_STRINGENCODING_ASCII);
    LOADER_Int4             getUserDefinedDefaultValues();
    LOADER_Int4             checkForLongColumns();
    void                    getRecordParameters();
    LOADER_Int4             BeginLoad();
    LOADER_Int4             EndLoad();
    void                    configurePageBuilder();
    LOADER_Int4             processSQLPacket(LOADER_Int4 lPacketNo, const LOADER_Char* pszCmd);
    LOADER_Int4             preparePagePacket(LOADER_Int4 lPacketNo);
    LOADER_Int4             sendPageRequest(LOADER_Int4 lPacketNo);
    LOADER_Int4             receivePageAnswer(LOADER_Int4 lPacketNo);
    LOADER_Int4             analyzeSQLError(LOADER_Int4 lPacketNo, const LOADER_Char* pszCmd);
    void                    InitPagePacket(LOADER_Int4 lPacketNo);
    LOADER_Int4             handleKeySequenceError();
    LOADER_Int4             insertAfterKeySequenceError();
    void                    initializeValues();
    void                    removeInternalObjects();


private:
    friend class Loader_Connection;
    friend class Loader_PageBuilder;

    /*!
       @brief hidden constructor: should not be called directly by any application
     */
//    Loader_FastloadCommand(Loader_ClientRuntime* pLoaderRuntime, Loader_Connection* pConnection);
    Loader_FastloadCommand(SQLDBC_IRuntime* pLoaderRuntime, Loader_Connection* pConnection);

    ~Loader_FastloadCommand();


//time    double                       receive_duration;
//time    double                       pagebuild_duration;
//time    double                       overall_duration;

//    Loader_ClientRuntime*        m_pLoaderRuntime;
    SQLDBC_IRuntime*             m_pLoaderRuntime;
    Loader_Connection*           m_pConnection;

    void*                        m_ReqPackets[2];
    void*                        m_RcvPackets[2];
    LOADER_Int4                  m_lReqPacketNo;
    LOADER_Int4                  m_lRcvPacketNo;

    LOADER_Byte*                 m_PacketInitBlock;
    LOADER_Int4                  m_PacketInitBlockLength;
    LOADER_Int4                  m_StartPosInBuffer;

    LOADER_UInt4                 m_PagePartPayload;


    LOADER_Byte                  m_BDInfo[128];            // This should be enough for the next years; the length is then computed for every packet
    LOADER_UInt4                 m_BDINFO_Length;


    void*                        m_pTableDescription;

    LOADER_UInt4                 m_ParamsSpecifiedCnt;
    LOADER_Bool                  m_StampDefined;
    LOADER_UInt4                 m_PagesPerPacket;           // number of possible pages per packet
    LOADER_UInt4                 m_RecordLength;             // length of one record on a page in a table
    LOADER_UInt4                 m_FixColumnsLength;         // length of all fix columns together
    LOADER_UInt4                 m_VarColumnsCount;          // number of columns with variable length

    LOADER_Bool                  m_TableHasSyskey;
    LOADER_Bool                  m_TableIsEmptyAtBeginLoad;

    LOADER_Char                  m_LastKey[1024];            // not null terminated string
    LOADER_UInt4                 m_LastKeyLength;

    const LOADER_Char*           m_UserDefault;
    const LOADER_Char*           m_UserGroupDefault;
    const LOADER_Char*           m_SYSDBADefault;
    LOADER_Char                  m_StampDefault[9];          // always 9 Bytes long

    // Parameter data
    LOADER_UInt4                 m_ParamBindingType;         //!< The row size of one row of host variables, or 0 if column-wise binding is used.
                                                            // The default value is column wise binding!
    LOADER_Bool                  m_ParamLengthIsFix;         // only with row wise binding of any sense - the length for a single parameter is the same for all data

    LOADER_UInt4                 m_BatchSize;                // Holds the number of elements in each parameter array
    void*                        m_ParamList;
    LOADER_UInt2                 m_ParameterCount;

    LOADER_Char                  m_pszErrText[1024];

    Loader_PageBuilder*          m_pPageBuilder;

    LOADER_UInt4                 m_lSndPcktCnt;
    LOADER_Int4*                 m_RowStatusArray;
    Loader_ErrorMsgHandler*      m_pError;
    void*                        m_pSQLDBC_RT_Error;


    LOADER_Bool                  m_bKeepPageBuilderObject;

    // Temporarily!
    /*LOADER_UInt8         m_RunTime;
    LOADER_UInt8         m_BindTime;
    LOADER_UInt8         m_ExecuteTime;
    LOADER_UInt8         m_Start;
    LOADER_Int8          m_End;*/
};
// Loader_FastloadCommand

#endif //LOADER_LOADER_HPP