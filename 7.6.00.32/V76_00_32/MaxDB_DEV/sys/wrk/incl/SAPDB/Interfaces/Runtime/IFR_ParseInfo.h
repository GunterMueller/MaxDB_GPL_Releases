/*!
  @file           IFR_ParseInfo.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Util
  @brief          Information for parsed statements
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
#ifndef IFR_PARSEINFO_H
#define IFR_PARSEINFO_H

#include "Interfaces/Runtime/IFR_Types.h" 
#include "Interfaces/Runtime/IFR_Connection.h"
#include "Interfaces/Runtime/IFR_ParameterMetaData.h"
#include "Interfaces/Runtime/IFR_ResultSetMetaData.h"
#include "Interfaces/Runtime/Packet/IFRPacket_FunctionCode.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"

IFR_BEGIN_NAMESPACE

/**
 * @internal Data stored in the parse info.
 */
class IFR_ParseInfoData
    : public IFRUtil_RuntimeItem 
{ 
public:
    IFR_ParseInfoData(const IFRPacket_FunctionCode& functioncode,
                      const IFR_String& sqlcommand,
                      const IFR_Int4 isolationlevel,
                      IFR_Connection *connection,
                      IFR_StringEncoding commandencoding,
                      IFR_Int4 paraminfohash,
                      IFR_Bool& memory_ok);
    ~IFR_ParseInfoData();
    
    // -- information about the statement
    IFR_ParseID                 m_parseid;           //!< The parse id stored here.
    IFR_ParseID                 m_massparseid;       //!< The parse id of the mass command for this command.
    IFRConversion_ConverterList m_paraminfos;        //!< Parameter and column infos.
    IFR_Int2                    m_inputparamcount;   //!< The number of input parameters.
    IFR_Int2                    m_outputparamcount;  //!< The number of output parameters.
    IFR_Int2                    m_outputcolumncount; //!< The number of output columns.

    IFRPacket_FunctionCode      m_functioncode;      //!< The function code.
    IFR_String                  m_sqlcommand;        //!< The SQL statement that was parsed.
    IFR_Int4                    m_isolationlevel;    //!< The isolation level.
    IFR_UInt4                   m_inputparamrowsize; //!< The size of a single input parameter row.
    IFR_Bool                    m_synthparsid;       //!< True if the single parse id is synthetic (generated).
    IFR_Bool                    m_noexecute;         //!< True if the command cannot be executed (DIAGNOSE CHECK INSTANCE)
    IFR_Connection::AppendMode  m_appendmode;        //!< True if this command likes that garbage collection commands are appended.
    IFR_Bool                    m_haslongs;          //!< True if at least one parameter has long kind.
    IFR_Int2                    m_lastinputlong;     //!< Index of last input LONG parameter (0-based).
    IFR_Int2                    m_lastoutputlong;    //!< Index of last output LONG parameter (0-based).
    IFR_StringEncoding          m_commandencoding;   //!< The actual encoding that was used.
    IFR_Int4                    m_paraminfohash;     //!< Hash code of parameter details.
    // -- management information 
    SQLDBC_IRuntime::MutexHandle   m_lock;           //!< The lock for data etc.
    IFR_Int4                       m_refcount;       //!< The reference count.
    IFR_Connection                *m_connection;     //!< The connection used.
    IFR_Bool                       m_cached;         //!< Is it cached?
    IFR_UInt4                      m_reparsestamp;   //!< Number of times this item was reparsed, resulting in
                                                     //!< an incompatible change of the parse info.
    /**
     * Locks the parse info, for changes in the 'cached' status.
     */
    inline void lock() { runtime.lockMutex(m_lock); }
    
    /**
     * Unlocks the parse info, for changes in the 'cached' status.
     */
    inline void unlock() { runtime.releaseMutex(m_lock); }
    /**
     * Increments the reference count.
     */
    inline void incRef() 
    {
        lock(); 
        ++m_refcount; 
        unlock(); 
    }
    
    /**
     * Decremenents the reference count.
     * @return The new value of the reference count.
     */
    inline IFR_Int4 decRef() 
    { 
        lock(); 
        IFR_Int4 result=--m_refcount; 
        unlock(); 
        return result; 
    }

    /**
     * Shreds this parse info, possibly dropping parse ids.
     */
    void runDecomission(IFR_Bool& memory_ok);
    
    /**
     * Internal method that destroy the parameter info.
     */
    void destroyParamInfos(IFR_Bool preserveparameters = false);
    
    /**
     * Sets the 'cached' flag.
     */
    inline void setCached(IFR_Bool cached) { m_cached=cached; }

    /**
     * Checks whether it is cached.
     */
    inline IFR_Bool isCached() const { return m_cached; }

    /**
     * Drops and deletes a parse id. The parse id is only deleted
     * if there is a connection and the connection id matches (implying
     * the parse id is valid).
     * @param parseid The parse id.
     */
    void dropParseID(IFR_ParseID& parseid, IFR_Bool& memory_ok);
};


