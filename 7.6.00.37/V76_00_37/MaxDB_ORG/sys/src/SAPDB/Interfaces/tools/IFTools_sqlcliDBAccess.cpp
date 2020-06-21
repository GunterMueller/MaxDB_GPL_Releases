/*!**************************************************************************

  module      : IFTools_sqlcliDBAccess.cpp

  -------------------------------------------------------------------------

  responsible : D031096

  special area:
  description : Interface Runtime 8.0

  description:  Provides an environment for command line tool sqlcli

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

#include "SAPDB/Interfaces/tools/IFTools_sqlcliDBAccess.h"
#include <stdarg.h>

#ifndef _WIN32
#  if !(defined(SVR4) || defined(SOLARIS))
#    if defined(LINUX) || defined(AIX)
#      include <time.h>
#    endif
#    include <sys/time.h>
#  else
#    ifdef _POSIX_SOURCE
#      undef _POSIX_SOURCE
#      include <sys/time.h>
#      define _POSIX_SOURCE
#    else
#      include <sys/time.h>
#    endif
#  endif
#endif

#ifdef _WIN32
static IFR_UInt8 time_performancefreq;
#endif

//----------------------------------------------------------------------
static SQLDBC_UInt8 currentMicroSeconds()
{
#ifdef _WIN32
    if(!time_performancefreq) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&time_performancefreq);
        if(!time_performancefreq) {
            time_performancefreq=1;
        }
    }
    SQLDBC_UInt8 counter=0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    // Conversion from unsigned 64 bit integer isn't possible
    return (SQLDBC_UInt8) ( ( (SQLDBC_UInt8)counter  * (double)1000000.0 ) / (SQLDBC_UInt8)time_performancefreq );
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm tmval; 
    localtime_r(&tv.tv_sec,
                &tmval);
    
    SQLDBC_UInt8 result = tmval.tm_sec + tmval.tm_min * 60 + tmval.tm_hour * 3600;
    result *= 1000000;
    return result + tv.tv_usec;
#endif
}

IFTools_sqlcliDBStmt::IFTools_sqlcliDBStmt (SQLDBC_Bool usePrepStmt, SQLDBC_Connection *conn)
{
    this->m_stmt=0;
    this->m_pstmt=0;
  setUsePreparedStmt(usePrepStmt, conn);
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBStmt::setUsePreparedStmt(SQLDBC_Bool usePrepStmt, SQLDBC_Connection *conn){
  if (! conn->isConnected()) return SQLDBC_FALSE;
  if (usePrepStmt){
	  if (this->m_stmt) conn->releaseStatement(this->m_stmt);
	  if (! this->m_pstmt) this->m_pstmt = conn->createPreparedStatement();  
  } else {
	  if (this->m_pstmt) conn->releaseStatement(this->m_pstmt);
	  if (! this->m_stmt) this->m_stmt = conn->createStatement();  
  }
  return SQLDBC_TRUE;
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBStmt::releaseStmt(SQLDBC_Connection *conn){
  if (! conn) return SQLDBC_FALSE;
  if (this->m_stmt) conn->releaseStatement(this->m_stmt);
  if (this->m_pstmt) conn->releaseStatement(this->m_pstmt);
  return SQLDBC_TRUE; 
}

//----------------------------------------------------------------------
SQLDBC_Retcode IFTools_sqlcliDBStmt::execute (const char * cmd){
  if (this->m_pstmt){
	  SQLDBC_Retcode erg = this->m_pstmt->prepare(cmd, SQLDBC_StringEncodingAscii);
	  if (erg != SQLDBC_OK) return erg;
	  return this->m_pstmt->execute();
  }else{
	  return this->m_stmt->execute(cmd, SQLDBC_StringEncodingAscii);
  }
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBStmt::hasOutputParameters(){
  if (this->m_stmt) return SQLDBC_FALSE;
  SQLDBC_ParameterMetaData *psmd = this->m_pstmt->getParameterMetaData();
  SQLDBC_Int2 paramCnt = psmd->getParameterCount();
  if (! paramCnt) return SQLDBC_FALSE;
  for (SQLDBC_Int2 i=1; i <= paramCnt; i++){
	  SQLDBC_ParameterMetaData::ParameterMode pm = psmd->getParameterMode(i); 
	  if ( pm == SQLDBC_ParameterMetaData::parameterModeOut 
		  || pm == SQLDBC_ParameterMetaData::parameterModeInOut) {
			  return SQLDBC_TRUE;
	   }
  }
  return SQLDBC_FALSE;
}

//----------------------------------------------------------------------
SQLDBC_ParameterMetaData* IFTools_sqlcliDBStmt::getParameterMetaData(){
  if (this->m_stmt) return 0;
  return this->m_pstmt->getParameterMetaData();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
IFTools_sqlcliDBAccess::IFTools_sqlcliDBAccess(IFTools_sqlcliProperties *sqlcliEnv, IFTools_sqlcliOutput *out):
 m_sqlcliProp(sqlcliEnv),
 m_out(out),
 m_connection(0),
 m_stmtHdl(0),
 m_metaData(0)
{
//	m_metaData = new IFTools_sqlcliMetaData(this);
}

//----------------------------------------------------------------------
IFTools_sqlcliDBAccess::~IFTools_sqlcliDBAccess(){
  this->releaseConnection();   
}
//----------------------------------------------------------------------
void IFTools_sqlcliDBAccess::printRowsAffected(SQLDBC_Int4 rcount, const char* action, SQLDBC_UInt8 time){
  double timeVal = (double)time/1000;
  char* unit = (char *)"msec";
  int   precision = 0;
  if (time > 1000000){
    timeVal = (double)time/1000000;
    unit = (char *)"sec";
    precision = 3;
  }
  if (rcount == 1)
    m_out->printComment("%d row %s (%.*f %s)\n", rcount, action, precision, timeVal, unit);
  else if (rcount >= 0)
    m_out->printComment("%d rows %s (%.*f %s)\n", rcount, action, precision, timeVal, unit);
  else if (rcount == -1)
    m_out->printComment("unknown number of rows %s (%.3f %s)\n",action, precision, timeVal, unit);
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::executeCMD (char *cmd){
  m_out->printCmdTrace("%s\n\n", cmd);
  if (! this->assertConnect()) return SQLDBC_FALSE;
    
  cmd = IFTools_sqlcliProperties::trimString(cmd);  
  if (cmd != 0 && strlen(cmd)){
    //return (SQLDBC_Bool) fprintf(stdout, "%s\n", cmd);
    return this->doExecuteSQLCmd(cmd);
  }  
  return SQLDBC_TRUE;
}
  
//----------------------------------------------------------------------

SQLDBC_Bool IFTools_sqlcliDBAccess::assertConnect(){
    if (this->m_connection)
      return SQLDBC_TRUE;
  
    SQLDBC_Environment* myEnv = m_sqlcliProp->getEnvironment();
    SQLDBC_Connection *connection = myEnv->createConnection();

    SQLDBC_Bool good = connection->connect(m_sqlcliProp->getdbhost(), 
                                        m_sqlcliProp->getdbname(),
                                        m_sqlcliProp->getusername(), 
                                        m_sqlcliProp->getpassword(), 
                                        m_sqlcliProp->getifrprops())==SQLDBC_OK;

    if(!good) {
        m_out->printError(connection->error());
        return SQLDBC_FALSE;        
    }
    m_sqlcliProp->setConnectState(SQLDBC_TRUE);
    if (m_sqlcliProp->getautocommit())
      connection->setAutoCommit(SQLDBC_TRUE);
    
    this->m_connection = connection;
	this->m_stmtHdl = new IFTools_sqlcliDBStmt (m_sqlcliProp->getUsePreparedStmt(), this->m_connection);

	if (this->supportsSchema()){
      m_metaData = new IFTools_sqlcliMetaDataWithSchemaSupport(this);
	} else {
      m_metaData = new IFTools_sqlcliMetaData(this);
	}

    if(! strlen(m_sqlcliProp->getdbname()) || ! strlen(m_sqlcliProp->getusername())){

      SQLDBC_Retcode erg = this->m_stmtHdl->execute ("select username,serverdb,servernode from users where user=username"); 
      if (erg != SQLDBC_OK){
            m_out->printError(this->m_stmtHdl->getStmt()->error());
            return SQLDBC_FALSE;        
      }
      
      SQLDBC_ResultSet * rs = this->m_stmtHdl->getResultSet();  
      if (rs != 0 && rs->next() == SQLDBC_OK){
        char tmpBuf[100]; SQLDBC_Length indicator;
        erg = rs->getRowSet()->getObject(1, SQLDBC_HOSTTYPE_ASCII, &tmpBuf[0],  &indicator, sizeof(tmpBuf));
        if (erg == SQLDBC_OK){
           char* user = IFTools_sqlcliProperties::trimString(&tmpBuf[0]);
           m_sqlcliProp->setuser(user, strlen(user));
        }
        erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_ASCII, &tmpBuf[0],  &indicator, sizeof(tmpBuf));
        if (erg == SQLDBC_OK){
          char* dbname = IFTools_sqlcliProperties::trimString(&tmpBuf[0]);
          m_sqlcliProp->setdbname(dbname, strlen(dbname));
        }
        erg = rs->getRowSet()->getObject(3, SQLDBC_HOSTTYPE_ASCII, &tmpBuf[0],  &indicator, sizeof(tmpBuf));
        if (erg == SQLDBC_OK) {
           char* dbhost = IFTools_sqlcliProperties::trimString(&tmpBuf[0]);
           m_sqlcliProp->setdbhost(dbhost, strlen(dbhost));
        }   
    }  

   }
    return SQLDBC_TRUE;        
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::doExecuteSQLCmd(char *cmd, char* searchSchema, char* searchTable, int useOpenPipe){
  SQLDBC_UInt8 duration, start = currentMicroSeconds();
  SQLDBC_Retcode erg = this->m_stmtHdl->execute (cmd); 
  duration = currentMicroSeconds() - start;
  
  if (! useOpenPipe) m_out->printComment("%s", m_sqlcliProp->getresultheader());
  if (erg != SQLDBC_OK){
        m_out->printError(this->m_stmtHdl->getStmt()->error());
        return SQLDBC_FALSE;        
  }
  
  SQLDBC_ResultSet * rs = this->m_stmtHdl->getStmt()->getResultSet ();  
  if (rs != 0){
    SQLDBC_Int4 rcount = 0;
    this->m_out->registerResultSet(rs->getResultSetMetaData());
    
    SQLDBC_Bool erger=SQLDBC_TRUE;
    if (rs->next() == SQLDBC_OK){
      //print ResultSet
      if (! useOpenPipe)  this->m_sqlcliProp->openResultOutput();
      erger = this->m_out->printColumnNames();
      if (erger==SQLDBC_TRUE){
        do{
            erger=this->m_out->printRow(rs);
            rcount++;
        } while (erger==SQLDBC_TRUE && rs->next() == SQLDBC_OK);
      }
      if (! useOpenPipe) {
        this->m_sqlcliProp->closeResultOutput();
        this->printRowsAffected(rcount, "selected", duration);
      }  
      rs->close();
    } else {
      if (searchSchema){
        m_out->printError(ERR_PATTERN_DOES_NOT_MATCH,searchSchema, searchTable);
      } else {
        this->m_out->printColumnNames();
        this->printRowsAffected(rcount, "selected", duration);
      }
    }
    if (! erger) return SQLDBC_FALSE;
  } else if (this->m_stmtHdl->hasOutputParameters()){
	  this->m_out->registerOutputValues(this->m_stmtHdl->getParameterMetaData());
      if (! this->m_out->printColumnNames()) return SQLDBC_FALSE;
	  if (! this->m_out->printOutputValues(this->m_stmtHdl->getPrepStmt())) return SQLDBC_FALSE;
      if (! useOpenPipe) this->printRowsAffected(this->m_stmtHdl->getStmt()->getRowsAffected(), "selected", duration);
  } else {
    //print rowcount
    if (! useOpenPipe) this->printRowsAffected(this->m_stmtHdl->getStmt()->getRowsAffected(), "affected", duration);
  }
  if (! useOpenPipe) m_out->printComment("%s", m_sqlcliProp->getresultfooter());
  return SQLDBC_TRUE; 
}  

//----------------------------------------------------------------------
void IFTools_sqlcliDBAccess::setSQLmode(){
    this->assertConnect();
    char* sqlmode = (char*) m_sqlcliProp->getifrprops().getProperty("SQLMODE");
    if (sqlmode){ 
      SQLDBC_SQLMode sqlmodeEnum = IFTools_sqlcliInput::getSQLModeFromString(sqlmode);           
      if (sqlmodeEnum != IFTools_sqlcliInput::cmdtype_unknown){ 
        this->m_connection->setSQLMode(sqlmodeEnum);
        m_out->printComment(COM_CHANGE_SQLMODE,sqlmode);
      } else {
        m_out->printError(ERR_INVALID_SQLMODE);
      }
    } else {
       m_out->printError(ERR_INVALID_SQLMODE);
    }
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::displayStatus(){
  if (! this->assertConnect()) return SQLDBC_FALSE;
  m_out->printComment(m_sqlcliProp->getresultheader());

  const char *cmd = "select description, value from dbparameters where Description = '_UNICODE'      \
                                                              or Description = 'KERNELVERSION' \
                                                              or Description = 'DEFAULT_CODE'  \
                                                              order by description ";

  SQLDBC_Length indicator=0;
  const char *sapdbcvers = m_sqlcliProp->getEnvironment()->getLibraryVersion();
  SQLDBC_Int4  kernelvers = m_connection->getKernelVersion();
  char kernelVersion[256], defaultCode[256], unicode[256];
  memset (&kernelVersion[0],0,sizeof(kernelVersion));
  memset (&unicode[0]      ,0,sizeof(unicode));
  strcpy (&defaultCode[0]  ,"unknown");
  
  m_out->printResult("host          : %s\n", (strlen(m_sqlcliProp->getdbhost())==0)?"localhost":m_sqlcliProp->getdbhost());
  m_out->printResult("database      : %s\n", m_sqlcliProp->getdbname());
  if (m_sqlcliProp->getxuserkey()) m_out->printResult("xuserkey      : %s\n", m_sqlcliProp->getxuserkey());
  m_out->printResult("user          : %s\n", m_sqlcliProp->getusername());

  SQLDBC_Retcode erg = this->m_stmtHdl->execute (cmd); 
  
  if (erg != SQLDBC_OK){
        m_out->printError(this->m_stmtHdl->getStmt()->error());
        return SQLDBC_FALSE;        
  }
  
  SQLDBC_ResultSet * rs = this->m_stmtHdl->getResultSet();  
  if (rs != 0 && rs->next() == SQLDBC_OK){
    erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_ASCII, &defaultCode[0],  &indicator, sizeof(defaultCode));
    if (rs->next() == SQLDBC_OK){
      erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_ASCII, &kernelVersion[0],  &indicator, sizeof(kernelVersion));
      if (rs->next() == SQLDBC_OK){
        erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_ASCII, &unicode[0],  &indicator, sizeof(unicode));
        m_out->printResult("kernel version: %.40s\n", &kernelVersion[0]);
        m_out->printResult("sapdbc version: %s\n", sapdbcvers);
        m_out->printResult("unicode       : %.20s\n", &unicode[0]);
        m_out->printResult("default code  : %.20s\n", &defaultCode[0]);
      }  
    }  
  } else {
    m_out->printResult("kernel version: %d.%02d.%02d\n", kernelvers/10000, (kernelvers%10000)/100, kernelvers%100);
    m_out->printResult("sapdbc version: %s\n", sapdbcvers);
    m_out->printResult("unicode       : %s\n", (m_connection->isUnicodeDatabase())?"YES":"NO" );
    m_out->printResult("default code  : %s\n", &defaultCode[0]);
  }
  m_out->printResult("sql mode      : %s\n", this->m_sqlcliProp->getifrprops().getProperty("SQLMODE"));
  m_out->printResult("autocommit    : %s\n", (this->m_sqlcliProp->getautocommit())?"ON":"OFF");
  m_out->printComment(m_sqlcliProp->getresultfooter());
  return SQLDBC_TRUE;
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::displayUsers(){
    if (! this->assertConnect()) return SQLDBC_FALSE;
	return this->m_metaData->displayUsers();
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::displaySchemas(){
    if (! this->assertConnect()) return SQLDBC_FALSE;
	return this->m_metaData->displaySchemas();
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::displayTablesViews(IFTools_sqlcliInput::sqlcliInput_cmdtype cmdtype){
    if (! this->assertConnect()) return SQLDBC_FALSE;
	return this->m_metaData->displayTablesViews(cmdtype);
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::displayColumns(){
   if (! this->assertConnect()) return SQLDBC_FALSE;
   return this->m_metaData->displayColumns();
} 
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::displayListDBProcs(){
   if (! this->assertConnect()) return SQLDBC_FALSE;
   return this->m_metaData->displayListDBProcs();
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::displayIndices(){
   if (! this->assertConnect()) return SQLDBC_FALSE;
   return this->m_metaData->displayIndices();
}

//----------------------------------------------------------------------
IFTools_sqlcliMetaData::IFTools_sqlcliMetaData   (IFTools_sqlcliDBAccess* dbaccess):
 m_dbaccess(dbaccess)
{}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliMetaData::displayUsers(){
  char * searchUser  = this->m_dbaccess->m_sqlcliProp->gettablepattern();
  SQLDBC_Length indicator=0;
  SQLDBC_Int4 userLen=36,userGroupLen=36;
  const char *cmdlen = "SELECT MAX(LENGTH(USERNAME)), MAX(LENGTH(GROUPNAME)) FROM USERS" ;
  char cmdPattern[]= "select SUBSTR(USERNAME, 1, %d) \"User name\", " 
              "SUBSTR(GROUPNAME, 1, %d) \"User group\", " 
              "USERMODE || decode(ConnectMODE, NULL, '', ', '||ConnectMODE)  \"Attributes\" "
              "from users where usermode <> 'COLDUSER' AND USERNAME like '%s' ORDER BY GROUPNAME, USERNAME\0";
  char cmd[sizeof(cmdPattern)+100];
  
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdlen);

  SQLDBC_Retcode erg = this->m_dbaccess->m_stmtHdl->execute (cmdlen); 
  if (erg != SQLDBC_OK){
        this->m_dbaccess->m_out->printError(this->m_dbaccess->m_stmtHdl->getStmt()->error());
        return SQLDBC_FALSE;        
  }
  
  SQLDBC_ResultSet * rs = this->m_dbaccess->m_stmtHdl->getResultSet();  
  if (rs != 0 && rs->next() == SQLDBC_OK){
      SQLDBC_RowSet *getRowSet ();
    erg = rs->getRowSet()->getObject(1, SQLDBC_HOSTTYPE_INT4, &userLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(userLen));
    erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_INT4, &userGroupLen,  &indicator, sizeof(userGroupLen));
  }
  sprintf (&cmd[0],cmdPattern,
           (userLen>sizeof("User name"))?userLen:sizeof("User name")-1, 
           (userGroupLen>sizeof("User group"))?userLen:sizeof("User group")-1,
		   searchUser);
 
  this->m_dbaccess->m_out->printComment("List of database users\n");
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmd);
  return this->m_dbaccess->doExecuteSQLCmd(cmd);
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliMetaData::displaySchemas(){
  if (! this->m_dbaccess->supportsSchema()){
    this->m_dbaccess->m_out->printError(ERR_SCHEMA_NOT_SUPPORTED);
	return SQLDBC_FALSE;        
  }

  char * searchSchema  = this->m_dbaccess->m_sqlcliProp->gettablepattern();
  SQLDBC_Length indicator=0;
  SQLDBC_Int4 schemaLen=36,ownerLen=36;
  const char *cmdlen = "SELECT MAX(LENGTH(SCHEMANAME)), MAX(LENGTH(OWNER)) FROM SCHEMAS" ;
  char cmdPattern[]= "select SUBSTR(SCHEMANAME, 1, %d) \"Schema name\", " 
              "SUBSTR(OWNER, 1, %d) \"Owner name\" " 
              "from SCHEMAS where SCHEMANAME like '%s' ORDER BY SCHEMANAME, OWNER\0";
  char cmd[sizeof(cmdPattern)+100];
  
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdlen);
  SQLDBC_Retcode erg = this->m_dbaccess->m_stmtHdl->execute (cmdlen); 
  if (erg != SQLDBC_OK){
        this->m_dbaccess->m_out->printError(this->m_dbaccess->m_stmtHdl->getStmt()->error());
        return SQLDBC_FALSE;        
  }
  
  SQLDBC_ResultSet * rs = this->m_dbaccess->m_stmtHdl->getResultSet();  
  if (rs != 0 && rs->next() == SQLDBC_OK){
      SQLDBC_RowSet *getRowSet ();
    erg = rs->getRowSet()->getObject(1, SQLDBC_HOSTTYPE_INT4, &schemaLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(schemaLen));
    erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_INT4, &ownerLen,  &indicator, sizeof(ownerLen));
  }
  sprintf (&cmd[0],cmdPattern,
           (schemaLen>sizeof("Schema name"))?schemaLen:sizeof("Schema name")-1, 
           (ownerLen>sizeof("Owner name"))?ownerLen:sizeof("Owner name")-1,
		   searchSchema);
 
  this->m_dbaccess->m_out->printComment("List of database schemas\n");
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmd);
  return this->m_dbaccess->doExecuteSQLCmd(cmd);
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliMetaData::displayTablesViews(IFTools_sqlcliInput::sqlcliInput_cmdtype cmdtype){
  char * searchSchema = this->m_dbaccess->m_sqlcliProp->getschemapattern();
  char * searchTable  = this->m_dbaccess->m_sqlcliProp->gettablepattern();
  SQLDBC_Int4 schemaLen=36,nameLen=36,typeLen=36;
  SQLDBC_Length indicator=0;
  
  char cmdlenPattern[] = "SELECT MAX(LENGTH(%s)), MAX(LENGTH(tablename)), MAX(LENGTH(tabletype)) FROM domain.tables" ;
  char cmdlen [sizeof(cmdlenPattern)+sizeof(SCHEMANAME)];
  sprintf (&cmdlen[0],cmdlenPattern, this->getSchemaname());
  
  char cmdPattern[]= "SELECT SUBSTR(%s, 1, %d) \"Schema\", SUBSTR(tablename, 1, %d) \"Name\", SUBSTR(tabletype, 1, %d) \"Type\" "
          "   from domain.tables WHERE %s LIKE '%s' AND tablename LIKE '%s' %s" 
          
          "   ORDER BY \"Schema\", \"Name\"";
  char wherePatternTab[]  =" AND type <> 'RESULT' AND type <> 'VIEW' AND tabletype <> 'VIEW' "; 
  char wherePatternView[] =" AND (type='VIEW' or tabletype='VIEW')"; 
  char *cmd = new char [sizeof(cmdPattern)+sizeof(wherePatternTab)+strlen(searchSchema)+strlen(searchTable)+2*sizeof(SCHEMANAME)]; //
  
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdlen);
  SQLDBC_Retcode erg = this->m_dbaccess->m_stmtHdl->execute (cmdlen); 
  if (erg != SQLDBC_OK){
        this->m_dbaccess->m_out->printError(this->m_dbaccess->m_stmtHdl->getStmt()->error());
        delete [] cmd;
        return SQLDBC_FALSE;        
  }
  
  SQLDBC_ResultSet * rs = this->m_dbaccess->m_stmtHdl->getResultSet();  
  if (rs != 0 && rs->next() == SQLDBC_OK){
      SQLDBC_RowSet *getRowSet ();
    erg = rs->getRowSet()->getObject(1, SQLDBC_HOSTTYPE_INT4, &schemaLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(schemaLen));
    erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_INT4, &nameLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(nameLen));
    erg = rs->getRowSet()->getObject(3, SQLDBC_HOSTTYPE_INT4, &typeLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(typeLen));

  sprintf (&cmd[0],cmdPattern,
	        this->getSchemaname(),
            schemaLen, //>sizeof("Schema"))?schemaLen:sizeof("Schema")-1, 
            nameLen,//>sizeof("Name"))?nameLen:sizeof("Name")-1, 
            typeLen,//>sizeof("Type"))?typeLen:sizeof("Type")-1,
            this->getSchemaname(),
			searchSchema,
            searchTable,
           (cmdtype == IFTools_sqlcliInput::cmdtype_showViews)?&wherePatternView[0]:&wherePatternTab[0]
           );
 
    if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmd);
    this->m_dbaccess->m_out->printComment("List of %s\n",(cmdtype == IFTools_sqlcliInput::cmdtype_showViews)?"views":"tables");
    SQLDBC_Bool ret = this->m_dbaccess->doExecuteSQLCmd(cmd,searchSchema, searchTable);
    delete [] cmd;
    return ret; 
  } else {
    this->m_dbaccess->m_out->printError(ERR_PATTERN_DOES_NOT_MATCH, searchSchema, searchTable);
  }
  delete [] cmd;
  return SQLDBC_TRUE; 
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliMetaData::displayColumns(){
  char * searchSchema = this->m_dbaccess->m_sqlcliProp->getschemapattern();
  char * searchTable  = this->m_dbaccess->m_sqlcliProp->gettablepattern();
  SQLDBC_Int4 colNameLen=36,typeLen=36,collenLen=36;
  char schema[33],tablename[33];
  SQLDBC_Length indicator=0;

  char cmdTablePattern[]="SELECT %s, TABLENAME, "
    "MAX(LENGTH(columnname)), "
    "MAX(LENGTH (datatype||' '||codetype)), "
    "MAX(LENGTH (decode(len,NULL,'-',len )|| decode(dec, NULL,'',0,'', ','||dec))) "
    "FROM domain.columns WHERE %s LIKE '%s' AND tablename LIKE '%s' AND tabletype <> 'RESULT' "
    "GROUP by tablename, tabletype, %s ";
  char *cmdTableNames = new char [sizeof(cmdTablePattern)+strlen(searchSchema)+strlen(searchTable)+2*sizeof(SCHEMANAME)];
  sprintf (&cmdTableNames[0],cmdTablePattern,
	  this->getSchemaname(),
	  this->getSchemaname(),
	  searchSchema,searchTable,
	  this->getSchemaname());
  
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printComment("cmdTableNames: %s\n",cmdTableNames);

  char cmdColPattern[]= "SELECT SUBSTR(columnname, 1, %d) \"Column Name\", "
      "SUBSTR(datatype||' '||codetype, 1, %d) \"Type\", "
      "SUBSTR(decode(len,NULL,'-',len )|| decode(dec, NULL,'',0,'', ','||dec), 1, %d) \"Length\", "
      "decode (mode, 'OPT', 'YES', 'NO') \"Nullable\", "
      "decode (KEYPOS,NULL,'',KEYPOS) Keypos "
      "FROM domain.columns "
      "WHERE %s = '%s' " 
      "      AND tablename = '%s' "
      "ORDER BY pos"; 
  char cmdColumns[sizeof(cmdColPattern)+sizeof(SCHEMANAME)+100]; //

  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdTableNames);
    SQLDBC_Statement *stmt = this->m_dbaccess->m_connection->createStatement();
	SQLDBC_Retcode erg = stmt->execute (cmdTableNames /*SQLDBC_StringEncodingUTF8*/); 
    if (erg != SQLDBC_OK){
      this->m_dbaccess->m_out->printError(stmt->error());
      delete [] cmdTableNames;
      return SQLDBC_FALSE;        
    }

    SQLDBC_ResultSet * rs = stmt->getResultSet();  
    SQLDBC_Bool erger = SQLDBC_TRUE;
    if (rs != 0 && rs->next() == SQLDBC_OK){
      this->m_dbaccess->m_sqlcliProp->openResultOutput();
      do {
        indicator = SQLDBC_NTS;
        erg = rs->getRowSet()->getObject(1, SQLDBC_HOSTTYPE_ASCII, &schema[0],   (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(schema));
        erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_ASCII, &tablename[0],(SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(tablename));
        erg = rs->getRowSet()->getObject(3, SQLDBC_HOSTTYPE_INT4,  &colNameLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(colNameLen));
        erg = rs->getRowSet()->getObject(4, SQLDBC_HOSTTYPE_INT4,  &typeLen,     (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(typeLen));
        erg = rs->getRowSet()->getObject(5, SQLDBC_HOSTTYPE_INT4,  &collenLen,   (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(collenLen));

        sprintf (&cmdColumns[0],cmdColPattern,
          colNameLen, typeLen, collenLen,this->getSchemaname(),
          &schema[0], &tablename[0]);

        if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdColumns);
        erger = this->m_dbaccess->m_out->printResult("Table \"%s.%s\"\n",IFTools_sqlcliProperties::trimString(&schema[0]),IFTools_sqlcliProperties::trimString(&tablename[0]));
        if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdColumns);
        if (erger == SQLDBC_TRUE) erger = this->m_dbaccess->doExecuteSQLCmd(cmdColumns,searchSchema,searchTable, 1);
        if (erger == SQLDBC_TRUE) this->m_dbaccess->m_out->printResult("\n");
      } while (erger == SQLDBC_TRUE && rs != 0 && rs->next() == SQLDBC_OK);
      this->m_dbaccess->m_sqlcliProp->closeResultOutput();
    } else {
      this->m_dbaccess->m_out->printError(ERR_PATTERN_DOES_NOT_MATCH,  searchSchema, searchTable);
    } 
    delete [] cmdTableNames; 
	this->m_dbaccess->m_connection->releaseStatement(stmt);
    return erger;
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliMetaData::displayListDBProcs(){
  char * searchSchema = this->m_dbaccess->m_sqlcliProp->getschemapattern();
  char * searchTable  = this->m_dbaccess->m_sqlcliProp->gettablepattern();
  SQLDBC_Int4 schemaLen=36,nameLen=36,packageLen=36,languageLen=36;
  SQLDBC_Length indicator=0;
  
  char cmdlenPattern[] = "SELECT MAX(LENGTH(%s)), MAX(LENGTH(dbprocname)), MAX(LENGTH(decode(package,NULL,'',package))),MAX(LENGTH(Language)) FROM DOMAIN.DBPROCEDURES" ;
  char cmdlen [sizeof(cmdlenPattern)+sizeof(SCHEMANAME)];
  sprintf (&cmdlen[0],cmdlenPattern, this->getSchemaname());
  
  char cmdPattern[]= "SELECT SUBSTR(%s, 1, %d) \"Schema\", " 
                "SUBSTR(dbprocname, 1, %d) \"Name\", "   
                "SUBSTR(decode(package,NULL,'',package), 1, %d) \"Package\" " 
//                "SUBSTR(Language, 1, %d) \"Language\" "   
                "FROM DOMAIN.DBPROCEDURES "
                "WHERE (package <>  'SYS_PACKAGE' or package is null) "
                "AND %s LIKE '%s' "
                "AND dbprocname LIKE '%s' "  
                "ORDER BY \"Schema\", \"Package\",\"Name\" ";
  char *cmd = new char [sizeof(cmdPattern)+strlen(searchSchema)+strlen(searchTable)+2*sizeof(SCHEMANAME)]; //

  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdlen);
  SQLDBC_Retcode erg = this->m_dbaccess->m_stmtHdl->execute (cmdlen); 
  if (erg != SQLDBC_OK){
        this->m_dbaccess->m_out->printError(this->m_dbaccess->m_stmtHdl->getStmt()->error());
        delete [] cmd;
        return SQLDBC_FALSE;        
  }
  
  SQLDBC_ResultSet * rs = this->m_dbaccess->m_stmtHdl->getResultSet();  
  if (rs != 0 && rs->next() == SQLDBC_OK){
      SQLDBC_RowSet *getRowSet ();
    erg = rs->getRowSet()->getObject(1, SQLDBC_HOSTTYPE_INT4, &schemaLen, (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(schemaLen));
    erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_INT4, &nameLen,   (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(nameLen));
    erg = rs->getRowSet()->getObject(3, SQLDBC_HOSTTYPE_INT4, &packageLen,(SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(packageLen));
    erg = rs->getRowSet()->getObject(4, SQLDBC_HOSTTYPE_INT4, &languageLen,(SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(languageLen));
  }

  sprintf (&cmd[0],cmdPattern,
	  this->getSchemaname(),
      (schemaLen)?schemaLen:1, 
      (nameLen)?nameLen:1,
      (packageLen)?packageLen:1,
//      (languageLen)?languageLen:1,
	  this->getSchemaname(),
	  searchSchema,
      searchTable);
 
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmd);
  this->m_dbaccess->m_out->printComment("List of DBProcedures\n");
  SQLDBC_Bool ret = this->m_dbaccess->doExecuteSQLCmd(cmd, searchSchema, searchTable);
  delete [] cmd;
  return ret;
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliMetaData::displayIndices(){
  char * searchSchema = this->m_dbaccess->m_sqlcliProp->getschemapattern();
  char * searchTable  = this->m_dbaccess->m_sqlcliProp->gettablepattern();
  SQLDBC_Int4 ownerLen=36, tableLen=36, columnLen=36,indexLen=36, posLen=36;
  SQLDBC_Length indicator=0;
  
  char cmdlenPattern[] = "SELECT MAX(LENGTH(%s)), MAX(LENGTH(TABLENAME||'.'||COLUMNNAME)), MAX(LENGTH(COLUMNNAME)), MAX(LENGTH(INDEXNAME)), MAX(LENGTH(CHR(columnno))) FROM \"DOMAIN\".\"INDEXCOLUMNS\"" ;
  char cmdlen [sizeof(cmdlenPattern)+sizeof(SCHEMANAME)];
  sprintf (&cmdlen[0],cmdlenPattern, this->getSchemaname());


  char cmdPattern[]= " SELECT   "
"         SUBSTR(INDEXNAME, 1, %d) \"Index Name\", "
"         SUBSTR(TABLENAME||'.'||COLUMNNAME, 1, %d) \"Table.Column\", "
//"         SUBSTR(COLUMNNAME, 1, %d) \"Column Name\", "
"         decode (TYPE, 'UNIQUE', 'true', 'false') \"Unique\", "
"         SUBSTR(CHR(columnno), 1, %d) \"POS\", "
"         sort \"Sort\" "
"         FROM \"DOMAIN\".\"INDEXCOLUMNS\"  WHERE %s LIKE '%s' AND tablename like '%s' "
"         ORDER BY INDEXNAME, columnno ";
              
  char *cmd = new char [sizeof(cmdPattern)+strlen(searchSchema)+strlen(searchTable)+sizeof(SCHEMANAME)]; //
  
  if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmdlen);
  SQLDBC_Retcode erg = this->m_dbaccess->m_stmtHdl->execute (cmdlen); 
  if (erg != SQLDBC_OK){
        this->m_dbaccess->m_out->printError(this->m_dbaccess->m_stmtHdl->getStmt()->error());
        delete [] cmd;
        return SQLDBC_FALSE;        
  }
  
  SQLDBC_ResultSet * rs = this->m_dbaccess->m_stmtHdl->getResultSet();  
  if (rs != 0 && rs->next() == SQLDBC_OK){
      SQLDBC_RowSet *getRowSet ();
    erg = rs->getRowSet()->getObject(1, SQLDBC_HOSTTYPE_INT4, &ownerLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(ownerLen));
    erg = rs->getRowSet()->getObject(2, SQLDBC_HOSTTYPE_INT4, &tableLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(tableLen));
    erg = rs->getRowSet()->getObject(3, SQLDBC_HOSTTYPE_INT4, &columnLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(columnLen));
    erg = rs->getRowSet()->getObject(4, SQLDBC_HOSTTYPE_INT4, &indexLen,  (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(indexLen));
	erg = rs->getRowSet()->getObject(5, SQLDBC_HOSTTYPE_INT4, &posLen,    (SQLDBC_Length*)&indicator, (SQLDBC_Length)sizeof(posLen));

  sprintf (&cmd[0],cmdPattern,
            indexLen,//>sizeof("Name"))?nameLen:sizeof("Name")-1, 
            tableLen,
//			columnLen, //>sizeof("Schema"))?schemaLen:sizeof("Schema")-1, 
            posLen,
			this->getSchemaname(),
            searchSchema,
            searchTable
           );
 
    if (this->m_dbaccess->m_sqlcliProp->getdebug()) this->m_dbaccess->m_out->printError("%s\n",cmd);
    this->m_dbaccess->m_out->printComment("List of indices Tablepattern \"%s.%s\"\n",IFTools_sqlcliProperties::trimString(searchSchema),IFTools_sqlcliProperties::trimString(searchTable));
    SQLDBC_Bool ret = this->m_dbaccess->doExecuteSQLCmd(cmd,searchSchema, searchTable);
    delete [] cmd;
    return ret; 
  } else {
    this->m_dbaccess->m_out->printError(ERR_PATTERN_DOES_NOT_MATCH, searchSchema, searchTable);
  }
  delete [] cmd;
  return SQLDBC_TRUE; 
}
//------------------------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::setAutocommit(SQLDBC_Bool mode){
  if (! this->assertConnect()) return SQLDBC_FALSE;
  m_connection->setAutoCommit(mode);
  m_out->printComment(COM_CHANGE_AUTOCOMMITMODE,(mode)?"ON":"OFF");
  return SQLDBC_TRUE;
}
//------------------------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::setUsePreparedStmt(SQLDBC_Bool mode){
  if (! this->assertConnect()) { 
	return SQLDBC_FALSE;
  }
  if (this->m_stmtHdl){
    this->m_stmtHdl->releaseStmt(this->m_connection);
    delete this->m_stmtHdl;
    this->m_stmtHdl = 0;
  }  
  this->m_stmtHdl = new IFTools_sqlcliDBStmt(mode, this->m_connection); 
  m_out->printComment(COM_CHANGE_STMTTYPE,(mode)?"ON":"OFF");
  return SQLDBC_TRUE;
}
//------------------------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliDBAccess::doConnect(){
    this->doDisconnect();
    SQLDBC_Bool erg = this->assertConnect();
    if (erg == SQLDBC_TRUE){
      m_out->printComment(COM_CONNECT, 
                          m_sqlcliProp->getdbhost(), 
                          (strlen(m_sqlcliProp->getdbhost())?"@":""),
                          m_sqlcliProp->getdbname());
    }
    return erg;
 }
 //------------------------------------------------------------------------------------
 SQLDBC_Bool IFTools_sqlcliDBAccess::doDisconnect(){
    if (this->m_connection){
		if (this->m_metaData) {
			delete this->m_metaData;
			this->m_metaData = 0;
		}
      this->releaseConnection();  
      m_sqlcliProp->setConnectState(SQLDBC_FALSE);
    }
    return SQLDBC_TRUE;
 }
//------------------------------------------------------------------------------------
 void IFTools_sqlcliDBAccess::releaseConnection(){
   if (this->m_stmtHdl){
	   this->m_stmtHdl->releaseStmt(this->m_connection);
       delete this->m_stmtHdl;
       this->m_stmtHdl = 0;
   }  
   if (this->m_connection){
    this->m_sqlcliProp->getEnvironment()->releaseConnection(this->m_connection);
    this->m_connection = 0; 
   }
 }
 
