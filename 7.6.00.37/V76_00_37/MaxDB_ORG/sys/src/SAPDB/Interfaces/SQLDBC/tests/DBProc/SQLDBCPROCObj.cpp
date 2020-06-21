/***************************************************************************\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
/* SQLDBCPROCObj.cpp : Implementation of CSQLDBCPROC                            */
/****************************************************************************/

#include <lcbasetypes.h>
#include <livecachetypes.h>
#include "SQLDBCPROC.h" /* no check */
#include "SAPDB/Interfaces/SQLDBC/tests/DBProc/SQLDBCPROCObj.h"
#include <livecachesqldbc.h> /* no check */
/****************************************************************************/
/* Register is the very first method called after the creation of the       */
/* liveCache package.                                                       */
/* (within the "constructor" of the liveCache package no communication with */
/*  the liveCache has been activated yet!)                                  */

STDMETHODIMP CSQLDBCPROC::Register()
{
  HRESULT hr = S_OK;
  try {
      co90CheckVersion ((IUnknown**)&this->m_pIUnknown, 
						ILIVECACHESINK_VERSION, 
						LIVECACHETYPES_VERSION, NULL, NULL );

/* *** TODO ***                                                            */
  }
  catch (DbpError e) {
    throw DbpError(e);
  }
  
  return hr;
}

void throwSQLDBCError(SQLDBC_ErrorHndl &e)
{	
	throw DbpError (e.getErrorCode(), e.getErrorText());
}

/****************************************************************************/
/* *** TODO ***
 * Stored procedure implementation (to be inserted here)
 * E.g. an implementation looks like:
 *   STDMETHODIMP CSQLDBCPROC::CALC_SUM(int par1, int par2, int* sum)
 *   {
 *       ...
 *       return S_OK;
 *   }
 */

 STDMETHODIMP CSQLDBCPROC::SQLDBC2Upper(TString *p1)
 {
   // TODO: Add your implementation code here
   SQL_SQLDBCConnection con;
   
   SQLDBC_PreparedStatement *stmt = con.createPreparedStatement();
   SQLDBC_Retcode rc;
   char szCmd[300];
   char* pStr = (char*) p1;
   pStr[20] = '\0';
   sprintf(szCmd, "SELECT UPPER('%s') from DUAL", pStr);
   rc = stmt->prepare(szCmd);
   if (rc != SQLDBC_OK)
     throwSQLDBCError(stmt->error());
   rc = stmt->execute();
   if (rc != SQLDBC_OK)
     throwSQLDBCError(stmt->error());
   //Check if the SQL command return a resultset and get a result set object. 
   SQLDBC_ResultSet *result;
   result = stmt->getResultSet();
   
   //Position the curors within the resultset by doing a fetch next call. 
   rc = result->next();
   if (rc != SQLDBC_OK)
     throwSQLDBCError(stmt->error());
   
   SQLDBC_Length ind;
   rc = result->getObject(1, SQLDBC_HOSTTYPE_ASCII, pStr, &ind, sizeof(*p1), false);
   if (rc != SQLDBC_OK)
     throwSQLDBCError(stmt->error());
   
   return S_OK;
 }

 STDMETHODIMP CSQLDBCPROC::SQLDBCGetKernelVersion(OmsTypeInt4* p1)
 {
   SQL_SQLDBCConnection con;
   if (con.error()) {
     SQL_SQLDBCErrorHndl err = con.error();
     throw DbpError (err.getErrorCode(), err.getErrorText());
   }
   else
     *p1 = con.getKernelVersion();
   return S_OK;
 }
