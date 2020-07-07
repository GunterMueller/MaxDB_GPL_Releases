/*!
  @file           SQLDBC.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        SQLDBC
  @brief          SAP Database Connectivity
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

// Change namespace in case of kernel.
#ifdef SQLDBC_FOR_KERNEL
#  undef SQLDBC
#  define SQLDBC SQLDBC_Kernel
#define SQLDBC_RUNTIME_IN_NAMESPACE
#define IFR_NAMESPACE SQLDBC_Kernel
#endif



#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"

#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_LinkedList.h"
#include "SAPDB/Interfaces/Runtime/IFR_Environment.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/Runtime/IFR_Statement.h"
#include "SAPDB/Interfaces/Runtime/IFR_PreparedStmt.h"
#include "SAPDB/Interfaces/Runtime/IFR_ResultSet.h"
#include "SAPDB/Interfaces/Runtime/IFR_RowSet.h"
#include "SAPDB/Interfaces/Runtime/IFR_UpdatableRowSet.h"
#include "SAPDB/RunTime/System/RTESys_SharedLib.h"

#include "SAPDB/RunTime/RTE_IInterface.hpp"

#include "gsp100.h"

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_StorageItems.h"

#include <malloc.h>

// 'this' is used in initializer list. giving the this pointer
// to the ctor of base class necessary for chaining of connections,
// statements etc.
#if defined(WIN32)
#pragma warning( disable: 4355 )
#endif

/********************************/

namespace SQLDBC {

#define SQLDBC_CHECK(expr, retval) \
  if (!expr) \
    return retval;

static IFR_ErrorHndl     global_oom_error;
static SQLDBC_ErrorHndl  global_sqldbc_oom_error;

//----------------------------------------------------------------------
#define SQLDBC_METHOD_BRACE do {} while(0)
// #define SQLDBC_METHOD_BRACE  SQLDBC_MethodBrace __check

// class SQLDBC_MethodBrace
// {
// public:
//     SQLDBC_MethodBrace() 
//     {
//         mallinfo();
//     }
    
//     ~SQLDBC_MethodBrace()
//     {
//         mallinfo();
//     }
// };

SQLDBC_ErrorHndl::SQLDBC_ErrorHndl()
  : m_error(0)
{
    SQLDBC_METHOD_BRACE;
}

SQLDBC_ErrorHndl::SQLDBC_ErrorHndl(IFR_ErrorHndl *error)
{
    SQLDBC_METHOD_BRACE;
    m_error = error;
}

SQLDBC_ErrorHndl::operator SQLDBC_Bool () const
{
    SQLDBC_METHOD_BRACE;
  if(m_error)
    return m_error->getErrorCode() != 0;
  return false;
}

const SQLDBC_Int4 SQLDBC_ErrorHndl::getErrorCode() const
{
    SQLDBC_METHOD_BRACE;
  if(m_error)
    return m_error->getErrorCode();
  return 0;
}

const char* SQLDBC_ErrorHndl::getSQLState() const
{
    SQLDBC_METHOD_BRACE;
  if(m_error)
    return m_error->getSQLState();
  return "";
}

const char* SQLDBC_ErrorHndl::getErrorText() const
{
    SQLDBC_METHOD_BRACE;
  if(m_error)
    return m_error->getErrorText();
  return "";
}

void
SQLDBC_ErrorHndl::setMemoryAllocationFailed()
{
    SQLDBC_METHOD_BRACE;
    global_oom_error.setMemoryAllocationFailed();
    m_error = &global_oom_error;
}

/********************************/

SQLDBC_SQLWarning::SQLDBC_SQLWarning(IFR_SQLWarning *warning)
{
    SQLDBC_METHOD_BRACE;
  m_warning = warning;
}

SQLDBC_SQLWarning* SQLDBC_SQLWarning::getNextWarning() const
{
    SQLDBC_METHOD_BRACE;
#ifndef HPUX
    return (SQLDBC_SQLWarning*) m_warning->getNextWarning();
#else
    return reinterpret_cast<SQLDBC_SQLWarning*>( m_warning->getNextWarning() );
#endif
}

SQLDBC_SQLWarningCode SQLDBC_SQLWarning::getWarningCode() const
{
    SQLDBC_METHOD_BRACE;
  return (SQLDBC_SQLWarningCode)m_warning->getWarningCode();
}


SQLDBC_ConnectionItem::SQLDBC_ConnectionItem(IFR_ConnectionItem *item, SQLDBC_Bool isCreatedForSQLDBC_Connection, SQLDBC_Connection* self)
{
    SQLDBC_METHOD_BRACE;
    if(item) {
        if(! item->getAllocator() ) {
          // set to 0
          // subsequent calls will return SQLDBC_INVALID_OBJECT
          m_citem = 0;
        }
      if (isCreatedForSQLDBC_Connection == SQLDBC_TRUE){
        SQLDBC_ConnectionItemStorageForConnection* newItem = new IFR_ALLOCATOR(*(item->getAllocator())) SQLDBC_ConnectionItemStorageForConnection(item, self);
        if(newItem) {
            m_citem = newItem;
            m_citem->m_item->getConnection()->setStmtList(&(newItem->m_StmtList));
        } else {
            m_citem = 0;
        }
      }else{
        m_citem = new IFR_ALLOCATOR(*(item->getAllocator())) SQLDBC_ConnectionItemStorage(item);
      }
   }
}

SQLDBC_ConnectionItem::~SQLDBC_ConnectionItem()
{
    SQLDBC_METHOD_BRACE;
    if (m_citem && m_citem->m_item) {
        SAPDBMem_IRawAllocator& alloc = *(m_citem->m_item->getAllocator());
        IFRUtil_Delete(m_citem, alloc);
    }
}

SQLDBC_ErrorHndl& SQLDBC_ConnectionItem::error()
{
    SQLDBC_METHOD_BRACE;
    if(m_citem) {
        m_citem->m_error = &m_citem->m_item->error();
        return m_citem->m_error;
    } else {
        global_sqldbc_oom_error.setMemoryAllocationFailed();
        return global_sqldbc_oom_error;
    }
}

SQLDBC_SQLWarning* SQLDBC_ConnectionItem::warning()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  if(m_citem && m_citem->m_item && m_citem->m_item->warning()) {
      m_citem->m_warning = m_citem->m_item->warning();
      return &m_citem->m_warning;
  }
  return 0;
}

void SQLDBC_ConnectionItem::clearWarnings()
{
    SQLDBC_METHOD_BRACE;
    if(!this) return;
    if(m_citem && m_citem->m_item) {
        m_citem->m_item->clearWarnings();
    }
}

void SQLDBC_ConnectionItem::clearError()
{
    SQLDBC_METHOD_BRACE;
    if(!this) return;
    if(m_citem && m_citem->m_item) {
        m_citem->m_item->clearError();
    }
}

#define SQLDBC_CHECK_CITEM                                              \
    if(m_citem == 0 || m_citem->m_item==0) {                            \
        ((SQLDBC_ConnectionItem *)this)->error().setMemoryAllocationFailed(); \
        return;                                                         \
    }

#define SQLDBC_CHECK_CITEM_ARG(x)                                       \
    if(m_citem == 0 || m_citem->m_item==0) {                            \
        ((SQLDBC_ConnectionItem *)this)->error().setMemoryAllocationFailed(); \
        return (x);                                                     \
    }


/********************************/

