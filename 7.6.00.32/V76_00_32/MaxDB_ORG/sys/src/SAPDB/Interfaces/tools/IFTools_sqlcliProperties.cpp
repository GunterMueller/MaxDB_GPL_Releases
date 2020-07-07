/*!**************************************************************************

  module      : IFTools_sqlcliProperties.cpp

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

#include "SAPDB/Interfaces/tools/IFTools_sqlcliProperties.h"
#include <stdlib.h>
/* 
#ifndef WIN32
#include <curses.h> 
#endif
*/
#define DEFAULT_PAGER "more"

SQLDBC_Environment* IFTools_sqlcliProperties::m_environment = 0;
//new SQLDBC_Environment(SQLDBC_ClientRuntimeInit());



const teo81_EvalProgArgs::teo81_OptionDesc IFTools_sqlcliProperties::Options[] = 
//    OptionIndex       ShortOption  LongOptionStr    OptionWithArg Optional ParamHelpStr                                          OptionHelpStr
   {
    { option_noHeader        , _T('a'), _T("noheader") ,       false, true, _T("\"Do not print any header for select commands.\""),_T("Do not print any header for select commands.") },
    { option_UnAligned       , _T('A'), _T("no-align") ,       false, true, _T("\"Switch to unaligned output mode.\""),            _T("Switch to unaligned output mode.") },
    { option_maxLongLen      , _T('b'), _T("maxlongoutput"),   true,  true, _T("\"<max length>\"|\"all\""),                        _T("max output length for long columns in bytes/characters (default is 10).") },
    { option_cmdseparator    , _T('c'), _T("cmdseparator"),    true,  true, _T("\"<separator>\""),                                 _T("Use separator to separate comamnds in batch files.") },
    { option_dbname          , _T('d'), _T("dbname") ,         true,  true, _T("\"<database name>\""),                             _T("Specifies the name of the database to connect to.") },
    { option_dateformat      , _T('D'), _T("dateformat") ,     true,  true, _T("\"[INTERNAL|ISO|USA|EUR|JIS]\""),                  _T("Use this date and/or time format instead of the one specified in the database set command. Possible options are INTERNAL, ISO, USA, EUR or JIS") },
    { option_stopOnError     , _T('E'), _T("stoponerror") ,    true,  true, _T("exit code"),                                       _T("exit sqlcli returning <exit code> in case of an error") },
    { option_writeCmdProt    , _T('f'), _T("cmdtrace") ,       false, true, _T("\"<protocol name>\""),                             _T("Printout the SQL commands") },
    { option_separator       , _T('F'), _T("fieldseparator") , true,  true, _T("\"<separator>\""),                                 _T("Use separator as the field separator.") },
    { option_nullval         , _T('g'), _T("null") ,           true,  true, _T("\"<null string>\""),                               _T("The argument is a string that should be printed whenever a field is null. The default is '?'.") },
    { option_batch           , _T('i'), _T("input") ,          true,  true, _T("\"<input file>\""),                                _T("Use file <input file> as the source of queries") },
    { option_nopager         , _T('j'), _T("nopager")         ,false, true, _T("\"Do not use a pager for output.\""),              _T("Do not use a pager for output..") },
    { option_rollbackOnError , _T('k'), _T("rollbackonerror") ,false, true, _T("\"Rollback work in case of an error.\""),          _T("Rollback work in case of an error.") },
    { option_logo            , _T('l'), _T("logo")            ,true,  true, _T(""),                                                _T("") },
    { option_multilinemode   , _T('m'), _T("multilinemode")   ,false, true, _T("\"Activates the multiline mode.\""),               _T("Activates the multiline mode.") },
    { option_dbhost          , _T('n'), _T("hostname") ,       true,  true, _T("\"<host name>\""),                                 _T("Specifies the name of the host to connect to.") },
    { option_outputfile      , _T('o'), _T("output") ,         true,  true, _T("\"<output file>\""),                               _T("Name of the output file. Default is stdout") },
    { option_rowprefix       , _T('p'), _T("rowprefix")       ,true,  true, _T(""),                                                _T("") },
    { option_rowsuffix       , _T('P'), _T("rowsuffix")       ,true,  true, _T(""),                                                _T("") },
    { option_oneColperLine   , _T('Q'), _T("onecolumnperline"),false, true, _T("\"Show each column on a separate line.\""),        _T("Show each column on a separate line") },
    { option_useDBSStmt      , _T('r'), _T("noPreparedStatement"),false,  true, _T(""),                                            _T("do not use prepared statements") },
    { option_sqlmode         , _T('S'), _T("sqlmode") ,        true,  true, _T("\"[INTERNAL|ORACLE|ANSI|DB2]\""),                  _T("The SQL Mode. One of \"INTERNAL\",\"ORACLE\",\"ANSI\", or \"DB2\". ") },
    { option_trace           , _T('T'), _T("trace")  ,         true,  true, _T("\"<tracefile name>\""),                            _T("Write a trace file.") },
    { option_userpasswd      , _T('u'), _T("user")   ,         true,  true, _T("\"<user>,<password>\""),                           _T("Username and password to be connected to the database.") },
    { option_xuserkey        , _T('U'), _T("xuserkey") ,       true,  true, _T("\"<xuser key>\""),                                 _T("Specifies the name of the xuser key for connecting the database.") },
    { option_nocomment       , _T('x'), _T("nocomment") ,      false, true, _T("\"turn off printing comments\""),                  _T("Turn off printing comments.") },
    { option_timeout         , _T('y'), _T("timeout") ,        true,  true, _T("\"<timeout>\""),                                   _T("Sets the connection timeout value.") },
    { option_autocommitOff   , _T('z'), _T("noautocommit"),    false, true, _T("\"autocommit off\""),                              _T("Switches autocommit off.") },

    { option_help            , _T('h'), _T("help")  ,          true,  true, _T("[<Option> | LONG]"),                               _T("Shows help about sqlcli command line arguments.") },
    { option_help            , _T('?'), _T("?")     ,          true,  true, _T("[<Option> | LONG]"),                               _T("Shows help about sqlcli command line arguments.") },
    { option_version         , _T('v'), _T("version") ,        false, true, _T(""),                                                _T("") },
    { option_debug           , _T('t'), _T("debug")     ,      false, true, _T("Printout debug informations"),                     _T("Printout debug informations") },

//    not supported option taken from xsql
//    puts("         -K Protocol commands to standard out");
//    puts("         -p <decimal format>: Use this decimal format instead of the");
//    puts("            one specified in the database set command.");

};
//-----------------------------------------------------------------------
void IFTools_sqlcliProperties::eo80_vHelp ( const TCHAR*         OptionArgument )
{
//   teo81_EvalProgArgs::eo80_vHelp(OptionArgument);
	printf(("SQLCLI version <MAJOR_VERSION>.<MINOR_VERSION>.<CORRECTION_LEVEL>, "
	        "the <PRODUCT_NAME> interactive terminal.\n\n"));
	puts(("Usage:"));
	puts(("  sqlcli [<options>] ... [<command>]\n"));

	puts(("\nOptions for connecting to the database:"));
	puts(("  -n <database_server>[:<port>] name of the host on which the database\n" 
	      "                          instance is installed(default: localhost)"));
	puts(("  -d <database_name>      name of the database instance"));
	puts(("  -u <user_name,password> user name and password"));
	puts(("  -U <user_key>           xuser key \n")); 
	puts(("  -S SQLMODE              SQL mode, possible values are\n"
        "                          \"INTERNAL\",\"ORACLE\",\"ANSI\", or \"DB2\""));
	puts(("  -y                      sets session timeout in seconds (0 means no timeout)"));
	puts(("  -z                      switches autocommit off"));
	puts(("  -r                      suppress usage of prepared statements"));
	
	puts(("\nInput and output options:"));
	puts(("  -i <file_name>  use file <file_name> to input queries (default: stdin)"));
	puts(("  -o <file_name>  use file <file_name> for output (default: stdout)")); 
	puts(("  -x              run quietly (no messages, only query output)")); 
	puts(("  -m              activates the multi-line mode for query input"));
	puts(("  -c <separator>  use <separator> for separate commands read from")); 
	puts(("                  input files (default: '//')"));

	puts(("\nOutput format options:"));
	puts(("  -F <separator>  use <separator> as the field separator (default: '|')"));
	puts(("  -g <null_value> use <null_value> whenever a field is NULL (default: '?'"));
	puts(("  -p <prefix>     use <prefix> as row prefix for printing (default: '|'"));
	puts(("  -P <suffix>     use <suffix> as row suffix for printing (default: '|'"));
	puts(("  -A              switch to unaligned output mode."));
	puts(("  -j              switch the page by page scroll output off."));
	puts(("  -Q              show each column on a separate line"));
	puts(("  -a              do not print any header for SELECT commands"));
	puts(("  -f              printout the SQL commands"));
	puts(("  -b MAXLENGTH    max. output length for long columns in\n"
	      "                  bytes/characters (default is 10)"));

	puts(("General options:"));
	puts(("  -h              show help, then exit"));
	puts(("  -v              printout version information"));
	puts(("  -E <exit code>  exit sqlcli returning <exit code> in case of an error"));
	puts(("  -t              printout debug information"));
	puts(("  -T <filename>   write a database interface trace (SQL trace)"));
	
	puts((
		   "\nFor further information, type \"\\?\" (for internal commands) from \n"
		   "within SQLCLI or consult the SQLCLI documentation.\n"));

   this->exitProgram();
}

