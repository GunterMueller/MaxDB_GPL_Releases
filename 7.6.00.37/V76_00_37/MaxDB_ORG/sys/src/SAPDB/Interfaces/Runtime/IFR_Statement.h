/*!
  @file           IFR_Statement.h
  @author         BurkhardD
  @ingroup        IFR_Statement
  @brief          Implements class for sql processing
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
#ifndef IFR_STATEMENT_H
#define IFR_STATEMENT_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "SAPDB/Interfaces/Runtime/IFR_String.h"
#include "SAPDB/Interfaces/Runtime/IFR_ErrorHndl.h"
#include "SAPDB/Interfaces/Runtime/IFR_ResultSet.h" 
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"
#include "SAPDB/Interfaces/Runtime/IFR_Types.h"
#include "SAPDB/Interfaces/Runtime/IFR_Parameter.h"
#include "SAPDB/Interfaces/Runtime/IFR_SQLWarning.h"
#include "SAPDB/Interfaces/Runtime/IFR_FetchInfo.h"
#include "SAPDB/Interfaces/Runtime/IFR_ConnectionItem.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h"

using namespace SQLDBC;

IFR_BEGIN_NAMESPACE

class IFR_Connection;
class IFRPacket_RequestSegment;

//------------------------------------------------------------
// INCREMENT PROFILE DATA COUNTER
#define IFR_STMT_PROFILE_COUNT_INC(x) (this->m_counter_4[IFR_StatementProfile:: x##_STMT]++)
#define IFR_STMT_PROFILE_COUNT_ADD(x, y) (this->m_counter_4[IFR_StatementProfile:: x##_STMT]+=y)
#define IFR_STMT_PROFILE_COUNT_8_ADD(x, y) (this->m_counter_8[IFR_StatementProfile:: x##_8_STMT]+=y)

class IFR_StatementProfile{
public:
    void resetCounters();
    void submitCounters(IFR_UInt8 *counter_8, IFR_UInt4 *counter_4);

    enum ProfileCounter_8 {
      IFR_DATAREADSIZE_8_STMT      = 0,
      IFR_DATAWRITESIZE_8_STMT     = 1,
      IFR_LONGDATAREADSIZE_8_STMT  = 2,
      IFR_LONGDATAWRITESIZE_8_STMT = 3
    };

    enum ProfileCounter {
      IFR_NOTSELECTFETCHOPTIMIZED_STMT = 0 ,
      IFR_PREPARECOUNT_STMT = 1,
      IFR_PARSEINFOHITCOUNT_STMT = 2,
      IFR_PARSINFOMISSCOUNT_STMT = 3,
      IFR_PARSEAGAINCOUNT_STMT = 4,
      IFR_PARSEINFOSQUEEZECOUNT_STMT = 5,
      IFR_EXECUTECOUNT_STMT = 6,
      IFR_INSERTCOUNT_STMT = 7,
      IFR_UPDATECOUNT_STMT = 8,
      IFR_SELECTCOUNT_STMT = 9,
      IFR_CALLDBPROCEDURECOUNT_STMT = 10,
      IFR_DELETECOUNT_STMT = 11,
      IFR_FETCHCOUNT_STMT = 12,
      IFR_SQLERRORCOUNT_STMT = 13,
      IFR_ROWNOTFOUNDCOUNT_STMT = 14,
      IFR_FETCHROWCOUNT_STMT = 15
     };
protected:
    friend class IFR_RowSet;
    friend class IFR_ResultSet;
    IFR_StatementProfile();
    
    IFR_UInt8 m_counter_8[4];
    IFR_UInt4 m_counter_4[16];

};

/*!
  @class IFR_Statement
  @brief A class for executing SQL commands immediately.
  
  Handles any DDL or DML command without parameter marker '?'.

  <b>Comments</b>
  - Applications should call createStatement to get a new statement object.
  - Use IFR_PreparedStmt for executing more than once or with
  parameters. 
  - Use hasResultSet to check if the SQL command returns a result set.
  - Manipulation of the result set behavior (rowlimits, scrollability, 
  updatability) is possible with some methodes. 
  - An opened result set is closed automaticly by reuse the execute
  method or destroying the object.

 */

