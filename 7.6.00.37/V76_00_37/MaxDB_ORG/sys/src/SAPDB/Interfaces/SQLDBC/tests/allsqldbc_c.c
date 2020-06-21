/*

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

*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SAPDB/Interfaces/SQLDBC/SQLDBC-sqldbc_c.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_Types.h"

#define EXIT_ON_ERROR if (tf.m_success != SQLDBC_TRUE) {printf("\nTest unsuccessfully\n"); return(-1);}

typedef struct testframe {
    char m_name[1024];
    char m_fullname[1024];

    SQLDBC_Bool m_good;
    FILE    *m_stream;
    char    *m_host;
    char    *m_database;
    char    *m_username;
    char    *m_password;
    SQLDBC_ConnectProperties *m_properties;
   
    SQLDBC_Environment              *m_environment;
    SQLDBC_Connection               *m_connection;
    SQLDBC_IRuntime                 *m_runtime;
    
    SQLDBC_Int4 m_lasterrorcode;
    char        m_lasterrormessage[512];
    SQLDBC_Bool m_silent;
    SQLDBC_Bool m_success;

} testframe;

static testframe tf;

/*----------------------------------------------------------------------*/
void
myusage()
{
    fprintf(stderr, "myusage: %s [options]\n", __FILE__);
    fprintf(stderr, "Where options are: \n");
    fprintf(stderr, "   -n <node>            the hostname of the database server\n");
    fprintf(stderr, "   -d <dbname>          the name of the database\n");
    fprintf(stderr, "   -u <user>,<password> username and password\n");
    fprintf(stderr, "   -p <key>=<value>     connect property key is set to value\n");
    fprintf(stderr, "   -t <filename>        stream trace file\n");
    fprintf(stderr, "   -s                   silent\n");
    exit(1);
}

static char*
strupper(char *str)
{
    char *p=str;
    while(*p) {
        *p=toupper(*p);
        ++p;
    }
    return str;
}

void 
loginfo(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if(! tf.m_silent) {
      char prefix[1024];
      char buffer[1024];
      strcpy(prefix, "INFO:");
      strcat(prefix, tf.m_fullname);
      strcat(prefix, ":");
      vsprintf(buffer, fmt, ap);
      fputs(prefix, stdout);
      fputs(buffer, stdout);
      if(buffer[strlen(buffer)-1]!='\n') {
          fputs("\n", stdout);
      }
    }
    va_end(ap);
}

/*----------------------------------------------------------------------*/
void 
logerror(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if(! tf.m_silent) {
      char prefix[1024];
      char buffer[1024];
      strcpy(prefix, "ERROR:");
      strcat(prefix, tf.m_fullname);
      strcat(prefix, ":");
      vsprintf(buffer, fmt, ap);
      fputs(prefix, stderr);
      fputs(buffer, stderr);
      if(buffer[strlen(buffer)-1]!='\n') {
          fputs("\n", stderr);
      }
    }
    va_end(ap);
    tf.m_success=SQLDBC_FALSE;
}

