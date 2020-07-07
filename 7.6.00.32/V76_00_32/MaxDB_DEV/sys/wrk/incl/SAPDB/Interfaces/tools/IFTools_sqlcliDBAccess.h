/*!**************************************************************************

  module      : IFTools_sqlcliDBAccess.h

  -------------------------------------------------------------------------

  responsible : D031096

  special area:
  description : Interface Runtime 8.0

  description:  Provides a class that handle the database access for command line tool sqlcli

  last changed: 2003-01-30
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2003-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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




*****************************************************************************/
#ifndef IFTOOLS_SQLCLIDBACCESS_H
#define IFTOOLS_SQLCLIDBACCESS_H

#include "SAPDB/Interfaces/tools/IFTools_sqlcliProperties.h"
#include "SAPDB/Interfaces/tools/IFTools_sqlcliOutput.h"
#include "SAPDB/Interfaces/tools/IFTools_sqlcliInput.h"

class IFTools_sqlcliDBAccess;

class IFTools_sqlcliMetaData {
public:
  IFTools_sqlcliMetaData   (IFTools_sqlcliDBAccess* m_dbaccess);
  ~IFTools_sqlcliMetaData  (){};
  virtual char* getSchemaname() {return "OWNER";}
  SQLDBC_Bool displayUsers();
  SQLDBC_Bool displaySchemas();
  SQLDBC_Bool displayTablesViews(IFTools_sqlcliInput::sqlcliInput_cmdtype cmdtype);
  SQLDBC_Bool displayColumns();
  SQLDBC_Bool displayListDBProcs();
  SQLDBC_Bool displayIndices();
  

protected:
	IFTools_sqlcliDBAccess* m_dbaccess;
};

#define SCHEMANAME "SCHEMANAME"
class IFTools_sqlcliMetaDataWithSchemaSupport : public IFTools_sqlcliMetaData {
public:
  inline IFTools_sqlcliMetaDataWithSchemaSupport(IFTools_sqlcliDBAccess* m_dbaccess) : 
  IFTools_sqlcliMetaData(m_dbaccess)
  {}
  ~IFTools_sqlcliMetaDataWithSchemaSupport  (){};
  virtual char* getSchemaname() {return SCHEMANAME;}
};

/**
 * @ingroup sqlcli
 * @brief database access for command line tool sqlcli. 
 */
class IFTools_sqlcliDBStmt {
public:
  IFTools_sqlcliDBStmt   (SQLDBC_Bool usePrepStmt, SQLDBC_Connection *conn);
  ~IFTools_sqlcliDBStmt  (){};
  SQLDBC_Bool setUsePreparedStmt(SQLDBC_Bool usePrepStmt, SQLDBC_Connection *conn);
  SQLDBC_Bool releaseStmt(SQLDBC_Connection *conn);
  SQLDBC_Bool hasOutputParameters();
  SQLDBC_ParameterMetaData* getParameterMetaData();
  SQLDBC_Retcode execute (const char * cmd); 
  inline SQLDBC_Statement *getStmt() {return (m_pstmt)?(SQLDBC_Statement *)m_pstmt:m_stmt;};
  inline SQLDBC_PreparedStatement *getPrepStmt() {return (m_pstmt)?m_pstmt:0;};
  inline SQLDBC_ResultSet *getResultSet() {return (m_pstmt)?m_pstmt->getResultSet():m_stmt->getResultSet();};

private:
    SQLDBC_Statement         *m_stmt;       //!<statement
    SQLDBC_PreparedStatement *m_pstmt;      //!<statement
};


class IFTools_sqlcliDBAccess {
public:

  IFTools_sqlcliDBAccess   (IFTools_sqlcliProperties *sqlcliProp, IFTools_sqlcliOutput *m_out);
  ~IFTools_sqlcliDBAccess  ();

  SQLDBC_Bool executeCMD (char *cmd);
  void setSQLmode();
  SQLDBC_Bool setAutocommit(SQLDBC_Bool mode);
  SQLDBC_Bool setUsePreparedStmt(SQLDBC_Bool mode);
  SQLDBC_Bool displayStatus();  
  SQLDBC_Bool displayUsers();
  SQLDBC_Bool displaySchemas();
  SQLDBC_Bool displayTablesViews(IFTools_sqlcliInput::sqlcliInput_cmdtype cmdtype);
  SQLDBC_Bool displayColumns();
  SQLDBC_Bool displayIndices();
  SQLDBC_Bool displayListDBProcs();
  SQLDBC_Bool doConnect();
  SQLDBC_Bool doDisconnect();

  friend class IFTools_sqlcliMetaData;
private:
#define VERSIONKERNELWITHSCHEMASUPPORT 70600
	inline SQLDBC_Bool supportsSchema() {
	  SQLDBC_Int4 kversion = this->m_connection->getKernelVersion();
	  return (kversion < VERSIONKERNELWITHSCHEMASUPPORT)?SQLDBC_FALSE:SQLDBC_TRUE;
	}
    SQLDBC_Bool assertConnect();
    void releaseConnection();
    SQLDBC_Bool doExecuteSQLCmd(char *cmd, char* searchSchema=0, char* searchTable=0, int useOpenPipe=0);
    SQLDBC_Bool printColumnnames(SQLDBC_ResultSetMetaData* rsmd);
    void printRowsAffected(SQLDBC_Int4 rcount, const char* action, SQLDBC_UInt8 time);

    IFTools_sqlcliOutput     *m_out;        //!<outputhandle                     
    IFTools_sqlcliProperties *m_sqlcliProp; //!<environment                     
    SQLDBC_Connection        *m_connection; //!<connection
	IFTools_sqlcliDBStmt     *m_stmtHdl;
	IFTools_sqlcliMetaData   *m_metaData;
};
#endif