SQLDBC_Environment::SQLDBC_Environment(SQLDBC_IRuntime* runtime)
{
    SQLDBC_METHOD_BRACE;
    IFR_Environment* containedEnvironment = new IFR_ALLOCATOR(runtime->getGlobalAllocator()) IFR_Environment(runtime);
    if(containedEnvironment) {
        m_eitem = new IFR_ALLOCATOR(runtime->getGlobalAllocator()) SQLDBC_EnvironmentItemStorage(containedEnvironment, runtime);
        if(m_eitem) {
            containedEnvironment->setConnectionList(&(m_eitem->m_ConnectionsList));
        } else {
            IFRUtil_Delete(containedEnvironment, containedEnvironment->allocator);
        }
    } else {
        m_eitem = 0;
    }
}

SQLDBC_Environment::~SQLDBC_Environment()
{
    SQLDBC_METHOD_BRACE;
    if(m_eitem && m_eitem->m_item){
      m_eitem->releaseAllConnections();
      IFR_Environment* containedEnvironment = m_eitem->m_item;
      m_eitem->m_ConnectionsList.deleteList(*containedEnvironment);
      IFRUtil_Delete(m_eitem,              containedEnvironment->allocator);
      IFRUtil_Delete(containedEnvironment, containedEnvironment->allocator);
    }
}

static const char* SQLDBCLibVersion =
  COMP_NAME_SQLDBC_DRV_SP100" <MAJOR_VERSION>.<MINOR_VERSION>.<CORRECTION_LEVEL>    BUILD <BUILD_NUMBER_EX_STRING>";

const char *SQLDBC_Environment::getLibraryVersion()
{
    SQLDBC_METHOD_BRACE;
    return SQLDBCLibVersion;
}

SQLDBC_Connection *SQLDBC_Environment::createConnection() const
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    if(m_eitem && m_eitem->m_item) {
        IFR_Connection *contained_connection=m_eitem->m_item->getConnection();
        if(contained_connection == 0) {
            return 0;
        }
        SQLDBC_Connection *result = new IFR_ALLOCATOR(m_eitem->m_item->allocator) SQLDBC_Connection(contained_connection);
        if(result == 0) {
            m_eitem->m_item->releaseConnection(contained_connection);
            return 0;
        }
        if(result->m_citem == 0) {
            m_eitem->m_item->releaseConnection(contained_connection);
            IFRUtil_Delete(result, m_eitem->m_item->allocator);
            return 0;
        }
        m_eitem->m_ConnectionsList.Insert(*(static_cast <SQLDBC_ConnectionItemStorageForConnection*>(result->m_citem))
        );
        return result;
    } else {
        return 0;
    }
}

SQLDBC_Connection *SQLDBC_Environment::createConnection(SAPDBMem_IRawAllocator& allocator) const
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    if(m_eitem && m_eitem->m_item) {
        IFR_Connection *conn = m_eitem->m_item->getConnection(allocator);
        if(conn == 0) {
            return 0;
        }
        SQLDBC_Connection *result = new IFR_ALLOCATOR(allocator) SQLDBC_Connection(conn);
        if(result == 0) {
            m_eitem->m_item->releaseConnection(conn);
            return 0;
        }
        if(result->m_citem == 0) {
            m_eitem->m_item->releaseConnection(conn);
            IFRUtil_Delete(result, allocator);
            return 0;
        }
        m_eitem->m_ConnectionsList.Insert(
            *(static_cast <SQLDBC_ConnectionItemStorageForConnection*>(result->m_citem))
            );
      return result;
    } else {
        return 0;
    }
}

void SQLDBC_Environment::releaseConnection(SQLDBC_Connection *conn) const
{
    SQLDBC_METHOD_BRACE;
  if (m_eitem && m_eitem->m_item && conn && conn->m_citem) {
      m_eitem->m_ConnectionsList.Remove(
        *(static_cast <SQLDBC_ConnectionItemStorageForConnection*>(conn->m_citem))
      );
      IFR_Connection *containedConnection=(IFR_Connection *)conn->m_citem->m_item;
      IFRUtil_Delete(conn, containedConnection->allocator);
      m_eitem->m_item->releaseConnection(containedConnection);
  }
}

SAPDBMem_IRawAllocator& SQLDBC_Environment::getAllocator()
{
    SQLDBC_METHOD_BRACE;
    return m_eitem->m_item->allocator;
}


void SQLDBC_Environment::setTraceOptions(const SQLDBC_ConnectProperties& traceoptions)
{
    SQLDBC_METHOD_BRACE;
    m_eitem->m_item->setTraceOptions(*(traceoptions.m_prop));
}


void SQLDBC_Environment::getTraceOptions(SQLDBC_ConnectProperties& traceoptions)
{
    SQLDBC_METHOD_BRACE;
    m_eitem->m_item->setTraceOptions(*(traceoptions.m_prop));
}


SQLDBC_Profile*
SQLDBC_Environment::getProfile()
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    IFR_Environment *env = (IFR_Environment *)m_eitem->m_item;
    IFR_Profile *profile = env->getProfile();
    SQLDBC_Profile *result=0;
    if (profile) {
        m_eitem->m_SQLDBCprofile.m_profile = profile;
        result = &(m_eitem->m_SQLDBCprofile);
    } else {
         m_eitem->m_SQLDBCprofile.m_profile = 0;
    }
    return result;
}

/********************************/

SQLDBC_ConnectProperties::SQLDBC_ConnectProperties()
{
    SQLDBC_METHOD_BRACE;
    IFR_Bool memory_ok_ignored=true;
    m_prop = new IFR_ALLOCATOR(RTE_IInterface::Instance().Allocator())
        IFR_ConnectProperties(RTE_IInterface::Instance().Allocator(), memory_ok_ignored);
}

SQLDBC_ConnectProperties::SQLDBC_ConnectProperties(SAPDBMem_IRawAllocator& allocator)
{
    SQLDBC_METHOD_BRACE;
    IFR_Bool memory_ok_ignored=true;
    m_prop = new IFR_ALLOCATOR(allocator) IFR_ConnectProperties(allocator, memory_ok_ignored);
}


SQLDBC_ConnectProperties::SQLDBC_ConnectProperties(const SQLDBC_ConnectProperties& copy)
:m_prop(0)
{
    if(copy.m_prop) {
        IFR_Bool memory_ok = true;
        m_prop = new IFR_ALLOCATOR(copy.m_prop->allocator) IFR_ConnectProperties(*(copy.m_prop), memory_ok);
        if(m_prop) {
            if(!memory_ok) {
                IFRUtil_Delete(m_prop, m_prop->allocator);
                m_prop = 0;
            }
        }
    }
}

SQLDBC_ConnectProperties::~SQLDBC_ConnectProperties()
{
    SQLDBC_METHOD_BRACE;
    SAPDBMem_IRawAllocator& allocator=m_prop->allocator;
    IFRUtil_Delete(m_prop, allocator);
}

void SQLDBC_ConnectProperties::setProperty(const char *key, const char *value)
{
    SQLDBC_METHOD_BRACE;
    if(m_prop) {
        IFR_Bool memory_ok_ignored=true;
        m_prop->setProperty(key, value, memory_ok_ignored);
    }
}

const char *SQLDBC_ConnectProperties::getProperty(const char *key, const char *defaultvalue) const
{
    SQLDBC_METHOD_BRACE;
    if(m_prop) {
        return m_prop->getProperty(key, defaultvalue);
    } else {
        return defaultvalue;
    }
}

/********************************/

SQLDBC_Connection::SQLDBC_Connection(IFR_Connection *conn)
  : SQLDBC_ConnectionItem(conn, SQLDBC_TRUE, this)
{
    SQLDBC_METHOD_BRACE;
}
//SQLDBC_Connection::~SQLDBC_Connection(){
//}

SQLDBC_Retcode SQLDBC_Connection::close()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return (SQLDBC_Retcode)conn->close();
}

SQLDBC_Retcode SQLDBC_Connection::commitWorkRelease()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return (SQLDBC_Retcode)conn->close(true, false);
}