/**
 * @ingroup IFR_Statement
 * Information on a parsed statement. A parse info contains the following items:
 * - The SQL command that was parsed.
 * - The function code of the statement.
 * - The isolation level.
 * - The parse id, and possibly the mass parse id.
 * - The short infos of the input and output parameters.
 * - The column names for the statement, if any.
 */
class IFR_ParseInfo 
    : public IFR_ParameterMetaData, 
      public IFR_ResultSetMetaData 
{
public:
    enum ColumnCount {
        UnknownColumnCount_C = -1
    };
    
    /**
     * Constructor.
     * @param sqlcommand The sql command that is represented by this
     *        parse info.
     * @param isolationlevel The isolation level that was current when this statement was parsed.
     * @param functionCode The function code of the statement.
     * @param connection A reference to the connection object.
     * @param commandencoding The command encoding.
     * @param parsedwithparamdesc Whether the command was parsed with parameter descriptions.
     */
    IFR_ParseInfo (const IFR_String& sqlcommand,
                   IFR_Int4          isolationlevel,
                   IFRPacket_FunctionCode functionCode,
                   IFR_Connection         *connection,
                   IFR_StringEncoding      commandencoding,
                   IFR_Int4 paraminfohash,
                   IFR_Bool& memory_ok);
    
    
    /**
     * Constructs a parse info from previously gathered data.
     * @internal
     * The reference count on <code>data</code> must be incremented,
     * and the <code>reparsestamp</code>
     * needs to be gathered before (cache lookup and 
     */
    inline IFR_ParseInfo(IFR_ParseInfoData* data, IFR_UInt4 reparsestamp)
    :m_data(data),
     m_reparsestamp(reparsestamp)
    {}


    /**
     * Destructor.
     */
    virtual ~IFR_ParseInfo();

    /**
     * Sets the parse id.
     * @param ParseID The parse id.
     * @param memory_ok Flag to indicate out of memory.
     */
    IFR_Retcode setParseID(const IFR_ParseID& ParseID, 
                           IFR_Bool& memory_ok);

    /**
     * Gets the parse id that is suitable for 
     * executing a single command.
     */
    inline IFR_ParseID *getParseID(IFR_Bool force=false) const
    {
        if(force) {
            return &(m_data->m_parseid); 
        } else {
            return m_data->m_parseid.isValid() ? &(m_data->m_parseid) : 0;
        }
    }
    
    /**
     * Gets the parse id that is needed for executing
     * a mass command.
     */
    inline IFR_ParseID *getMassParseID() const
    {
        return m_data->m_massparseid.isValid() ? &(m_data->m_massparseid) : 0;
    }
    
    /**
     * Sets the short field infos, and optional column names.
     * @param shortinfo The short field infos of this command.
     * @param paramcount The parameter count (number of elements in <code>shortinfo</code>)
     * @param columnnames The column names (may be 0).
     * @return @c IFR_OK on success, @c IFR_NOT_OK if some memory allocation 
     *            fails.
     */
    IFR_Retcode setShortInfosAndColumnNames(IFRConversion_ConverterList& convList,
                                            IFRUtil_Vector<IFR_String>* columnnames);

    /**
     * Sets the short field infos, and optional column names. This is called for 
     * SELECT commands that had no parameter, and are later described.
     * @param shortinfo The short field infos of this command.
     * @param paramcount The parameter count (number of elements in <code>shortinfo</code>)
     * @param columnnames The column names (may be 0).
     * @return @c IFR_OK on success, @c IFR_NOT_OK if some memory allocation 
     *            fails.
     */
    IFR_Retcode addShortInfosAndColumnNames(IFRConversion_ConverterList& convList,
                                            IFRUtil_Vector<IFR_String>* columnnames);
        
    /**
     * Executes a describe operation on the result table.
     * @param command encoding.
     */
    IFR_Retcode describeParseID (const IFR_StringEncoding m_CommandEncoding,
                                 IFR_ConnectionItem& clink) ;

    /**
     * Gets the short infos.  
     * @return pointer to an array of <code>paramcount</code> pointers
     * to converter instances.
     */
    IFRConversion_ConverterList& getParameterInfos () const;

    /**
     * Gets the number of parameters.
     */
    virtual IFR_Int2 getParameterCount() const;

    /**
     * Retrieves whether the command that has been parsed is a query,
     * i.e. a command that produces a result set.
     */
    IFR_Bool isQuery() const;
    
    /**
     * Retrieves whether the command that has been parsed is a mass command,
     * i.e. a command that can take more than one argument in the data part
     * on execution.
     * @return <code>true</code> if it is a mass command, 
     *   false if not.
     */
    IFR_Bool isMassCommand() const;
    
    /**
     * Get the SQL command string.
     */
    const IFR_String& getSQLCommand() const;

    /**
     * Sets the function code.
     * @param functionCode The function code.
     */
    void setFunctionCode(const IFRPacket_FunctionCode& functionCode);
    
    /**
     * The size in bytes needed for one row of parameters
     */
    IFR_UInt4 getInputParameterRowSize();

    /**
     * The number of input parameters.
     */
    IFR_Int2 getInputParameterCount() const;

    /**
     * The number of output parameters.
     */
    IFR_Int2 getOutputParameterCount() const;
    
    /**
     * Locks the parse info, for use or data replacement.
     */
    void lock();
    
    /**
     * Unlocks the parse info.
     */
    void unlock();
    
    
    /**
     * Gets whether this parse info is cached.
     * @return <code>true</code> if this parse info is in the cache, a 
     *   cached parse info will never become 'not cached' again.
     */
    IFR_Bool isCached() const;

    /**
     * Gets the function code.
     */
    IFRPacket_FunctionCode getFunctionCode() const;

    
    /**
     * Sets the 'no execution' flag. Execution requests for
     * this command are ignored.
     */
    void setNoExecute(IFR_Bool noExecute);
    
    /**
     * Get whether the command is not for execution.
     */
    IFR_Bool isNoExecute() const;
        

    /**
     * Invalidates the parse ids stored in the parse info.
     */
    void invalidate();

    /**
     * Gets whether it is suggested/allowed that garbage collecting
     * commands are appended onto this command when being executed.
     * Commands that have LONG parameters, and commands that have
     * output parameters are unused here.
     */
    IFR_Connection::AppendMode getAppendMode() const;

    /**
     * @brief Gets whether there are parameters of long type.
     * @return <code>true</code> if there is at least one parameter of
     *         long type, <code>false</code> otherwise.
     */
    IFR_Bool hasLongs ();
    
    /**
     * Gets the index of the last LONG input value.
     * @return The index of the last LONG input parameter, or -1 if none 
     *         exists.
     */
     IFR_Int2 getLastLongInput() const;

    /**
     * Gets the index of the last LONG output value.
     * @return The index of the last LONG output parameter, or -1 if none 
     *         exists.
     */
     IFR_Int2 getLastLongOutput() const;

    /**
     * Gets the number of columns.
     */
     virtual IFR_Int2 getColumnCount();
    
    /**
     * Internal method to avoid virtual function call where possible.
     */
    inline IFR_Int2 getDirectColumnCount() 
    {
        return m_data->m_outputcolumncount;
    }

    /**
     * Internal method to avoid virtual function call where possible.
     */
    inline IFR_Int2 getDirectParameterCount() 
    {
        if(m_data->m_outputcolumncount > 0) {
            return (IFR_Int2)(m_data->m_paraminfos.size() - m_data->m_outputcolumncount);
        } else {
            return (IFR_Int2) m_data->m_paraminfos.size();
        }
    }

    
    /**
     * Get the column infos if and only if they are stored
     * in the parse info (command was parsed with info, and 
     * delivered the needed information.
     * @return A pointer to the first column descriptor,
     *         or 0, if there are none.
     */
    IFRConversion_Converter **getColumnInfos();


    /**
     * Checks whether the underlying field have been changed 
     * in a possibly incompatible way.
     * @param <code>true</code> if the field have changed,
     *        <code>false</code> if not.
     */
    IFR_Bool hasChangedFields();

#ifdef BUILD_INTERFACE_RUNTIME
    /**
     * Get the command encoding.
     */
    inline IFR_StringEncoding getCommandEncoding() 
    {
        return m_data->m_commandencoding;
    }
#endif

    /**
     * Write the SQL Trace.
     */
    void sqlTrace(IFR_TraceStream& s);

    /**
     * Checks whether the parse info is parsed with parameter descriptions.
     */
    IFR_Bool isParsedWithParamDesc() const;

    /**
     * Check whether variable input parameters are enabled.
     * @return @c true if variable input parameters are enabled.
     */
    inline IFR_Bool hasVariableInput() 
    {
        return m_data->m_paraminfos.hasVariableInput();
    }
        
protected:
    virtual IFRConversion_Converter * findParamInfo (IFR_Int2 param); 
    virtual IFRConversion_Converter * findColInfo   (IFR_Int2 columnIndex);
    
private:
    /**
     * Copy constructor 
     * Declared <tt>private</tt> to prevent its usage.
     */
    IFR_ParseInfo(const IFR_ParseInfo&);
    /**
     * Assignment operator.
     * Declared <tt>private</tt> to prevent its usage.
     */
    IFR_ParseInfo& operator =(const IFR_ParseInfo&);
    
    /**
     * Deletes the parameter informations.
     */
    void destroyParamInfos(IFR_Bool preserveparameters = false);
    
    IFR_ParseInfoData          *m_data;         //!< The parse info data is stored here.
    IFR_UInt4                   m_reparsestamp; //!< Stamp on the reparse counter.
    
    /**
     * Checks a new shortinfo set against an old one. The reparse
     * count of the parse info data is incremented in case the parse
     * infos do not match.
     * @param convList The list of converters.
     */
    void checkParamInfos(IFRConversion_ConverterList& convList);

    friend class IFR_ParseInfoCache; //!< Needs access the <code>m_data</code> member as only this is cached.

};

/**
 * Traces the parse id as a hexadecimal string similar to the kernel VTRACE format.
 * @param s The trace stream.
 * @param p The parse id to trace.
 * @return The trace stream <var>s</var>.
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, const class IFR_ParseID &p );

/**
 * Traces the parseinfo to the trace stream. The trace includes informations 
 * about the caching of parse ids.
 * @param s The trace stream.
 * @param parseinfo The parse information to trace.
 * @return The trace stream <var>s</var>.
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, const class IFR_ParseInfo &parseinfo );

IFR_END_NAMESPACE

#endif //IFR_PARSEINFO_H