static char Utfmap[] ={  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x5F, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x5F, 0x20,
                    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x27, 0x20, 0x29, 0x29, 0x0D, 0x0A, 0x20, 0x20, 0x20, 0x20,
                    0x20, 0x2E, 0x2F, 0x2F, 0x2F, 0x2F, 0x2F, 0x7C, 0x7C, 0x5C, 0x5C, 0x5C, 0x2E, 0x20, 0x20, 0x20,
                    0x20, 0x2E, 0x2D, 0x27, 0x7E, 0x20, 0x60, 0x2D, 0x2E, 0x0D, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x28,
                    0x28, 0x2F, 0x2F, 0x27, 0x27, 0x20, 0x20, 0x27, 0x27, 0x5C, 0x5C, 0x29, 0x29, 0x20, 0x20, 0x2F,
                    0x2F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5C, 0x5C, 0x0D, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20,
                    0x47, 0x2F, 0x20, 0x40, 0x20, 0x6F, 0x20, 0x40, 0x20, 0x20, 0x5C, 0x3F, 0x20, 0x20, 0x28, 0x28,
                    0x20, 0x65, 0x20, 0x2C, 0x20, 0x65, 0x20, 0x20, 0x29, 0x29, 0x0D, 0x0A, 0x20, 0x20, 0x20, 0x20,
                    0x20, 0x20, 0x5C, 0x20, 0x20, 0x2E, 0x5F, 0x5F, 0x2E, 0x20, 0x20, 0x2F, 0x20, 0x2C, 0x5F, 0x2F,
                    0x2F, 0x5C, 0x20, 0x2E, 0x5F, 0x2C, 0x20, 0x20, 0x2F, 0x5C, 0x5C, 0x5F, 0x29, 0x0D, 0x0A, 0x20,
                    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x60, 0x2D, 0x2E, 0x5F, 0x5F, 0x2E, 0x2D, 0x27, 0x20, 0x20,
                    0x60, 0x2D, 0x27, 0x20, 0x20, 0x60, 0x2E, 0x5F, 0x5F, 0x2E, 0x27, 0x20, 0x20, 0x60, 0x2D, 0x27,
                    0x0D, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x2E, 0x2D, 0x27, 0x20, 0x20, 0x2F, 0x5C, 0x20, 0x20,
                    0x27, 0x2D, 0x2E, 0x20, 0x20, 0x20, 0x2E, 0x2D, 0x2D, 0x3C, 0x5F, 0x20, 0x5F, 0x3B, 0x2E, 0x2D,
                    0x2E, 0x0D, 0x0A, 0x20, 0x20, 0x20, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x2F, 0x20, 0x20, 0x5C,
                    0x5F, 0x20, 0x20, 0x20, 0x60, 0x5C, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x4F, 0x20, 0x20, 0x20,
                    0x20, 0x20, 0x5C, 0x0D, 0x0A, 0x20, 0x20, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5C, 0x7C, 0x20,
                    0x20, 0x7C, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x20,
                    0x20, 0x20, 0x20, 0x20, 0x20, 0x5C, 0x0D, 0x0A, 0x20, 0x2F, 0x20, 0x20, 0x20, 0x27, 0x20, 0x20,
                    0x6F, 0x5C, 0x20, 0x20, 0x2F, 0x20, 0x20, 0x20, 0x27, 0x2F, 0x20, 0x20, 0x3B, 0x20, 0x20, 0x20,
                    0x7C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3E, 0x20, 0x20, 0x29, 0x0D, 0x0A, 0x2F, 0x20, 0x20, 0x20,
                    0x2F, 0x7C, 0x2E, 0x5F, 0x6F, 0x2F, 0x5F, 0x5F, 0x5C, 0x2E, 0x5F, 0x5F, 0x7C, 0x5C, 0x5F, 0x5F,
                    0x5F, 0x29, 0x5F, 0x2E, 0x2D, 0x2D, 0x2E, 0x5F, 0x5F, 0x2F, 0x5F, 0x2E, 0x2F, 0x0D, 0x0D, 0x00};

