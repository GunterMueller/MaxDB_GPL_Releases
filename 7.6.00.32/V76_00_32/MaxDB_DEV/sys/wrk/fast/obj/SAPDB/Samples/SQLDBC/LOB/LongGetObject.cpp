/*!
  @file           LongGetObject.cpp
  @author         BurkhardD
  @brief          Example program for reading long data

                  This example program will queries the HOTEL table and 
                  fetches through the 'INFO' column. 
                  The 'INFO' column is from type LONG CHAR

                  Prerequisite:
                  The SQL Tutorial demo database. 

                  Build:
                  [n]make SQLDBCSDK=<sqldbc_sdk_path> <os>

                  Usage: LongGetObject [-h] | [\<connect_options\>]
                  connect_options ::
                  [ \<username\> [ \<password\> [ \<database_name\> [ \<database_server\> ]]]]

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2004 SAP AG

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


#include "SQLDBC.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
using namespace SQLDBC;

/**
 * @brief Default argument parser of the SQLDBC demo.
 *
 * @param argc [in] The number of arguments passed.
 * @param argv [in] An array of comandline arguments.
 */
static void parseArgs(int argc, char **argv);

/**
 * @brief Default connect function of the SQLDBC demo.
 *
 * @return A pointer to the a SQLDBC_Connection object on success or exits the 
 * process on failure.
 */
static SQLDBC_Connection *connectDB();

/**
 * @brief Display the program usage and exit..
 */
static void usage();

/**
 * @brief Exists the application on any error was set.
 */
void exitOnError(SQLDBC_ErrorHndl &err);

/**
 * @brief Main function.
 * 
 * @param argc [in] The number of arguments passed.
 * @param argv [in] An array of comandline arguments. 
 * The arguments, if given, should passed in the following order:
 *         <ol>
 *           <li>username</li>
 *           <li>password</li>
 *           <li>database name</li>
 *           <li>database server</li>
 *         </ol>
 */
int main(int argc, char *argv[])
{
   parseArgs(argc, argv);

   char szHotelName[16];
   SQLDBC_Retcode rc;

   SQLDBC_Connection *conn = connectDB();

   /*
    * Create a new statment object and execute it.
    */
   SQLDBC_Statement *stmt = conn->createStatement();
   
   /*
    * Asign a query use table HOTEL in schema HOTEL and ignore all rows
    * containing NULL values in the Info column.
    */
   char *command = "select NAME, INFO from HOTEL.HOTEL where INFO is not null";

   /*
    * Queries the hotel table.
    */
   stmt->execute((const char*)command);

   exitOnError(stmt->error());

   /*
    * Get the resultset.
    */  
   SQLDBC_ResultSet *result;
   result = stmt->getResultSet();
   exitOnError(stmt->error());

   /*
    * Bound the first column of the HOTEL table (containing the name of the hotel)
    */  
   result->bindColumn(1, SQLDBC_HOSTTYPE_ASCII, szHotelName,
                      NULL, sizeof(szHotelName));

   /*
    * Position the cursor and fetch one row.
    */  
   result->next();

   exitOnError(result->error());

   result->getRowSet()->fetch();

   exitOnError(result->error());

   /*
    * Retrieve a string value from the column.
    * In this example we use an big buffer to ensure that the whole columns 
    * is read.
    */

   char szBigString[32000];
   SQLDBC_Length ind;

   rc = result->getObject(2, SQLDBC_HOSTTYPE_ASCII, szBigString, &ind, sizeof(szBigString));
   exitOnError(result->error());
   printf("Info of hotel '%s' length = %d\n", szHotelName, ind);
   printf("%s\n", szBigString);

   /*
    * Fetch the next row of the resultset.
    */  
   result->next();

   exitOnError(result->error());

   result->getRowSet()->fetch();

   exitOnError(result->error());

   /*
    * Retrieve a string value from the column.
    * In this example the place to store the data is small.
    * We need a loop to get all contents of the LONG column.
    */

   char szSmallString[80];

   do {
     rc = result->getObject(2, SQLDBC_HOSTTYPE_ASCII, szSmallString, &ind, sizeof(szSmallString));
     exitOnError(result->error());
     printf("Info of hotel '%s' length = %d\n", szHotelName, ind);
     printf("%s\n", szSmallString);     
   } while(rc == SQLDBC_DATA_TRUNC);

   conn->releaseStatement(stmt);

   conn->disconnect();
   
   return 0;
}