SQLDBC_Retcode SQLDBC_Connection::disconnect()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return (SQLDBC_Retcode)conn->close(false, true);
}

SQLDBC_Retcode SQLDBC_Connection::commit()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return (SQLDBC_Retcode)conn->commit();
}

SQLDBC_Statement *SQLDBC_Connection::createStatement()
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    SQLDBC_CHECK_CITEM_ARG(0);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    IFR_Statement *s = conn->createStatement();
    if(s == 0) {
        conn->error().setMemoryAllocationFailed();
        return 0;
    }
    SQLDBC_Statement *erg = new IFR_ALLOCATOR(conn->allocator) SQLDBC_Statement(s);
    if(erg && erg->m_cstmt == 0) {
        IFRUTIL_DELETEOBJECT(erg, SQLDBC_Statement, conn->allocator);
        erg = 0;
    }
    if (erg != 0) {
        (static_cast <SQLDBC_ConnectionItemStorageForConnection*> (m_citem))->m_StmtList.Insert(*(erg->m_cstmt));
    } else {
        conn->releaseStatement(s);
        conn->error().setMemoryAllocationFailed();
    }
    return erg;
}

SQLDBC_PreparedStatement *SQLDBC_Connection::createPreparedStatement()
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    SQLDBC_CHECK_CITEM_ARG(0);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    IFR_PreparedStmt *ps = conn->createPreparedStatement();
    if(ps == 0) {
        conn->error().setMemoryAllocationFailed();
        return 0;
    }
    SQLDBC_PreparedStatement *erg = new IFR_ALLOCATOR(conn->allocator) SQLDBC_PreparedStatement(ps);
    if(erg && (erg->m_cstmt == 0 || erg->m_pcstmt==0)) {
        IFRUTIL_DELETEOBJECT(erg, SQLDBC_PreparedStatement, conn->allocator);
        erg = 0;
    }
    if (erg != 0) {
        (static_cast <SQLDBC_ConnectionItemStorageForConnection*>(m_citem))->m_StmtList.Insert(*(erg->m_cstmt));
    } else {
        conn->releaseStatement(ps);
        conn->error().setMemoryAllocationFailed();
    }
    return erg;
}


void SQLDBC_Connection::releaseStatement(SQLDBC_Statement *stmt)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK_CITEM;
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;

  if(stmt && stmt->m_cstmt && stmt->m_cstmt->m_prepared) {
      SQLDBC_PreparedStatement *ps = (SQLDBC_PreparedStatement *) stmt->m_cstmt->m_self;
      releaseStatement(ps);
      return;
  }

  if(conn) {
    conn->clearError();
    if(stmt) {
      IFR_Statement *m_stmt = (IFR_Statement *)stmt->m_citem->m_item;     
      (static_cast <SQLDBC_ConnectionItemStorageForConnection*>(m_citem))->m_StmtList.Remove(*(stmt->m_cstmt));
      IFRUTIL_DELETEOBJECT(stmt, SQLDBC_Statement, conn->allocator);
      conn->releaseStatement(m_stmt);
    }
  }
}

void SQLDBC_Connection::releaseStatement(SQLDBC_PreparedStatement *stmt)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK_CITEM;
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  if(conn) {
    conn->clearError();
    if(stmt) {
      IFR_PreparedStmt *m_stmt = (IFR_PreparedStmt *)stmt->m_citem->m_item;
      (static_cast <SQLDBC_ConnectionItemStorageForConnection*>(m_citem))->m_StmtList.Remove(*(stmt->m_cstmt));
      IFRUTIL_DELETEOBJECT(stmt, SQLDBC_PreparedStatement, conn->allocator);
      conn->releaseStatement(m_stmt);
    }
  }
}

SQLDBC_Bool SQLDBC_Connection::getAutoCommit() const
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK_CITEM_ARG(false);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    return conn->getAutoCommit();
}

void SQLDBC_Connection::setAutoCommit(SQLDBC_Bool autocommit)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK_CITEM;
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    conn->setAutoCommit(autocommit);
}

void SQLDBC_Connection::setSQLMode(SQLDBC_SQLMode sqlmode)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK_CITEM;
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    conn->setSQLMode((IFR_SQLMode)sqlmode);
}

SQLDBC_Retcode SQLDBC_Connection::rollback()
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    return (SQLDBC_Retcode)conn->rollback();
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const char *connectURL,
                           SQLDBC_Length connectURLLength,
                           const char *connectCommand,
                           SQLDBC_Length connectCommandLength,
                           SQLDBC_StringEncoding connectCommandEncoding,
                           SQLDBC_ConnectProperties& connectProperties)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    return (SQLDBC_Retcode) conn->connect(connectURL,
                                          connectURLLength,
                                          connectCommand,
                                          connectCommandLength,
                                          connectCommandEncoding,
                                          *connectProperties.m_prop);
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const char *servernode,
                           const char *serverdb,
                           const char *username,
                           const char *password)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
    return this->connect(servernode, SQLDBC_NTS,
                         serverdb, SQLDBC_NTS,
                         username, SQLDBC_NTS,
                         password, SQLDBC_NTS,
                         SQLDBC_StringEncodingAscii);
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const char *servernode,
                           const char *serverdb,
                           const char *username,
                           const char *password,
                           const SQLDBC_StringEncoding userpwdEncoding)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
    return this->connect(servernode, SQLDBC_NTS,
                         serverdb, SQLDBC_NTS,
                         username, SQLDBC_NTS,
                         password, SQLDBC_NTS,
                         userpwdEncoding);
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const char *servernode,
                           SQLDBC_Length servernodeLength,
                           const char *serverdb,
                           SQLDBC_Length serverdbLength,
                           const char *username,
                           SQLDBC_Length usernameLength,
                           const char *password,
                           SQLDBC_Length passwordLength,
                           const SQLDBC_StringEncoding userpwdEncoding)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return (SQLDBC_Retcode) conn->connect(servernode, servernodeLength,
                                          serverdb, serverdbLength,
                                          username, usernameLength,
                                          password, passwordLength,
                                          IFR_StringEncoding(userpwdEncoding));
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const char *servernode,
                           const char *serverdb,
                           const char *username,
                           const char *password,
                           const SQLDBC_ConnectProperties& properties)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return this->connect(servernode, SQLDBC_NTS,
                       serverdb, SQLDBC_NTS,
                       username, SQLDBC_NTS,
                       password, SQLDBC_NTS,
                       SQLDBC_StringEncodingAscii,
                       properties);
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const char *servernode,
                           const char *serverdb,
                           const char *username,
                           const char *password,
                           const SQLDBC_StringEncoding userpwdEncoding,
                           const SQLDBC_ConnectProperties& properties)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return this->connect(servernode, SQLDBC_NTS,
                       serverdb, SQLDBC_NTS,
                       username, SQLDBC_NTS,
                       password, SQLDBC_NTS,
                       userpwdEncoding,
                       properties);
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const char *servernode,
                           SQLDBC_Length servernodeLength,
                           const char *serverdb,
                           SQLDBC_Length serverdbLength,
                           const char *username,
                           SQLDBC_Length usernameLength,
                           const char *password,
                           SQLDBC_Length passwordLength,
                           const SQLDBC_StringEncoding userpwdEncoding,
                           const SQLDBC_ConnectProperties& properties)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    return (SQLDBC_Retcode) conn->connect(servernode, servernodeLength,
                                          serverdb, serverdbLength,
                                          username, usernameLength,
                                          password, passwordLength,
                                          IFR_StringEncoding(userpwdEncoding),
                                          *properties.m_prop);
}

