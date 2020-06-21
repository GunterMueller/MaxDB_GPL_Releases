/*!
  @file           SQL_Connection.cpp
  @author         ThomasA
  @brief          Provides a 'connection' for SQLDBC calls.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#include "geo00.h"
#include "ggg01.h"
#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_GlobalAllocatorWrapper.hpp"
#include "CppSQL/SQL_SQLDBCConnection.hpp"
#include "CppSQL/SQL_SQLDBCRuntime.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "liveCache/LVC_KernelInterface.hpp"

class SQL_ErrorHndlImplementation
{
public: 
  /**
   * @brief The constructor for an ErrorHndl object.
   */
  SQL_ErrorHndlImplementation() 
    :m_errorcode(0)
    ,m_message(0)
    ,m_error(0)
    ,m_connection(0)
  {
  };
  
  /**
   * @brief Constructs a new error message.
   * This constructor will mostly used in combination with a static errormessage
   * to advoid memory allocation during construction.
   */
  SQL_ErrorHndlImplementation(char *message, SQLDBC_Int4 errorcode) 
    :m_errorcode(errorcode)
    ,m_message(message)
    ,m_error(0)
    ,m_connection(0)
  {
  };

  SQL_SQLDBCErrorHndl &error() { return m_error; };

  /**
   * @brief set the SQLDBC connection to map the errors occured there
   */
  void setConnection(SQLDBC::SQLDBC_Connection *connection)
  { 
    m_connection = connection; 
    m_error = this;
  };

  /**
   * @brief Returns the error message stored in the ErrorHndl object.
   * @note This is an UTF8 string.
   */
  const char *getErrorText() const
  { 
    return ((0 == m_connection) ? m_message : m_connection->error().getErrorText());
  };

  /**
   * @brief Returns the error code from the ErrorHndl object.
   */
  const SQLDBC_Int4 getErrorCode() const
  {
    return ((0 == m_connection) ? m_errorcode : m_connection->error().getErrorCode());
  };

  /**
   * @brief Returns the SQL state from the ErrorHndl object.
   */
  const char *getSQLState() const
  {
    return ((0 == m_connection) ? "" : m_connection->error().getSQLState());
  };

private:
  friend class SQL_SQLDBCErrorHndl;
  /**
   * The error code.
   */
  SQLDBC_Int4  m_errorcode;
  /**
   * The error message. 
   */
  char *m_message;

  SQLDBC::SQLDBC_Connection *m_connection;
  SQL_SQLDBCErrorHndl m_error;
};

/* static error messages if memory allocation or initialisation failed */ 
static SQL_ErrorHndlImplementation ErrOperatorNEWfailed((char*)"Operator NEW failed", e_new_failed);
static SQL_ErrorHndlImplementation ErrSessionNotConnected((char*)"SQLDBC connection not established", e_no_sqldbc_connection);

static SQL_SQLDBCErrorHndl SQL_ErrOperatorNEWfailed(&ErrOperatorNEWfailed);
static SQL_SQLDBCErrorHndl SQL_ErrSessionNotConnected(&ErrSessionNotConnected);

/*!
   @brief implementation class
 */
class SQL_ConnectionImplementation
{
public :
    /*!
       @brief constructor, create the runtime object used by SQLDBC
     */
    SQL_ConnectionImplementation(IliveCacheSink& kernelSink, SAPDBMem_IRawAllocator& allocator, int packetSize)
        : m_allocator(allocator)
        , m_runTime  (kernelSink, allocator, packetSize)
        , m_environment(&m_runTime)
        , m_connection(0)
    {
    }
    /*!
       @brief destructor, releases connection
     */
    ~SQL_ConnectionImplementation();
    /*!
       @brief establishes the connection
     */
    SQLDBC_Connection* Connect();
    /*!
       @brief returns the allocator used
     */
    SAPDBMem_IRawAllocator& GetAllocator()
    {
        return m_allocator;
    }
    /*!
       @brief returns the error handle for this session
     */
    SQL_SQLDBCErrorHndl& error()
    {
      if (!m_connection) {
        return SQL_ErrSessionNotConnected;
      }
      return m_errorimp.error();
    }

    SQLDBC_Profile * getProfile() { return m_environment.getProfile(); }
    
private :
    SAPDBMem_IRawAllocator& m_allocator;         //!< reference to the allocator given to an used by SQLDBC
    SQL_SQLDBCRuntime       m_runTime;           //!< instance implementing the run time interface used by SQLDBC
    SQLDBC_Environment      m_environment;       //!< SQLDBC environment
    SQLDBC_Connection*      m_connection;        //!< connection pointer
    SQL_ErrorHndlImplementation  m_errorimp;
};

