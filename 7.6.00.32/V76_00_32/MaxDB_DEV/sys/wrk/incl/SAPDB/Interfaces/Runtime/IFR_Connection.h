/*!
  @file           IFR_Connection.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Connection
  @brief          Implements class for database connection
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
#ifndef IFR_CONNECTION_H
#define IFR_CONNECTION_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_Environment.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h"
#include "Interfaces/Runtime/Packet/IFRPacket_LongDescriptor.h"
#include "Interfaces/Runtime/IFR_ConnectionItem.h"
#include "Interfaces/Runtime/IFR_ConnectProperties.h"
#include "Interfaces/Runtime/Util/IFRUtil_UniqueID.h"
#include "Interfaces/Runtime/Util/IFRUtil_AsyncOperation.h"

IFR_BEGIN_NAMESPACE

class IFR_Environment;
class IFR_Savepoint;
class IFR_Statement;
class IFR_PreparedStmt;
class IFR_ParseInfoCache;
class IFRPacket_SessionInfoPart;
class IFRPacket_FeaturePart;

//------------------------------------------------------------
class IFR_ConnectionProfile{
public:
    void resetCounters();
    void submitCounters(IFR_UInt8 *counter_8, IFR_UInt4 *counter_4);
    void collectCounters(); 
    void setStmtList (IFRUtil_LinkedList<SQLDBC::SQLDBC_StatementStorage>* list){
      m_StmtList = list;
    }

protected:
    IFR_ConnectionProfile(IFRUtil_LinkedList<SQLDBC::SQLDBC_StatementStorage>* list);
    
    IFR_UInt8 m_counter_8[6];
    IFR_UInt4 m_counter_4[27];

    IFRUtil_LinkedList<SQLDBC::SQLDBC_StatementStorage>* m_StmtList;
};
/**
 * @ingroup IFR_Connection
 * @brief The connection to the database kernel.
 *
 * This class combines the functionality that is needed to manage a
 * connection to the database. A connection object that is created
 * is not connected, only after a successful call to connect
 * has been performed.
 *
 * @todo check more parameters after connect (version).
 * @todo propagate the results of a COMMIT or ROLLBACK into dependent
 *   objects (for handling PUTVALs correctly)
 * @todo capture connection-releasing errors, and connection releases.
 */
class IFR_Connection
    : public IFRUtil_RuntimeItem,
      public IFR_ConnectionItem,
      public IFR_ConnectionProfile