//-----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::openResultOutput(){
  FILE* out=NULL;
  if ( !this->m_noPager &&
       this->isInteractive()){
		char *pager = getenv("PAGER");
  	if (!pager){
			pager = (char*)DEFAULT_PAGER;
    }
#ifdef WIN32
  	out = _popen(pager, "w");
#else
  	out =  popen(pager, "w");
  	signal (SIGPIPE, SIG_IGN);
#endif
    if (out) { 
      m_outfile = out;
      m_pagerOutput = SQLDBC_TRUE;
    }
	}
	return SQLDBC_TRUE;
}

SQLDBC_Bool IFTools_sqlcliProperties::closeResultOutput(){
  if(m_pagerOutput) {
#ifdef WIN32
   _pclose(m_outfile);
#else
   pclose (m_outfile);
#endif
    m_outfile = stdout;
    m_pagerOutput = SQLDBC_FALSE;
	}
	return SQLDBC_TRUE;
}
//-----------------------------------------------------------------------
IFTools_sqlcliProperties::~IFTools_sqlcliProperties(){
  this->removeTmpEditFile();
}                            

IFTools_sqlcliProperties::IFTools_sqlcliProperties ( int argc, TCHAR** argv )
       :m_dbname    (0),
        m_dbhost    (0),
        m_username  (0),
        m_password  (0),
        m_xuserkey  (0),
        m_nullvalue (0),
        m_resultheader(0),
        m_resultfooter(0),
        m_rowprefix(0),
        m_rowsuffix(0),
        m_columnprefix(0),
        m_columnsuffix(0),
        m_fieldseparator (0), 
        m_cmdseparator (0),
        m_command(0),
        m_logo(0),
        m_searchSchema (0),
        m_searchTable (0),
        m_editFileName (0),
        m_defaultEditor (0),
        m_dateformat(dateformat_INTERNAL),     
        m_errfile   (stderr),     
        m_outfile   (stdout),    
        m_infile    (stdin),    
        m_cmdtrace  (SQLDBC_FALSE),          
        m_autocommit(SQLDBC_TRUE),     
        m_noHeader  (SQLDBC_FALSE),       
		m_usePreparedStmt  (SQLDBC_TRUE),       
        m_batch     (SQLDBC_FALSE),          
        m_rollbackOnError (SQLDBC_FALSE),
        m_noComment(SQLDBC_FALSE),
        m_noPager(SQLDBC_FALSE),
        m_multilinemode(SQLDBC_FALSE),
        m_alignedOutput(SQLDBC_TRUE),
        m_maxLongLenth(10),
        m_ifrprops(),
        m_debug     (SQLDBC_FALSE),
        m_editFileIsTmpFile (SQLDBC_FALSE),
        m_connectState(SQLDBC_FALSE),
        m_pagerOutput(SQLDBC_FALSE),
        m_versionInfo(SQLDBC_FALSE),
		m_exitCode(0),
		m_ExitForced(SQLDBC_FALSE),
        m_StopOnError(SQLDBC_FALSE),
        teo81_EvalProgArgs( argc, argv, 
                           (teo81_EvalProgArgs::teo81_OptionDesc*)&Options, 
                            NUM_OF_OPTIONS_EO80(Options),
                            option_help )
{
 setuser          ("", 0);
 setdbname        ("", 0);
 setdbhost        ("", 0);
 setxuserkey      ("", 0);
 setnullvalue     ("?", 1);
 setcolumnprefix  ("| ",2);
 setcolumnsuffix  (" " ,1);
 setrowprefix     ("",2);
 setrowsuffix     ("|\n",2);
 setcmdseparator  ("//",2);
// setrecordsep     ("\n",1);
 setsearchpattern ("", 0);
 setlogo          ("", 0); 
 setResultHeader  ("", 0);
 setResultFooter  ("\n", 1);
#ifdef WIN32 
 setdefaulteditor ("notepad.exe"); 
#else
 setdefaulteditor ("vi"); 
#endif
 m_ifrprops.setProperty("SQLMODE", "INTERNAL");
 m_ifrprops.setProperty("STATEMENTCACHESIZE", "0");
 m_ifrprops.setProperty("SELECTFETCHOPTIMIZE", "1");
 m_ifrprops.setProperty("CHOPBLANKS", "1"); 

 if (m_environment == 0){ 
     char errorText[256];
     m_environment=new SQLDBC_Environment(SQLDBC_SingleThreadedRuntime::GetSingleThreadClientRuntime(errorText, sizeof(errorText)));
//     m_environment=new SQLDBC_Environment(SQLDBC::GetClientRuntime(errorText, sizeof(errorText)));
 }
//  m_properties.setProperty  ("COMPNAME",  "CLI"  ); 
  if(this->eo80_ParseArguments() == SQLDBC_OK) {}
};


