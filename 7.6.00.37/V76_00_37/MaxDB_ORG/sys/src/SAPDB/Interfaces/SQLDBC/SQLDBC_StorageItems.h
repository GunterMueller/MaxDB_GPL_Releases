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
#ifndef SQLDBC_STORAGEITEMS_H
#define SQLDBC_STORAGEITEMS_H

#ifdef SQLDBC_FOR_KERNEL
#  define IFR_NAMESPACE SQLDBC_Kernel
#  define SQLDBC SQLDBC_Kernel
#  define SQLDBC_RUNTIME_IN_NAMESPACE
#endif

#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_Types.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_LinkedList.h"

#ifdef IFR_NAMESPACE
namespace IFR_NAMESPACE { 
#endif

class IFR_Statement;
class IFR_PreparedStmt;
class IFR_ConnectionItem;
class IFR_Environment;

#ifdef IFR_NAMESPACE
};
using namespace IFR_NAMESPACE;
#endif

class SAPDBMem_IRawAllocator;

namespace SQLDBC {    

struct SQLDBC_StatementStorage  : public IFRUtil_LinkedListItem<SQLDBC_StatementStorage> {
    SQLDBC_StatementStorage(IFR_Statement *statement, void *selfinit);
    SQLDBC_StatementStorage(IFR_PreparedStmt *preparedstatement, void *selfinit);
   
    IFR_Statement* getStatement(); 

    SAPDBMem_IRawAllocator& m_allocator;  
    SQLDBC_ResultSet       *m_resultset;
// Extra fence to prevent that errornously releaseStatement is called for an SQLDBC_Statement
// that is in fact an SQLDBC_PreparedStatement, but was casted. In case of m_prepared==true,
// the m_self pointer is then used to call the correct method and 'heal' the mistake.
    SQLDBC_Bool             m_prepared;
    void                   *m_self;
};

/********************************/
struct SQLDBC_ConnectionItemStorage
{
    SQLDBC_ConnectionItemStorage(IFR_ConnectionItem *item);
    virtual ~SQLDBC_ConnectionItemStorage();

    IFR_ConnectionItem *m_item;  
    SQLDBC_ErrorHndl    m_error;  
    SQLDBC_SQLWarning   m_warning;     
};

/********************************/
struct SQLDBC_ConnectionItemStorageForConnection : public SQLDBC_ConnectionItemStorage, IFRUtil_LinkedListItem<SQLDBC_ConnectionItemStorageForConnection>
{
    SQLDBC_ConnectionItemStorageForConnection(IFR_ConnectionItem *item, SQLDBC_Connection* selfinit);
    virtual ~SQLDBC_ConnectionItemStorageForConnection();
    void releaseAllStatements();

    IFRUtil_LinkedList<SQLDBC_StatementStorage> m_StmtList;
    SQLDBC_Connection*                       m_self;
};

/********************************/
struct SQLDBC_EnvironmentItemStorage
{
    SQLDBC_EnvironmentItemStorage(IFR_Environment *item, SQLDBC_IRuntime* runtime);
    IFR_Environment *m_item; 
    IFRUtil_LinkedList<SQLDBC_ConnectionItemStorageForConnection> m_ConnectionsList;
    SQLDBC_Profile m_SQLDBCprofile;
    void releaseAllConnections();
};

/********************************/
struct SQLDBC_PreparedStatementStorage 
{
    SQLDBC_PreparedStatementStorage(IFR_PreparedStmt* stmt);

    SAPDBMem_IRawAllocator&  m_allocator;
    SQLDBC_ParameterMetaData m_paraminfo;
    SQLDBC_ResultSetMetaData m_columninfo;
};

struct SQLDBC_ResultSetStorage {
    SQLDBC_ResultSetStorage(SQLDBC_Statement *statement, 
                            IFR_ResultSet *resultset);
    
    SQLDBC_Statement        *m_statement;
    SAPDBMem_IRawAllocator&  m_allocator;  
    SQLDBC_RowSet            m_rowset;
    SQLDBC_UpdatableRowSet   m_upd_rowset;
    SQLDBC_ResultSetMetaData m_columninfo;
};


}   //namespace
#endif