SQLDBC_Retcode
SQLDBC_Connection::connect()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_ConnectProperties unused;
  return this->connect((const char*)0,
                 (SQLDBC_Length)0,
                 (const char*)0,
                 (SQLDBC_Length)0,
                 SQLDBC_StringEncodingAscii,
                 unused);
}

SQLDBC_Retcode
SQLDBC_Connection::connect(const SQLDBC_ConnectProperties& properties)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);

  return this->connect((const char*)0,
                 (SQLDBC_Length)0,
                 (const char*)0,
                 (SQLDBC_Length)0,
                 SQLDBC_StringEncodingAscii,
                 (SQLDBC_ConnectProperties&)properties);
}


SQLDBC_Bool SQLDBC_Connection::isConnected() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, false);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return conn->isConnected();
}

SQLDBC_Retcode SQLDBC_Connection::cancel()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return (SQLDBC_Retcode)conn->cancel();
}

SQLDBC_DateTimeFormat::Format SQLDBC_Connection::getDateTimeFormat() const
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK_CITEM_ARG(SQLDBC_DateTimeFormat::None_C);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    return (SQLDBC_DateTimeFormat::Format)conn->getDateTimeFormat();
}

SQLDBC_Bool SQLDBC_Connection::isUnicodeDatabase() const
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, false);
    SQLDBC_CHECK_CITEM_ARG(false);
    IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
    conn->clearError();
    return conn->isUnicodeDatabase();
}

SQLDBC_Retcode SQLDBC_Connection::setTransactionIsolation(SQLDBC_Int4 isolationlevel)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  SQLDBC_CHECK_CITEM_ARG(SQLDBC_NOT_OK);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return (SQLDBC_Retcode)conn->setTransactionIsolation(isolationlevel);
}

SQLDBC_Int4 SQLDBC_Connection::getTransactionIsolation() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  SQLDBC_CHECK_CITEM_ARG(0);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return conn->getTransactionIsolation();
}

SQLDBC_Int4 SQLDBC_Connection::getKernelVersion() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  SQLDBC_CHECK_CITEM_ARG(0);
  IFR_Connection *conn = (IFR_Connection *)m_citem->m_item;
  conn->clearError();
  return conn->getKernelVersion();
}

/********************************/

SQLDBC_RowSet::SQLDBC_RowSet(IFR_ResultSet *resultset)
  : SQLDBC_ConnectionItem(resultset, SQLDBC_FALSE, 0)
{
    SQLDBC_METHOD_BRACE;
}

SQLDBC_Retcode SQLDBC_RowSet::setPos(SQLDBC_UInt4 pos)
{
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode)resultset->getRowSet ()->setPos(pos);
}

SQLDBC_Retcode
SQLDBC_RowSet::getObject(const SQLDBC_Int4     columnIndex,
                         const SQLDBC_HostType columnType,
                         void                 *columnAddr,
                         SQLDBC_Length        *columnLengthIndicator,
                         const SQLDBC_Length   columnSize,
                         const SQLDBC_Bool     terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_RowSet *rowset = resultset->getRowSet();
  if(rowset) {
    SQLDBC_Retcode rc = (SQLDBC_Retcode) rowset->getObject(columnIndex,
							   columnAddr,
							   columnSize,
							   (IFR_HostType)columnType,
							   columnLengthIndicator,
							   terminate);
    if(rc != SQLDBC_OK) {
      if(rowset->error() && !resultset->error()) {
          resultset->error().assign(rowset->error(), true); // for sqldbc, rowset error is transported to
                                                            // result set.
      }
    }
    return rc;
  } else {
    return SQLDBC_INVALID_OBJECT;
  }
}

SQLDBC_Retcode
SQLDBC_RowSet::getObject(const SQLDBC_Int4     columnIndex,
                         const SQLDBC_HostType columnType,
                         void                 *columnAddr,
                         SQLDBC_Length        *columnLengthIndicator,
                         const SQLDBC_Length   columnSize,
                         SQLDBC_Length         columnStartPos,
                         const SQLDBC_Bool     terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_RowSet *rowset = resultset->getRowSet();
  if(rowset) {
    SQLDBC_Retcode rc = (SQLDBC_Retcode) rowset->getObject(columnIndex,
							   columnAddr,
							   columnSize,
							   (IFR_HostType)columnType,
							   columnLengthIndicator,
							   terminate,
							   &columnStartPos);
    if(rc != SQLDBC_OK) {
        if(rowset->error() && !resultset->error()) {
            resultset->error().assign(rowset->error(), true); // transport rowset error to result set.
        }
    }
    return rc;
  } else {
    return SQLDBC_INVALID_OBJECT;
  }
}

SQLDBC_Retcode SQLDBC_RowSet::fetch()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode)resultset->getRowSet ()->fetch();
}

const SQLDBC_Int4 SQLDBC_RowSet::getRowsAffected () const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode)resultset->getFetchedRows();
}

const SQLDBC_Int4* SQLDBC_RowSet::getRowStatus() const
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
    static SQLDBC_Int4 retcode = SQLDBC_INVALID_OBJECT;
    return &retcode;
  }
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Int4 *)resultset->getRowStatus();
}


/********************************/

SQLDBC_UpdatableRowSet::SQLDBC_UpdatableRowSet(IFR_ResultSet *resultset)
  : SQLDBC_RowSet(resultset)
{
    SQLDBC_METHOD_BRACE;
}

SQLDBC_Retcode SQLDBC_UpdatableRowSet::insertAllRows()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->insertRow(0);
}

SQLDBC_Retcode SQLDBC_UpdatableRowSet::insertOneRow()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->insertRow(1);
}

SQLDBC_Retcode SQLDBC_UpdatableRowSet::insertRow(SQLDBC_UInt4 position)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->insertRow(position);
}

SQLDBC_Retcode SQLDBC_UpdatableRowSet::nextParameter(SQLDBC_Int2& paramIndex, void*& paramAddr)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->nextParameter(paramIndex, paramAddr);
}

SQLDBC_Retcode SQLDBC_UpdatableRowSet::nextParameterByIndex(SQLDBC_Int2& paramIndex, void*& paramAddr)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->nextParameterByIndex(paramIndex, paramAddr);
}

SQLDBC_Retcode SQLDBC_UpdatableRowSet::putData(void *paramAddr, SQLDBC_Length *paramLengthIndicator)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->putData(paramAddr, paramLengthIndicator);
}


/* compatibilty to older releases (7.5.00) */
SQLDBC_Retcode SQLDBC_UpdatableRowSet::updateRow(int pos)
{
    SQLDBC_METHOD_BRACE;
  return this->updateRow((SQLDBC_UInt4) pos);

}
SQLDBC_Retcode SQLDBC_UpdatableRowSet::updateRow(SQLDBC_UInt4 pos)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->updateRow(pos);
}

/* compatibilty to older releases (7.5.00) */
SQLDBC_Retcode SQLDBC_UpdatableRowSet::deleteRow(int pos)
{
    SQLDBC_METHOD_BRACE;
  return this->deleteRow((SQLDBC_UInt4) pos);
}

SQLDBC_Retcode SQLDBC_UpdatableRowSet::deleteRow(SQLDBC_UInt4 pos)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_UpdatableRowSet *rws = resultset->getUpdatableRowSet();
  if(rws == 0) {
      return SQLDBC_INVALID_OBJECT;
  }
  return (SQLDBC_Retcode)rws->deleteRow(pos);
}

/********************************/

SQLDBC_ResultSet::SQLDBC_ResultSet(SQLDBC_Statement *statement,
                                   IFR_ResultSet *resultset)
  : SQLDBC_ConnectionItem(resultset, SQLDBC_FALSE, 0)
{
    SQLDBC_METHOD_BRACE;
    m_cresult = new IFR_ALLOCATOR(resultset->allocator) SQLDBC_ResultSetStorage(statement, resultset);
}

