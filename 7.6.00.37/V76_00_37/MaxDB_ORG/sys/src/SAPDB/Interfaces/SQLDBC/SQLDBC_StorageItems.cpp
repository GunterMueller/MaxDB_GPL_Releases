/*!
  @file           SQLDBC_Internals.h
  @author         DL SAP DB INTERFACES
  @ingroup        SQLDBC
  @brief          Contains typedefs and class definitions for internal use.

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

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_StorageItems.h"
#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/IFR_Environment.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/Runtime/IFR_Statement.h"
#include "SAPDB/Interfaces/Runtime/IFR_PreparedStmt.h"
#include "SAPDB/Interfaces/Runtime/IFR_ResultSet.h"


namespace SQLDBC {

    SQLDBC_StatementStorage::SQLDBC_StatementStorage(IFR_Statement *statement, void *selfinit)
    :m_allocator(statement->allocator),
     m_resultset(0),
     m_prepared(false),
     m_self(selfinit)
    {}

    SQLDBC_StatementStorage::SQLDBC_StatementStorage(IFR_PreparedStmt *preparedstatement, void *selfinit)
    :m_allocator(preparedstatement->allocator),
     m_resultset(0),
     m_prepared(true),
     m_self(selfinit)
    {}

  IFR_Statement* SQLDBC_StatementStorage::getStatement(){
    return (IFR_Statement*) ((SQLDBC::SQLDBC_Statement *)this->m_self)->m_citem->m_item;
  }

/********************************/

    SQLDBC_ConnectionItemStorage::SQLDBC_ConnectionItemStorage(IFR_ConnectionItem *item) 
    :m_item(item), 
     m_error(0), 
     m_warning(0)
    {}
    
    SQLDBC_ConnectionItemStorage::~SQLDBC_ConnectionItemStorage(){}
    
 /********************************/
    SQLDBC_ConnectionItemStorageForConnection::SQLDBC_ConnectionItemStorageForConnection(IFR_ConnectionItem *item, SQLDBC_Connection* selfinit)
      :SQLDBC_ConnectionItemStorage(item),
     m_StmtList( item->getConnection()->runtime.createSpinlock(*(item->getAllocator()))),
     m_self(selfinit)
     {}

    SQLDBC_ConnectionItemStorageForConnection::~SQLDBC_ConnectionItemStorageForConnection(){
      this->releaseAllStatements();
      this->m_StmtList.deleteList (*(this->m_item->getConnection()));
    }
   
    void SQLDBC_ConnectionItemStorageForConnection::releaseAllStatements(){
       SQLDBC_StatementStorage * stmt = 0;
       IFR_Statement *InternalStmt;
       void* self;
       IFR_Connection *conn = this->m_item->getConnection();
       while (0 != (stmt = (static_cast<SQLDBC_StatementStorage*> (this->m_StmtList.Last())))){  
         if(stmt == 0 || stmt->m_self == 0) {                           
           continue;                                                         
         }
         this->m_StmtList.Remove(*(stmt));
         InternalStmt = (IFR_Statement *)stmt->getStatement();
         self = stmt->m_self;
         if(stmt->m_prepared) {
           ((SQLDBC_PreparedStatement *) self)->~SQLDBC_PreparedStatement();  
         }else{
           ((SQLDBC_Statement *) self)->~SQLDBC_Statement();                                 
         } 
         if(conn) {
           conn->releaseStatement(InternalStmt);
           conn->allocator.Deallocate(self);
         }
       }
    }

/********************************/
    SQLDBC_EnvironmentItemStorage::SQLDBC_EnvironmentItemStorage(IFR_Environment *item, SQLDBC_IRuntime* runtime) 
    :m_item(item),
    m_ConnectionsList( runtime->createSpinlock(runtime->getGlobalAllocator()) ),
    m_SQLDBCprofile(0)  
    {}

    void SQLDBC_EnvironmentItemStorage::releaseAllConnections(){
      SQLDBC_ConnectionItemStorageForConnection * conn = 0;
      IFR_Connection *containedConnection;
      SQLDBC_Connection* self;
      IFR_Environment *env = this->m_item;
      if(!env){
         return;
      }
      while (0 != (conn = (static_cast<SQLDBC_ConnectionItemStorageForConnection*> (this->m_ConnectionsList.Last())))){  
        if(conn == 0 || conn->m_item == 0) {                           
           continue;                                                         
        }
        containedConnection = (IFR_Connection *)conn->m_item->getConnection();
        this->m_ConnectionsList.Remove(*(conn));
        self = conn->m_self;
        self->~SQLDBC_Connection();
        containedConnection->allocator.Deallocate(self);
        env->releaseConnection(containedConnection);
      }
    }


/********************************/
  SQLDBC_PreparedStatementStorage::SQLDBC_PreparedStatementStorage(IFR_PreparedStmt* stmt) 
    :m_allocator(stmt->allocator),
     m_paraminfo(0), 
     m_columninfo(0) 
    {} 

/********************************/
  SQLDBC_ResultSetStorage::SQLDBC_ResultSetStorage(SQLDBC_Statement *statement, 
                            IFR_ResultSet *resultset) 
    :m_statement(statement),
     m_allocator(resultset->allocator),
     m_rowset(resultset), 
     m_upd_rowset(resultset), 
     m_columninfo(0) 
    {}

}   //namespace