/*---------------------------------------------------------------------------*/

bool IFTools_sqlcliProperties::eo80_vNextValidOption ( const int            OptionIndex,
                                                const TCHAR*         OptionArg )
{
  bool   Ok = true;
  char*  OptionArgument = (char*)OptionArg;

  switch (OptionIndex)
  {
    case option_userpasswd      : { 
      setuser(OptionArgument, strlen(OptionArgument));  
      break;
    }
    case option_dbname          : { 
      setdbname(OptionArgument, strlen(OptionArgument));  
      break;
    }
    case option_dateformat      : {
      this->m_dbname = OptionArgument;
      break;
    }  
    case option_trace           : {
        m_environment->getTraceOptions(m_traceprops);
        m_traceprops.setProperty("SQL", "TRUE");
        if(OptionArgument && strlen(OptionArgument)) {
            m_traceprops.setProperty("FILENAME", OptionArgument);
        }
        m_environment->setTraceOptions(m_traceprops);
        break;
    }  
    case option_separator       : {
      setfieldseparator(OptionArgument, strlen(OptionArgument));
      break;
    }  
    case option_cmdseparator    : {
      setcmdseparator(OptionArgument, strlen(OptionArgument));
      break;
    }
    case option_nullval         : {
      setnullvalue(OptionArgument, strlen(OptionArgument));
      break;
    }  
    case option_autocommitOff   : {
      this->m_autocommit = false;
      break;
    }  
    case option_noHeader        : {
      this->m_noHeader = true;
      break;
    }  
	case option_useDBSStmt      : {
		this->m_usePreparedStmt = SQLDBC_FALSE;
      break;
    }  
    case option_batch           : { 
      setinfile(OptionArgument);
      m_noComment = true;
      break;
    }
    case option_nopager           : { 
      m_noPager = true;
      break;
    }
    case option_outputfile      : { 
      setoutfile(OptionArgument);
      break;
    }
    case option_stopOnError     : {
      setStopOnError(SQLDBC_TRUE);
      setExitCode(atoi(OptionArgument));
      break;
    }
    case option_oneColperLine   : {
      setcolumnprefix  ("",0);
      setrowprefix     ("",0);
      setcolumnsuffix  ("\n",1);
      setrowsuffix     ("\n",1);
      break;
    }
    case option_rollbackOnError : {
      this->m_rollbackOnError = true;
      break;
    }
    case option_writeCmdProt    : {
      this->m_cmdtrace = true;
      break;
    }
    case option_xuserkey        : {
      setxuserkey(OptionArgument, strlen(OptionArgument));
      this->m_ifrprops.setProperty("KEY", getxuserkey());
      break;
    }
    case option_timeout        : {
      this->m_ifrprops.setProperty("TIMEOUT", OptionArgument);
      break;
    }
    case option_dbhost          : {
      setdbhost (OptionArgument, strlen(OptionArgument));
      break;
    }  
    case option_sqlmode          : {
      this->m_ifrprops.setProperty("SQLMODE", OptionArgument); 
      break;
    }  
    case option_sqlcmd          : {
      setcommand(OptionArgument, strlen(OptionArgument));
      break;
    }  
    case option_debug    : {
      this->m_debug = true;
      break;
    }
    case option_multilinemode    : {
      this->m_multilinemode = true;
      break;
    }
    case option_maxLongLen    : {
      setmaxLongLenth(OptionArgument);
      break;
    }
    case option_nocomment    : {
      this->m_noComment = true;
      this->m_cmdtrace = false;
      break;
    }
    case option_UnAligned    : {
      this->setAlignedOutput(SQLDBC_FALSE);
      break;
    }
    case option_logo    : {
      this->setlogo(OptionArgument, strlen(OptionArgument));
      break;
    }    
    case option_version : {
      m_versionInfo=SQLDBC_TRUE;
      break;
    }
    default: {
      printf ( _T("Unknown option index '%d'\n"), OptionIndex  );
      Ok = false;
      break;
    }
  }

  return Ok;
}

