/*!
  @file           IFR_Environment.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          Global environment
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
#ifndef IFR_ENVIRONMENT_H
#define IFR_ENVIRONMENT_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"
#include "Interfaces/Runtime/IFR_Profile.h"
#include "Interfaces/Runtime/Util/IFRUtil_LinkedList.h"

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_StorageItems.h"

IFR_BEGIN_NAMESPACE

class IFR_Connection;
class IFR_ConnectProperties;
/**
 * Global swap kind of the operating system.
 * Valid swap kinds are
 * <ul>
 *   <li><code>SwapUnknown</code> - swap not yet determined (environment not initialised)</li>
 *   <li><code>SwapNormal</code>  - H2H1L2L1 (big endian)</li>
 *   <li><code>SwapFull</code>    - L1L2H1H2 (little endian)</li>
 *   <li><code>SwapHalf</code>    - L2L1H2H1</li>
 * </ul>
 */
extern IFR_ClientSwap IFR_EnvironmentSwapKind;

//------------------------------------------------------------

class IFR_EnvironmentProfile {
public:
    void resetCounters();
    void collectCounters(); 
    void submitCounters(IFR_UInt8 *counter_8);

    void setConnectionList (IFRUtil_LinkedList<SQLDBC::SQLDBC_ConnectionItemStorageForConnection>* list){
      m_ConnectionList = list;
    }
    
    enum ProfileCounter_8 {
      IFR_SENDSIZE_8_ENV          = 0,
      IFR_RECEIVESIZE_8_ENV       = 1,
      IFR_DATAREADSIZE_8_ENV      = 2,
      IFR_DATAWRITESIZE_8_ENV     = 3,
      IFR_LONGDATAREADSIZE_8_ENV  = 4,
      IFR_LONGDATAWRITESIZE_8_ENV = 5
    };

    enum ProfileCounter {
      IFR_CONNECTIONCOUNT_ENV = 0 ,
      IFR_COMMITCOUNT_ENV = 1 ,
      IFR_ROLLBACKCOUNT_ENV = 2 ,
      IFR_SETISOLATIONCOUNT_ENV = 3 ,
      IFR_SENDCOUNT_ENV = 4 ,
      IFR_CANCELCOUNT_ENV = 5 ,
      IFR_STATEMENTCOUNT_ENV = 6 ,
      IFR_PREPAREDSTATEMENTCOUNT_ENV = 7 ,
      IFR_NOTSELECTFETCHOPTIMIZED_ENV = 8 ,
      IFR_DROPPARSEIDCOUNT_ENV = 9 ,
      IFR_DROPCURSORCOUNT_ENV = 10,
      IFR_DROPLONGDESCCOUNT_ENV = 11,
      IFR_PREPARECOUNT_ENV = 12,
      IFR_PARSEINFOHITCOUNT_ENV = 13,
      IFR_PARSINFOMISSCOUNT_ENV = 14,
      IFR_PARSEAGAINCOUNT_ENV = 15,
      IFR_PARSEINFOSQUEEZECOUNT_ENV = 16,
      IFR_EXECUTECOUNT_ENV = 17,
      IFR_INSERTCOUNT_ENV = 18,
      IFR_UPDATECOUNT_ENV = 19,
      IFR_SELECTCOUNT_ENV = 20,
      IFR_CALLDBPROCEDURECOUNT_ENV = 21,
      IFR_DELETECOUNT_ENV = 22,
      IFR_FETCHCOUNT_ENV = 23,
      IFR_SQLERRORCOUNT_ENV = 24,
      IFR_ROWNOTFOUNDCOUNT_ENV = 25,
      IFR_FETCHROWCOUNT_ENV = 26
     };

protected:
    IFR_EnvironmentProfile();

    IFR_UInt8 m_counter_8[6];
    IFR_UInt4 m_counter_4[27];

    IFRUtil_LinkedList<SQLDBC::SQLDBC_ConnectionItemStorageForConnection>* m_ConnectionList;

};

/**
 * The environment is the frame around getting connections and
 * applying general settings in the interface runtime library.
 *
 * An environment object uses a runtime environment implementation
 * to fulfill its requests. 
 */
class IFR_Environment 
    : public IFRUtil_RuntimeItem, 
      public IFR_ITraceController,
      public IFR_EnvironmentProfile,
      public IFRUtil_LinkedListItem<IFR_Environment>
{
public:
    /**
     * Constructor. 
     * @param runtime The runtime environment on which this 
     *                environment is based. This must not be 0.
     */
    IFR_Environment(SQLDBC_IRuntime* runtime);
    
    /**
     * Destructor.
     */
    ~IFR_Environment();
    
    /**
     * Get the major version.
     */
    IFR_Int4 getMajorVersion() const;

    /**
     * Get the minor version.
     */
    IFR_Int4 getMinorVersion() const;

    /**
     * Gets the runtime version in the format:
     *   - 1 digit  major number,
     *   - 2 digits minor number,
     *   - 2 digits correction level.
     * @return The runtime version.
     */
    static IFR_Int4 getRuntimeVersion();

    /**
     * Gets a new connection.
     * @return A newly created connection.
     */
    IFR_Connection* getConnection();

    /**
     * Gets a new connection.
     * @param customAllocator The allocator to be used for the connection.
     * @return A newly created connection.
     */
    IFR_Connection* getConnection(SAPDBMem_IRawAllocator& allocator);
    
    /**
     * Releases a connection.
     * @param c The connection to be released. 
     */
    void releaseConnection(IFR_Connection *c);

    
    inline IFR_TraceStream* getTraceStream() 
    {
        return m_tracestream;
    }

    void setTraceOptions(const IFR_ConnectProperties& traceoptions);
    void getTraceOptions(IFR_ConnectProperties& traceoptions);
    
    inline IFR_Profile* getProfile()
    {
        IFR_STATIC_ASSERT(sizeof(IFR_Profile) == sizeof(SQLDBC_IRuntime::Profile));
        IFR_STATIC_ASSERT(sizeof(IFRUtil_LinkedList<IFR_Environment>) == sizeof(SQLDBC_IRuntime::EnvironmentList));
        return reinterpret_cast<IFR_Profile *>(&(runtime.m_profile));

    }    

public:
    // -- trace controller methods
    virtual void write(const char *data, IFR_Int4 length = -1);
    virtual void writeln(const char *data, IFR_Int4 length = -1);
    virtual SQLDBC_IRuntime::TaskTraceContext *traceflags();
    virtual SAPDBMem_IRawAllocator& getAllocator();
    virtual void traceError(IFR_Int4 errorcode, IFR_Connection *connection);

private:
    IFR_Environment(const IFR_Environment&);
    IFR_Environment& operator =(const IFR_Environment&);    

    IFR_TraceStream   *m_tracestream;
    IFR_CallStackInfo  m_callstackroot;
};

IFR_END_NAMESPACE

#endif //IFR_ENVIRONMENT_H
