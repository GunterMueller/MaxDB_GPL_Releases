   /*!**************************************************************************

  module      : IFTools_sqlcliProperties.h

  -------------------------------------------------------------------------

  responsible : D031096

  special area:
  description : Interface Runtime 8.0

  description:  Provides a property container for command line tool sqlcli

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
#ifndef IFTOOLS_SQLCLIPROPERTIES_H
#define IFTOOLS_SQLCLIPROPERTIES_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_Types.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"
#include "geo81.h"
#ifdef WIN32
#include <io.h>
#endif


#define ERR_INVALID_SQLMODE         "Unknown SQL mode. Use one of these modes: [INTERNAL|ORACLE|ANSI|DB2]\n"
#define ERR_SEARCHPATTERN_TO_LONG   "Current search pattern is too long (%d - max. size is 100)."
#define ERR_AUTOCOMMIT_UNKNOWN      "Unknown autocommit option. Use one of these values: [ON|OFF].\n"
#define ERR_REMOVE_FILE             "Cannot delete file %s: %s\n"
#define ERR_OPEN_FILE               "Cannot open file %s: %s\n"
#define ERR_EXEC_CMD                "Cannot execute command %s: %s\n"
#define ERR_READ_FILE               "Cannot read from file %s: %s\n"
#define ERR_GET_COLUMNNAME          "Cannot get a columnname for column %d.\n"
#define ERR_UNKNOWN_COLTYPE         "Unknown type %d for column %d\n"
#define ERR_INTERNAL_TRUNC          "Internal Error: Data truncated - Column %d ColLen %d, BufLen %d\n"
#define ERR_PATTERN_DOES_NOT_MATCH  "No match found for search pattern: %s.%s\n"
#define ERR_SCHEMA_NOT_SUPPORTED    "SCHEMAS not supported by the underlaying database\n"
#define COM_CHANGE_SQLMODE          "SQL mode changed to %s\n"
#define COM_RETURNCODE              "Shell command returned with %d\n"
#define COM_CHANGE_AUTOCOMMITMODE   "Autocommit mode switched %s\n"
#define COM_CHANGE_STMTTYPE         "Usage of prepared statement switched %s\n"
#define COM_CHANGE_ALIGNMODE        "Aligned output mode switched %s\n"
#define COM_CHANGE_PAGEROUTPUT      "Page by page scroll output switched %s\n"
#define COM_CHANGE_MULTILINE        "Multiline mode switched %s\n"
#define COM_DISCONNECT              "Connection closed\n"
#define COM_CONNECT                 "Connected to %s%s%s\n"

#define IFTOOLS_PROP_WHOLE_LONG   -1

using namespace SQLDBC;
/**
 * @ingroup sqlcli
 * @brief Provides a property container for command line tool sqlcli
 */