//-----------------------------------------------------------------------
bool IFTools_sqlcliProperties::eo80_vParseArgumentsFinished  ( const _TCHAR* const* ArgvRemaining,
                                             int                  ArgsRemaining,
                                             int                  ArgsParsed,
                                             bool                 BreakOptionFound ){ 
  bool Ok = true;
  if (( ArgsRemaining > 0 )) {
    SQLDBC_Int4 cmdlen=0;
    int i,len,pos;
    for (i=0;i<ArgsRemaining;cmdlen+=strlen(ArgvRemaining[i])+1,i++){};
    delete [] this->m_command;
    this->m_command = new char[cmdlen];
    memset (this->m_command,' ',cmdlen);
    for (i=0,pos=0;i<ArgsRemaining;i++){
      len = strlen(ArgvRemaining[i]);
      strncpy(&m_command[pos],ArgvRemaining[i],len);
      pos+=len+1;       
    }
    this->m_command[cmdlen-1]='\0';
  }
  return Ok;
}  
static  char* mapArrUtf (int arrIdx) {
     tsp00_Int4	i, val=0, num[] = {7,53, 97, 193, 389, 769, 1543, 3079, 0 };
    
     for (i=0; i<arrIdx && i< sizeof(Utfmap)/sizeof(char); i++){
       val = (val<<5) +num[(i%(sizeof(num)/sizeof(tsp00_Uint4)))] + Utfmap[i];
     }
     return Utfmap;               
}                

