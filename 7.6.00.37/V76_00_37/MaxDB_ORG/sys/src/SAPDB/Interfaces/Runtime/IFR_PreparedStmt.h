/*!
  @file           IFR_PreparedStmt.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Statement
  @brief          Implements a class for prepared statement processing
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
#ifndef IFR_PREPAREDSTMT_H
#define IFR_PREPAREDSTMT_H

#include "SAPDB/Interfaces/Runtime/IFR_Statement.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_DefaultRawAllocator.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "SAPDB/Interfaces/Runtime/IFR_LOBHost.h"

IFR_BEGIN_NAMESPACE

class IFR_DataAtExecuteDescriptor;
class IFR_BatchFillOperation;

/**
  @brief A class for preparing and executing SQL commands.
  @ingroup IFR_Common
  A prepared statement, i.e. one that is parsed and can take input and
  output parameters. DDL command are handled also but there is no need to
  do this. The SQL command can processed as mass command with an array
  of parameters.

  @p Notes
  - the application has to call <code>IFR_Connection::createPreparedStatement</code>
    in order to obtain a prepared statement,
  - the <code>prepare</code> method must be called to parse a command,
  - for a single-row execution, use the <code>execute</code> method,
  - for mass execution, bind arrays of parameter values,
    set the row array size (<code>setRowArraySize</code>),
    and use the <code>executeBatch</code> method,
  - use the <code>isQuery</code> method to check whether the parsed
    statement will create a result set upon execution.
  - although the prepared statement inherits all direct execution functions
    from <code>IFR_Statement</code>, you are encouraged to use
    <code>IFR_Statement</code> itself if you want direct execution.
  - the current result set is deleted when a new statement is parsed,
    when the statement is re-executed, or when the statement itself is
    deleted.

  @todo binding by name
  @todo with info/describe for fetch info
 */