/**
 * @brief converts a string to upper case.
 */
static char* strupper(char *str)
{
    char *p=str;
    while(*p) {
        *p=toupper(*p);
        ++p;
    }
    return str;
}

/**
 * @brief Global connect argument structure.
 */
typedef struct ConnectArgsT {
  char * username; //!< Database username 
  char * password; //!< Database users password
  char * dbname;   //!< Name of the database instance
  char * host;     //!< Servername of the database
} ConnectArgsT;

ConnectArgsT connectArgs; //!< Global connection arguments

static char *argv0;       //!< Reference to argument 0 of the aplication. Holds the name of the aplication

/**
 * Compares to strings case independent.
 */   
#ifdef WIN32
#define STRICMP _stricmp
#else
#define STRICMP strcasecmp
#endif

SQLDBC_Connection *connectDB()
{
  char errorText[200];
  SQLDBC_Retcode rc;
  /*
   * Every application has to initialize the SQLDBC library by getting a 
   * reference to the ClientRuntime and calling the SQLDBC_Environment constructor.
   */
  SQLDBC_IRuntime *runtime;
  runtime = SQLDBC::GetClientRuntime(errorText, sizeof(errorText));
  if (!runtime) {
    fprintf(stderr, "Getting instance of the ClientRuntime failed %s\n", errorText);
    usage();
  }
  SQLDBC_Environment *env = new SQLDBC_Environment(runtime);
  /*
   * Create a new connection object and open a session to the database.
   */
  SQLDBC_Connection *conn = env->createConnection();
  printf("Connecting to '%s' on '%s' as user '%s'\n", 
         connectArgs.dbname, connectArgs.host, connectArgs.username);

  rc = conn->connect(connectArgs.host, connectArgs.dbname, 
                     connectArgs.username, connectArgs.password);
  if(SQLDBC_OK != rc) {
    fprintf(stderr, "Can't connect to '%s'.\nERROR: %d:'%s'\n", 
            connectArgs.dbname, conn->error().getErrorCode(), conn->error().getErrorText());
    exit(1);
  }
  return conn;
}

static void parseArgs (int argc, char **argv)
{
  argv0 = strdup(argv[0]);

  /*
   * setting defaults for demo database
   */
  connectArgs.username = strdup("MONA");
  connectArgs.password = strdup("RED");
  connectArgs.dbname = (char*)"HOTELDB";
  connectArgs.host = (char*)"localhost";
  /*
   * use values from command line
   */
  if (argc > 4) {
    connectArgs.host = argv [4];
  }
  if (argc > 3) {
    connectArgs.dbname = argv [3];
  }
  if (argc > 2) {
    connectArgs.password = argv [2];
  }
  if (argc > 1) {
    if (!STRICMP(argv [1], "-h"))
      usage();
    else {
      connectArgs.username = argv [1];
    }
  }
  strupper(connectArgs.username);
  strupper(connectArgs.password);
}

void exitOnError(SQLDBC_ErrorHndl &err) 
{
  if(err) {
    fprintf(stderr, "Execution stopped %d:'%s'", err.getErrorCode(), err.getErrorText());
    exit(1);
  }  
}
  
static void usage() 
{
  char *s = strrchr(argv0, '/');
  if (!s) 
    s = strrchr(argv0, '\\');
  if (s) 
    *s = '\0';
  printf("Syntax: %s [-h] | [<connect_options>]\n"
         "\tconnect_options :: \n"
         "\t\t[ <username> \n"
         "\t\t[ <password>\n"
         "\t\t[ <database_name>\n"
         "\t\t[ <database_server> ]]]]\n"
         "\tCalling %s without any arguments will use user '%s','%s'\n"
         "\t\ton database '%s' at server '%s'\n",
         argv0, argv0, 
         connectArgs.username, connectArgs.password, 
         connectArgs.dbname, connectArgs.host);
  exit(1);
}