//-----------------------------------------------------------------------------

SQLDBC_Connection* SQL_ConnectionImplementation::Connect()
{
  m_connection = m_environment.createConnection(m_allocator); 
  m_errorimp.setConnection(m_connection);
  if (m_connection) {
    SQLDBC_ConnectProperties properties;
    //properties.setProperty ("APPLICATION", "KNL");
    properties.setProperty ("STATEMENTCACHESIZE", "0");
    // PTS 1135750
    // we must prepare the connection so that it reacts properly
    // to the 7-bit ASCII restriction of Unicode LiveCache
    if(m_runTime.isUnicodeInstance()) {
        properties.setProperty("ASCII_IS_7BIT", "1");
    } else {
        properties.setProperty("ASCII_IS_7BIT", "0");
    }
    properties.setProperty ("CURSORPREFIX", "SQLOMS");
    m_connection->connect(properties);
  }
  return m_connection;
}

//-----------------------------------------------------------------------------

SQL_ConnectionImplementation::~SQL_ConnectionImplementation()
{
  if (m_connection) {
    m_connection->disconnect();                      
    m_environment.releaseConnection(m_connection);
  }
}

//-----------------------------------------------------------------------------

SQL_SQLDBCConnection::SQL_SQLDBCConnection(int packetSize)
: m_connection(0)
, m_implementation(0)
{
    IliveCacheSink* pKernelSink = OMS_Globals::GetKernelInterface()->GetSinkPtr();
    SAPDBMem_IRawAllocator* pAllocator; 
    pAllocator = &OMS_Globals::m_globalsInstance->m_SQLDBCAllocatorWrapper;
    //pKernelSink->GetSessionAllocator(pAllocator);
    m_implementation = new(*pAllocator) SQL_ConnectionImplementation(*pKernelSink, *pAllocator, packetSize);
    if (!m_implementation) {
      return;
    }
    m_connection = m_implementation->Connect();
}

//-----------------------------------------------------------------------------

SQL_SQLDBCConnection::~SQL_SQLDBCConnection()
{
  if (m_implementation)
    destroy (m_implementation, m_implementation->GetAllocator());
}

//-----------------------------------------------------------------------------

SQL_SQLDBCErrorHndl &SQL_SQLDBCConnection::error()
{
  if (!m_implementation)
    return SQL_ErrOperatorNEWfailed;
  return m_implementation->error();
}

//----------------------------------------------------------------------
SQLDBC_Profile* 
SQL_SQLDBCConnection::getProfile()
{
    return m_implementation->getProfile();
}

const SQLDBC_Int4 SQL_SQLDBCErrorHndl::getErrorCode() const 
{
  return m_error->getErrorCode();
}

const char *SQL_SQLDBCErrorHndl::getErrorText() const 
{
  return m_error->getErrorText();
}

const char* SQL_SQLDBCErrorHndl::getSQLState() const
{
  return m_error->getSQLState();
}

SQL_SQLDBCErrorHndl::operator SQLDBC_Bool () const
{
  return m_error->getErrorCode() != 0;
}

SQLDBC::SQLDBC_Statement *SQL_SQLDBCConnection::createStatement()
{
  if (m_connection)
    return m_connection->createStatement();
  return 0; 
};

SQLDBC::SQLDBC_PreparedStatement *SQL_SQLDBCConnection::createPreparedStatement()
{
  if (m_connection)
    return m_connection->createPreparedStatement(); 
  return 0; 
};

void SQL_SQLDBCConnection::releaseStatement(SQLDBC::SQLDBC_Statement *stmt)
{
  if (m_connection)
    m_connection->releaseStatement(stmt); 
};
  
void SQL_SQLDBCConnection::releaseStatement(SQLDBC::SQLDBC_PreparedStatement *stmt)
{
  if (m_connection)
    m_connection->releaseStatement(stmt); 
};

SQLDBC_Int4 SQL_SQLDBCConnection::getKernelVersion()
{
  if (m_connection)
    return m_connection->getKernelVersion(); 
  return 0;
};

SQLDBC_Bool SQL_SQLDBCConnection::isUnicodeDatabase()
{
  if (m_connection)
    return m_connection->isUnicodeDatabase(); 
  return SQLDBC_FALSE;
};