class IFR_PreparedStmt
    : public IFR_Statement,
      virtual public IFR_GetvalHost,
      virtual public IFR_PutvalHost
{
    friend class IFR_Connection; //!< for calling the private constructor.

    /**
     * Status information for the statement (for function sequence).
     */
    enum StatementStatus {
        Status_Other_C,          //!< The usual statement status - outside a paramdata/putdata operation.
        Status_ParamData_C,      //!< The statement expects a call to <code>nextParameter</code>
        Status_PutData_C,        //!< The statement expects a call to <code>putData</code> or <code>nextParam</code>.
        Status_ParamDataBatch_C, //!< The statement expects a call to <code>nextParameter</code>, and this was triggered by
                                 //!< a batch execution.
        Status_PutDataBatch_C,   //!< The statement expects a call to <code>putData</code> or <code>nextParameter</code>, and
                                 //!< this was triggered by a batch execution.
        Status_Keep_C,           //!< The statement has LOB objects, and is kept 'open' to put these LOB objects.
        Status_LobsClosed_C      //!< The statement had LOB objects, and all LOBs had been processed (but not detached).
    };

public:
    /**
     * @brief Destructor.
     */
    ~IFR_PreparedStmt();

    // inherited from connection item
    virtual IFR_PutvalHost *getPutvalHost();
    virtual IFR_GetvalHost *getGetvalHost();

    /**
     * Adds a statement for batched execution. This is a IFR_Statement-only-method and it will
     * return an error if it is called from a IFR_PreparedStmt.
     * @param sql       The statement.
     * @param sqlLength The length of the statement.
     * @param encoding  The encoding of the statement.
     * @return always IFR_NOT_OK.
     */
    virtual IFR_Retcode addBatch(const char *sql, const IFR_Length sqlLength, const IFR_StringEncoding encoding);

    /**
     * Adds a statement for batched execution. This is a IFR_Statement-only-method and it will
     * return an error if it is called from a IFR_PreparedStmt.
     * @param sql       The statement.
     * @param encoding  The encoding of the statement.
     * @return always IFR_NOT_OK.
     */
    virtual IFR_Retcode addBatch(const char *sql, const IFR_StringEncoding encoding);

    /**
     * Clears the batch array. This is a IFR_Statement-only-method and it will
     * return an error if it is called from a IFR_PreparedStmt.
     * @return always IFR_NOT_OK.
     */
    virtual IFR_Retcode clearBatch();

    /**
     * Clears all input/output parameters.
     * Note that the parameter bindings are only cleared if @c unbind is true,
     * otherwise only the internal data structures are cleared.
     * @param unbind Flags to indicate also the parameter binding is cleared.
     * @param closeLOBs Flag to indicate that LOB objects are to be closed and cleared.
     */
    IFR_Retcode clearParameters(IFR_Bool unbind=true, IFR_Bool closeLOBs=true);

    /**
     * @brief Binds an input and/or output parameter.
     * @param paramindex [in] Index of the parameter, starting with 1.
     * @param paramtype [in] Host type of the parameter
     * @param paramaddr [in] Pointer to the host variable
     * @param paramlengthindicator [in] Pointer to parameter length or indicator.
     * @param parambytelength [in] Byte length of the parameter.
     * @param terminate Specifies that the output buffer should be finished
     *        with a C style zero terminator. The <code>terminated</code> flag
     *        works only for the hostvar type character (ASCII, UCS2 or UTF8).
     * @return <code>IFR_OK</code> always.
     * @todo Implement various error checks.
     */
    IFR_Retcode bindParameter (IFR_UInt2 paramindex,
                               IFR_HostType paramtype,
                               void *paramaddr,
                               IFR_Length *paramlengthindicator,
                               IFR_Length parambytelength,
                               IFR_Bool terminate=IFR_TRUE);

    /**
     * @brief Binds an input and/or output parameter <i>by address</i>.
     * @param paramindex [in] Index of the parameter, starting with 1.
     * @param paramtype [in] Host type of the parameter
     * @param paramaddr [in] Pointer to <i>a pointer</i> to the host variable.
     * @param paramlengthindicator [in] Pointer to parameter length or indicator.
     * @param parambytelength [in] Byte length of the parameter.
     * @param terminate Specifies that the output buffer should be finished
     *        with a C style zero terminator. The <code>terminated</code> flag
     *        works only for the hostvar type character (ASCII, UCS2 or UTF8).
     * @return <code>IFR_OK</code> always.
     * @todo Implement various error checks.
     */
    IFR_Retcode bindParameterAddr(IFR_UInt2 paramindex,
                                  IFR_HostType paramtype,
                                  void        *paramaddr,
                                  IFR_Length  *paramlengthindicator,
                                  IFR_Length parambytelength,
                                  IFR_Bool terminate=IFR_TRUE);

    /**
     * Retrieve the information about the host variable bound to a parameter.
     * @todo Implement.
     */
    IFR_Retcode getParameterDescriptor(IFR_UInt2 paramindex,
                                       IFR_HostType* paramtype,
                                       void **       paramaddr,
                                       IFR_Length**  paramlengthindicator,
                                       IFR_Length*   parambytelength,
                                       IFR_Bool*     terminate,
                                       IFR_Bool*     byaddr);

    /**
     * Sets the binding type. This is normally 0, which implies a
     * <i>column-wise</i> binding. If it is set to a value <code>!=</code>
     * 0, a <i>row-wise</i> binding is implied.
     * So, the address offset of the next parameter value computes differently for
     * column-wise and row-wise binding:
     * <dl>
     *   <dt>Column-wise Binding</dt>
     *   <dd>byte length of the parameter</dd>
     *   <dt>Row-wise Binding</dt>
     *   <dd><var>size</var></dd>
     * </dl>
     * @param size The row-wise parameter offset, if set to 0, column-wise binding
     *             is implied.
     * @return <code>IFR_OK</code> always.
     */
    IFR_Retcode setBindingType(IFR_size_t size);

    /**
     * Sets the row array size. In contrast to IFR_Statement, where the row array size is
     * determined by the number of statements in the batch, this can be set
     * explicitely.
     * @param rowarraysize The new row array size.
     * @return IFR_OK on success, IFR_NOT_OK otherwise.
     */
    inline IFR_Retcode setRowArraySize(IFR_Length rowarraysize)
    {
        return IFR_Statement::setRowArraySize(rowarraysize);
    }

    IFR_UInt4 getPreferredRowArraySize();
    
    /**
     * @brief Parses a SQL statement an generates short field info.
     * @param sql The SQL statement to prepare.
     * @param encoding The character encoding of the SQL command.
     * @return <code>IFR_OK</code> if successful, <code>IFR_NOT_OK</code> else.
     */
    IFR_Retcode prepare (const char *sql, const IFR_Length sqlLength, IFR_StringEncoding encoding);

    IFR_Retcode prepare (const char *sql, IFR_StringEncoding encoding);

    /**
     * @brief Parses a SQL statement an generates short field info.
     * @param sql The SQL statement to prepare.
     * @return <code>IFR_OK</code> if successful, <code>IFR_NOT_OK</code> else.
     */
    IFR_Retcode prepare(const IFR_String& sql);

    /**
     * @brief Executes a prepared SQL statement
     * The statement must be prepared before, and the appropriate binding variables must have been set.
     * @return - <code>IFR_OK</code> if successful,
     *         - <code>IFR_NOT_OK</code> otherwise, in which case the error has been set,
     *         - <code>IFR_NEED_DATA</code> if the application shall provide the data using
     *           a <code>nextParameter</code> - <code>putData</code> sequence.
     */
    IFR_Retcode execute();

    /**
     * Switches to the next input parameter if the application marked input data
     * as <i>data at execute</i> values. After calling, the <var>index</var>
     * and the <var>parameterdata</var> parameters are set.
     * @param index The index of the next parameter.
     * @param parameterdata The data pointer that was supplied when binding the parameter, plus any offset that
     *        relates from a batch execution sequence (including row-wise binding).
     * @return - <code>IFR_NEED_DATA</code> if <var>index</var> and <var>parameterdata</var> contain
     *           the index resp. address of the next parameter that shall be delivered using
     *           <code>putData</code>.
     *         - <code>IFR_OK</code> if the last parameter has been processed, and the statement was executed successfully.
     *         - <code>IFR_NOT_OK</code> if there is an error (in which case the error of the statement is set).
     */
    IFR_Retcode nextParameter(IFR_Int2& index, void*& parameterdata);

    IFR_Retcode nextParameterByIndex(IFR_Int2& index, void*& parameterdata);

    /**
     * Put data for an input parameter.
     * @param parameterdata The data to put.
     * @param lengthindicator Length, or special indicator value.
     */
    IFR_Retcode putData(void *parameterdata, IFR_Length  *lengthindicator);

    /**
     * Retrieves and converts the value of the designated output column in the current row of this
     * <code>IFR_PreparedStmt</code> object as a <code>IFR_BYTE</code> with the given length and
     * the given <code>IFR_HostType</code>.
     *
     * @param paramindex the first column is 1, the second is 2, ...
     * @param paramaddr pointer the output buffer.
     * @param parambytelength length the output buffer.
     * @param paramtype hostvartype of the output buffer.
     * @param paramlengthindicator pointer to input length or output status indicator.
     * @param terminate indicates if the output buffer should be null terminated or not
     * @param paramposindicator start position in long column to read data from (in bytes). Negative
     *        paramposindicator counts from the end of the long column.
     *
     * @return <code>IFR_OK</code>, if all worked fine; <code>IFR_NOT_OK</code> if a database access
     * error occurs.
     */
    IFR_Retcode getObject (IFR_Int4 paramindex,
	        	   void* paramaddr,
			   IFR_Length parambytelength,
			   IFR_HostType paramtype,
                           IFR_Length* paramlengthindicator,
			   IFR_Bool terminate,
			   IFR_Length* paramposindicator = 0);

    virtual IFR_Retcode closeLOB(IFR_LOB *lob);

    virtual IFR_Retcode getData(IFR_LOB    *lob,
                                void       *data,
                                IFR_Length *lengthindicator,
                                IFR_Length  bytelength,
                                IFR_Length *posindicator,
                                IFR_Bool    terminate);

    virtual IFR_Length getLength(IFR_LOB *lob);

    virtual IFR_Retcode putData(IFR_LOB *lob, void *parameterdata, IFR_Length* lengthindicator);
    
    /**
     * @brief Executes a (mass)command and honors the row array size.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if at least one row failed. Get the row status array in this
     *   case to inspect the situation.
     */
    IFR_Retcode executeBatch();

    /**
     * @brief Retrieves the number, types and properties of this <code>IFR_PreparedStmt</code> object's parameters.
     * @return A <code>IFR_ParameterMetaData</code> that contains information about the
     * number, types and properties of this <code>IFR_PreparedStmt</code> object's parameters. If the
     * meta data cannot retrieved a NULL pointer will be returned
     */
    IFR_ParameterMetaData* getParameterMetaData();

    /**
     * @brief Retrieves a <code>IFR_ResultSetMetaData</code> object.
     * Retrieves a <code>IFR_ResultSetMetaData</code> object that contains information
     * about the columns of the <code> IFR_ResultSet</code> object that will be returned
     * when this <code>IFR_PreparedStmt</code> object is executed.
     * Because a <code>IFR_PreparedStmt</code> object is parsed, it is possible to
     * know about the ResultSet object that it will return without having to execute it.
     * Consequently, it is possible to invoke the method <code>getMetaData</code> on a
     * <code>IFR_PreparedStmt</code> object rather than waiting to execute it and then
     * invoking the <code>IFR_ResultSet</code>.getMetaData method on the ResultSet object
     * that is returned.
     *
     * NOTE: Using this method may be expensive due to the effort to retrieve the meta data
     * from the underlying database kernel.
     * @return A <code>IFR_ResultSetMetaData</code> object that describes the columns or
     * null if the meta data cannot retrieved.
     */
    IFR_ResultSetMetaData* getResultSetMetaData();

    /**
     * Checks if the statement is a query. A statement is a query only
     * if it returns a result set - which can be determined for a
     * statement object only after it has been prepared.
     * @return <code>true</code> if the statement did produce a result
     *         set, <code>false</code> if not, or it cannot be determined.
     */
    virtual IFR_Bool isQuery() const;


protected:
	/**
	 * Adds an application info part to the request segment for parsing.
	 * @param requestsegment The request segment where to insert the application
	 *        info part.
	 * @return <code>IFR_OK</code> on success.
	 */
    virtual IFR_Retcode addApplicationInfo(IFRPacket_RequestSegment& requestsegment);

    virtual IFR_Retcode createResultSet (IFR_String& tableName,
                                         IFR_String& cursorName,
                                         IFR_Bool rowNotFound,
                                         IFRConversion_ConverterList& info,
                                         IFRUtil_Vector<IFR_String>* colName,
                                         IFR_Bool closeOnLast,
                                         IFR_FetchChunk *firstchunk);

    virtual void resetResults();

private:
    /**
     * @brief Constructor for a prepared statement.
     * @param conn The connection of the statement.
     */
    IFR_PreparedStmt (IFR_Connection& conn, IFR_Bool& memory_ok);

    /**
     * Initializes the packet for an execute.
     * <b>The parse info will remain if this operation succeeds, and will get unlocked
     *    if it fails.</b>
     * @param requestpacket The request packet that is used.
     * @param segment The segment.
     * @param datapart The data part which is initialized if
     *                 necessary (if input parameters are present).
     *                 If no input parameters are present, the data
     *                 part will be not valid on output.
     * @return - <code>IFR_NO_DATA_FOUND</code> if the parse id has not been valid,
     *         - <code>IFR_NOT_OK</code> if the creation of packet structures fails,
     *         - <code>IFR_OK</code> on success.
     * @todo Make this function inserting a table name part only in case a table name
     *       is needed by the statement (check the function code).
     */
    IFR_Retcode executeInitPacket(IFRPacket_RequestPacket& requestpacket,
                                  IFRPacket_RequestSegment& segment,
                                  IFRPacket_DataPart &datapart);

    /**
     * Initializes a packet for a PUTVAL operation.
     */
    IFR_Retcode putvalInitPacket(IFRPacket_RequestPacket& requestpacket,
                                 IFRPacket_RequestSegment& segment,
                                 IFRPacket_LongDataPart &datapart);

    /**
     * Fills the data into the execution packet that is available,
     * hereby performing the necessary data conversion.
     * @param segment The request segment.
     * @param datapart The datapart to fill.
     * @param streamstruncated Whether the LONG data to insert has been truncated.
     * @param dontclose Whether the part shall be closed at the end or not.
     * @param startindex The start parameter index to begin with. 
     * @return <code>IFR_OK</code> on success, otherwise the result of the
     *         last data conversion operation.
     */
    IFR_Retcode executeFillDirectData(IFRPacket_RequestSegment& segment,
                                      IFRPacket_DataPart& datapart,
                                      IFR_Bool& streamstruncated,
                                      IFR_Bool dontclose,
                                      IFR_Bool& hasLOBs,
                                      IFR_Int2 startindex);
                               

    /**
     * Populates the output data necessary.
     * hereby performing the necessary data conversion.
     * Output parameters, long descriptors, and a result count is extracted.
     * @param replypacket The reply packet to inspect.
     * @param hasLOBs  Flag, whether there a LOBs.
     * @return <code>IFR_OK</code> on success.
     */
    IFR_Retcode executeFillOutputData(IFRPacket_ReplyPacket& replypacket,
                                      IFR_Bool hasLOBs);

    /**
     * Clears the parameter data information for data at execute handling,
     * and resets the status.
     */
    void clearParamData();


    /**
     * Retrieves the statement that is currently prepared.
     * @param buffer The buffer into which the statement is copied.
     * @param length The length of the buffer on input, and the
     *        length of the string on return.
     * @param encoding The encoding of @c buffer
     * @param
     * @return @c IFR_OK on success, @c IFR_DATA_TRUNC if @c buffer was too small,
     *   @c IFR_NOT_OK if some other error has occurred.
     */
    IFR_Retcode getStatement(char *buffer,
                             IFR_StringEncoding& encoding,
                             IFR_Length bytelength,
                             IFR_Length *length);

    /**
     * @brief Parses the SQL command and stores the result in the parse info
     * member.
     * If the connection has a parse info cache, the parse information
     * is looked up there first, and if the SQL command was really parsed,
     * it is inserted into the cache.
     * @param sql [in] SQL command
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *   a failure.
     */
    IFR_Retcode doParse (const IFR_String& sql);

    enum ColumnChange 
    {
        ColumnChange_Forbidden_C,
        ColumnChange_Allowed_C,
        ColumnChange_Required_C
    };
    
    /**
     * @brief Re-parses the SQL command.
     * Sometimes a parsed statement becomes invalid, thus requiring
     * a re-parse of the SQL statement.
     * @param allowColumnChange Flag, whether changes in the structure of
     *        the parameters are tolerated or not.
     * @param notDropped The parse ids are not dropped yet, or invalidated.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *         a failure.
     */
    IFR_Retcode doParseAgain(ColumnChange columnChange,
                             IFR_Bool parseagain);

    /**
     * @brief Check whether we can parse the command as mass command.
     * @param sqlcommand [in] SQL command
     * @return <code>true</code> if it is possible
     */
    IFR_Bool parseAsMassCommand(const IFR_String& sqlcommand) const;

    /**
     * @brief Insert LONG data for insert at execution time.
     * @param dataPart Destination data part
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *   a failure, <code>IFR_DATA_TRUNC</code> if the LONG data was not
     *   inserted completely.
     */
    IFR_Retcode handleStreamsForExecute (IFRPacket_DataPart& dataPart);


    /**
     * @brief Get long descriptors from the reply segment.
     * @param replySegment Segment filled by kernel with long descriptors
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *   a failure.
     */
    IFR_Retcode getChangedPutvalDescriptors (IFRPacket_ReplySegment& replySegment);

    /**
     * @brief Parses the reply segment after parsing a command by the kernel.
     * @param replySegment Segment filled by kernel
     * @return <code>IFR_OK</code> if the handling went ok, <code>IFR_NOT_OK</code> if
     *         there were errors (e.g. through the incompatibility of parameters and
     *         columns).
     * @todo Handle custom cursor names.
     */
    IFR_Retcode handleParseResult (IFRPacket_ReplyPacket& replypacket,
                                   IFRPacket_ReplySegment& segment);

    /**
     * Gets the <code>IFR_ParseInfo</code>.
     * @return <code>IFR_ParseInfo</code>
     */
    inline IFR_ParseInfo* getParseInfo()
    {
        return this->m_parseinfo;
    }

    /**
     * @brief Inserts the remaining LONG data as individual PUTVAL commands.
     * @param replyPacket The last reply packet, still locked.
     * @param replySegment Segment filled by kernel with LONG descriptors.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *   a failure.
     */
    IFR_Retcode handleStreamsForPutval (IFRPacket_ReplyPacket& replyPacket,
                                        IFRPacket_ReplySegment& replySegment);
    /**
     * @brief Inserts the remaining LONG data as individual PUTVAL commands.
     * @param replyPacket The last reply packet, still locked.
     * @param replySegment Segment filled by kernel with LONG descriptors.
     * @param requestpacket The request packet to use.
     * @param requestsegment The request segment to use.
     * @param longdatapart The LONG data part to use.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *   a failure, <code>IFR_NEED_DATA</code> if
     */
    IFR_Retcode handleStreamsForPutval (IFRPacket_ReplyPacket& replyPacket,
                                        IFRPacket_ReplySegment& replySegment,
                                        IFRPacket_RequestPacket& requestpacket,
                                        IFRPacket_RequestSegment& requestsegment,
                                        IFRPacket_LongDataPart& longdatapart,
                                        IFR_Bool ignore_data_at_execute=false);

    /**
     * @brief Gets the remaining LONG data as individual GETVAL commands.
     *
     * Directly bound parameters are filled directly, LOB objects are just
     * stored. If necessary (for directly bound parameters) additional
     * GETVAL commands are sent.
     * @param oldreplypacket The reply packet of the execute call (for releasing the lock).
     * @param part The data part containing the LONG descriptors.
     * @param info The parameter info.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *   a failure.
     */
    IFR_Retcode handleStreamsForGetval(IFRPacket_ReplyPacket& oldreplypacket,
                                       IFRPacket_DataPart& part,
                                       IFRConversion_ConverterList& info,
                                       IFR_Retcode *colstatus);


    /**
     * Initializes a packet for a GETVAL operation.
     * @param requestpacket the actual request packet
     * @param segment the segment for sending the getval message
     * @param datapart the long data part to be added to the segment
     * @return <code>IFR_OK</code> on success
     *         <code>IFR_NOT_OK</code> otherwise
     */
    IFR_Retcode getvalInitPacket(IFRPacket_RequestPacket& requestpacket,
                                 IFRPacket_RequestSegment& segment,
                                 IFRPacket_LongDataPart &datapart);

    /**
     * Closes the current data item that is subject of the PUTDATA operation.
     */
    IFR_Retcode closeCurrentPutData();

    IFR_Retcode nextParameterInternal(IFR_Int2& index, void*& parameterdata);

    /**
     * Gets the next parameter when the statement is in the
     * status <code>Status_ParamData_C</code>, just right after the
     * <code>execute()</code> call.
     * @param parameterindex The parameter index.
     * @param parameterdata The pointer to the parameter data.
     * @return <code>IFR_NEED_DATA</code> on success, <code>IFR_NOT_OK</code>
     *    on failures.
     */
    IFR_Retcode nextParameterParamData(IFR_Int2& parameterindex, void *& parameterdata);

    /**
     * Gets the next parameter when the statement is in the
     * status <code>Status_ParamDataBatch_C</code>, just right after the
     * <code>execute()</code> call.
     * @param parameterindex The parameter index.
     * @param parameterdata The pointer to the parameter data.
     * @return <code>IFR_NEED_DATA</code> on success, <code>IFR_NOT_OK</code>
     *    on failures.
     */
    IFR_Retcode nextParameterParamDataBatch(IFR_Int2& parameterindex, void *& parameterdata);

    /**
     * Gets the next parameter when the statement is in the
     * status <code>Status_PutData_C</code>, just right after the
     * <code>execute()</code> call.
     * @param parameterindex The parameter index.
     * @param parameterdata The pointer to the parameter data.
     * @return <code>IFR_NEED_DATA</code> on success, <code>IFR_NOT_OK</code>
     *    on failures.
     */
    IFR_Retcode nextParameterPutData(IFR_Int2& parameterindex, void *& parameterdata);

    /**
     * Gets the next parameter when the statement is in the
     * status <code>Status_PutDataBatch_C</code>, just right after the
     * <code>execute()</code> call.
     * @param parameterindex The parameter index.
     * @param parameterdata The pointer to the parameter data.
     * @return <code>IFR_NEED_DATA</code> on success, <code>IFR_NOT_OK</code>
     *    on failures.
     */
    IFR_Retcode nextParameterPutDataBatch(IFR_Int2& parameterindex, void *& parameterdata);

	/**
	 * Handles an 'execute' when late binding is applied.
	 * This method is called when late binding is in action (a
	 * <code>nextParameter</code> - <code>putData</code> sequence),
	 * and the first packet is sent to the database, which is an
	 * execute command, having the data and possibly parts of
	 * the LONG data that is transmitted.
	 * @return <code>IFR_OK</code> if the execution is ok, otherwise
	 *         error codes of the execution.
	 */
    IFR_Retcode handleExecuteForLateBinding();

    /**
	 * Handles a 'putval' when late binding is applied.
	 * This method is called when late binding is in action (a
	 * <code>nextParameter</code> - <code>putData</code> sequence),
	 * and a follow-up packet is sent to the database, which is a
	 * putval command, having all the LONG data that is transmitted with
	 * this packet.
	 * @param close If true, an extra LONG descriptor with <code>valmode</code>
	 *              set to <code>vm_close</code> is appended to the packet
	 *              (or sent as extra packet if necessary).
	 * @return <code>IFR_OK</code> if the execution is ok, otherwise
	 *         error codes of the execution.
	 */
    IFR_Retcode handlePutvalForLateBinding(IFR_Bool close);

    /**
     * Sends an extra 'putval' command having only the descriptor
     * for closing the operation.
     * @return <code>IFR_OK</code> if the execute was ok, otherwise
     *         the error of the execute.
     */
    IFR_Retcode sendTrailingPutvalClose();


    /**
     * Initializes the packet for a batch execution. A new packet is requested
     * and stored in <code>requestpacket</code>, <code>segment</code> is initialized.
     * The parse id, and the <code>resultCount</code> if it is a mass command, are added.
     * If the input parameter count of the statement is greater 0, a data part is added
     * and initialized (record size, and offsets).
     * @param requestpacket The request packet.
     * @param segment The request segment.
     * @param datapart The data part.
     * @param resultCount The result count that was propagated from the operation before.
     *
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on error.
     */
    IFR_Retcode executeBatchInitPacket(IFRPacket_RequestPacket& requestpacket,
                                       IFRPacket_RequestSegment& segment,
                                       IFRPacket_LongDataPart& datapart,
                                       IFR_Int4  resultCount,
                                       IFR_ErrorHndl& error);

    IFR_Retcode executeBatchInitPacket(IFRPacket_RequestPacket& requestpacket,
                                       IFRPacket_RequestSegment& segment,
                                       IFRPacket_LongDataPart& datapart,
                                       IFR_Int4  resultCount,
                                       IFRPacket_ParseIDPart&     parseid,
                                       IFRPacket_ResultCountPart& resultcount,
                                       IFR_ErrorHndl& error);


    /**
     * Fills the datapart for batch execution.
     *
     * @param requestpacket The request packet to use.
     * @param requestsegment The request segment.
     * @param datapart The data part where the data is inserted.
     * @param inputcursor The position within the row array. This will be updated when data is inserted.
     * @param firstrecord The position in the row array of the first row that is in this packet.
     * @param recordsize The size of one row in the data part in bytes.
     * @param massCommand Whether the command is a mass command (multiple rows can be inserted).
     *
     * @return <code>IFR_OK</code> on success,
     * <code>IFR_NEED_DATA</code> if the data insertion process has
     * found element which are flagged with DATA AT EXECUTE. In that
     * case the descriptor for the
     * <code>nextParameter</code>/<code>putData</code> sequence is
     * already initialized.
     */
    IFR_Retcode executeBatchFillData(IFRPacket_RequestPacket&  requestpacket,
                                     IFRPacket_RequestSegment& requestsegment,
                                     IFRPacket_LongDataPart&   datapart,
                                     IFR_Int4              &   inputcursor,
                                     IFR_Int4              &   firstrecord,
                                     IFR_UInt4                 recordsize,
                                     IFR_Bool                  massCommand,
                                     IFR_Int4                  executecount,
                                     IFR_Int2                  startindex,
                                     IFR_ErrorHndl&            error);

    /**
     * Sends the command to the database for batch execution and checks the result.
     *
     * @param requestpacket The used request packet.
     * @param segment The request segment.
     * @param inputcursor The position within the input row array.
     * @param firstrecord The position of the first row in the input row array that is sent with this packet.
     * @param executecount The row count that is managed by the database for mass commands.
     * @param masscommand Whether the command is a mass command.
     *
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode executeBatchSendCommand(IFRPacket_RequestPacket&  requestpacket,
                                        IFRPacket_RequestSegment& segment,
                                        IFR_Int4&                 inputcursor,
                                        IFR_Int4&                 firstrecord,
                                        IFR_Int4&                 executecount,
                                        IFR_Bool                  masscommand,
                                        IFR_BatchFillOperation   *operation=0);

    /**
     * Clears everything that is related to LONG data and
     * DATA AT EXECUTE, and returns the provided code.
     */
    IFR_Retcode clearParamForReturn(IFR_Retcode rc);

    /**
     * Aborts the putval sequence. (Sends descriptor with valmode @c vm_error).
     * @param pv The putval instance to use.
     */
    IFR_Retcode abortPutval(IFRConversion_Putval *pv);

    /**
     * Returns the current data part positioned at the current position,
     * hereby acquiring the data lock.
     * @param part Data part istance to which the data part is assigned.
     * @return <code>IFR_OK</code> if all went ok, <code>
     */
    IFR_Retcode getCurrentData (IFRPacket_DataPart& part);

    /**
     * Checks whether the parse info is still valid (the connection hasn't been changed).
     * @return @c true if the parse info is invalid, in this case the error is set.
     */
    IFR_Bool assertCurrentParseInfo();

    IFR_Int4 getParameterInfoHash();

    IFR_Int4 getABAPTableParameterIndex(IFR_Int4 abaptabid);
    IFR_Retcode handleABAPStreams(IFRPacket_PartKind::PartKind partkind, IFRPacket_ReplyPacket& replypacket);
    IFR_Retcode sendABAPErrorPacket(IFRPacket_ReplyPacket& replypacket);
    
    IFR_ParseInfo *               m_parseinfo;      //!< the parse information stored in this statement.
    IFRUtil_Vector<IFR_Parameter> m_paramvector;    //!< the parameters that are bound are set here.
    IFR_Int4                      m_param_info_hash;//!< the hashcode of the parameters information
    IFR_size_t                    m_bindingtype;    //!< The row size of one row of host variables, or 0 if column-wise binding is used.
    StatementStatus               m_status;         //!< Status information for function sequence monitoring.
    IFR_DataAtExecuteDescriptor  *m_paramdata;      //!< Information about parameters that are bound at execute.
    IFR_UInt4                     m_lastgetobjcol;  //!< column index from last getObject() call
    IFR_Length                    m_offset;         //!< The offset for piecewise reading of data
    IFR_Bool                      m_parsed_with_paramdesc; //!< Whether an @c appl_parameter_description part was added on parsing.
    CopiedDataPart               *m_copieddata;     //!< The copied data part from the original packet.

    friend class IFR_BatchFillOperation;
};

/**
 * @ingroup IFR_Trace
 * Trace operator.
 * @param s The stream.
 * @param statement The statement to trace.
 * @return <code>s</code>
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, IFR_PreparedStmt& statement);

IFR_END_NAMESPACE

#endif // IFR_PREPAREDSTMT_H