//-----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setoutfile(const char * outfilename){
  fflush(m_outfile);
  if (m_outfile != stdout && m_outfile != stderr ){
    fclose(m_outfile);
  }
  if (!strcmp(outfilename,"stderr")){
    m_outfile = stderr;
  } else if (!strcmp(outfilename,"stdout")) {
    m_outfile = stdout;
  } else {
    if ( (m_outfile = fopen(outfilename,"w")) == 0) {
      m_outfile = stdout;
      fprintf(m_errfile,ERR_OPEN_FILE, outfilename,strerror( errno ) );
      return SQLDBC_FALSE;
    }
  }
  return SQLDBC_TRUE;
}
//-----------------------------------------------------------------------
void IFTools_sqlcliProperties::dumpClass(){
      
    fprintf(m_errfile,"\nUsername  : \"%s\"\n", m_username);                     
//    printf("Password  : \"%s\"\n", m_password);
    fprintf(m_errfile,"DB name   : \"%s\"\n", m_dbname);
    fprintf(m_errfile,"hostname  : \"%s\"\n", m_dbhost);
    fprintf(m_errfile,"xuserkey  : \"%s\"\n", m_xuserkey);
    fprintf(m_errfile,"nullvalue : \"%s\"\n", m_nullvalue);
    fprintf(m_errfile,"fieldsep  : \"%s\"\n", m_fieldseparator);
    fprintf(m_errfile,"cmdsep    : \"%s\"\n", m_cmdseparator);
    fprintf(m_errfile,"dateformat: \"%d\"\n",  m_dateformat);
//    FILE *               m_errfile;           //!< File fuer "stderr" 
    fprintf(m_errfile,"CMDTrace  : \"%s\"\n",  (m_cmdtrace  ==true)?"TRUE":"FALSE");
    fprintf(m_errfile,"autocommit: \"%s\"\n",  (m_autocommit==true)?"TRUE":"FALSE");
    fprintf(m_errfile,"noHeader  : \"%s\"\n",  (m_noHeader  ==true)?"TRUE":"FALSE");
	fprintf(m_errfile,"useDBS    : \"%s\"\n",  (m_usePreparedStmt ==true)?"TRUE":"FALSE");
    fprintf(m_errfile,"batch     : \"%s\"\n",  (m_batch     ==true)?"TRUE":"FALSE");

    fprintf(m_errfile,"rollbOnErr: \"%s\"\n",  (m_rollbackOnError==true)?"TRUE":"FALSE");
    fprintf(m_errfile,"SQL mode  : \"%s\"\n",  m_ifrprops.getProperty("SQLMODE"));
/*    fprintf(m_errfile,"Screensize: \"%d\"\n",  getConsoleScreenSize()); */
    fprintf(m_errfile,"Command   : \"%s\"\n", (m_command)?m_command:"NULL");
}

//----------------------------------------------------------------------
char * IFTools_sqlcliProperties::trimString(char* val){
  if (! val) return 0;
  size_t len=strlen(val);
  for (;(len>0 && isspace(val[0]));val++, len--){};    
  for (;(len>0 && isspace(val[len-1]));val[--len]='\0'){};
  return val;    
}

