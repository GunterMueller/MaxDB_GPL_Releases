/*!
  @file           HelloWorld.cpp
  @author         BurkhardD
  @ingroup        Demo
  @brief          Simple Hello World application

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


/*
 * First you have to include SQLDBC.h
 */
#include "SQLDBC.h"
#include <stdio.h>

typedef struct ConnectArgsT {
    char * username;
    char * password;
    char * dbname;
    char * host;
} ConnectArgsT;

static void parseArgs (ConnectArgsT * connectArgs, int argc, char **argv);

using namespace SQLDBC;

/*
 * Let start your program with a main function
 */
int main(int argc, char *argv[])
{
   ConnectArgsT connectArgs;
   parseArgs (&connectArgs, argc, argv);
  
   char errorText[200];
   /*
    * Every application has to initialize the SQLDBC library by getting a 
    * reference to the ClientRuntime and calling the SQLDBC_Environment constructor.
    */
   SQLDBC_IRuntime *runtime;
   runtime = SQLDBC::GetClientRuntime(errorText, sizeof(errorText));
   if (!runtime) {
     fprintf(stderr, "Getting instance of the ClientRuntime failed %s", errorText);
     return (1);
   }
   SQLDBC_Environment env(runtime);
   /*
    * Create a new connection object and open a session to the database.
    */
   SQLDBC_Connection *conn = env.createConnection();
   SQLDBC_Retcode rc;
   rc = conn->connect(connectArgs.host, connectArgs.dbname, 
                      connectArgs.username, connectArgs.password);
   if(SQLDBC_OK != rc) {
     fprintf(stderr, "Connecting to the database failed %s", conn->error().getErrorText());
     return (1);
   }
   printf("Sucessfull connected to %s as user %s\n", 
          connectArgs.dbname, connectArgs.username);
   /*
    * Create a new statment object and execute it.
    */
   SQLDBC_Statement *stmt = conn->createStatement();
   rc = stmt->execute("SELECT 'Hello world' from DUAL");
   if(SQLDBC_OK != rc) {
     fprintf(stderr, "Execution failed %s", stmt->error().getErrorText());
     return (1);
   }
   /*
    * Check if the SQL command return a resultset and get a result set object.
    */  
   SQLDBC_ResultSet *result;
   result = stmt->getResultSet();
   if(!result) {
     fprintf(stderr, "SQL command doesn't return a result set %s", stmt->error().getErrorText());
     return (1);
   }
   /*
    * Position the curors within the resultset by doing a fetch next call.
    */  
   rc = result->next();
   if(SQLDBC_OK != rc) {
     fprintf(stderr, "Error fetching data %s", stmt->error().getErrorText());
     return (1);
   }
   char szString[30];
   SQLDBC_Length ind;
   /*
    * Get a string value from the column.
    */
   rc = result->getObject(1, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
   if(SQLDBC_OK != rc) {
     fprintf(stderr, "Error getObject %s", stmt->error().getErrorText());
     return (1);
   }
   printf("%s\n", szString);
   /*
    * Finish your program with a returncode.
    */
   return 0;
}

static void parseArgs (ConnectArgsT * connectArgs, int argc, char **argv)
{
    /*
     * setting defaults for demo database
     */
    connectArgs->username = (char*)"TEST";
    connectArgs->password = (char*)"TEST";
    connectArgs->dbname = (char*)"TST";
    connectArgs->host = (char*)"localhost";
    /*
     * use values from command line
     */
    if (argc > 4) {
        connectArgs->host = argv [4];
    }
    if (argc > 3) {
        connectArgs->dbname = argv [3];
    }
    if (argc > 2) {
        connectArgs->password = argv [2];
    }
    if (argc > 1) {
        connectArgs->username = argv [1];
    }
}