/*----------------------------------------------------------------------*/
void 
logprot(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if(! tf.m_silent) {
      char prefix[1024];
      char buffer[1024];
      strcpy(prefix, "PTOT:");
      strcat(prefix, tf.m_fullname);
      strcat(prefix, ":");
      vsprintf(buffer, fmt, ap);
    }
    va_end(ap);
}
/*-------------------------------------------------------------------------------------------*/
void myconnect()
{
    loginfo("----------------------------------------------------------------------\n");
    loginfo("Connect to host \'%s\', database \'%s\'.\n", tf.m_host, tf.m_database);
    loginfo("        as user %s with password %s\n", tf.m_username, tf.m_password);
    loginfo("----------------------------------------------------------------------\n");
    /*loginfo("Making new environment.\n");*/
    if (!tf.m_environment)
      tf.m_environment=SQLDBC_Environment_new_SQLDBC_Environment(tf.m_runtime);
    
    /*loginfo("New environment created.\n");*/
    tf.m_connection= SQLDBC_Environment_createConnection(tf.m_environment);
    if(!tf.m_connection) {
        logerror("Setup - SQLDBC_Environment_createConnection()");
        tf.m_good = SQLDBC_FALSE;
        return;
    }
    tf.m_good = 
        SQLDBC_Connection_connectASCII(tf.m_connection, 
                                       tf.m_host, 
                                       tf.m_database, 
                                       tf.m_username, 
                                       tf.m_password, 
                                       tf.m_properties)==SQLDBC_OK;

    if(!tf.m_good) {
        SQLDBC_ErrorHndl* err = SQLDBC_Connection_getError(tf.m_connection); 
        logerror("Error: %d sql state %5s, %s", 
                 SQLDBC_ErrorHndl_getErrorCode(err), 
                 SQLDBC_ErrorHndl_getSQLState(err),
                 SQLDBC_ErrorHndl_getErrorText(err));
    }
}
/*----------------------------------------------------------------------*/
SQLDBC_Bool init(int argc, char **argv)
{
    int i=1;
    strcpy(tf.m_name, argv[0]);
    strcpy(tf.m_fullname, argv[0]);
    
    tf.m_host=tf.m_database=tf.m_username=tf.m_password=0;
    tf.m_properties=SQLDBC_ConnectProperties_new_SQLDBC_ConnectProperties();
    tf.m_environment=0;
    tf.m_success=SQLDBC_TRUE;
    while(i<argc) {
        if(strcmp("-s", argv[i])==0) {
            tf.m_silent = SQLDBC_TRUE;
        }
        if(strcmp("-n", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -n requires an argument.\n"); exit(1); }
            tf.m_host=strdup(argv[i]);
            ++i;
            continue;
        } 
        if(strcmp("-d", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -d requires an argument.\n"); exit(1); }
            tf.m_database=strupper(strdup(argv[i]));
            ++i;
            continue;
        } 
        if(strcmp("-u", argv[i])==0) {
            char *comma;
            ++i;
            if(i==argc) { logerror("Option -u requires an argument.\n"); exit(1); }
            comma=strchr(argv[i], ',');
            if(comma==0) { logerror("Option -u requires username,password.\n"); exit(1); }
            tf.m_username=(char*)malloc(comma - argv[i] +1);
            memcpy(tf.m_username, argv[i], comma - argv[i]);
            tf.m_username[comma - argv[i]]='\0';
            tf.m_password=(char*)malloc(strlen(comma));
            strcpy(tf.m_password, comma+1);
            strupper(tf.m_password);
            strupper(tf.m_username);
            ++i;
            continue;
        } 
        if(strcmp("-t", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -t requires an argument.\n"); exit(1); }
            tf.m_stream=fopen(argv[i], "w");
            if(tf.m_stream==0) { logerror("Could not open trace file %s.\n", argv[i]); exit(1); }
            ++i;
            continue;
        }
        if(strcmp("-p", argv[i])==0) {
            char *s,*eq;
            ++i;
            if(i==argc) { logerror("Option -p requires an argument.\n"); exit(1); }
            s=argv[i];
            eq=strchr(s, '=');
            if(eq==0) {
                logerror("Syntax error in property argument %s.\n", s);
            }
            *eq=0;
            SQLDBC_ConnectProperties_setProperty(tf.m_properties, s, eq+1);  
            *eq='=';
            ++i;
            continue;
        }
        ++i;
    }
    if(tf.m_host==0) {
        tf.m_host=strdup("");
    }
    if(!(tf.m_username && tf.m_password && tf.m_database)) {
        myusage();
    }
    /* Initialize the runtime, if the user didn't specify a custom one.*/
    if(tf.m_runtime == 0) {
        char errorText[512];
        tf.m_runtime = ClientRuntime_GetClientRuntime(errorText, sizeof(errorText));
        if(!tf.m_runtime) {
            logerror("Error: %s", errorText);
            return SQLDBC_FALSE;
        }
    }
        
    myconnect();
    return tf.m_good;
}

void check_error(SQLDBC_Retcode ret, SQLDBC_ErrorHndl* err){
    if(   ret == SQLDBC_INVALID_OBJECT
       || ret == SQLDBC_NOT_OK  ) {
        tf.m_success = SQLDBC_FALSE;    
        logerror("Error: %d sql state %5s, %s", 
                 SQLDBC_ErrorHndl_getErrorCode(err), 
                 SQLDBC_ErrorHndl_getSQLState(err),
                 SQLDBC_ErrorHndl_getErrorText(err));
    }
}
    
void expected_check_error(SQLDBC_Int4 errorcode, SQLDBC_ErrorHndl* err){
  SQLDBC_Int4 ret = SQLDBC_ErrorHndl_getErrorCode(err);
  if (errorcode != ret) {
    logerror("Unexpected Error: %d sql state %5s, %s, expected was %d.", 
             SQLDBC_ErrorHndl_getErrorCode(err), 
             SQLDBC_ErrorHndl_getSQLState(err),
             SQLDBC_ErrorHndl_getErrorText(err),
             errorcode);
  }
}
    
int main(int argc, char** argv){
  SQLDBC_Retcode ret;
  SQLDBC_Statement *stmt;
  SQLDBC_ResultSet *rs;
  SQLDBC_RowSet *rset; 
  char result[512];
  init(argc,argv);
  EXIT_ON_ERROR
  loginfo ("Connect successfully\n");
  
  stmt = SQLDBC_Connection_createStatement(tf.m_connection);
  ret  = SQLDBC_Statement_executeASCII(stmt, "SELECT 'Homer is cool' as Simpson from dual");
  check_error(ret, SQLDBC_Statement_getError(stmt));
  EXIT_ON_ERROR
  loginfo ("Select successfully\n");
    
  rs = SQLDBC_Statement_getResultSet(stmt);
  if (! rs)
    check_error(SQLDBC_INVALID_OBJECT, SQLDBC_Statement_getError(stmt));
  EXIT_ON_ERROR
  
  ret = SQLDBC_ResultSet_bindColumn(rs, 1, SQLDBC_HOSTTYPE_ASCII, &result[0], 0, sizeof(result), SQLDBC_TRUE);
  check_error(ret, SQLDBC_ResultSet_getError(rs));
  EXIT_ON_ERROR

  ret = SQLDBC_ResultSet_next(rs);
  check_error(ret, SQLDBC_ResultSet_getError(rs));
  EXIT_ON_ERROR
                             
  rset = SQLDBC_ResultSet_getRowSet (rs);
  if (!rset)
    check_error(SQLDBC_INVALID_OBJECT, SQLDBC_ResultSet_getError(rs));
  EXIT_ON_ERROR

  ret = SQLDBC_RowSet_fetch(rset);
  check_error(ret, SQLDBC_RowSet_getError(rset));
  EXIT_ON_ERROR

  loginfo("Result %s\n", result);
  /*  connection()->releaseStatement(rset);*/
  {
    SQLDBC_Statement *stmt;
    SQLDBC_ResultSet *rs;
    SQLDBC_UpdatableRowSet *urs;
    SQLDBC_SQLWarning *wrn;
    SQLDBC_ErrorHndl *err;
    SQLDBC_Int4 pos = 1;
    SQLDBC_Int4 *rowstat;
    stmt = SQLDBC_Connection_createStatement(tf.m_connection);
    ret  = SQLDBC_Statement_executeASCII(stmt, "DROP TABLE UPDATABLE");
    ret  = SQLDBC_Statement_executeASCII(stmt, "CREATE TABLE UPDATABLE ( I INTEGER )");
    ret  = SQLDBC_Statement_executeASCII(stmt, "INSERT INTO UPDATABLE values ( 1 )");
    ret  = SQLDBC_Statement_executeASCII(stmt, "COMMIT WORK");
    SQLDBC_Statement_setResultSetConcurrencyType(stmt, SQLDBC_Statement_ConcurrencyType_CONCUR_UPDATABLE);  

    SQLDBC_Statement_setResultSetType(stmt, SQLDBC_Statement_ResultSetType_SCROLL_SENSITIVE);
    ret  = SQLDBC_Statement_executeASCII(stmt, "SELECT * from UPDATABLE");
    check_error(ret, SQLDBC_Statement_getError(stmt));
    EXIT_ON_ERROR
      loginfo ("Select successfully\n");
    
    rs = SQLDBC_Statement_getResultSet(stmt);
    if (! rs)
      check_error(SQLDBC_INVALID_OBJECT, SQLDBC_Statement_getError(stmt));
    EXIT_ON_ERROR
  
    urs = SQLDBC_ResultSet_getUpdatableRowSet(rs);
    if (! urs)
      check_error(SQLDBC_INVALID_OBJECT, SQLDBC_Statement_getError(stmt));
    EXIT_ON_ERROR;
    
    loginfo ("SQLDBC_ResultSet_getUpdatableRowSet successfully\n");
    SQLDBC_UpdatableRowSet_clearError(urs);  
    err = SQLDBC_UpdatableRowSet_getError(urs);
    SQLDBC_UpdatableRowSet_clearWarnings(urs);  
    wrn = SQLDBC_UpdatableRowSet_getWarning(urs);
    ret = SQLDBC_UpdatableRowSet_setPos(urs, pos);
    expected_check_error(-10501, SQLDBC_UpdatableRowSet_getError(urs));
    ret = SQLDBC_UpdatableRowSet_fetch(urs);
    expected_check_error(-10501, SQLDBC_UpdatableRowSet_getError(urs));
    ret = SQLDBC_UpdatableRowSet_getRowsAffected(urs);
    expected_check_error(0, SQLDBC_UpdatableRowSet_getError(urs));
    rowstat = SQLDBC_UpdatableRowSet_getRowStatus(urs);
    expected_check_error(0, SQLDBC_UpdatableRowSet_getError(urs));
    ret = SQLDBC_UpdatableRowSet_insertAllRows(urs);
    expected_check_error(-10510, SQLDBC_UpdatableRowSet_getError(urs));
    ret = SQLDBC_UpdatableRowSet_insertOneRow(urs);
    expected_check_error(-10510, SQLDBC_UpdatableRowSet_getError(urs));
    ret = SQLDBC_UpdatableRowSet_updateRow(urs, 1);
    expected_check_error(-10510, SQLDBC_UpdatableRowSet_getError(urs));
    ret = SQLDBC_UpdatableRowSet_deleteRow(urs, 1);
    expected_check_error(0, SQLDBC_UpdatableRowSet_getError(urs));
    
    SQLDBC_Connection_commit(tf.m_connection);
 }
  return(0);
}