SQLDBC_ResultSet::~SQLDBC_ResultSet()
{
    SQLDBC_METHOD_BRACE;
    if (m_cresult) {
        IFRUtil_Delete(m_cresult, m_cresult->m_allocator);
    }
}

SQLDBC_Retcode SQLDBC_ResultSet::setBindingType(SQLDBC_size_t size)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
    return (SQLDBC_Retcode) resultset->setBindingType(size);
}

SQLDBC_Retcode
SQLDBC_ResultSet::bindColumn (const SQLDBC_UInt4 columnIndex,
                              const SQLDBC_HostType columnType,
                                               void *columnAddr,
                                               SQLDBC_Length *columnLengthIndicator,
                                               const SQLDBC_Length columnLen,
                              const SQLDBC_Bool terminate)
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
    return (SQLDBC_Retcode) resultset->bindColumn(columnIndex,
                                                  (IFR_HostType)columnType,
                                                  columnAddr,
                                                  columnLengthIndicator,
                                                  columnLen,
                                                  terminate);
}

SQLDBC_Retcode SQLDBC_ResultSet::bindColumn (const SQLDBC_UInt4 columnIndex,
                                             const SQLDBC_HostType columnType,
                                             void *columnAddr,
                                             SQLDBC_Length *columnLengthIndicator,
                                             SQLDBC_Length *columnPositionIndicator,
                                             const SQLDBC_Length columnLen,
                                             const SQLDBC_Bool terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->bindColumn(columnIndex,
                                                (IFR_HostType)columnType,
                                                columnAddr,
                                                columnLengthIndicator,
                                                columnPositionIndicator,
                                                columnLen,
                                                terminate);
}

SQLDBC_Retcode
SQLDBC_ResultSet::clearColumns()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->clearColumns();
}


void SQLDBC_ResultSet::setFetchSize(SQLDBC_Int2 fetchsize)
{
    SQLDBC_METHOD_BRACE;
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  resultset->setFetchSize(fetchsize);
}


void SQLDBC_ResultSet::setRowSetSize(SQLDBC_UInt4 rowsetsize)
{
    SQLDBC_METHOD_BRACE;
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  resultset->setRowSetSize(rowsetsize);
}

const SQLDBC_UInt4 SQLDBC_ResultSet::getRowSetSize() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return resultset->getRowSetSize();
}

const SQLDBC_Int4 SQLDBC_ResultSet::getResultCount() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return resultset->getResultCount();
}

SQLDBC_RowSet *SQLDBC_ResultSet::getRowSet()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  if (((IFR_ResultSet *) m_citem->m_item)->assertNotClosed () == IFR_OK) {
    ((IFR_ResultSet *) m_citem->m_item)->error().clear();
    return &m_cresult->m_rowset;
  } else {
    return ((SQLDBC_RowSet *) 0);
  }
}

SQLDBC_UpdatableRowSet *SQLDBC_ResultSet::getUpdatableRowSet()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  if (((IFR_ResultSet *) m_citem->m_item)->assertNotClosed () == IFR_OK) {
    ((IFR_ResultSet *) m_citem->m_item)->error().clear();
    return &m_cresult->m_upd_rowset;
  } else {
    return ((SQLDBC_UpdatableRowSet *) 0);
  }
}

SQLDBC_Statement *SQLDBC_ResultSet::getStatement()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_cresult->m_statement;
}

SQLDBC_Bool SQLDBC_ResultSet::isUpdatable()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_FALSE);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Bool) resultset->isUpdatable();
}

SQLDBC_Retcode SQLDBC_ResultSet::absolute(int row)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->absolute(row);
}

SQLDBC_Retcode SQLDBC_ResultSet::next()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->next();
}

SQLDBC_Retcode SQLDBC_ResultSet::relative(int relativePos)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->relative(relativePos);
}

SQLDBC_Retcode SQLDBC_ResultSet::previous()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->previous();
}

SQLDBC_Retcode SQLDBC_ResultSet::first()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->first();
}

SQLDBC_Retcode SQLDBC_ResultSet::last()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return (SQLDBC_Retcode) resultset->last();
}

void SQLDBC_ResultSet::close()
{
    SQLDBC_METHOD_BRACE;
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  resultset->close();
}

const SQLDBC_UInt4 SQLDBC_ResultSet::getRowNumber() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  return resultset->getRow();
}

SQLDBC_ResultSetMetaData* SQLDBC_ResultSet::getResultSetMetaData()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_ResultSet *stmt = (IFR_ResultSet *)m_citem->m_item;
  IFR_ResultSetMetaData *metadata = stmt->getResultSetMetaData();
  if (metadata) {
    m_cresult->m_columninfo = metadata;
    return &m_cresult->m_columninfo;
  }
  return 0;
}

//----------------------------------------------------------------------
SQLDBC_Retcode
SQLDBC_ResultSet::getObject(const SQLDBC_Int4     columnIndex,
                            const SQLDBC_HostType columnType,
                            void                 *columnAddr,
                            SQLDBC_Length        *columnLengthIndicator,
                            const SQLDBC_Length   columnSize,
                            const SQLDBC_Bool     terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_RowSet *rowset = resultset->getRowSet();
  if(rowset) {
      SQLDBC_Retcode rc= (SQLDBC_Retcode) rowset->getObject(columnIndex,
                                                            columnAddr,
                                                            columnSize,
                                                            (IFR_HostType)columnType,
                                                            columnLengthIndicator,
                                                            terminate);
      if(rc != SQLDBC_OK) {
          if(rowset->error() && !resultset->error()) {
              resultset->error().assign(rowset->error(), true);
          }
      }
      return rc;
  } else {
      return SQLDBC_INVALID_OBJECT;
  }
}

SQLDBC_Retcode
SQLDBC_ResultSet::getObject(const SQLDBC_Int4     columnIndex,
                            const SQLDBC_HostType columnType,
                            void                 *columnAddr,
                            SQLDBC_Length        *columnLengthIndicator,
                            const SQLDBC_Length   columnSize,
                            SQLDBC_Length         columnStartPos,
                            const SQLDBC_Bool     terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_ResultSet *resultset = (IFR_ResultSet *)m_citem->m_item;
  IFR_RowSet *rowset = resultset->getRowSet();
  if(rowset) {
      SQLDBC_Retcode rc = (SQLDBC_Retcode)rowset->getObject(columnIndex,
                                                            columnAddr,
                                                            columnSize,
                                                            (IFR_HostType)columnType,
                                                            columnLengthIndicator,
                                                            terminate,
                                                            &columnStartPos);
      if(rc != SQLDBC_OK) {
          if(rowset->error() && !resultset->error()) {
              resultset->error().assign(rowset->error(), true);
          }
      }
      return rc;
  } else {
      return SQLDBC_INVALID_OBJECT; // At least not crash
  }
}

SQLDBC_Statement::SQLDBC_Statement(IFR_Statement *stmt)
  : SQLDBC_ConnectionItem(stmt, SQLDBC_FALSE, 0)
{
    SQLDBC_METHOD_BRACE;
    m_cstmt = new IFR_ALLOCATOR(stmt->allocator) SQLDBC_StatementStorage(stmt, (void *)this);
}

SQLDBC_Statement::SQLDBC_Statement(IFR_PreparedStmt* stmt)
: SQLDBC_ConnectionItem(stmt, SQLDBC_FALSE, 0)
{
    SQLDBC_METHOD_BRACE;
    m_cstmt = new IFR_ALLOCATOR(stmt->allocator) SQLDBC_StatementStorage(stmt, 0);
}