class IFR_Statement 
    : public IFRUtil_RuntimeItem, 
      public IFR_ConnectionItem,
      public IFR_StatementProfile
{

public:
  /*!
    @brief Destructor.
  */
  virtual ~IFR_Statement ();
    
  enum ResultSetType {
    /**
     * The result set can scroll forward only.
     */
    FORWARD_ONLY = SQLDBC_Statement::FORWARD_ONLY,

    /**
     * The content of result set may change.
     */
    SCROLL_SENSITIVE = SQLDBC_Statement::SCROLL_SENSITIVE,

    /**
     * The content of result set may not change (FOR REUSE).
     */
    SCROLL_INSENSITIVE = SQLDBC_Statement::SCROLL_INSENSITIVE
  };

  enum ConcurrencyType {
    /**
     * The result can be updated (FOR UPDATE).
     */
    UPDATABLE = SQLDBC_Statement::CONCUR_UPDATABLE,

    /**
     * The result is read only.
     */
    READ_ONLY = SQLDBC_Statement::CONCUR_READ_ONLY
  };

  enum HoldabilityType {
    /**
     * The result persists over a COMMIT command.
     */
    HOLD_CURSORS_OVER_COMMIT = SQLDBC_Statement::CURSOR_HOLD_OVER_COMMIT,

    /**
     * The result is closed on COMMIT.
     */
    CLOSE_CURSOR_ON_COMMIT = SQLDBC_Statement::CURSOR_CLOSE_ON_COMMIT
  };

public:
    /**
     * Executes an SQL command. 
     * 
     * The SQL command is executed immediately by sending a DBS request to the
     * database. In case of an error the error object (to be queried using <code>error()</code>)
     * of this object is set. If the executed statement is a query, a result set is created,
     * which may the application check using <code>isQuery()</code>, and obtain using
     * <code>getResultSet()</code>.
     * @param sql The SQL statement to execute.
     * @param encoding The character encoding of the SQL statement.
     * @param recycleObjects Whether it is allowed for the runtime to append garbage collection
     *        internal commands to the command (default <code>true</code>).
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> else, in which case
     *        the error has been set.
     */
    IFR_Retcode execute(const char *sql, const IFR_Length sqlLength, const IFR_StringEncoding encoding, IFR_Bool recycleObjects=true);

    IFR_Retcode execute(const char *sql, const IFR_StringEncoding encoding, IFR_Bool recycleObjects=true);
    
  
  /*!
    @brief Sets the maximum number of rows to fetch at once.

    Sets the maximum number of rows to fetch from a result set. Use this to 
    to manipulate the number of rows in the packet interface. 
    Set rows to a value > 0 to set the number of rows. 
    Set rows to value <= 0 to reset the number of rows to the default. 
    The default value is unlimited (32767).
    Setting this value doesn't affect an already executed SQL command.
    @param rows [in] Number of Rows to fetch.
    @return none
  */
  
  void setFetchSize (IFR_Int2 rows);
  
  /*!
    @brief Gets the maximum number of rows to fetch.
    
    Returns the maximum number of rows to fetch from a result set at once
    from the database.
    This value may set with setFetchSize before the execution of a SQL command.
    @return Maximum number of rows Number to fetch at once.
    
   */
  
  IFR_Int2 getFetchSize() const;
  
  /*!
    @brief Limits the maximum number of rows of a resultset.

    The numer of rows are truncated to the value of rows if the result of a
    query statement is larger.
    The default is unlimitted (0).
    Setting this value doesn't affect an already executed SQL command.
    @param rows [in] Maximum number of rows of a query
    @return none
  */
  
  void setMaxRows(IFR_UInt4 rows);

  /*!
    @brief Gets the maximum number of rows returned from a resultset.

    This value may set with setMaxRows before the execution of a SQL command.
    @return Maximum number of rows of a query
   */
  
  IFR_UInt4 getMaxRows() const;
  
  /*!
    @brief Returns a reference to the resultset object.
    
    @return <code>*resultset</code> if the statement object has a resultset
    or NULL if not.
  */
  IFR_ResultSet* getResultSet() const;

public:  
    /**
     * Adds a statement for batched execution.
     * @param sql       The statement.
     * @param sqlLength The length of the statement.
     * @param encoding  The encoding of the statement.
     * @return IFR_OK on success.
     */
    virtual IFR_Retcode addBatch(const char *sql, const IFR_Length sqlLength, const IFR_StringEncoding encoding);
    /**
     * Adds a statement for batched execution.
     * @param sql       The statement.
     * @param encoding  The encoding of the statement.
     * @return IFR_OK on success, IFR_NOT_OK on error. The batch array is not cleared on error,
     *   just the element isn't inserted.
     */
    virtual IFR_Retcode addBatch(const char *sql, const IFR_StringEncoding encoding);

    /**
     * Clears the batch array.
     */
    virtual IFR_Retcode clearBatch();

    /**
     * Gets the last inserted key.
     */
    IFR_Retcode getLastInsertedKey(IFR_Int4     tag,
                                   IFR_HostType type,
                                   void            *paramAddr,
                                   IFR_Length   *lengthIndicator,
                                   IFR_Length    size,
                                   IFR_Bool      terminate);
    
    /**
     * Executes the batch commands. 
     * @return #IFR_OK on success, #IFR_NOT_OK on error, in which case the row status array 
     *   will give more details, IFR_NO_DATA_FOUND if there are no batch statements to
     *   execute.
     */
    virtual IFR_Retcode executeBatch();
    
   /**
     * @brief Gets the size of the row array.
     * @return The number of elements of the row status array, which is also
     *   the number of array elements that are used for batch execution, but 
     *   at least 1 (for single execution).
     */
    IFR_Length getRowArraySize() const;

    /**
     * @brief Gets the row status array.
     * @return A pointer to the first element of the row status array.
     */
    const IFR_Int4 *getRowStatus() const;

    
    /**
     * Get the cursor name.
     * @param buffer The buffer where the cursor name is copied into.
     * @param encoding the encoding of the buffer that is requested.
     * @param bufferSize The size in bytes of the target buffer.
     * @param bufferLength [out] The number of bytes copied to the buffer, 
     *        except the number of bytes necessary for the zero terminator. 
     *        If the source string exceeds the <code>bufferSize</code>.
     *        IFR_DATA_TRUNC will returned and the <code>bufferLength</code>
     *        is set to the number of bytes (except the terminator bytes) 
     *        needed to copy without truncation.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code>
     *         if the provided buffer could not store the complete name.
     */
    IFR_Retcode getCursorName(char                *buffer,
                              IFR_StringEncoding   encoding,
                              IFR_Length           bufferSize,
                              IFR_Length          *bufferLength) const;

    /**
     * Gets the cursor name.
     * @return A reference to the cursor name.
     */
    const IFR_String& getCursorName() const;


    /**
     * Get the table name (select for update).
     * @param buffer The buffer where the table name is copied into.
     * @param encoding the encoding of the buffer that is requested.
     * @param bufferSize The size in bytes of the target buffer.
     * @param bufferLength [out] The number of bytes copied to the buffer, 
     *        except the number of bytes necessary for the zero terminator. 
     *        If the source string exceeds the <code>bufferSize</code>.
     *        IFR_DATA_TRUNC will returned and the <code>bufferLength</code>
     *        is set to the number of bytes (except the terminator bytes) 
     *        needed to copy without truncation.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code>
     *         if the provided buffer could not store the complete name.
     */
    IFR_Retcode getTableName(char                *buffer,
                             IFR_StringEncoding   encoding,
                             IFR_Length           bufferSize,
                             IFR_Length          *bufferLength) const;

    /**
     * Gets the table name (select for update of)
     * @return A reference to the table name.
     */
    const IFR_String& getTableName() const;
    
    /**
     * Sets the cursor name.
     * @param name The new cursor name.
     * @param length The length of the cursor name, or #IFR_NTS to signal a null-terminated string.
     * @param encoding The encoding of the cursor name.
     * @return @c IFR_OK on success, @c IFR_NOT_OK on failure, in which
     *         case the error is set.
     */
    IFR_Retcode setCursorName(const char *name, 
                       IFR_Length length, 
                       IFR_StringEncoding encoding);
    
    /**
     * Sets the command info (for example name of programm) and a line number. 
     * This will be send to the database kernel as part of a parse order. 
     * If the kernel runs in a special diagnose mode these information will be 
     * stored in a system table and can be used to profile your application.
     *
     * @param commandInfo the command info.
     * @param length The length of commandInfo, or #IFR_NTS to signal a null-terminated string,
     * or SQLDBC_NULL_DATA to unset the command info.
     * @param lineNumber line number.
     * @return @c IFR_OK on success, @c IFR_NOT_OK on failure, in which
     *         case the error is set.
     */
    IFR_Retcode setCommandInfo(const char *commandInfo, 
                               SQLDBC_Length bufferLength, 
                               SQLDBC_Int4 lineNumber);                   
  
    /**
     * Checks if the statement is a query. A statement is a query only
     * if it returns a result set - which can be determined for a
     * statement object only after execution. <i>Note that this method
     * is overridden by the <code>IFR_PreparedStmt</code> class, which
     * can determine this property before a statement is executed.</i>
     * @return <code>true</code> if the statement did produce a result
     *         set, <code>false</code> if not, or it cannot be determined.
     */
    virtual IFR_Bool isQuery() const;
    
    
  /*!
    @brief Return the affected rows of the executed SQL.

    This method returns a none zero value if more than one row was affected 
    by the SQL. If the return value is lower than zero the number of 
    rows affected are more than one but the exact number of rows can 
    not be determine by the database. 
    @return Number of rows affected 
  */
  const int getRowsAffected() const;
  
  /*!
    @brief Set the type of a resultset.

    There are three kind of resultset possible: 
    - The result set can only scroll forward <b>FORWARD_ONLY</b>
    - The result set is scrollable but may change <b>SCROLL_SENSITIVE</b>
    - The result set is scrollable and not change <b>SCROLL_INSENSITIVE</b>
    
    The default for the resultset type is <b>IFR_TYPE_SCROLL_INSENSITIVE</b>
    @param type [in] The resultset type
    @return none 
    @todo Implement this methode
  */
    void setResultSetType (ResultSetType type );

  /*!
    @brief Set the type of the resultset concurrency.

    There are two kinds of concurrency: 
    - The result set is updatable <b>UPDATABLE</b>
    - The result set is read only <b>READ_ONLY</b>
    
    The default for the concurrency is <b>READ_ONLY</b>
    @param type [in] The resultset type
    @return none 
    @todo Implement this methode
  */
    void setResultSetConcurrencyType (ConcurrencyType type );
  
  /*!
    @brief Gets the type of a resultset.

    If the requested resultsettype isn't possible the interface runtime change
    it to a possible on. Check this if the execute method returns with a warning.
    @return IFR_TYPE_FORWARD_ONLY, IFR_TYPE_SCROLL_SENSITIVE or IFR_TYPE_SCROLL_INSENSITIVE
    @todo Implement this methode
  */
  int getResultSetType ();
    
  /*!
    @brief Gets the concurrency type of a resultset.

    If the requested concurrency resultsettype isn't possible the interface runtime change
    it to a possible on. Check this if the execute method returns with a warning.
    @return IFR_TYPE_FORWARD_ONLY, IFR_TYPE_SCROLL_SENSITIVE or IFR_TYPE_SCROLL_INSENSITIVE
    @todo Implement this methode
  */
  ConcurrencyType getResultSetConcurrencyType ();
  
  //    void close();    in C++ we need only the destructor
    
    /**
     * Gets the encoding of the command.
     *
     * The command encoding depends on the encoding of the parsed/executed
     * SQL command. It directly derives from the encoding of the instance type:
     * <ul>
     *   <li>If it is an ASCII instance, all commands are sent using 
     *       ASCII (<code>IFR_StringEncodingAscii</code>) encoding.
     *   </li>
     *   <li>If it is an UNICODE instance, commands that have ASCII encoding
     *       are sent as ASCII, commands that have other encodings are sent
     *       in UCS2 encoding, in the native swapping of the client 
     *       (<code>IFR_StringEncodingUCS2Native</code>).
     *   </li>
     * </ul>
     * @return <code>encoding</code> The encoding of the command.
     */
    IFR_StringEncoding getCommandEncoding() const;
    
    /**
     * Gets the connection object for this statement.
     * @return A pointer to the connection object.
     */  
    virtual IFR_Connection *getConnection() const;

    inline void setCursorClosed()
    {
        m_cursorstate = CursorStateUsedClosed_C;
    }
    
protected:
    /**
     * @brief Sets the size of the row array expected at a batch execute.
     * @param rowarraysize [in] the number of rows expected.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   otherwise.
     */
    IFR_Retcode setRowArraySize(IFR_Length rowarraysize);

  
    /*!
      @brief Creates a new statement object using a connection. 
      
      This constructor should never called directly. Functions should use
      IFR_Connection->createStatement to create a new statement object.
      @param conn [in] connection object
    */
    IFR_Statement(IFR_Connection & conn, IFR_Bool& retcode);
    
  /*!
    @brief Parses the result set informations returned by a statement execution.
    @param replyPacket [in] The reply packet of the statement after execution.
    @param parseonly   [in] Whether this was a parse, and result table name should
                       be handled sensitive.
    @return IFR_Retcode IFR_OK on success 
  */
    IFR_Retcode parseResult (const IFR_String* command, 
                             IFRPacket_ReplyPacket &replyPacket, 
                             IFR_Bool parseonly);

  /**
     @brief Sends the given SQL command to the database honoring the 
     result set kind.
     
     The command is either parsed, or executed with the mass command
     flag for array command. If there is a special result set type is set
     the command will append it with the FOR UPDATE or FOR REUSE attribute.
     @param sql [in] The SQL command.
     @param replyPacket [out] The reply packet, to which the reply is assigned.
     @param masscommand [in] Whether this command should be sent as mass command.
     @param parseonly [in] Whether this command should be parsed
     only (<code>true</code>) or executed (<code>false</code>).
     @param parseAgain [in] Whether this command should be parsed with a 
     'parse again' mark set.
     @return <code>IFR_OK</code> if the request was ok, <code>IFR_NOT_OK</code>
     otherwise (in which case the error in this statement is set).
  */
  IFR_Retcode sendSQL (const IFR_String &sql, 
                       IFRPacket_ReplyPacket& replyPacket,
                       IFR_Bool masscommand,
                       IFR_Bool parseonly,
                       IFR_Bool parseAgain,
                       IFR_Connection::AppendMode appendFlag);
  
  /**
     @brief Sends the given SQL command to the database.
     
     The command is either parsed, or executed with honoring the mass 
     command flag.
     @param requestPacket [in] The request packet to use for sending.
     @param sqlCmd [in] The SQL command.
     @param masscommand [in] Whether this command should be sent as mass command.
     @param parseonly [in] Whether this command should be parsed only (<code>true</code>) or executed (<code>false</code>).
     @param parseAgain [in] Whether this command should be parsed with a 
     'parse again' mark set.
     @param appendFlag [in] Whether appended housekeeping commands are allowed.
     @param replyPacket [out] The reply packet, to which the reply is assigned.
     @return <code>IFR_OK</code> if the request was ok, <code>IFR_NOT_OK</code>
     *       otherwise (in which case the error in this statement is set).
     */
  IFR_Retcode sendCommand (IFRPacket_RequestPacket &requestPacket,
                           const IFR_String& sqlCmd,
                           IFR_Bool masscommand,
                           IFR_Bool parseonly,
                           IFR_Bool parseAgain,
                           IFR_Connection::AppendMode appendFlag,
                           IFRPacket_ReplyPacket& replyPacket);
    
  /**
     @brief Creates a new (updatable) result set object.
     
     Since result sets may be updatable the tablename of the referencing query
     is neccassary. 
     @param tableName [in] The name of the table which is referenced by this 
     query. NULL if the tablename is unknown (updates are impossible on this 
     result set)  
     @param cursorName [in] The name of the result set.
     @param rowNotFound [in] true if the resultsset exsist but empty.
     @return <code>IFR_OK</code> if the resultset is prepared for fetch, 
     *       <code>IFR_NOT_OK</code> otherwise if not.
     @todo Implement UPDATE tableName WHERE CURRENT OF cursorName
     */
  virtual IFR_Retcode createResultSet (IFR_String& tableName,
                                       IFR_String& cursorName,
                                       IFR_Bool rowNotFound,
                                       IFRConversion_ConverterList& info,
                                       IFRUtil_Vector<IFR_String>* colName,
                                       IFR_Bool closeOnLast,
                                       IFR_FetchChunk *firstchunk);
  
    
  /*!
    @brief Set the encoding of the command.

    The command encoding is the encoding that is used to send the command
    to the database. It depends on the packet's swap kind and on the
    command string encoding.
    For ASCII databases, only ASCII encoding is possible. For Unicode
    databases, ASCII and UCS2 in the byte order of the request are
    possible (all encodings can be converted to one of these).
    @param encodingOfCmd [in] The encoding of the command string.
    @param swapped [in] Whether the packet is little endian 
    *      <code>true</code> or big endian <code>false</code>.
    @return <code>true</code> if the encoding of the command matches with the
    *      packet encoding in a way that lossless transfer is possible.
    */  
    IFR_Bool setCommandEncoding(IFR_StringEncoding encodingOfCmd, IFR_Bool swapped);
    
    /**
     * Forcibly sets the command encoding. No checks are done. This is used in
     * case the encoding is stored e.g. in parseinfo cache.
     */
    inline void unsafeSetCommandEncoding(IFR_StringEncoding encoding)
    {
        m_CommandEncoding = encoding;
    }
    
    /**
     * @brief Checks whether the statement's first command is a SELECT.
     *
     * This method honors whether the command starts with space or 
     * <code>'('</code> characters.
     * @param sql The SQL command to check.
     * @param encoding The encoding of the command.
     * @return <code>true</code> if the command starts with a SELECT,
     *       <code>false</code> otherwise.
     */
    // static IFR_Bool isQuery(const char *sql, IFR_StringEncoding encoding);
    

      /**
     * @brief Checks the function code and updates the coresponding statistic.
     *
     * @param fuction code.
     * @return void
     */
     void functionCode2Statistic(IFRPacket_FunctionCode::FunctionCode fcode);

protected:
    /**
     * Closes the current result set and resets the row count.
     * @todo Keep column binding (avoid rebinding).
     */
    virtual void resetResults();

     
    /**
     * Adds an application info part, if applicable. This is actually
     * done only in the derived class.
     * @param segment The request segment.
     * @return IFR_OK on success, IFR_NOT_OK if there was no space left in the packet.
     */
    virtual  IFR_Retcode addApplicationInfo(IFRPacket_RequestSegment& segment);

      
    /**
     * Executes an SQL command. 
     * 
     * The SQL command is executed immediately by sending a DBS request to the
     * database. In case of an error the error object (to be queried using <code>error()</code>)
     * of this object is set. If the executed statement is a query, a result set is created,
     * which may the applicatiin check using <code>isQuery()</code>, and obtain using
     * <code>getResultSet()</code>.
     * @param sql The SQL statement to execute.
     * @param recycleObjects Whether it is allowed for the runtime to append garbage collection
     *        internal commands to the command.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> else, in which case
     *        the error has been set.
     */
    IFR_Retcode execute (const IFR_String &sql, IFR_Bool recycleObjects=true);
    
    /**
     * @brief Checks whether the statement's first command is a SELECT.
     *
     * This method honors whether the command starts with space or 
     * <code>'('</code> characters.
     * @param sql The SQL command to check.
     * @return <code>true</code> if the command starts with a SELECT,
     *       <code>false</code> otherwise.
     */
    static IFR_Bool isQuery(const IFR_String& sql);
    
    /**
     * Sets a new serial part.
     * @param part The serial part.
     * @return @c IFR_OK on success, @c IFR_NOT_OK if a 
     * memory allocation error occurs.
     */
    IFR_Retcode setSerialPart(IFRPacket_DataPart& part);

    IFR_Retcode updateSerialPart(IFRPacket_DataPart& part);

    friend class IFR_FetchInfo; //!< The fetch info needs access to cursor name etc. directly.
    friend class IFR_Connection; //!< For calling the private constructor.    

    class CopiedDataPart
        : public IFRPacket_DataPart
    {
    public:
        inline CopiedDataPart(IFR_Byte *copiedbuffer,
                              IFR_StringEncoding encoding,
                              SAPDBMem_IRawAllocator& _allocator)
        :IFRPacket_DataPart((tsp1_part*)copiedbuffer, encoding),
         allocator(_allocator)
        {
        }
        
        inline ~CopiedDataPart()
        {
            IFR_Byte *to_del=(IFR_Byte*)GetRawPart();
            IFR_ALLOCATOR(allocator).Deallocate(to_del);
        }
        
    private:
        SAPDBMem_IRawAllocator& allocator;
    };
    
    enum CursorState {
        CursorStateNotUsed_C     = 0, //!< 
        CursorStateUsed_C        = 1, //!< Used, must be closed.
        CursorStateUsedClosed_C  = 2  //!< Used and implicitely closed (Oracle, SELECT FETCH Optimization)
    };

    enum CommandInfoState {
        CommandInfoNotSet_C   = 0,
        CommandInfoSet_C      = 1,
        CommandInfoPrepared_C = 2,
        CommandInfoError_C    = 3,
        CommandInfoExecuted_C = 4
    };

    IFR_Int4 m_RowsAffected;                  //!< Number of rows affected by last command (-1 for unknown).
    ResultSetType m_ResultSetType;        //!< Sensitivity of the result set (changes are visible or not, use 
                                              //!< of <code>FOR REUSE</code>)
    ConcurrencyType m_ResultSetConcurrency; //!< Changeability of the result set (read-only, updateable).
    HoldabilityType m_ResultSetHoldability; //!< Commit behavior of the result set (closed on commit, held over commit).
    ResultSetType m_FetchDirection;       //!< Scrollability of the result set (forward only).
    IFR_Int4 m_FetchSize;                     //!< Number of rows to fetch at once (performance hint).
    IFR_Int4 m_MaxRows;                       //!< Maximum number of rows in the result set (fetch will be limited). 
    IFR_Int4 m_QueryTimeout;   //!< Execution time out for sql command    

    
    IFR_Int4         m_commandInfoState;   //!< Status of command info for disposal.
    IFR_Int4         m_commandLineNumber; //!< line number used in command info.    
    IFR_String       m_commandInfo;//!< command info of the statement.

    IFR_String       m_CursorName; //!< Cursor name of the statement.
    IFR_String       m_TableName;  //!< Table name of the statement.
    IFR_ResultSet   *m_resultset;  //!< Result set of this statement.
    IFR_Connection  *m_Connection; //!< Connection of this statement.    
    IFR_StringEncoding m_CommandEncoding; //!< Encoding of the executed command.
    IFR_Int4           m_cursorstate;     //!< Status of cursor.

    IFR_Length                    m_rowarraysize;   //!< number of rows that are subject to the statement
    IFR_Int4                     *m_rowstatusarray; //!< status array for batch execute
    IFR_Length                    m_rowstatussize;  //!< physical size of row status array.
    IFRUtil_Vector<IFR_String>   *m_batchelements;  //!< Elements of batch execution, if any.
    
    CopiedDataPart               *m_lastserial;     //!< Last inserted serials, if any.

    friend class ClearCommandInfo;
    friend class IFR_StatementProfile;
};

/**
 * @ingroup IFR_Trace
 * Trace operator.
 */ 
IFR_TraceStream& operator <<(IFR_TraceStream& s, 
                             const IFR_Statement& statement);

IFR_END_NAMESPACE

#endif // IFR_STATEMENT_H
