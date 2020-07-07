/*!
  @file           IFR_ErrorHndl.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          Storage for error messages and error codes
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
#ifndef IFR_ERRORHNDL_H
#define IFR_ERRORHNDL_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "Interfaces/Runtime/IFR_ErrorCode.h"
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_String.h"

IFR_BEGIN_NAMESPACE

class IFR_Connection;

/**
 * @ingroup IFR_Common
 * Storage for an error message and error code.
 */
class IFR_ErrorHndl 
{
public:
    /**
     * Constructor. 
     * No error is set.
     */
    explicit inline IFR_ErrorHndl()
    :m_errorcode(0),
     m_message(0),
     m_byteslength(0),
     m_allocator(0),
     m_connection(0),
     m_errorcount(0),
     m_RowNotFoundCount(0)
    {
        m_sqlstate[0]='\0';
    }


    /**
     * Constructor. 
     * No error is set.
     */
    inline IFR_ErrorHndl(SAPDBMem_IRawAllocator& allocator)
    :m_errorcode(0),
     m_message(0),
     m_byteslength(0),
     m_allocator(&allocator),
     m_connection(0),
     m_errorcount(0),
     m_RowNotFoundCount(0)
    {
        m_sqlstate[0]='\0';
    }
    
    
    /**
     * Constructor. 
     * No error is set.
     */
    IFR_ErrorHndl(SAPDBMem_IRawAllocator& allocator, IFR_Connection *connection)
    :m_errorcode(0),
     m_message(0),
     m_byteslength(0),
     m_allocator(&allocator),
     m_connection(connection),
     m_errorcount(0),
     m_RowNotFoundCount(0)
    {
        m_sqlstate[0]='\0';
    }
    
    
    /**
     * Destructor. If there is an allocator set, the 
     * message is freed using that allocator, except
     * if it is the static 'memory allocation failed' 
     * message.
     */
    ~IFR_ErrorHndl ();
    
    /**
     * Assignment function. 
     * @param error The error assigned to this one.
     * @param counterAdd If true, the error count has to be incremented.
     */
    void assign(const IFR_ErrorHndl& error, IFR_Bool counterAdd);
    
    /**
     * Sets an error from a kernel message.
     * @param errorcode the error code.
     * @param sqlstate  the sql state, or 0 if not applicable
     * @param message   the error message itself
     * @param encoding  the encoding of the error message, default is <code>IFR_StringEncodingAscii</code>
     */
    void setSQLError(const IFR_Int4 errorcode, 
                     const char* sqlstate, 
                     const char *message, 
                     IFR_StringEncoding encoding=IFR_StringEncodingAscii,
                     IFR_Bool notrace=false);

    /**
     * Sets an error from a kernel message.
     * @param errorcode the error code.
     * @param sqlstate  the sql state, or 0 if not applicable
     * @param message   the error message itself
     */
    void setSQLError(const IFR_Int4 errorcode, const char* sqlstate, const IFR_String& message, IFR_Bool notrace = false);

    /**
     * Sets an error from a runtime error object.
     * @param error The error object.
     */
    void setFromRuntimeError(SQLDBC_IRuntime::Error& error);
    
    inline void toRuntimeError(SQLDBC_IRuntime::Error& error) 
    {
        error.errorcode = getErrorCode();
        strncpy(error.errortext, getErrorText(), sizeof(error.errortext));
        error.errortext[sizeof(error.errortext) -1] = false;
    }

    /**
     * Gets the error code from the error.
     */
    inline const IFR_Int4 getErrorCode() const 
    {
        return m_errorcode;
    }

    /**
     * Gets the SQL state from the error.
     */
    inline const char* getSQLState() const
    {
        return m_sqlstate;
    }

    /**
     * Get the error message. Note that this is an UTF8 string.
     */
    const char *getErrorText() const
    {
        return m_message==0 ? "" : m_message;
    }

    /**
     * Sets a runtime error.
     * @param error the internal error code
     * @param ... optional arguments which are stored in the message
     */
    void setRuntimeError(IFR_ErrorCode error, ...);

    
    /**
     * Sets the runtime error, that a memory allocation has failed.
     * This is handled special, as the 'memory allocation failed' error
     * is static, so that in this case under no circumstances an
     * allocation does happen.
     */
    void setMemoryAllocationFailed();


    /**
     * Clears this error. Resets error code and sql state to 0,
     * and removes the message.
     */
    void clear();