SQLDBC_Statement::~SQLDBC_Statement()
{
    SQLDBC_METHOD_BRACE;
    if (m_cstmt) {
        clearResultSet();
        IFRUtil_Delete(m_cstmt, m_cstmt->m_allocator);
    }
}

void
SQLDBC_Statement::clearResultSet()
{
    SQLDBC_METHOD_BRACE;
    if(m_cstmt && m_cstmt->m_resultset) {
        IFRUTIL_DELETEOBJECT(m_cstmt->m_resultset, SQLDBC_ResultSet, m_cstmt->m_allocator);
        m_cstmt->m_resultset = 0;
    }
}

SQLDBC_Retcode SQLDBC_Statement::execute(const char *sql)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return this->execute(sql, SQLDBC_NTS, SQLDBC_StringEncodingAscii);
}

SQLDBC_Retcode SQLDBC_Statement::execute(const char *sql,
                                         const SQLDBC_StringEncoding encoding)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return this->execute(sql, SQLDBC_NTS, encoding);
}

SQLDBC_Retcode SQLDBC_Statement::execute(const char *sql,
                                         const SQLDBC_Length sqlLength,
                                         const SQLDBC_StringEncoding encoding)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  clearResultSet();
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->execute(sql, sqlLength, IFR_StringEncoding(encoding));
}

void SQLDBC_Statement::setResultSetFetchSize (SQLDBC_Int2 rows)
{
    SQLDBC_METHOD_BRACE;
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  stmt->setFetchSize(rows);
}

void SQLDBC_Statement::setMaxRows(SQLDBC_UInt4 rows)
{
    SQLDBC_METHOD_BRACE;
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  stmt->setMaxRows(rows);
}

const SQLDBC_UInt4 SQLDBC_Statement::getMaxRows() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return stmt->getMaxRows();
}

SQLDBC_ResultSet *SQLDBC_Statement::getResultSet()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  if(m_cstmt->m_resultset == 0) {
      IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
      IFR_ResultSet *resultset = stmt->getResultSet();
      if(resultset) {
          m_cstmt->m_resultset = new IFR_ALLOCATOR(m_cstmt->m_allocator)
              SQLDBC_ResultSet(this, resultset);
      }
  }
  return m_cstmt->m_resultset;
}

SQLDBC_Bool SQLDBC_Statement::isQuery() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, false);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return stmt->isQuery();
}

SQLDBC_Int4 SQLDBC_Statement::getKernelVersion() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, false);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return stmt->getConnection()->getKernelVersion();
}


const SQLDBC_Int4 SQLDBC_Statement::getRowsAffected() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return stmt->getRowsAffected();
}

const SQLDBC_Statement::ResultSetType SQLDBC_Statement::getResultSetType() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_Statement::FORWARD_ONLY);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Statement::ResultSetType) stmt->getResultSetType();
}

void SQLDBC_Statement::setResultSetType (SQLDBC_Statement::ResultSetType rsetType)
{
    SQLDBC_METHOD_BRACE;
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  stmt->setResultSetType((IFR_Statement::ResultSetType) rsetType);
}

void SQLDBC_Statement::setResultSetConcurrencyType (ConcurrencyType conType)
{
    SQLDBC_METHOD_BRACE;
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  stmt->setResultSetConcurrencyType((IFR_Statement::ConcurrencyType)conType);
}

 const SQLDBC_Statement::ConcurrencyType SQLDBC_Statement::getResultSetConcurrencyType() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_Statement::CONCUR_READ_ONLY);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Statement::ConcurrencyType) stmt->getResultSetConcurrencyType();
}
//----------------------------------------------------------------------
SQLDBC_Retcode
SQLDBC_Statement::setCommandInfo(const char *buffer,
                               SQLDBC_Length bufferLength,
                               SQLDBC_Int4 lineNumber)
{
    SQLDBC_METHOD_BRACE;
    IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
    return (SQLDBC_Retcode) stmt->setCommandInfo(buffer, bufferLength, lineNumber);
}
//----------------------------------------------------------------------
void
SQLDBC_Statement::setCursorName(const char *buffer,
                                SQLDBC_Length bufferLength,
                                const SQLDBC_StringEncoding encoding)
{
    SQLDBC_METHOD_BRACE;
    IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
    stmt->setCursorName(buffer, bufferLength, IFR_StringEncoding(encoding));
    return;
}

SQLDBC_Retcode
SQLDBC_Statement::getCursorName(char  *buffer,
                                const SQLDBC_StringEncoding encoding,
                                const SQLDBC_Length bufferSize,
                                SQLDBC_Length *bufferLength) const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->getCursorName(buffer,
                                             IFR_StringEncoding(encoding),
                                             bufferSize, bufferLength);
}

SQLDBC_Retcode
SQLDBC_Statement::getTableName(char         *buffer,
                               const SQLDBC_StringEncoding encoding,
                               const SQLDBC_Length bufferSize,
                               SQLDBC_Length *bufferLength) const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->getTableName(buffer,
                                            IFR_StringEncoding(encoding),
                                            bufferSize, bufferLength);
}


const SQLDBC_UInt4 SQLDBC_Statement::getBatchSize() const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return stmt->getRowArraySize();
}

const SQLDBC_Int4*SQLDBC_Statement::getRowStatus() const
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
    static SQLDBC_Int4 retcode = SQLDBC_INVALID_OBJECT;
    return &retcode;
  }
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Int4 *)stmt->getRowStatus();
}

const SQLDBC_Retcode
SQLDBC_Statement::addBatch(const char *sql,
                           SQLDBC_Length length,
                           SQLDBC_StringEncoding encoding)
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
      return SQLDBC_INVALID_OBJECT;
  }
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode) stmt->addBatch(sql, length, encoding);
}

const SQLDBC_Retcode
SQLDBC_Statement::addBatch(const char *sql,
                           SQLDBC_StringEncoding encoding)
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
      return SQLDBC_INVALID_OBJECT;
  }
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode) stmt->addBatch(sql, SQLDBC_NTS, encoding);
}

const SQLDBC_Retcode
SQLDBC_Statement::addBatch(const char *sql)
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
      return SQLDBC_INVALID_OBJECT;
  }
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode) stmt->addBatch(sql, SQLDBC_NTS, SQLDBC_StringEncodingAscii);
}

const SQLDBC_Retcode
SQLDBC_Statement::executeBatch()
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
      return SQLDBC_INVALID_OBJECT;
  }
  clearResultSet();
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode) stmt->executeBatch();
}

void
SQLDBC_Statement::clearBatch()
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
      return;
  }
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  stmt->clearBatch();
  return;
}

SQLDBC_Retcode
SQLDBC_Statement::getLastInsertedKey(SQLDBC_Int4     tag,
                                     SQLDBC_HostType type,
                                     void            *paramAddr,
                                     SQLDBC_Length   *lengthIndicator,
                                     SQLDBC_Length    size,
                                     SQLDBC_Bool      terminate)
{
    SQLDBC_METHOD_BRACE;
  if (!this) {
      return SQLDBC_INVALID_OBJECT;
  }
  IFR_Statement *stmt = (IFR_Statement *)m_citem->m_item;
  return (SQLDBC_Retcode) stmt->getLastInsertedKey(tag,
                                                   (IFR_HostType)type,
                                                   paramAddr,
                                                   lengthIndicator,
                                                   size,
                                                   terminate);
}


/********************************/

SQLDBC_PreparedStatement::SQLDBC_PreparedStatement(IFR_PreparedStmt *stmt)
:SQLDBC_Statement(stmt)
{
    SQLDBC_METHOD_BRACE;
    if(m_cstmt) {
        m_cstmt->m_self = this;
    }
    m_pcstmt = new IFR_ALLOCATOR(stmt->allocator) SQLDBC_PreparedStatementStorage(stmt);
}