//----------------------------------------------------------------------
  SQLDBC_Bool IFTools_sqlcliProperties::setinfile (const char * filename){
  if (m_infile != stdin){
    fclose(m_infile);
  }
  if (!strcmp(filename,"stdin")){
    m_infile = stdin;
  } else {
    if ( (m_infile = fopen(filename,"r")) == 0) {
      m_infile = stdin;
      fprintf(m_errfile,ERR_OPEN_FILE, filename ,strerror( errno ) );
      return SQLDBC_FALSE;
    }
  }
  return SQLDBC_TRUE;
  }
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setuser(const char * userName, SQLDBC_Int4 userLen){
  SQLDBC_Int4 i,usernameLen, passwdLen;
  char * tmpBuf = new char[userLen+1];
  strncpy (tmpBuf, userName, userLen);
  tmpBuf[userLen]='\0';
  teo80_SeparateArgs   SeparateArgs(tmpBuf, _T(','));
  char*         user   = trimString( (char*)SeparateArgs.NextPart());
  char*         passwd = trimString( (char*)SeparateArgs.NextPart());

  if (user &&(usernameLen = strlen(user))){
    if (user[0]!= '"' || user[usernameLen-1] != '"'){
      for (i=0; i<usernameLen; i++) user[i]=toupper(user[i]);
      setCharProperty(&m_username, user,usernameLen);
    } else {
      setCharProperty(&m_username, &user[1],usernameLen-2);
    }
  } else {
    setCharProperty(&m_username, "",0);
  }

  if (passwd && (passwdLen = strlen(passwd))){
    if (passwd[0]!= '"' || passwd[passwdLen-1] != '"'){
      for (i=0; i<passwdLen; i++) passwd[i]=toupper(passwd[i]);
      setCharProperty(&m_password, passwd, passwdLen);
    } else {
      setCharProperty(&m_password, &passwd[1],passwdLen-2);
    }
} else {
    setCharProperty(&m_password, "",0);
  }
  return SQLDBC_TRUE;   
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setCharProperty(char ** prop, const char* value, SQLDBC_Int4 len){
  delete [] *prop;
  *prop = 0;
  if (value[0]== '\"' && value[len-1] == '\"'){
    value++; len -= 2;
  }
  if (value){
    *prop = new char[len+1];
    strncpy (*prop, value, len);
    (*prop)[len]='\0';
  }
  return SQLDBC_TRUE;  
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setSeparator(char ** prop, const char* invalue, SQLDBC_Int4 len){
  char *src = (char*)invalue;
  char *dest = (char*) alloca(len+1);
  int slen, dlen;

  if (!src || !len){
    return setCharProperty(prop, "",0);  
  }

  if (src[0]== '\"' && src[len-1] == '\"'){
    src++; len -= 2;
  }
  
  for (slen=0, dlen=0; slen < len; slen++, dlen++){
    if (len-slen >= 2 && src[slen]=='\\' ){
      if (src[slen+1]=='n') 
        dest[dlen] = '\n', slen++; 
      else if (src[slen+1]=='b')
        dest[dlen] = '\b', slen++; 
      else if (src[slen+1]=='r')
        dest[dlen] = '\r', slen++; 
      else if (src[slen+1]=='a')
        dest[dlen] = '\a', slen++; 
      else if (src[slen+1]=='f')
        dest[dlen] = '\f', slen++; 
      else if (src[slen+1]=='t')
        dest[dlen] = '\t', slen++; 
      else 
        dest[dlen]=src[slen];  
    } else {
      dest[dlen]=src[slen];
    }   
  }
  return setCharProperty(prop, dest, dlen);  
}

//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setmaxLongLenth         (const char* value){
  if (value) {
    if (strlen(value)==3 && (!strncmp ("all",value,3) || !strncmp ("ALL",value,3))){
      m_maxLongLenth = IFTOOLS_PROP_WHOLE_LONG;
    } else {
      char *stopstring; 
      long val = strtol(value ,&stopstring, 10);
      m_maxLongLenth = (val)?val:10;
    }
  }
  return true;  
}

//----------------------------------------------------------------------
SQLDBC_Bool  IFTools_sqlcliProperties::setlogo(const char * name, SQLDBC_Int4 len){
  if (len>=4 && ! strncmp(name,"logo",4)){
    if (len >= 6 && ! strcmp("SAP DB","<PRODUCT_NAME>")){
      char* logo = (char*)
        "   _____             ____  ____    \n"
        "  / ___/____ _____  / __ \\/ __ )  \n"
        "  \\__ \\/ __ `/ __ \\/ / / / __  |\n"
        " ___/ / /_/ / /_/ / /_/ / /_/ /  \n" 
        "/____/\\__,_/ .___/_____/_____/  \n" 
        "          /_/                    \n"; 
        setCharProperty(&m_logo, logo, strlen(logo));
    } else {
        char* logo = (char*)
        "    __  ___           ____  ____ \n"
        "   /  |/  /___ __  __/ __ \\/ __ )\n"
        "  / /|_/ / __ `/ |/_/ / / / __  |\n"
        " / /  / / /_/ />  </ /_/ / /_/ / \n"
        "/_/  /_/\\__,_/_/|_/_____/_____/  \n"
        "                                  \n"; 
        setCharProperty(&m_logo, logo, strlen(logo));
    }
  } else if (len>=4 && ! strncmp(name,"btmtja",6)){
      setCharProperty(&m_logo, mapArrUtf (0), 368);
  } else {  
    setCharProperty(&m_logo,"",0);
  } 
  return SQLDBC_TRUE;
}
//----------------------------------------------------------------------
void   IFTools_sqlcliProperties::removeTmpEditFile(){
  if (m_editFileIsTmpFile){
    errno=0;
    if (remove(geteditfilename()) == -1)
		{
			fprintf(m_errfile ,ERR_REMOVE_FILE , geteditfilename(), strerror(errno));
		}
		m_editFileIsTmpFile = SQLDBC_FALSE;
  }
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::seteditfilename  (const char * name){
#ifndef WIN32
  tsp00_Path tmpfilename;
#else
  char tmpfilename[MAX_PATH];
#endif
	const char* filename;

  this->removeTmpEditFile();
	if (name){
	  m_editFileIsTmpFile = SQLDBC_FALSE;
		filename = name;
	}	
	else	{
	  m_editFileIsTmpFile = SQLDBC_TRUE;
#ifndef WIN32
		char *tmpdirenv = getenv("TMP");
		if (tmpdirenv && strlen(tmpdirenv)>(sizeof(tmpfilename)-25)) tmpdirenv = 0;
		sprintf(tmpfilename,"%s/sqlcli.tmp.%ld.%ld",
				 tmpdirenv ? tmpdirenv : "/tmp",
				 (long) geteuid(), (long) getpid());
#else
    char tmpdir[MAX_PATH];
    DWORD pathlen = GetTempPath(sizeof(tmpdir), tmpdir);
    GetTempFileName(tmpdir, "sqlcli", 0, tmpfilename);
		if (m_debug) fprintf(m_outfile,"name of temporary file:%s\n",tmpfilename);
#endif
    filename = tmpfilename;
  }
  return setCharProperty(&m_editFileName, filename, strlen(filename));
}
//----------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setAlignedOutput(SQLDBC_Bool alignment){
  if (alignment) {
    m_alignedOutput = SQLDBC_TRUE;
    setcolumnprefix  ("| ",2);
    setcolumnsuffix  (" " ,1);
    setrowsuffix     ("|\n",2);
    setrowprefix     ("",0);
  } else{
    m_alignedOutput = SQLDBC_FALSE;
    setcolumnprefix  ("",0);
    setrowprefix     ("",0);
    setcolumnsuffix  ("," ,1);
    setrowsuffix     ("\n",1);
  }
  return SQLDBC_TRUE;
}
//--------------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setsearchpattern(const char * schemaPattern, SQLDBC_Int4 patternLen){
  SQLDBC_Int4 i,schemanameLen, tableLen;
  char * tmpBuf = new char[patternLen+1];
  strncpy (tmpBuf, schemaPattern, patternLen);
  tmpBuf[patternLen]='\0';
  teo80_SeparateArgs   SeparateArgs(tmpBuf, _T('.'));
  char* schema = trimString( (char*)SeparateArgs.NextPart());
  char* table  = trimString( (char*)SeparateArgs.NextPart());

  if (schema && !table){
    table = schema;
    schema = 0;
  }
  
  if (schema &&(schemanameLen = strlen(schema))){
    if (schema[0]!= '"' || schema[schemanameLen-1] != '"'){
      for (i=0; i<schemanameLen; i++) schema[i]=toupper(schema[i]);
      setCharProperty(&m_searchSchema, schema,schemanameLen);
    } else {
      setCharProperty(&m_searchSchema, &schema[1],schemanameLen-2);
    }
  } else {
    setCharProperty(&m_searchSchema, "%",1);
  }

  if (table && (tableLen = strlen(table))){
    if (table[0]!= '"' || table[tableLen-1] != '"'){
      for (i=0; i<tableLen; i++) table[i]=toupper(table[i]);
      setCharProperty(&m_searchTable, table, tableLen);
    } else {
      setCharProperty(&m_searchTable, &table[1],tableLen-2);
    }
  } else {
    setCharProperty(&m_searchTable, "%",1);
  }
  return SQLDBC_TRUE;   
}
//--------------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setResultHeader(const char * name, SQLDBC_Int4 len){
  return setSeparator(&m_resultheader, name, len);
}
//--------------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setResultFooter(const char * name, SQLDBC_Int4 len){
  return setSeparator(&m_resultfooter, name, len);
}
//--------------------------------------------------------------------------
SQLDBC_Bool IFTools_sqlcliProperties::setfieldseparator   (const char * name, SQLDBC_Int4 len){
  if(!len || !name){
    setcolumnprefix  ("| ",2);
    setcolumnsuffix  (" " ,1);
    setrowsuffix     ("|\n",2);
  }else{
    char *dest = (char*) alloca(len+1);
    strncpy(dest,name,len);
    dest[len]=' ';
    this->setcolumnprefix (dest,len+1);
    this->setcolumnsuffix (" " ,1);
    dest[len]='\n';
    this->setrowsuffix (dest,len+1);
  }
  return SQLDBC_TRUE;   
}
//--------------------------------------------------------------------------
void IFTools_sqlcliProperties::exitProgram(){
	exit(this->m_exitCode);
}
//--------------------------------------------------------------------------
#ifdef _WIN32
#define POPEN(a) _popen (a, "rt")
#define PCLOSE _pclose
#else
#include <errno.h>
#define POPEN(a) popen (a, "r")
#define PCLOSE pclose
#endif

int IFTools_sqlcliProperties::executeCommand (char* cmd){
   char   psBuffer[128];
   FILE   *pPipe;

  if( (pPipe = POPEN(cmd)) == NULL ){
     fprintf(m_errfile,ERR_EXEC_CMD, cmd, strerror( errno ) );
     return 1;
  }

   while( !feof( pPipe ) )
   {
       if( fgets( psBuffer, 128, pPipe ) != NULL ){
         fprintf(m_outfile,"%.*s", sizeof(psBuffer), psBuffer );
       }
   }

   return PCLOSE( pPipe );
}


/*
#SQLDBC_Int4 IFTools_sqlcliProperties::getConsoleScreenSize(){
#ifdef WIN32 
  HANDLE hStdout; 
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
     
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
  // Get the current screen buffer size and window position. 
  if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) 
      return 0; 
  return  (csbiInfo.srWindow.Bottom-csbiInfo.srWindow.Top+1);
#else
  SQLDBC_Int4 lineNum;
  if(!(initscr())) {   
    endwin();
    return 0;
  }
  lineNum = (SQLDBC_Int4) LINES;
  endwin();
  setbuf(stdout, NULL);
  return lineNum;
#endif  
}
*/