class IFTools_sqlcliProperties
    : public teo81_EvalProgArgs
{
public:
  IFTools_sqlcliProperties                               ( int argc, TCHAR** argv );
  ~IFTools_sqlcliProperties                              ();
  
  enum {
    TSP00_KnlIdentifier = 64+1,
    TSP00_Pw            = 18+1,
    XU_MAX_KEY_LEN      = 18+1,    
    XU_MAX_NAME_LEN     = 64+1,
    XU_MAX_PWD_LEN      = 18+1,
    XU_MAX_DBNAME_LEN   = 18+1,
    XU_MAX_DBNODE_LEN   = 64+1
  };  
  
  enum sqlcliEnv_dateformat{    
    dateformat_INTERNAL = 1,
    dateformat_ISO = 2,
    dateformat_USA = 3,
    dateformat_EUR = 4,
    dateformat_JIS = 5        
  };

  inline char*          getusername()         {return m_username;}	
  inline char*          getpassword()         {return m_password;}	
  inline char*          getdbname()           {return m_dbname;}	
  inline char*          getdbhost()           {return m_dbhost;}	
  inline char*          getxuserkey()         {return m_xuserkey;}	
  inline char*          getnullvalue()        {return m_nullvalue;}	
  inline char*          getcommand()          {return m_command;}	
  inline char*          getcmdsep()           {return m_cmdseparator;}
  inline char*          getlogo()             {return m_logo;}
  inline char*          getschemapattern()    {return m_searchSchema;}
  inline char*          gettablepattern()     {return m_searchTable;}
  inline char*          geteditfilename()     {return m_editFileName;}
  inline char*          getdefaulteditor()    {return m_defaultEditor;}
  inline char*          getresultheader()     {return m_resultheader;}
  inline char*          getresultfooter()     {return m_resultfooter;}
  inline char*          getrowprefix()        {return m_rowprefix;}
  inline char*          getrowsuffix()        {return m_rowsuffix;}
  inline char*          getcolumnprefix()     {return m_columnprefix;}
  inline char*          getcolumnsuffix()     {return m_columnsuffix;}
  inline char*          getfieldseparator()   {return m_fieldseparator;}	
  inline sqlcliEnv_dateformat getdateformat() {return m_dateformat;}	
  inline FILE *               geterrfile()    {return m_errfile;}	
  inline FILE *               getoutfile()    {return m_outfile;}
  inline FILE *               getinfile ()    {return m_infile;}
  inline SQLDBC_Bool          getcmdtrace()   {return m_cmdtrace;}	
  inline SQLDBC_Bool          getautocommit() {return m_autocommit;}	
  inline SQLDBC_Bool          getnoHeader()   {return m_noHeader;}	
  inline SQLDBC_Bool          getbatch()      {return m_batch;}	
  inline SQLDBC_Bool          getrollbackOnError() {return m_rollbackOnError;}
  inline SQLDBC_Bool          getdebug()       {return m_debug;}
  inline SQLDBC_Bool          getmultilinemode(){return m_multilinemode;}
  inline SQLDBC_Bool          getnoComment    (){return m_noComment;}
  inline SQLDBC_Bool          getalignedOutput(){return m_alignedOutput;}
  inline SQLDBC_Length        getmaxLongLenth (){return m_maxLongLenth;}
  inline SQLDBC_Bool          isConnected     (){return m_connectState;}
  inline SQLDBC_Bool          isVersionInfoFlagSet (){return m_versionInfo;}
  inline SQLDBC_Bool          isExitForced (){return m_ExitForced;}
  inline SQLDBC_Bool          getUsePreparedStmt (){return m_usePreparedStmt;}
  inline SQLDBC_Bool          getnoPager (){return m_noPager;}
  inline SQLDBC_Bool          isInteractive   (){
    return (
#ifdef WIN32
		_isatty(_fileno(stdin)) &&
		_isatty(_fileno(stdout))
#else
		isatty(fileno(stdin)) &&
		isatty(fileno(stdout))
#endif
     && (this->getoutfile() == stdout) 
		)? SQLDBC_TRUE:SQLDBC_FALSE;
  }
  inline SQLDBC_Bool          isReadingFromStdin   (){
    return (
#ifdef WIN32
		_isatty(_fileno(stdin))
#else
		isatty(fileno(stdin))
#endif
     && (this->getinfile() == stdin) 
		)? SQLDBC_TRUE:SQLDBC_FALSE;
  }
  inline SQLDBC_ConnectProperties&  getifrprops() {return m_ifrprops;}
  inline SQLDBC_ConnectProperties& gettraceprops() { return m_traceprops; }
  inline static SQLDBC_Environment* getEnvironment() {return m_environment;}

  SQLDBC_Bool setmaxLongLenth         (const char* value);
  inline SQLDBC_Bool setnocomment     (SQLDBC_Bool value){return (m_noComment = value);}
  inline SQLDBC_Bool setdbname        (const char * name, SQLDBC_Int4 len){return setCharProperty(&m_dbname   , name, len);}
  inline SQLDBC_Bool setdbhost        (const char * name, SQLDBC_Int4 len){return setCharProperty(&m_dbhost   , name, len);}
  inline SQLDBC_Bool setxuserkey      (const char * name, SQLDBC_Int4 len){return setCharProperty(&m_xuserkey , name, len);}
  inline SQLDBC_Bool setnullvalue     (const char * name, SQLDBC_Int4 len){return setCharProperty(&m_nullvalue, name, len);}
  inline SQLDBC_Bool setcmdseparator  (const char * name, SQLDBC_Int4 len){return setSeparator(&m_cmdseparator, name, len);}
  inline SQLDBC_Bool setcommand       (const char * name, SQLDBC_Int4 len){return setCharProperty(&m_command, name, len);}
  inline SQLDBC_Bool setrowprefix     (const char * name, SQLDBC_Int4 len){return setSeparator(&m_rowprefix, name, len);}
  inline SQLDBC_Bool setrowsuffix     (const char * name, SQLDBC_Int4 len){return setSeparator(&m_rowsuffix, name, len);}
  inline SQLDBC_Bool setcolumnprefix  (const char * name, SQLDBC_Int4 len){return setSeparator(&m_columnprefix, name, len);}
  inline SQLDBC_Bool setcolumnsuffix  (const char * name, SQLDBC_Int4 len){return setSeparator(&m_columnsuffix, name, len);}
  SQLDBC_Bool setsearchpattern        (const char * name, SQLDBC_Int4 len);
  SQLDBC_Bool seteditfilename         (const char * name);
  inline SQLDBC_Bool setdefaulteditor (const char * name){return setCharProperty(&m_defaultEditor, name, strlen(name));}
  inline SQLDBC_Bool setExitCode      (SQLDBC_Int4 eCode){m_exitCode = eCode; return SQLDBC_TRUE;}

  SQLDBC_Bool setoutfile(const char * outfilename);
  SQLDBC_Bool setinfile (const char * filename);
  SQLDBC_Bool setuser(const char * userName, SQLDBC_Int4 userLen);
  SQLDBC_Bool setlogo(const char * name, SQLDBC_Int4 len);
  SQLDBC_Bool setResultHeader(const char * name, SQLDBC_Int4 len);
  SQLDBC_Bool setResultFooter(const char * name, SQLDBC_Int4 len);
  SQLDBC_Bool setAlignedOutput(SQLDBC_Bool alignment);
  SQLDBC_Bool setfieldseparator     (const char * name, SQLDBC_Int4 len);
  SQLDBC_Bool openResultOutput();
  SQLDBC_Bool closeResultOutput();
  int executeCommand (char* cmd);

  inline SQLDBC_Bool setAutocommit(SQLDBC_Bool mode){m_autocommit=mode; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setAutocommit(){m_autocommit=(m_autocommit)?SQLDBC_FALSE:SQLDBC_TRUE; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setAlignedOutput(){return setAlignedOutput((m_alignedOutput)?SQLDBC_FALSE:SQLDBC_TRUE);}
  inline SQLDBC_Bool setConnectState(SQLDBC_Bool state){m_connectState=state; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setUsePreparedStmt(SQLDBC_Bool state){m_usePreparedStmt=state; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setUsePreparedStmt(){m_usePreparedStmt=(m_usePreparedStmt)?SQLDBC_FALSE:SQLDBC_TRUE; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setNoPager(SQLDBC_Bool state){m_noPager=state; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setNoPager(){m_noPager=(m_noPager)?SQLDBC_FALSE:SQLDBC_TRUE; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setMultiline(SQLDBC_Bool state){m_multilinemode=state; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setMultiline(){m_multilinemode=(m_multilinemode)?SQLDBC_FALSE:SQLDBC_TRUE; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setStopOnError(SQLDBC_Bool state){m_StopOnError=state; return SQLDBC_TRUE;}
  inline SQLDBC_Bool setStopOnError(){m_StopOnError=(m_StopOnError)?SQLDBC_FALSE:SQLDBC_TRUE; return SQLDBC_TRUE;}

  static char * trimString(char* val);
  void   removeTmpEditFile();
  
  enum {maxKnlIdentifierUTF8 = 2* TSP00_KnlIdentifier};

  void       dumpClass();

  void exitProgram();
  inline void forceProgramExit(){if (this->m_StopOnError) this->m_ExitForced = SQLDBC_TRUE;}

private:

  SQLDBC_Bool setCharProperty(char ** prop, const char* value, SQLDBC_Int4 len);
  SQLDBC_Bool setSeparator(char ** prop, const char* value, SQLDBC_Int4 len);
  
  enum sqlcliEnv_option {
    option_userpasswd      = 1 ,
    option_dbname          = 2 ,
    option_dateformat      = 3 ,
    option_trace           = 4 ,
    option_separator       = 5 ,
    option_nullval         = 6 ,
    option_autocommitOff   = 7 ,
    option_noHeader        = 8 ,
    option_batch           = 9 ,
    option_oneColperLine   = 10,
    option_rollbackOnError = 11,
    option_writeCmdProt    = 12,
    option_xuserkey        = 13,
    option_dbhost          = 14,
    option_sqlmode         = 15,
    option_sqlcmd          = 16,
    option_multilinemode   = 17,
    option_outputfile      = 18, 
    option_nocomment       = 19,
    option_maxLongLen      = 20, 
    option_UnAligned       = 21, 
    option_cmdseparator    = 23,
    option_logo            = 24,
    option_rowprefix       = 25,
    option_rowsuffix       = 26,
    option_columnprefix    = 27,
    option_columnsuffix    = 28,
    option_version         = 29,
    option_timeout         = 30,
    option_useDBSStmt      = 31,
    option_stopOnError     = 32,
    option_nopager         = 33,
    option_debug           = 98, 
    option_help            = 99
  }sqlcliEnv_option ;
  
  void       eo80_vHelp                    ( const TCHAR*         OptionArgument );
  bool       eo80_vNextValidOption         ( const int            OptionIndex,
                                             const TCHAR*         OptionArgument );
  bool       eo80_vParseArgumentsFinished  ( const _TCHAR* const* ArgvRemaining,
                                             int                  ArgsRemaining,
                                             int                  ArgsParsed,
                                             bool                 BreakOptionFound ); 

  static const teo81_EvalProgArgs::teo81_OptionDesc Options[];  //!< The command line options
  static SQLDBC_Environment* m_environment;
  static SQLDBC_Int4 getConsoleScreenSize();
  
    char*           m_username;         //!< username 
    char*           m_password;         //!< password                     
    char*           m_dbname;           //!< dbname                       
    char*           m_dbhost;           //!< hostname                     
    char*           m_xuserkey;         //!< xuserkey                     
    char*           m_nullvalue;        //!< null value string           
    char*           m_fieldseparator;   //!< separator           
    char*           m_command;          //!< command           
    char*           m_logo;             //!< 
    char*           m_searchSchema;     //!< schema searchpattern for like expressions
    char*           m_searchTable;      //!< table  searchpattern for like expressions
    char*           m_editFileName;     //!< 
    char*           m_defaultEditor;    //!< 
    char*           m_cmdseparator;     //!< 
    char*           m_resultheader;     //!< 
    char*           m_resultfooter;     //!< 
    char*           m_rowprefix;        //!< 
    char*           m_rowsuffix;        //!< 
    char*           m_columnprefix;     //!< 
    char*           m_columnsuffix;     //!< 

    sqlcliEnv_dateformat m_dateformat;  //!< dateformat          
    FILE *               m_errfile;     //!< File for error messages" 
    FILE *               m_outfile;     //!< File for output" 
    FILE *               m_infile;      //!< File for intput" 
    SQLDBC_Bool          m_cmdtrace;    //!< cmd trace enabled
    SQLDBC_Bool          m_noComment;   //!< cmd trace enabled
    SQLDBC_Bool          m_autocommit;  //!< autocommit
    SQLDBC_Bool          m_noHeader;    //!< don't print any header
    SQLDBC_Bool          m_batch;       //!< batch mode
    SQLDBC_Bool          m_rollbackOnError;   //!< rollback on error
    SQLDBC_Bool          m_debug;         //!< print debug infos enabled
    SQLDBC_Bool          m_multilinemode; //!< activates the multiline mode
    SQLDBC_Bool          m_alignedOutput; //!< activates the multiline mode
    SQLDBC_Length        m_maxLongLenth;  //!< max output length for long columns in bytes/characters (default is 10).  

    SQLDBC_Bool          m_editFileIsTmpFile;  //!< max output length for long columns in bytes/characters (default is 10).  
    SQLDBC_ConnectProperties m_ifrprops;       //!< connect properties
    SQLDBC_ConnectProperties m_traceprops;     //!< trace settings
    SQLDBC_Bool          m_pagerOutput;
    SQLDBC_Bool          m_connectState; //!< activates the multiline mode
    SQLDBC_Bool          m_versionInfo;
    SQLDBC_Bool          m_usePreparedStmt;
	int                  m_exitCode;
    SQLDBC_Bool          m_ExitForced;
    SQLDBC_Bool          m_StopOnError;
    SQLDBC_Bool          m_noPager;
};
#endif