SQLDBC_PreparedStatement::~SQLDBC_PreparedStatement()
{
    SQLDBC_METHOD_BRACE;
    if (m_pcstmt) {
        IFRUtil_Delete(m_pcstmt, m_pcstmt->m_allocator);
    }
}


SQLDBC_Retcode
SQLDBC_PreparedStatement::setBatchSize(SQLDBC_UInt4 rowarraysize)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->setRowArraySize(rowarraysize);
}

SQLDBC_UInt4
SQLDBC_PreparedStatement::getPreferredBatchSize()
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
    IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
    return stmt->getPreferredRowArraySize();
}


SQLDBC_Retcode
SQLDBC_PreparedStatement::clearParameters()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->clearParameters();

}

SQLDBC_Retcode
SQLDBC_PreparedStatement::bindParameter(const SQLDBC_UInt2     paramIndex,
                                        const SQLDBC_HostType  paramType,
                                        void                  *paramAddr,
                                        SQLDBC_Length         *paramLengthIndicator,
                                        const SQLDBC_Length    paramSize,
                                        const SQLDBC_Bool      terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->bindParameter(paramIndex,
                                             (IFR_HostType)paramType,
                                             paramAddr,
                                             paramLengthIndicator,
                                             paramSize,
                                             terminate);
}

SQLDBC_Retcode
SQLDBC_PreparedStatement::bindParameterAddr(const SQLDBC_UInt2    paramIndex,
                                            const SQLDBC_HostType paramType,
                                            void           *paramAddr,
                                            SQLDBC_Length  *paramLengthIndicator,
                                            const SQLDBC_Length   paramByteLength,
                                            const SQLDBC_Bool terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->bindParameterAddr(paramIndex,
                                                 (IFR_HostType) paramType,
                                                 paramAddr,
                                                 paramLengthIndicator,
                                                 paramByteLength,
                                                 terminate);
}

SQLDBC_Retcode SQLDBC_PreparedStatement::setBindingType(SQLDBC_size_t size)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->setBindingType(size);
}

SQLDBC_Retcode SQLDBC_PreparedStatement::nextParameter(SQLDBC_Int2& paramIndex, void*& paramAddr)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->nextParameter(paramIndex, paramAddr);
}

SQLDBC_Retcode SQLDBC_PreparedStatement::nextParameterByIndex(SQLDBC_Int2& paramIndex, void*& paramAddr)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->nextParameterByIndex(paramIndex, paramAddr);
}

SQLDBC_Retcode SQLDBC_PreparedStatement::putData(void *paramAddr, SQLDBC_Length  *paramLengthIndicator)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->putData(paramAddr, paramLengthIndicator);
}

SQLDBC_Retcode
SQLDBC_PreparedStatement::getObject(const SQLDBC_Int4     paramIndex,
				    const SQLDBC_HostType paramType,
				    void                 *paramAddr,
				    SQLDBC_Length        *paramLengthIndicator,
				    const SQLDBC_Length   paramSize,
				    const SQLDBC_Bool     terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->getObject(paramIndex,
					 paramAddr,
					 paramSize,
					 (IFR_HostType)paramType,
					 paramLengthIndicator,
					 terminate);
}

SQLDBC_Retcode
SQLDBC_PreparedStatement::getObject(const SQLDBC_Int4     paramIndex,
				    const SQLDBC_HostType paramType,
				    void                 *paramAddr,
				    SQLDBC_Length        *paramLengthIndicator,
				    const SQLDBC_Length   paramSize,
				    SQLDBC_Length         paramStartPos,
				    const SQLDBC_Bool     terminate)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->getObject(paramIndex,
					 paramAddr,
					 paramSize,
					 (IFR_HostType)paramType,
					 paramLengthIndicator,
					 terminate,
					 &paramStartPos);
}

SQLDBC_Retcode SQLDBC_PreparedStatement::prepare (const char *sql)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return this->prepare(sql, SQLDBC_NTS, SQLDBC_StringEncodingAscii);
}

SQLDBC_Retcode SQLDBC_PreparedStatement::prepare (const char   *sql,
                           const SQLDBC_StringEncoding encoding)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return this->prepare(sql, SQLDBC_NTS, encoding);
}

SQLDBC_Retcode SQLDBC_PreparedStatement::prepare (const char   *sql,
                           const SQLDBC_Length sqlLength,
                           const SQLDBC_StringEncoding encoding)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->prepare(sql, sqlLength, IFR_StringEncoding(encoding));
}

SQLDBC_Retcode SQLDBC_PreparedStatement::execute ()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  clearResultSet();
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  return (SQLDBC_Retcode)stmt->executeBatch();
}

SQLDBC_ParameterMetaData*
SQLDBC_PreparedStatement::getParameterMetaData()
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
    IFR_ParameterMetaData *metadata = stmt->getParameterMetaData();
    SQLDBC_ParameterMetaData *result=0;
    if (metadata) {
        m_pcstmt->m_paraminfo.m_metadata = metadata;
        result = &(m_pcstmt->m_paraminfo);
    } else {
        m_pcstmt->m_paraminfo.m_metadata = 0;
    }
    return result;
}

SQLDBC_ResultSetMetaData* SQLDBC_PreparedStatement::getResultSetMetaData()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  IFR_PreparedStmt *stmt = (IFR_PreparedStmt *)m_citem->m_item;
  IFR_ResultSetMetaData *metadata = stmt->getResultSetMetaData();
  if (metadata) {
    m_pcstmt->m_columninfo = metadata;
    return &m_pcstmt->m_columninfo;
  }
  return 0;
}


/********************************/

SQLDBC_ResultSetMetaData::SQLDBC_ResultSetMetaData(IFR_ResultSetMetaData *metadata)
  : m_metadata(metadata)
{
    SQLDBC_METHOD_BRACE;
}

SQLDBC_Int2 SQLDBC_ResultSetMetaData::getColumnCount()
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getColumnCount();
}

SQLDBC_ResultSetMetaData::ColumnNullBehavior SQLDBC_ResultSetMetaData::isNullable(SQLDBC_Int2 column)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, columnNullableUnknown);
  return (SQLDBC_ResultSetMetaData::ColumnNullBehavior)m_metadata->isNullable(column);
}

SQLDBC_Int4 SQLDBC_ResultSetMetaData::getColumnLength(SQLDBC_Int2 column)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getColumnLength(column);
}

SQLDBC_Int4 SQLDBC_ResultSetMetaData::getPhysicalLength(SQLDBC_Int2 column)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getPhysicalLength(column);
}

SQLDBC_Retcode
SQLDBC_ResultSetMetaData::getColumnName(SQLDBC_Int2 column,
                                        char *buffer,
                                        const SQLDBC_StringEncoding encoding,
                                        const SQLDBC_Length bufferSize,
                                        SQLDBC_Length *bufferLength) const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return (SQLDBC_Retcode)m_metadata->getColumnName(column, buffer,
                                                   IFR_StringEncoding(encoding), bufferSize, bufferLength);
}

SQLDBC_Int4 SQLDBC_ResultSetMetaData::getPrecision(SQLDBC_Int2 column)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getPrecision(column);
}

SQLDBC_Int4 SQLDBC_ResultSetMetaData::getScale(SQLDBC_Int2 column)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getScale(column);
}

SQLDBC_SQLType SQLDBC_ResultSetMetaData::getColumnType(SQLDBC_Int2 column)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_SQLTYPE_UNKNOWN);
  return (SQLDBC_SQLType)m_metadata->getColumnType(column);
}

SQLDBC_Bool SQLDBC_ResultSetMetaData::isWritable(SQLDBC_Int2 column)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, false);
  return m_metadata->isWritable(column);
}