{
public:
    /**
     * Modes for appending maintenance commands to
     * regular commands.
     */
    enum AppendMode {
        AppendAllowed_C,      //!< No restriction, commands can be appended.
        AppendNotAllowed_C,   //!< Commands should not be appended, but they can be sent after the command.
        StreamMode_C          //!< Stream mode. No command can be appended, no command can be sent after the
                              //!< command, as stream context would be destroyed.
    };

    /**
     * Constructor. Creates a connection which is not connected.
     * @param environment The environment used for the connection.
     * @param allocator The allocator to be used for the objects
     *   that depend on this connection.
     * @param memory_ok Parameter to check whether an out of
     *                  memory condition was raised.
     */
    IFR_Connection(IFR_Environment& environment,
                   SAPDBMem_IRawAllocator& allocator,
                   IFR_Bool& memory_ok);


    /**
     * Destructor. Will close the connection, if it is still open.
     */
    ~IFR_Connection();

    /**
     * Releases this @c IFR_Connection object's database and
     * Interface Runtime resources immediately instead of waiting for
     * them to be released by the destructor. After you have called
     * this method, you may again use one of the @c connect(...)
     * methods to connect again to a database.
     * @param commit Whether a COMMIT WORK RELEASE should be sent
     *   instead of a ROLLBACK WORK RELEASE.
     * @param sessionclosed Whether the session has already been closed by
     *        an external event, and no rollback should be sent. @internal
     * @return IFR_OK if the IFR_Connection object has been released,
     *   IFR_NOT_OK if an erro occured.
     */
    IFR_Retcode close(IFR_Bool commit=false, IFR_Bool sessionclosed=false);

    /**
     * Makes all changes made since the previous commit/rollback
     * permanent and releases any database locks currently held by
     * this <code>Connection</code> object.
     * @return IFR_OK if the commit command has been sent,
     *   IFR_NOT_OK if the commit command could not be
     *   sent, because of an error or timeout.
     */
    IFR_Retcode commit();

    /**
     * Creates a <code>IFR_Statement</code> object for sending SQL
     * statements to the database. SQL statements without parameters
     * are normally executed using an <code>IFR_Statement</code>
     * objects. An operation that is executed multiple times, or
     * wants to bind parameters for input and/or output should
     * make use of IFR_PreparedStmt
     * @return The statement instance created by this method.
     */
    IFR_Statement *createStatement();

    /**
     * Creates a prepared statement. A prepared statement
     * is 'prepared' in the database server and can make use of
     * binding variables for input/output parameters.
     * @return The statement instance created by this method.
     */
    IFR_PreparedStmt *createPreparedStatement();

    /**
     * Releases a statement.
     * @param stmt The statement to be released.
     */
    void releaseStatement(IFR_Statement *stmt);

    /**
     * Releases a prepared statement.
     * @param stmt The prepared statement to be released.
     */
    void releaseStatement(IFR_PreparedStmt *stmt);

    /**
     * Get a requestpacket for sending commands to the
     * database. Acquiring the non-synthetic request packet acquires
     * the <b>request packet lock</b>. The request packet lock is
     * removed when the destructor of the request packet is called.
     * @param result The resulting request packet will be filled in here.
     * @param packetType The needed kind of request packet. Use
     *                   - <code>IFRPacket_RequestPacket::Root_C</code> for the RTE packet,
     *                   - <code>IFRPacket_RequestPacket::Dynamic_C</code> for a plain packet that is not related
     *                     to the RTE packet.
     *                   - <code>IFRPacket_RequestPacket::Any_C</code> for leaving decision up to the runtime.
     * @return <code>IFR_OK</code>, if the request packet could be acquired,
     *         <code>IFR_NOT_OK</code> if there is a system problem.
     */
    IFR_Retcode getRequestPacket(IFRPacket_RequestPacket& result,
                                 IFR_ErrorHndl& err,
                                 const IFRPacket_RequestPacket::PacketType packetType=IFRPacket_RequestPacket::Any_C);

    /**
     * Gets whether this connection is in autcommit mode.
     * @return <code>true</code> if the connection is in autocommit mode.
     */
    IFR_Bool getAutoCommit() const;

    /**
     * Sets the autocommit mode.
     * @param autocommit The new value for autocommit.
     */
    void setAutoCommit(IFR_Bool autocommit);

    /**
     * Sets the SQL mode.
     * @param sqlmode The new value for SQL mode.
     */
    void setSQLMode(IFR_SQLMode sqlmode);

    inline IFR_SQLMode getSQLMode() const
    {
        return m_client_sqlmode;
    }

    /**
     * Checks whether a connection has been closed.
     * @return <code>true</code> if the connection has been closed actively (by the close method).
     */
    IFR_Bool isClosed() const;

    /**
     * Undoes all changes made in the current transaction and releases
     * any database locks currently held by this Connection
     * object.
     */
    IFR_Retcode rollback();

    /**
     * Executes a request packet, i.e. sends the command to the database
     * and retrieves the result packet.
     * <b>This method synchronizes on the connection, i.e. only one
     *    call can be done at one time for a single connection.</b>
     * @param requestPacket The request packet to send.
     * @param replyPacket The received packet will be assigned to this one.
     * @param appendFlag Whether maintenance commands can be appended, or can be sent
     *     in subsequent packages.
     * @param execError The error instance to be set in case there is an error
     *        happening during the execution (this also includes the SQL error).
     * @param operation An asynchronous operation to perform while the execution
     *                  of the command is in progress.
     * @return <code>IFR_OK</code> if the request/receive process was ok, <code>IFR_NOT_OK</code>
     *         if there was a communication error.
     */
    IFR_Retcode sqlaexecute(IFRPacket_RequestPacket &requestPacket,
                            IFRPacket_ReplyPacket& replyPacket,
                            AppendMode appendFlag,
                            IFR_ErrorHndl& execError,
                            IFRUtil_AsyncOperation *operation = 0);

    /**
   * Opens a new database session using the default properties (as if there are
   * no properties given).
   * @param servernode The database server name where the instance is running (ASCII).
   * @param serverdb The name of the database instance (ASCII).
   * @param username The name of the database user.
   * The encoding of this string depends from the encoding argument.
   * @param password The database users password.
   * The encoding of this string depends from the encoding argument.
   * @param servernodeLength The length in bytes of the database server name.
   * @param serverdbLength The length in bytes of the database instance name.
   * @param usernameLength The length in bytes of the database user name.
   * @param passwordLength The length in bytes of the database users password.
   * @param userpwdEncoding The character encoding of the username/password combination.
   * @return <code>IFR_OK</code> if the connection is established,
   *   <code>IFR_NOT_OK</code> otherwise, in which case also the
   *   appropriate error is set on this connection object.
   * @todo implement length for arguments honoring the userpwdEncoding
   */
    IFR_Retcode connect(const char *servernode,
                        IFR_Length servernodeLength,
                        const char *serverdb,
                        IFR_Length serverdbLength,
                        const char * username,
                        IFR_Length usernameLength,
                        const char * password,
                        IFR_Length passwordLength,
                        const IFR_StringEncoding userpwdEncoding);

    /**
     * Connects by an URL. Useful only in conjunction with a custom underlying runtime
     * implementation.
     * @param connectURL The connect URL, which may be 0.
     * @param connectURLLength The length of the connect URL (may be IFR_NTS).
     * @param connectCommand The connect command.
     * @param connectCommandLength The length of the connect command.
     * @param connectCommandEncoding The encoding of the connect command.
     * @param connectProperties Connect properties.
     * @return <code>IFR_OK</code> if the connection is established,
     *   <code>IFR_NOT_OK</code> otherwise, in which case also the
     *   appropriate error is set on this connection object.
     */
    IFR_Retcode connect(const char *connectURL,
                        IFR_Length connectURLLength,
                        const char *connectCommand,
                        IFR_Length connectCommandLength,
                        const IFR_StringEncoding connectCommandEncoding,
                        IFR_ConnectProperties& connectProperties);


    /**
     * Opens a new database session. For missing properties the following
     * default values are used:
     * <dl>
     *   <dt><code>COMPNAME</code></dt><dd>"R ifr80"</dd>
     *   <dt><code>APPLICATION</code></dt><dd>"ODB"</dd>
     *   <dt><code>APPVERSION</code></dt><dd>"70404"</dd>
     *   <dt><code>UNICODE</code></dt><dd>false</dd>
     *   <dt><code>SQLMODE</code></dt><dd>INTERNAL</dd>
     * </dl>
     * @param servernode The database server name where the instance is running (ASCII).
     * @param serverdb The name of the database instance (ASCII).
     * @param username The name of the database user.
     * The encoding of this string depends from the encoding argument.
     * @param password The database users password.
     * The encoding of this string depends from the encoding argument.
     * @param properties further connection properties.
     * @param servernodeLength The length in bytes of the database server name.
     * @param serverdbLength The length in bytes of the database instance name.
     * @param usernameLength The length in bytes of the database user name.
     * @param passwordLength The length in bytes of the database users password.
     * @param userpwdEncoding The character encoding of the username/password combination.
     * @return <code>IFR_OK</code> if the connection is established,
     *   <code>IFR_NOT_OK</code> otherwise, in which case also the
     *   appropriate error is set on this connection object.
     */
    IFR_Retcode connect(const char *servernode,
                        IFR_Length servernodeLength,
                        const char *serverdb,
                        IFR_Length serverdbLength,
                        const char * username,
                        IFR_Length usernameLength,
                        const char * password,
                        IFR_Length passwordLength,
                        const IFR_StringEncoding userpwdEncoding,
                        const IFR_ConnectProperties& properties);

    /**
     * Returns true if the connection is correctly connected.
     * @return <code>true</code> if the connection has been
     *   established correctly. It is not checked whether
     *   the connection may have been timed out or the
     *   database server went down in the meantime.
     */
    inline IFR_Bool isConnected() const
    {
        return ((m_connectionid >= 0) ? true : false);
    }

    /**
     * Cancels the currently running request that is executed
     * on the connection.
     * @return <code>IFR_OK</code> if the cancel request was sent,
     *   <code>IFR_NO_DATA_FOUND</code> if there was nothing
     *   to cancel.
     */
    IFR_Retcode cancel();

    /**
     * Gets the date/time format currently active.
     * @return the id of the date/time format (tgg_datetimeformat).
     */
    IFR_DateTimeFormat::Format getDateTimeFormat() const;

    /**
     * Retrieves whether the database is an unicode database or
     * not.
     * @return <code>true</code> if the database is an unicode database.
     */
    IFR_Bool isUnicodeDatabase() const;

  /**
   * Gets the kernel version in the format:.
   *   - 1 digit  major number,
   *   - 2 digits minor number,
   *   - 2 digits correction level.
   * @return The kernel version, which is a computed major * 10000 +
   * minor * 100 + correction_level.  E.g. version 7.4.4 has the
   * version number 70404.
   */
    IFR_Int4 getKernelVersion() const;

    /**
     * Gets the next cursor name.
     * @param cursorname The cursor name to fill.
     * @param memory_ok Flag to indicate an out of memory error.
     */
    void nextCursorName(IFR_String& cursorname,
                        IFR_Bool& memory_ok);

    /**
     * Sets the transaction isolation level.
     * @param isolationlevel The (native) isolation level that shall be used.
     * @return <code>IFR_OK</code> if the isolation level was set,
     *   <code>IFR_NOT_OK</code> if the isolation level could not be changed,
     *    in this case the error of the connection is set.
     */
    IFR_Retcode setTransactionIsolation(IFR_Int4 isolationlevel);

    /**
     * Gets the transaction isolation level that is currently set.
     * @return The isolation level (0, 1, 10, 15, 2, 20, 3, or 30).
     */
    IFR_Int4 getTransactionIsolation() const;

    
    /**
     * Set the client application id.
     * @return @c IFR_OK if the client application id is set.
     */
    IFR_Retcode setClientApplication(const char *clientapplication);
    
    /**
     * Get the client application.
     */
    const char *getClientApplication() const;
    
    /**
     * Get the connection id returned in the session info part
     * of the connect statement.
     * @return The connection id, or -1 if not connected.
     */
    inline IFR_Int4 getConnectionID() const
    {
        return m_sql_connectionid;
    }

    /**
     * Get the session id.
     */
    inline IFR_Int8 getSessionID() const
    {
        return m_connectionid;
    }

    /**
     * Drops a parse id.
     * @param parseid The parse id to drop.
     * @todo Make this function collecting and doing the physical dropping
     *       as appendix to the physical execute.
     */
    void dropParseID(IFR_ParseID& parseid, IFR_Bool& memory_ok);

    /**
     * Drops a cursor.
     * @param cursorname The cursor name to drop.
     * @todo Make this function collecting and doing the physical dropping
     *       as appendix to the physical execute.
     */
    void dropCursor(IFR_String& cursorname, IFR_Bool& memory_ok);


    /**
     * Get the parse info cache instance.
     * @return The parse info cache instance, or 0 if the object is not connected.
     */
     inline IFR_ParseInfoCache* getParseInfoCache()
    {
        return m_parseinfocache;
    }

    /**
     * Get the maximum size of packets transmitted to the database
     * kernel.
     * @return The packet size that is valid for this connection, or 0
     * if the connection is not connected.
     */
    inline IFR_Int4 getPacketSize() const
    {
        return m_packetsize;
    }

    /**
     * Get the producer flag of the connection.
     * @return The procucer that can be set via the connect properties.
     */
    inline tsp1_producer_Enum getProducer() const
    {
        return m_producer;
    }

   /**
     * Returns true if the with info flag was set.
     * @return <code>true</code> if the with info flag was set.
     */
    inline IFR_Bool isWithInfoSet() const
    {
        return m_withInfo;
    }

    /**
     * @brief Gets the connection object.
     * @return It self.
     */
    virtual IFR_Connection* getConnection() const;


    inline IFR_Environment& getEnvironment()
    {
        return m_environment;
    }

    inline IFR_Int4 getConnectCount() const
    {
        return m_connectcount;
    }

    IFR_Int4 getMinimumLongFetchSize() const;


    /**
     * Drops accumulated long descriptors that are no longer needed.
     */
    void dropLongDescriptor(IFRPacket_LongDescriptor& longdesc, IFR_Bool& memory_ok);

    void freeRequestPacket(void *packetPointer);

    /**
     * Flag check, whether blanks should be always trimmed, also for CHAR columns.
     */
    inline IFR_Bool chopBlanks() const
    {
        return m_connectionflags.chop_blanks==1 ? true : false;
    }
    
    /**
     * Flag check, whether numbers shall be formatted always without
     * exponent.
     */
    inline IFR_Bool numbersWithoutExponent() const
    {
        return m_connectionflags.numbers_without_exponent==1 ? true : false;
    }

    /**
     * Flag check, whether numbers are truncated.
     */
    inline IFR_Bool truncateNumbers() const
    {
        return m_connectionflags.number_trunc==1 ? true : false;
    }
    
    /**
     * Check whether ASCII shall mean 7-bit ASCII.
     */
    inline IFR_Bool ascii7Bit() const
    {
        return m_connectionflags.ascii_7bit == 1 ? true : false;
    }

    /**
     * Check whether ODBC binary <-> hexstring conversion
     * is needed.
     */
    inline IFR_Bool odbcBinToHex() const
    {
        return m_connectionflags.odbc_bin2hex == 1 ? true : false;
    }

    /**
     * Check whether on conversion errors the validity of a 
     * parse id should be checked.
     */
    inline IFR_Bool reparseOnConversionError() const
    {
        return m_connectionflags.reparse_conversionerror == 1 ? true : false;
    }
    
    /**
     * Flag check, whether even statements with application parameter
     * descriptions shall be cached.
     */
    inline IFR_Bool getCacheAllStatements() const
    {
        return m_connectionflags.cache_all_statements==1 ? true : false;
    }

    IFRConversion_Converter *findSpecialParameter(IFR_Int4 tag);


    /**
     * Gets the default result set type, which depends on the
     * 'scrollable' option set.
     */
    inline IFR_Int4 getDefaultResultSetType() const
    {
        if(m_connectionflags.scrollable == 1) {
            return ::SQLDBC::SQLDBC_Statement::FORWARD_ONLY;
        } else {
            return ::SQLDBC::SQLDBC_Statement::SCROLL_SENSITIVE;
        }
    }
        
    inline IFR_Bool getScrollableOption(IFR_Int4 resultSetType)
    {
        if(m_connectionflags.scrollable == 1) {
            if(resultSetType == ::SQLDBC::SQLDBC_Statement::FORWARD_ONLY) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;  // always 0 if the kernel does not understand this feature
        }
    }

    enum SelectFetchStrategy {
        SelectFetchNone_C = 0,
        SelectFetchPrepare_C = 1,
        SelectFetchScrollable_C = 2,
        SelectFetchFetchImmediate_C = 3
    };
    
    inline SelectFetchStrategy getSelectFetchStrategy()
    {
        return (SelectFetchStrategy) m_connectionflags.selectfetchstrategy;
    }

    /**
     * Switches off the kernel trace.
     * @return IFR_OK if the kernel trace stop command has been sent,
     *   IFR_NOT_OK if the kernel trace stop command could not be
     *   sent, because of an error or timeout.
     */
    IFR_Retcode stopKerneltrace();

    /**
     * Drops accumulated long descriptors that are no longer needed.
     */
    void dropGarbageLongDescriptors(IFR_Bool& memory_ok);
  
private:

    IFR_Retcode executeInternalCommand(const char *command);

    /*!
    @brief Get infos from a connection. The replypacket of the connect command
    is filled with various infos about the database.
    This function will get the:
    - The date/time format
    - whether the database has unicode enabled
    - The kernel session id
    - The kernel version
   */
    void setSessionInfos(IFRPacket_SessionInfoPart& sessioninfo);
    
    /**
     * Evaluates the feature part returned on connect and checks
     * the enabled/disabled features.
     * @param featurepart The feature part to evaluate.
     */
    void evalFeaturePart(IFRPacket_FeaturePart& featurepart);
        
    /**
     * @internal
     * Creates the connection URL from the server node name, the
     * database name, and the connection properties.
     * @param url The url is assigned to this string.
     * @param servernode The node name of the database (may be 0).
     * @param servernodeLength The length of the server node name (if IFR_NTS, it is assumed
     *    that the server node name is zero-terminated).
     * @param memory_ok Flag to indicate an out of memory error.
     */
    IFR_Bool createConnectURL(IFR_String& url,
                              const char *servernode,
                              IFR_Length servernodeLength,
                              const char *serverdb,
                              IFR_Length serverdbLength,
                              const IFR_ConnectProperties& properties,
                              IFR_Bool& memory_ok);

    IFR_Bool createConnectCommand(IFR_String& connectCommand,
                                  const IFR_ConnectProperties& properties,
                                  IFR_Bool& memory_ok);

    /**
     * Check for XUSER key and evaluate it. The properties are updated according to it.
     */
    void evaluateKey(IFR_ConnectProperties& properties);

    IFR_Bool checkProperties(IFR_ConnectProperties& properties);

    /**
     * Convert client mode to database mode. Client mode knows of SAP R/3.
     */
    static inline IFR_SQLMode getDatabaseSQLMode(IFR_SQLMode clientSQLMode)
    {
        if(clientSQLMode == IFR_SAPR3) {
            return IFR_ORACLE;
        } else {
            return clientSQLMode;
        }
    }

    inline IFR_Int8 getConnectionId() { return m_connectionid; }
    
    /**
     * Releases the RTE connection, hereby taking care of appropriate locking.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> if
     *   the session was already released.
     */
    IFR_Retcode release();

    /**
     * Initializes all members. This is called in constructor and
     * after <code>close(IFR_Bool, IFR_Bool)</code>
     * @param memory_ok Flag to check for memory problems.
     * @param clearStatement Flags whether to erase the statement member.
     */
    void initializeMembers(IFR_Bool& memory_ok, IFR_Bool clearStatement=true);

    /**
     * Appends parse ids that must be dropped to the request packet.
     * @param requestPacket The request packet.
     * @param memory_ok Flag to indicate an out of memory condition.
     */
    void appendGarbageParseIDs(IFRPacket_RequestPacket& requestPacket,
                               IFR_Bool& memory_ok);
    /**
     * Drops parse ids that are no longer needed.
     */
    void dropGarbageParseIDs(IFR_Bool& memory_ok);


    /**
     * Drops accumulated long descriptors that are no longer needed.
     */
    void appendGarbageLongDescriptors(IFRPacket_RequestPacket& requestPacket, 
                                      IFR_Bool& memory_ok);

    /**
     * Setups data attached to the connection:
     * - the connect command
     * - the internal statement for SET ISOLATION LEVEL etc.
     * - the special value converter instances.
     * @param connectCommand The connect command.
     * @param memory_ok Memory check flag.
     */
    void setupConnectionData(IFR_String& connectCommand, IFR_Bool& memory_ok);
    
    struct ConnectionFlags {
        unsigned unicode             : 1;
        unsigned autocommit          : 1;
        unsigned collectparseids     : 1;
        unsigned withinfo            : 1;
        unsigned spaceoption         : 1;
        unsigned optimizedstreams    : 1;
        unsigned variableinput       : 1;
        unsigned variableoutput      : 1;
        unsigned multipleparseiddrop : 1;
        unsigned clientunicode       : 1;
        unsigned ignore_columnnames  : 1;
        unsigned ignore_serials      : 1;
        unsigned cache_all_statements: 1;
        unsigned scrollable          : 1;
        unsigned selectfetchstrategy : 2;
        unsigned chop_blanks         : 1;
        unsigned numbers_without_exponent : 1;
        unsigned ascii_7bit          : 1;
        unsigned odbc_bin2hex        : 1;
        unsigned reparse_conversionerror : 1;
        unsigned number_trunc        : 1;
    };
    
    IFR_Environment&              m_environment;       //!< The environment under which the connection is created.
    IFRUtil_UniqueID              m_uniqueid;          //!< ID generator for cursor names.
    IFR_Int8                      m_connectionid;      //!< RTE handle of connection.
    IFR_DateTimeFormat::Format    m_datetimeformat;    //!< Current date/time format (<code>tgg_datetimeformat</code>).
    char                          m_kernelversion[6];  //!< The kernel version.
    SQLDBC_IRuntime::MutexHandle  m_sqlaexecute_lock;  //!< The lock that is around the sqlaexecute method.
    IFR_Int4                      m_sqlaexecute_executing; //!< Flag, set to 1 when executing.
    IFRPacket_RequestPacket      *m_requestpacket;     //!< The request packet.
    IFR_Int4                      m_packetsize;        //!< The packet size.
    IFRPacket_RequestPacket::PacketType m_packetmode;  //!< Packet mode.
    IFR_String m_connecturl;      //!< The URL that was used in the connect.
    IFR_String m_connectcommand;  //!< The command that was used in the connect.
    IFR_String m_databasename;    //!< Instance.
    IFR_String m_hostname;        //!< Host.
    IFR_String m_username;        //!< User.
    IFR_Int4           m_sql_connectionid;       //!< The connection id from the sessioninfo part.
    IFR_SQLMode         m_client_sqlmode;        //!< SQL Mode the client has specified.
    char                m_client_application[4]; //!< Application id.
    char                m_client_version[6];     //!< Application version.
    SQLDBC_IRuntime::MutexHandle m_status_lock;  //!< Lock around some status changing methods.
    SQLDBC_IRuntime::MutexHandle m_packetcache_lock;  //!< Lock around packet memory cache.
    IFR_Int4            m_isolationlevel;        //!< The isolation level.
    IFR_Statement      *m_internalstatement;     //!< The internal statement used for set commands, commit, rollback.
    IFR_ParseInfoCache *m_parseinfocache;        //!< The parse info cache.
    IFR_Bool            m_accumulate_parseids;   //!< Should we accumulate parse ids or drop them immediately?
    IFR_Bool            m_withInfo;              //!< Marks wheather the with info flag should be set or not
    IFR_Bool            m_cancellationpoint;     //!< Flag set if a cancellation is appropriate.
    IFR_Bool            m_connection_closing;    //!< Flag set when in mid of connection closing.
    IFR_Int4            m_connectcount;          //!< Counter, increased each time the connection has been closed.
    IFR_Int4            m_mingetvalsize;         //!< Minimum size of a GETVAL operation.
    void               *m_packetcache;           //!< Pointer to list of allocated but released packets.
    ConnectionFlags     m_connectionflags;       //!< Various flags for various purposes.
    tsp1_producer_Enum  m_producer;              //!< producer label in segment

    IFRUtil_Vector<IFR_ParseID> m_garbageparseids; //!< The parse ids which are to be dropped are collected here.
    IFRUtil_Vector<IFR_String>  m_garbagecursors;  //!< The cursor names which are to be dropped are collected here.
    IFRUtil_Vector<IFRPacket_LongDescriptor> m_garbagelongs; //!< The long descriptors that need to be dropped.
    
    IFRConversion_Converter  *m_specialparameters[2]; //!< Special parameters.
 
    friend IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_Connection& p );
};

/**
   Traces the connection object.
   Currently are traced:
    - The connection id
    - The kernel session id
    - The sqlmode of the connection
    - If the client is unicode
    - If autocommit is switched on

 */
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_Connection& connection);

IFR_END_NAMESPACE

#endif //IFR_CONNECTION_H