    /**
     * Conversion operator. Returns true if the error is set.
     * This operator makes it possible to write constructs like
     * <code>if (error) { ...</code>.
     */
    inline operator IFR_Bool () const 
    {
        return m_errorcode!=0;
    }

    /**
     * Query whether the error is a <i>parse again</i> error, as sometimes
     * reported by the database.
     * @return <code>true</code> if the error is a <i>parse again</i> error and
     * can possibly be resolved by reparsing the statement.
     */
    inline IFR_Bool isParseAgain() const
    {
        return m_errorcode==-8 ||
            m_errorcode==-9;
    }
    
    /**
     * Query whether the error is session-releasing.
     * The following errors qualify for this criteria:
     * - -904 - Space for result tables exhausted
     * - 700, -70 - Session inactivity timeout (work rolled back)
     * - 710, -71 - Session terminated by shutdown (work rolled back)
     * - 750, -75 - Too many SQL statements (work rolled back)
     * @return <code>true</code> if that error kills the SQL session, <code>false</code>
     *   if not.
     */
    inline IFR_Bool isConnectionReleasing() const
    {
        return m_errorcode == -904 ||
            m_errorcode == 700 ||
            m_errorcode == -70 || 
            m_errorcode == 710 ||
            m_errorcode == -71 ||
            m_errorcode == 750 ||
            m_errorcode == -75 ||
            m_errorcode == -10760;
    }
    
    /**
     * Query whether the error indicates a memory allocation
     * failure.
     */
    inline IFR_Bool isMemoryAllocationFailed() const
    {
        return m_errorcode==-10760;
    }
    
    /**
     * Sets the allocator.
     * @param allocator The allocator to be used for allocating and releasing
     *        the memory used for the error message.
     */
    inline void setAllocator(SAPDBMem_IRawAllocator& allocator)
    {
        m_allocator = &allocator;
    }
    
    void sqlTrace(IFR_TraceStream& s) const;

    /**
     * Sets the connectio.
     * @param connection The connection to be used for tracing error messages.
     */
    inline void setConnection(IFR_Connection *connection)
    {
        m_connection = connection;
    }

  /**
   * @brief Gets the connection object.
   * @return The reference to the connection on which the error occured.
   */  
  inline IFR_Connection* getConnection() const
  {
    return m_connection;
  };

  /**
   * @brief Gets the error count and resets the value.
   * @return error count for profiling.
   */  
    inline IFR_UInt4 getErrorCount(){
      IFR_UInt4 erg = m_errorcount;
      m_errorcount = 0;
      return erg;
    }

    /**
   * @brief Gets the RowNotFound count and resets the value.
   * @return RowNotFound count for profiling.
   */  
    inline IFR_UInt4 getRowNotFoundCount(){
      IFR_UInt4 erg = m_RowNotFoundCount;
      m_RowNotFoundCount = 0;
      return erg;
    }

private:
    IFR_ErrorHndl(const IFR_ErrorHndl& copy);
    IFR_ErrorHndl& operator=(const IFR_ErrorHndl& assign);
    
    /**
     * The error code.
     */
    IFR_Int4  m_errorcode;

    /**
     * The SQL state, or "" if not set/applicable.
     */
    char m_sqlstate[6];
    
    /**
     * The error message. 
     */
    char *m_message;

    /**
     * The length of the error message in bytes (without terminator char).
     */
    IFR_Int4 m_byteslength;
    
    /**
     * The allocator used for allocating the message.
     * If 0, the message is not freed on destruction.
     */
    SAPDBMem_IRawAllocator* m_allocator;

    /**
     * A reference to the connection item creates this error handle object.
     */
    IFR_Connection *m_connection;

    /**
     * The 'memory allocation failed' message.
     */
    static char *m_memory_allocation_failed;

    /**
     * The error count for profiling.
     */
    IFR_UInt4 m_errorcount;

        /**
     * The RowNotFound count for profiling.
     */
    IFR_UInt4 m_RowNotFoundCount;

};

class IFR_TraceStream;

/**
 * @ingroup IFR_Trace
 * Prints an error.
 * @param s The output stream.
 * @param e The error.
 * @return The stream <code>s</code>.
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_ErrorHndl& e);

SQLDBC_IRuntime::TaskTraceContext *IFR_GetTraceContext(IFR_ErrorHndl* e);
IFR_TraceStream *IFR_GetTraceStream(IFR_ErrorHndl *e);

IFR_END_NAMESPACE

#endif //IFR_ERRORHNDL_H