/********************************/

SQLDBC_ParameterMetaData::SQLDBC_ParameterMetaData(IFR_ParameterMetaData *metadata)
  : m_metadata(metadata)
{
    SQLDBC_METHOD_BRACE;
}

SQLDBC_ParameterMetaData::ParameterMode
SQLDBC_ParameterMetaData::getParameterMode(SQLDBC_Int2 param)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, parameterModeUnknown);
  return (SQLDBC_ParameterMetaData::ParameterMode)m_metadata->getParameterMode(param);
}

SQLDBC_Int2 SQLDBC_ParameterMetaData::getParameterCount()
{
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    return m_metadata->getParameterCount();
}

SQLDBC_ParameterMetaData::ParameterNullBehavior
SQLDBC_ParameterMetaData::isNullable(SQLDBC_Int2 param)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, parameterNullableUnknown);
  return (SQLDBC_ParameterMetaData::ParameterNullBehavior)m_metadata->isNullable(param);
}

SQLDBC_Int4 SQLDBC_ParameterMetaData::getParameterLength(SQLDBC_Int2 param)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getParameterLength(param);
}

SQLDBC_Int4 SQLDBC_ParameterMetaData::getPhysicalLength(SQLDBC_Int2 param)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getPhysicalLength(param);
}

SQLDBC_Retcode
SQLDBC_ParameterMetaData::getParameterName(SQLDBC_Int2    parameter,
                                           char *buffer,
                                           const SQLDBC_StringEncoding encoding,
                                           const SQLDBC_Length bufferSize,
                                           SQLDBC_Length *bufferLength) const
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_INVALID_OBJECT);
  return (SQLDBC_Retcode)m_metadata->getParameterName(parameter, buffer,
                                                      IFR_StringEncoding(encoding), bufferSize, bufferLength);
}

SQLDBC_Int4 SQLDBC_ParameterMetaData::getPrecision(SQLDBC_Int2 param)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getPrecision(param);
}

SQLDBC_Int4 SQLDBC_ParameterMetaData::getScale(SQLDBC_Int2 param)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, 0);
  return m_metadata->getScale(param);
}

SQLDBC_SQLType SQLDBC_ParameterMetaData::getParameterType(SQLDBC_Int2 param)
{
    SQLDBC_METHOD_BRACE;
  SQLDBC_CHECK(this, SQLDBC_SQLTYPE_UNKNOWN);
  return (SQLDBC_SQLType)m_metadata->getParameterType(param);
}

//----------------------------------------------------------------------
SQLDBC_Retcode
SQLDBC_LOB::putData(void *paramAddr, SQLDBC_Length *paramLengthIndicator)
{
    SQLDBC_METHOD_BRACE;
    if(m_citem == 0) {
        return SQLDBC_INVALID_OBJECT;
    } else if(m_citem->getPutvalHost() == 0) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return SQLDBC_NOT_OK;
    } else if(m_citem->getGetvalHost()->checkLOB(m_lobdata) == false) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return SQLDBC_NOT_OK;
    }
    return (SQLDBC_Retcode) m_lobdata->putData(paramAddr, paramLengthIndicator);
}

//----------------------------------------------------------------------
SQLDBC_Retcode
SQLDBC_LOB::close()
{
    SQLDBC_METHOD_BRACE;
    if(m_citem == 0) {
        return SQLDBC_INVALID_OBJECT;
    }
    if(m_citem->getPutvalHost()) {
        if(m_citem->getPutvalHost()->checkLOB(m_lobdata) == false) {
            return SQLDBC_INVALID_OBJECT;
        }
    } else if(m_citem->getGetvalHost()) {
        if(m_citem->getGetvalHost()->checkLOB(m_lobdata) == false) {
            return SQLDBC_INVALID_OBJECT;
        }
    } else {
        return SQLDBC_INVALID_OBJECT;
    }
    return (SQLDBC_Retcode) m_lobdata->close();
}

//----------------------------------------------------------------------
SQLDBC_Retcode
SQLDBC_LOB::getData(void *paramAddr,
                    SQLDBC_Length *LengthIndicator,
                    const SQLDBC_Length Size,
                    const SQLDBC_Bool Terminate)
{
    SQLDBC_METHOD_BRACE;
    if(m_citem == 0) {
        return SQLDBC_INVALID_OBJECT;
    } else if(m_citem->getGetvalHost() == 0) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return SQLDBC_NOT_OK;
    } else if(m_citem->getGetvalHost()->checkLOB(m_lobdata) == false) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return SQLDBC_NOT_OK;
    }
    return (SQLDBC_Retcode) m_lobdata->getData(paramAddr,
                                               LengthIndicator,
                                               Size,
                                               (SQLDBC_Length*)0,
                                               Terminate);
}

//----------------------------------------------------------------------
SQLDBC_Retcode
SQLDBC_LOB::getData(void *paramAddr,
                    SQLDBC_Length *LengthIndicator,
                    const SQLDBC_Length Size,
                    const SQLDBC_Length position,
                    const SQLDBC_Bool Terminate)
{
    SQLDBC_METHOD_BRACE;
    if(m_citem == 0) {
        return SQLDBC_INVALID_OBJECT;
    } else if(m_citem->getGetvalHost() == 0) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return SQLDBC_NOT_OK;
    } else if(m_citem->getGetvalHost()->checkLOB(m_lobdata) == false) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return SQLDBC_NOT_OK;
    }

    return (SQLDBC_Retcode) m_lobdata->getData(paramAddr,
                                               LengthIndicator,
                                               Size,
                                               (IFR_Length *)&position,
                                               Terminate);
}

//----------------------------------------------------------------------
SQLDBC_Length
SQLDBC_LOB::getLength()
{
    SQLDBC_METHOD_BRACE;
    if(m_citem == 0) {
        return -1;
    } else if(m_citem->getGetvalHost() == 0) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return -1;
    } else if(m_citem->getGetvalHost()->checkLOB(m_lobdata) == false) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return -1;
    }

    return m_lobdata->getLength();
}

//----------------------------------------------------------------------
SQLDBC_Length
SQLDBC_LOB::getPosition()
{
    SQLDBC_METHOD_BRACE;
    if(m_citem == 0) {
        return -1;
    } else if(m_citem->getGetvalHost() == 0) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return -1;
    } else if(m_citem->getGetvalHost()->checkLOB(m_lobdata) == false) {
        m_citem->error().setRuntimeError(IFR_ERR_INVALID_LOB);
        return -1;
    }

    return m_lobdata->getPosition();
}

//----------------------------------------------------------------------
SQLDBC_Length
SQLDBC_LOB::getPreferredDataSize()
{
    SQLDBC_METHOD_BRACE;
    return MAX_IFR_INT4 - 1;
}

//----------------------------------------------------------------------
SQLDBC_Profile::SQLDBC_Profile(IFR_Profile *profile)
  : m_profile(profile)
{
    SQLDBC_METHOD_BRACE;
}
//----------------------------------------------------------------------

void
SQLDBC_Profile::resetCounters(){
    SQLDBC_METHOD_BRACE;
    if (!this) return;
    return m_profile->resetProfileCounters();
}

//----------------------------------------------------------------------
void
SQLDBC_Profile::collectCounters(){
    SQLDBC_METHOD_BRACE;
    if (!this) return;
    return m_profile->collectCounters();
}
//----------------------------------------------------------------------
SQLDBC_UInt8
SQLDBC_Profile::getCounter(SQLDBC_ProfileCountValues counter){
    SQLDBC_METHOD_BRACE;
    SQLDBC_CHECK(this, 0);
    return m_profile->getCounter((IFR_ProfileCountValues)counter);
}

}; // namespace SQLDBC

