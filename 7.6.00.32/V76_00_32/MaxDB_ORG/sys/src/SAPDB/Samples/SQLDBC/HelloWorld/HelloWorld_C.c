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
#include "SQLDBC_C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ConnectArgsT {
    char * username;
    char * password;
    char * dbname;
    char * host;
} ConnectArgsT;

static void parseArgs (ConnectArgsT * connectArgs, int argc, char **argv);
static void usage();

/*
 * Let start your program with a main function
 */
int main(int argc, char *argv[])
{
   SQLDBC_IRuntime *runtime;
   SQLDBC_Environment *environment;
   SQLDBC_Connection *conn;
   SQLDBC_ConnectProperties *conn_prop;
   SQLDBC_Statement *stmt;
   SQLDBC_ResultSet *result;
   SQLDBC_Retcode rc;
   char szString[30];
   SQLDBC_Length ind;
   char errorText[200];

   ConnectArgsT connectArgs;
   parseArgs (&connectArgs, argc, argv);
  
   /*
    * Every application has to initialize the SQLDBC library by getting a 
    * reference to the ClientRuntime and calling the SQLDBC_Environment constructor.
    */
   runtime = ClientRuntime_GetClientRuntime(errorText, sizeof(errorText));
   if (!runtime) {
     fprintf(stderr, "Getting instance of the ClientRuntime failed %s", errorText);
     return (1);
   }
   environment = SQLDBC_Environment_new_SQLDBC_Environment(runtime);
   /*
    * Create a new connection object and open a session to the database.
    */
   conn = SQLDBC_Environment_createConnection(environment);
   conn_prop = SQLDBC_ConnectProperties_new_SQLDBC_ConnectProperties();

   rc = SQLDBC_Connection_connectASCII(conn, connectArgs.host, connectArgs.dbname, 
                                       connectArgs.username, connectArgs.password, conn_prop);
   if(SQLDBC_OK != rc) {
     SQLDBC_ErrorHndl *herror = SQLDBC_Connection_getError(conn);
     fprintf(stderr, "Connecting to the database failed %s", SQLDBC_ErrorHndl_getErrorText(herror));
     return (1);
   }
   printf("Sucessfull connected to %s as user %s\n", 
          connectArgs.dbname, connectArgs.username);
   /*
    * Create a new statment object and execute it.
    */
   stmt = SQLDBC_Connection_createStatement(conn);
   rc = SQLDBC_Statement_executeASCII(stmt, "SELECT 'Hello world' from DUAL");
   if(SQLDBC_OK != rc) {
     SQLDBC_ErrorHndl *herror = SQLDBC_Statement_getError(stmt);
     fprintf(stderr, "Execution failed %s", SQLDBC_ErrorHndl_getErrorText(herror));
     return (1);
   }
   /*
    * Check if the SQL command return a resultset and get a result set object.
    */  
   result = SQLDBC_Statement_getResultSet(stmt);
   if(!result) {
     SQLDBC_ErrorHndl *herror = SQLDBC_Statement_getError(stmt);
     fprintf(stderr, "SQL command doesn't return a result set %s", SQLDBC_ErrorHndl_getErrorText(herror));
     return (1);
   }
   /*
    * Position the curors within the resultset by doing a fetch next call.
    */  
   rc = SQLDBC_ResultSet_next(result);
   if(SQLDBC_OK != rc) {
     SQLDBC_ErrorHndl *herror = SQLDBC_ResultSet_getError(result);
     fprintf(stderr, "Error fetching data %s", SQLDBC_ErrorHndl_getErrorText(herror));
     return (1);
   }
   /*
    * Get a string value from the column.
    */
   rc = SQLDBC_ResultSet_getObject(result, 1, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString), SQLDBC_TRUE);
   if(SQLDBC_OK != rc) {
     SQLDBC_ErrorHndl *herror = SQLDBC_ResultSet_getError(result);
     fprintf(stderr, "Error getObject %s", SQLDBC_ErrorHndl_getErrorText(herror));
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
    connectArgs->username = (char*)"MONA";
    connectArgs->password = (char*)"RED";
    connectArgs->dbname = (char*)"HOTELDB";
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
      if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-?") || !strcmp(argv[1], "/?")) {
        usage();
      }
        connectArgs->username = argv[1];
    }
}

static void usage()
{
  fprintf(stderr, "Usage: HelloWorld [<user name> [<password> [<database name> [<server name>]]]]\n");
  fprintf(stderr, "\n   Arguments:\n\n");
  fprintf(stderr, "      <user name>      The name of the database user. Default 'MONA'\n");
  fprintf(stderr, "      <password>       The password of the database user. Default 'RED'\n");
  fprintf(stderr, "      <database name>  The database name to connect to. Default 'HOTELDB'\n");
  fprintf(stderr, "      <server name>    The hostname or IP adress of the datase server. Default 'localhost'\n");
  exit(1);
}

