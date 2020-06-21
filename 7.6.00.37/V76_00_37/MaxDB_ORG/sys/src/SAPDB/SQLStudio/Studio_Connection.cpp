/*
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



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




*/

/*!**********************************************************************

  module: Studio_Connection.cpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: SQLStudio

  description:  class for handling a database connection				

  see also:		/


************************************************************************/




/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "SAPDB/SQLStudio/Studio_Connection.hpp"



Studio_Connection::Studio_Connection(const sapdbwa_DBCPoolP pDbcPool,sapdbwa_DBCExclType nPoolingMode)
{
	m_pDbcHandle = NULL;
	m_pDbcPool = pDbcPool;	

	m_nAutoCommit = SQL_AUTOCOMMIT_ON;
	m_eSQLMode = cpr_kind_internal;
	m_nIsoLevel = SQL_TXN_READ_COMMITTED;

	m_nPoolingMode = nPoolingMode;
}

Studio_Connection::Studio_Connection(const Studio_Connection& oCon)
{
	m_pDbcHandle = NULL;

	m_sDBServer = oCon.m_sDBServer;
	m_sDBName = oCon.m_sDBName;
	m_sDBUser = oCon.m_sDBUser;
	m_sPassWord = oCon.m_sPassWord;

	m_sODBCDriver = oCon.m_sODBCDriver;

	m_pDbcPool = oCon.m_pDbcPool;

	m_nAutoCommit = oCon.m_nAutoCommit;
    m_eSQLMode = oCon.m_eSQLMode;
    m_nIsoLevel = oCon.m_nIsoLevel;
	m_nPoolingMode = oCon.m_nPoolingMode;
}
	

Studio_Connection::~Studio_Connection()
{
	Tools_DynamicUTF8String	sError;
	dbLogOff(sError);
}


SAPDB_Bool Studio_Connection::dbLogOff(Tools_DynamicUTF8String&	sError){


	if (m_pDbcPool != NULL && m_pDbcHandle != NULL) {

		if (sapdbwa_DBCDisconnect(m_pDbcPool,m_pDbcHandle ) == false){

			sapdbwa_ErrP pError =  sapdbwa_DBCGetErr(m_pDbcHandle);
			if (sapdbwa_GetErrId(pError) != sapdbwa_Error_NoError){

				sError = Tools_DynamicUTF8String((SAPDB_UTF8*) sapdbwa_GetErrMsg(pError));
				//destroy database connection
				sapdbwa_DestroyDBC(m_pDbcHandle);
				m_pDbcHandle = NULL;	

				return SAPDB_FALSE;
			}
		}	
		//destroy database connection
		sapdbwa_DestroyDBC(m_pDbcHandle);
		m_pDbcHandle = NULL;	
	}

	return SAPDB_TRUE; 
}


SAPDB_Bool Studio_Connection::dbLogOn(Tools_DynamicUTF8String&	sError)
{

	//dbc handle
	SQLHDBC			hdbc = 0;
	SQLHENV			henv = 0;		
	sapdbwa_ErrP pError = NULL;

	m_pDbcHandle = sapdbwa_CreateDBC();

    // http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844
    // The cast is fine because ODBC re-casts this value back to its appropriate type
    // depending on the attribute type
	if (sapdbwa_DBCSetAttr(m_pDbcHandle, SQL_AUTOCOMMIT, (SQLPOINTER) m_nAutoCommit) == false) {

		pError =  sapdbwa_DBCGetErr(m_pDbcHandle);
        sError = Tools_DynamicUTF8String((SAPDB_UTF8*) sapdbwa_GetErrMsg(pError));
		sapdbwa_DestroyDBC(m_pDbcHandle);
		m_pDbcHandle = NULL;	
        return SAPDB_FALSE;
    }

    // http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844
    // The cast is fine because ODBC re-casts this value back to its appropriate type
    // depending on the attribute type
	if (sapdbwa_DBCSetAttr(m_pDbcHandle, SQL_SQLMODE, (SQLPOINTER) m_eSQLMode) == false){

		pError =  sapdbwa_DBCGetErr(m_pDbcHandle);
        sError = Tools_DynamicUTF8String((SAPDB_UTF8*) sapdbwa_GetErrMsg(pError));
		sapdbwa_DestroyDBC(m_pDbcHandle);
		m_pDbcHandle = NULL;	
        return SAPDB_FALSE;
    }

    // http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118844
    // The cast is fine because ODBC re-casts this value back to its appropriate type
    // depending on the attribute type
	if (sapdbwa_DBCSetAttr(m_pDbcHandle, SQL_TXN_ISOLATION, (SQLPOINTER) m_nIsoLevel) == false) {

		pError =  sapdbwa_DBCGetErr(m_pDbcHandle);
        sError = Tools_DynamicUTF8String((SAPDB_UTF8*) sapdbwa_GetErrMsg(pError));
		sapdbwa_DestroyDBC(m_pDbcHandle);
		m_pDbcHandle = NULL;	
        return SAPDB_FALSE;
    }	

	if (sapdbwa_DBCConnect(m_pDbcPool,
						   m_pDbcHandle,
						   "",
						   (char*)m_sODBCDriver.StrPtr(),
						   (char*)m_sDBServer.StrPtr(),
						   (char*)m_sDBName.StrPtr(),
						   (char*)m_sDBUser.StrPtr(),
						   (char*)m_sPassWord.StrPtr(),
						   "", ///tmp/sqltrace
						   0,
						   0,
						   m_nPoolingMode)) {				

		hdbc = sapdbwa_DBCHandle(m_pDbcHandle);
		henv = sapdbwa_DBCEnv(m_pDbcHandle);
		if (hdbc ==  SQL_NULL_HDBC) {			
			
			sapdbwa_ErrP pError =  sapdbwa_DBCGetErr(m_pDbcHandle);
			if (sapdbwa_GetErrId(pError) != sapdbwa_Error_NoError){

				sError = Tools_DynamicUTF8String((SAPDB_UTF8*) sapdbwa_GetErrMsg(pError));
				//destroy database connection
				sapdbwa_DestroyDBC(m_pDbcHandle);
				m_pDbcHandle = NULL;	

				return SAPDB_FALSE;
			}			
		}
	}
	else {			
		sapdbwa_ErrP pError =  sapdbwa_DBCGetErr(m_pDbcHandle);
		if (sapdbwa_GetErrId(pError) != sapdbwa_Error_NoError){


			sError = Tools_DynamicUTF8String((SAPDB_UTF8*) sapdbwa_GetErrMsg(pError));
			//destroy database connection
			sapdbwa_DestroyDBC(m_pDbcHandle);
			m_pDbcHandle = NULL;	

			return SAPDB_FALSE;
		}
	};		

	return SAPDB_TRUE;
}


