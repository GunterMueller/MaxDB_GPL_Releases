/*!
  @file           helloWorld.cpp
  @author         D041388
  @ingroup        Tests
  @brief          Hello World Example from SQLDBC API Description
  @keywords       hello world

  @see            @link http://pwww.bea.sap.corp:1111/Interfaces/SAPDB_8.0/SQLDBC_apidoc/helloworld.html Hello world example @endlink 

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"

class helloWorld : public SQLDBC_TestCase
{

public:
  helloWorld ()
    : SQLDBC_TestCase ("helloWorld")
  {}

	virtual SQLDBC_Bool run ()
	{
		
		//Every application has to initialize the SQLDBC library by getting a 
		//reference to the ClientRuntime and calling the SQLDBC_Environment constructor.
		char errorText[200];
		SQLDBC_IRuntime *runtime;
		runtime = SQLDBC::GetClientRuntime((char *)errorText, sizeof(errorText));
		if (!runtime) {
			logerror("Getting instance of the ClientRuntime failed %s\n", errorText);
			return (1);
		}

		SQLDBC_Environment env(runtime);
	
		//Create a new connection object and open a session to the database. 
		SQLDBC_Connection *conn = env.createConnection();
		SQLDBC_Retcode rc;
		rc = conn->connect(hostname(), database(), username(), password());
		if(SQLDBC_OK != rc) {
			logerror("Connecting to the database failed %s\n", conn->error().getErrorText());
			return (1);
		}

		//Create a new statement object and execute it.
		SQLDBC_Statement *stmt = conn->createStatement();
		rc = stmt->execute("SELECT 'Hello world' from DUAL");
		if(SQLDBC_OK != rc) {
			logerror("Execution failed %s\n", stmt->error().getErrorText());
			return (1);
		}

		//Check if the SQL command return a resultset and get a result set object. 
		SQLDBC_ResultSet *result;
		result = stmt->getResultSet();
		if(!result) {
			logerror("SQL command doesn't return a result set %s\n", stmt->error().getErrorText());
			return (1);
		}
		
		//Position the curors within the resultset by doing a fetch next call. 
		rc = result->next();
		if(SQLDBC_OK != rc) {
			logerror("Error fetching data %s\n", stmt->error().getErrorText());
			return (1);
		}

		//Get a string value from the column. 
		char szString[30];
		SQLDBC_Length ind;
		rc = result->getObject(1, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
		if(SQLDBC_OK != rc) {
			logerror("Error getObject %s\n", stmt->error().getErrorText());
			return (1);
		}
                printf("%s\n", szString);

		env.releaseConnection(conn);

		//finish your program with a returncode 
		return m_success;
	}
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new helloWorld());
  if (testframe.run ()) 
    return 0;

  return 1;
}


