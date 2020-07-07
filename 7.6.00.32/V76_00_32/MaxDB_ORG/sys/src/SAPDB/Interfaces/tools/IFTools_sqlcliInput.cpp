/*!**************************************************************************

module      : IFTools_sqlcliInput.cpp

-------------------------------------------------------------------------

responsible : D031096

special area:
description : Interface Runtime 8.0

description:  Provides a class that handles input for command line tool sqlcli

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

#include "SAPDB/Interfaces/tools/IFTools_sqlcliInput.h"

/*
#ifdef _WIN32
#include <conio.h>
#define GETCH _getch
#else
#include <curses.h>
#define GETCH getch
#endif
#include "heo06.h"
*/
//----------------------------------------------------------------------

#define CMDKEYWORDTABLESIZE        (sizeof(mycmdKeywordTab)     /sizeof(cmdKeywordTable))
#define SQLMODEKEYWORDTABLESIZE    (sizeof(sqlmodeKeywordTab)   /sizeof(cmdKeywordTable))
#define AUTOCOMMITKEYWORDTABLESIZE (sizeof(autocommitKeywordTab)/sizeof(cmdKeywordTable))
#define CONNECTKEYWORDTABLESIZE    (sizeof(connectKeywordTab)   /sizeof(cmdKeywordTable))

static SQLDBC_Bool cmdKeywordTabIsInit = SQLDBC_FALSE;

IFTools_sqlcliInput::cmdKeywordTable IFTools_sqlcliInput::sqlmodeKeywordTab [ ]= {
  { SQLDBC_ANSI,     {{"A"        ,0},{"",0},{"",0}} },
  { SQLDBC_ANSI,     {{"ANSI"     ,0},{"",0},{"",0}} },
  { SQLDBC_DB2,      {{"D"        ,0},{"",0},{"",0}} },
  { SQLDBC_DB2,      {{"DB2"      ,0},{"",0},{"",0}} },
  { SQLDBC_INTERNAL, {{"I"        ,0},{"",0},{"",0}} },
  { SQLDBC_INTERNAL, {{"INTERNAL" ,0},{"",0},{"",0}} },
  { SQLDBC_ORACLE,   {{"O"        ,0},{"",0},{"",0}} },
  { SQLDBC_ORACLE,   {{"ORACLE"   ,0},{"",0},{"",0}} }
};

IFTools_sqlcliInput::cmdKeywordTable IFTools_sqlcliInput::autocommitKeywordTab [ ]= {
  { autocommit_off,    {{"0"      ,0},{"",0},{"",0}} },
  { autocommit_on,     {{"1"      ,0},{"",0},{"",0}} },
  { autocommit_off,    {{"OFF"    ,0},{"",0},{"",0}} },
  { autocommit_on,     {{"ON"     ,0},{"",0},{"",0}} }
};

IFTools_sqlcliInput::cmdKeywordTable IFTools_sqlcliInput::connectKeywordTab [ ]= {
  { connect_dbname,    {{"-D"     ,0},{"",0},{"",0}} },
  { connect_dbhost,    {{"-N"     ,0},{"",0},{"",0}} },
  { connect_user,      {{"-U"     ,0},{"",0},{"",0}} },
  { connect_xuser,     {{"-X"     ,0},{"",0},{"",0}} }
};

IFTools_sqlcliInput::cmdKeywordTable IFTools_sqlcliInput::mycmdKeywordTab [ ]= {
  { cmdtype_shellcmd,   {{"!"         ,0},{"",0},{"",0}} },
  { cmdtype_help,       {{"?"         ,0},{"",0},{"",0}} },
  { cmdtype_autocommit, {{"A"         ,0},{"",0},{"",0}} },
  { cmdtype_alignment,  {{"AL"        ,0},{"",0},{"",0}} },
  { cmdtype_alignment,  {{"ALIGN"     ,0},{"",0},{"",0}} },
  { cmdtype_autocommit, {{"AUTOCOMMIT",0},{"",0},{"",0}} },
  { cmdtype_connect,    {{"C"         ,0},{"",0},{"",0}} },
  { cmdtype_connect,    {{"CONNECT"   ,0},{"",0},{"",0}} },
  { cmdtype_showColumns,{{"DC"        ,0},{"",0},{"",0}} },
  { cmdtype_showIndices,{{"DE"        ,0},{"",0},{"",0}} },
  { cmdtype_disconnect, {{"DI"        ,0},{"",0},{"",0}} },
  { cmdtype_disconnect, {{"DISCONNECT",0},{"",0},{"",0}} },
  { cmdtype_showProcs,  {{"DP"        ,0},{"",0},{"",0}} },
  { cmdtype_showSchemas,{{"DS"        ,0},{"",0},{"",0}} },
  { cmdtype_showTables, {{"DT"        ,0},{"",0},{"",0}} },
  { cmdtype_showUsers,  {{"DU"        ,0},{"",0},{"",0}} },
  { cmdtype_showViews,  {{"DV"        ,0},{"",0},{"",0}} },
  { cmdtype_edit,       {{"E"         ,0},{"",0},{"",0}} },
  { cmdtype_edit,       {{"EDIT"      ,0},{"",0},{"",0}} },
  { cmdtype_shellcmd,   {{"EXEC"      ,0},{"",0},{"",0}} },
  { cmdtype_exit,       {{"EXIT"      ,0},{"",0},{"",0}} },
  { cmdtype_fieldsep,   {{"F"         ,0},{"",0},{"",0}} },
  { cmdtype_fieldsep,   {{"FIELDSEP"  ,0},{"",0},{"",0}} },
  { cmdtype_help,       {{"H"         ,0},{"",0},{"",0}} },
  { cmdtype_help,       {{"HELP"      ,0},{"",0},{"",0}} },
  { cmdtype_input,      {{"I"         ,0},{"",0},{"",0}} },
  { cmdtype_input,      {{"INPUT"     ,0},{"",0},{"",0}} },
  { cmdtype_sqlmode,    {{"M"         ,0},{"",0},{"",0}} },
  { cmdtype_sqlmode,    {{"MODE"      ,0},{"",0},{"",0}} },
  { cmdtype_multiline,  {{"MU"        ,0},{"",0},{"",0}} },
  { cmdtype_multiline,  {{"MULTILINE" ,0},{"",0},{"",0}} },
  { cmdtype_outfile,    {{"O"         ,0},{"",0},{"",0}} },
  { cmdtype_outfile,    {{"OUTPUT"    ,0},{"",0},{"",0}} },
  { cmdtype_print,      {{"P"         ,0},{"",0},{"",0}} },
  { cmdtype_pager,      {{"PA"        ,0},{"",0},{"",0}} },
  { cmdtype_pager,      {{"PAGER"     ,0},{"",0},{"",0}} },
  { cmdtype_usePreparedStmt, {{"PREPARED"  ,0},{"",0},{"",0}} },
  { cmdtype_print,      {{"PRINT"     ,0},{"",0},{"",0}} },
  { cmdtype_usePreparedStmt, {{"PS"        ,0},{"",0},{"",0}} },
  { cmdtype_exit,       {{"Q"         ,0},{"",0},{"",0}} },
  { cmdtype_exit,       {{"QUIT"      ,0},{"",0},{"",0}} },
  { cmdtype_reset,      {{"R"         ,0},{"",0},{"",0}} },
  { cmdtype_reset,      {{"RESET"     ,0},{"",0},{"",0}} },
  { cmdtype_status,     {{"S"         ,0},{"",0},{"",0}} },
  { cmdtype_status,     {{"STATUS"    ,0},{"",0},{"",0}} }
};

const char * IFTools_sqlcliInput::getshortcutHelp(){
  return (
    " \n \\? or \\h[elp]  show help on internal slash commands\n"  
    " \\q[uit]        quit SQLCLI\n"
    " \\c[onnect]     -n <database_server>[:<port>]\n"
    "                -d <database_name>\n" 
    "                -u <user_name,password>\n"
    "                connecting to the database\n"
    " \\di[sconnect]  disconnecting from the database\n"
    "\n"
    " \\mu[ltiline]   [ON|OFF] toggle the multiline mode on or off\n"
    " \\a[utocommit]  [ON|OFF] switch autocommit mode on or off\n"
    " \\m[ode]        [INTERNAL|ORACLE|ANSI|DB2] change SQL mode\n"
    " \\ps            [ON|OFF] toggle the usage of prepared statements on or off\n"
    " \\o[utput]      <filename> send all query results to file\n"
    " \\i[nput]       <filename> read input from the file <filename>\n"
    "\n"
    " \\p[rint]       print out the current query buffer\n"
    " \\r[eset]       reset (clear) the query buffer\n"
    " \\e[dit]        <filename>  edit the query buffer (or file) with external editor\n"
    " \\g             send query buffer to server (and print out the results)\n"
    "\n"
    " \\al[ign]       [ON|OFF] toggle the aligned output on or off\n"
    " \\pa[ger]       [ON|OFF] toggle page by page scroll output on or off\n"
    " \\f[ieldsep]    <separator> use <separator> as the field separator\n"
    " \\s[tatus]      print out host, database, user etc.\n"
    " \\! <command>   execute <command> in shell\n"
    "\n"
    " \\dc [PATTERN]  list columns\n"
    " \\de [PATTERN]  list indices\n"
    " \\dp [PATTERN]  list procedures\n"
    " \\ds [NAME]     list schemas\n"
    " \\dt [PATTERN]  list tables\n"
    " \\du [NAME]     list users\n"
    " \\dv [PATTERN]  list views\n"
    " [PATTERN] = [OWNER.][OBJECT NAME] eg. <username>.%A%\n\n"
    );
}
                    
static void initCMDKeywordTable (IFTools_sqlcliInput::cmdKeywordTable *keywordTable, 
                                 SQLDBC_Int4 numberElements)
{
  SQLDBC_Int4 i;  
  for (i=0; i< numberElements; i++)
  {
    SQLDBC_Int4 keyLen = (SQLDBC_Int4) strlen(keywordTable[i].keyword[0].rawString);
    keywordTable[i].keyword[0].length = (SQLDBC_Int4) keyLen;
  }
  cmdKeywordTabIsInit = SQLDBC_TRUE;
}
//----------------------------------------------------------------------



SQLDBC_Bool IFTools_sqlcliInput::startprocess (const char **args) {



   SQLDBC_Int4 pos = 0;
   char                  cmdline[1024];
   char *space = (char *)"";

# if defined(_WIN32) /*------- _WIN32 -------------------*/
   STARTUPINFO             StartupInfo;
   BOOL                    Ok;
   PROCESS_INFORMATION     m_ProcInfo;
 
   GetStartupInfo (&StartupInfo);
   memset ( &m_ProcInfo, 0, sizeof(m_ProcInfo) );

   StartupInfo.lpTitle = cmdline;
   StartupInfo.dwFlags = 0;
#else
   pos = sp77sprintf(&cmdline[pos], sizeof(cmdline)-pos,"exec ");
#endif
   
   for (int i = 0; args[i] != 0 ; i++){
     pos += sp77sprintf(&cmdline[pos], sizeof(cmdline)-pos,"%s\"%s\"", space, args[i]);
     space = (char *)" ";
   }
   cmdline[pos] = 0;
   
# if defined(_WIN32) /*------- _WIN32 -------------------*/
   Ok = CreateProcess ( 0, cmdline, 0, 0, TRUE, CREATE_NO_WINDOW,
                        0, 0, &StartupInfo, &m_ProcInfo);
   if (Ok == 0){
      LPVOID lpMsgBuf;
      DWORD dw = GetLastError(); 

      FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
      fprintf(m_sqlcliProp->geterrfile(),"could not execute command \"%s\" Error: %d: %s\n", cmdline, dw, lpMsgBuf);
      LocalFree(lpMsgBuf);
      return SQLDBC_FALSE;
   }
   // Wait until child process exits.
   WaitForSingleObject( m_ProcInfo.hProcess, INFINITE );

   // Close process and thread handles. 
   CloseHandle( m_ProcInfo.hProcess );
   CloseHandle( m_ProcInfo.hThread );


# else  /*--------------------- UNIX --------------------*/

   errno = 0;
   int erg = system(cmdline);
   if (errno != 0){
     fprintf(m_sqlcliProp->geterrfile(),"could not execute command \"%s\" Error: %d: %s\n", cmdline, errno, strerror(errno));
     return SQLDBC_FALSE;
   }
#endif /*----------------- _WIN32/UNIX -----------------*/
   return SQLDBC_TRUE;
}

//----------------------------------------------------------------------
void IFTools_sqlcliInput::printSplashScreen(FILE * out){

  fprintf(out,"%s%s",
    m_sqlcliProp->getlogo(),
    "\nWelcome to the <PRODUCT_NAME> interactive terminal.\n"
    "                                           \n"
    "Type:  \\h for help with commands          \n"
    "       \\q to quit                         \n\n" ); 
}
//----------------------------------------------------------------------

class IFTools_sqlcliSimpleInput : public IFTools_sqlcliInput {
public:
  IFTools_sqlcliSimpleInput(IFTools_sqlcliProperties *sqlcliProp):
      IFTools_sqlcliInput(sqlcliProp),
        m_alreadyExecuted(SQLDBC_FALSE) {}

        ~IFTools_sqlcliSimpleInput   (){}

        virtual char* getnextCMD(){
          char* erg;
          if (m_alreadyExecuted){
            this->m_cmdtype=cmdtype_exit;
            return 0;
          }  
          else {
            m_alreadyExecuted = SQLDBC_TRUE; 
            erg = this->m_sqlcliProp->getcommand();
            this->analyzeSQL(erg);
            return erg;
          } 
        } 
private:
  SQLDBC_Bool m_alreadyExecuted;
}; 
//----------------------------------------------------------------------

class IFTools_sqlcliInputInteractive : public IFTools_sqlcliInput {
public:
  IFTools_sqlcliInputInteractive(IFTools_sqlcliProperties *sqlcliProp):
      IFTools_sqlcliInput(sqlcliProp),
      m_alreadyExecuted (SQLDBC_FALSE),
      m_curLen (0),
      m_multiLinePrompt(SQLDBC_FALSE) {
#ifndef WIN32
        setbuf(stdout, NULL);
#endif
      }
      
      int printPrompt(){
        if (m_sqlcliProp->isReadingFromStdin())
          if (m_multiLinePrompt)
            return fprintf (stdout,"> ");
          else{
            const char * dbname =  (m_sqlcliProp->isConnected())?m_sqlcliProp->getdbname():"";
            return fprintf (stdout,"sqlcli%s%s=> ",(strlen(dbname))?" ":"",dbname);
          }  
        return 1;
      }

      SQLDBC_Bool editQueryBuffer(){
        char* filename = m_sqlcliProp->geteditfilename();
        char* editorName;
        char* editcmd[3];
        FILE* tmpFile = 0;
        if ( (tmpFile = fopen(filename,"w")) == 0) {
          fprintf(m_sqlcliProp->geterrfile(),"open of %s failed: %d %s\n", m_sqlcliProp->geteditfilename(), errno ,strerror( errno ) );
          return SQLDBC_FALSE;
        }
        fprintf(tmpFile,"%s", m_stmt);
        fflush(tmpFile);
        fclose(tmpFile);

      	editorName = getenv("SQLCLI_EDITOR");
      	if (!editorName)
		      editorName = getenv("EDITOR");
	      if (!editorName)
		      editorName = getenv("VISUAL");
	      if (!editorName)
		      editorName = m_sqlcliProp->getdefaulteditor();

          editcmd [0]   = editorName;
          editcmd [1]   = filename;
          editcmd [2]   = 0;

        if (this->startprocess ((const char**)editcmd)== SQLDBC_FALSE){
          return SQLDBC_FALSE;
        }
          
        tmpFile = fopen(filename, "r");
		    if (!tmpFile) {
			    fprintf(m_sqlcliProp->geterrfile(),ERR_OPEN_FILE, filename, strerror(errno));
			    return SQLDBC_FALSE;
		    } else {
          m_curLen = 0;
          while (fgets(m_stmt+m_curLen,sizeof(m_stmt)-2-m_curLen,tmpFile) )  {
            m_curLen += strlen (m_stmt+m_curLen);
            m_stmt[m_curLen]=' ';
          }
          m_stmt[m_curLen]='\0';
          if (ferror(tmpFile)) {
			      fprintf(m_sqlcliProp->geterrfile(),ERR_READ_FILE, filename, strerror(errno));
			      fclose(tmpFile);
			      return SQLDBC_FALSE;
  	      }
		      fclose(tmpFile);
       }
       m_sqlcliProp->removeTmpEditFile();
       return SQLDBC_TRUE;
      }
#ifdef _WIN32
#define CRVALUE 0x0D
#define PUTCH _putch
#else
#define CRVALUE 0x0A
#define PUTCH _putch
#endif
      virtual char* getnextCMD(){
        if (!m_alreadyExecuted){
          m_alreadyExecuted= SQLDBC_TRUE;
          IFTools_sqlcliInput::printSplashScreen(this->m_sqlcliProp->getoutfile());
          if (   !m_sqlcliProp->isConnected() 
              && m_sqlcliProp->isReadingFromStdin()
              && !strlen(m_sqlcliProp->getpassword())
              && strlen(m_sqlcliProp->getusername())
              && strlen(m_sqlcliProp->getdbname())){

          tsp00_Namec passwd;
          tsp05_RteFileError err;
          tsp00_Bool erg = sqlgetpass("Enter password: ", passwd, err);
          size_t len = sprintf(&m_curLine[0],"\"%s\",%s",m_sqlcliProp->getusername(), passwd.asCharp());
//            fprintf (stdout,"password: ");
//            size_t len = sprintf(&m_curLine[0],"\"%s\",",m_sqlcliProp->getusername());
//            char ch;
//            do {
//              ch = (char) _getch();
//              if (ch == CRVALUE){
//                break;
//              }
//              m_curLine[len]=ch;
//              len++;
//              PUTCH('*');
//              //printf("%02X",ch);
//            } while( 1);
//              PUTCH('\r');
//              PUTCH('\n');
// 
////            if (fgets(&m_curLine[len],sizeof(m_curLine)-2,m_sqlcliProp->getinfile())){
              m_sqlcliProp->setuser(&m_curLine[0],len); 
              this->m_cmdtype = cmdtype_connect;
              return NULL;             
//            }
          } 
        }

        //getCMD
        sqlcliInput_cmdtype cmdtype= cmdtype_eof;
        size_t    tmpLen = 0;
        char     *linptr = 0;
        m_cmdtype = cmdtype;
        if (! m_sqlcliProp->getmultilinemode()) m_curLen = 0;
        while ( (printPrompt())
                && (linptr = fgets(m_curLine,sizeof(m_curLine)-1,m_sqlcliProp->getinfile())) )
        {
          tmpLen = strlen (m_curLine);
          if ( m_curLine[tmpLen-1] == '\n' ||  m_curLine[tmpLen-1] == 0x0a) tmpLen -- ; //ignore linebreaks
          m_curLine[tmpLen]='\0';
          cmdtype = this->analyzeSQL(m_curLine);

          if (cmdtype == cmdtype_reset){
            m_stmt[0]='\0';
            m_curLen =0;
            continue;
          }
          else if (cmdtype == cmdtype_print){
            m_stmt[m_curLen]='\0';
            fprintf (m_sqlcliProp->getoutfile(),"%.*s\n", m_curLen, m_stmt);
            continue;
          }
          else if (cmdtype == cmdtype_edit){
            m_stmt[m_curLen]='\0';
            editQueryBuffer();
            break;
          }
          if ( m_curLine[tmpLen-1] == 0x5c /*'\'*/ ) {//concat lines
            m_curLine[tmpLen-1] = ' ';
            strncpy(m_stmt+m_curLen,m_curLine,tmpLen);
            m_curLen += tmpLen;
            continue;
          }
          else if(m_sqlcliProp->getmultilinemode()
                  && cmdtype == cmdtype_unknown ){
            if ( m_stmt[m_curLen-1] == '\n' ||  m_stmt[m_curLen-1] == '\0') m_stmt[m_curLen-1]=' ' ;
            if (!strncmp(&m_curLine[tmpLen-1],";", 1)){
              strncpy(m_stmt+m_curLen,m_curLine,tmpLen-1);
              m_curLen += tmpLen;
              m_stmt[m_curLen-1]='\0';
              m_multiLinePrompt = SQLDBC_FALSE;
              break;
            }
            if (!strncmp(&m_curLine[tmpLen-2],"\\g", 2)){
              strncpy(m_stmt+m_curLen,m_curLine,tmpLen-2);
              m_curLen += tmpLen-1;
              m_stmt[m_curLen-1]='\0';
              m_multiLinePrompt = SQLDBC_FALSE;
              break;
            }
            if(!strncmp(&m_curLine[tmpLen-3],"\\go", 3)){
              strncpy(m_stmt+m_curLen,m_curLine,tmpLen-3);
              m_curLen += tmpLen-2;
              m_stmt[m_curLen-1]='\0';
              m_multiLinePrompt = SQLDBC_FALSE;
              break;
            }
            strncpy(m_stmt+m_curLen,m_curLine,tmpLen);
            m_curLen += tmpLen;
            m_stmt[m_curLen] = ' '; m_curLen++;
            m_multiLinePrompt = SQLDBC_TRUE;
            continue;
          }else if(m_sqlcliProp->getmultilinemode()
                  && cmdtype != cmdtype_unknown ){
             m_curLen = 0;      
             m_multiLinePrompt = SQLDBC_FALSE;
          } else {//single line modus 
            strncpy(m_stmt,m_curLine,tmpLen+1);
            m_curLen = tmpLen+1;
            break;
          }
          break ;
        }
        if ((cmdtype == cmdtype_eof && m_sqlcliProp->getinfile()!=stdin)){
          m_sqlcliProp->setinfile("stdin");
        }
        if (! linptr){
          this->m_cmdtype = cmdtype_exit;
        }
//        fprintf(stdout,"\n");
        return m_stmt;
      }

      ~IFTools_sqlcliInputInteractive   (){}

protected:
  SQLDBC_Bool m_alreadyExecuted;
  SQLDBC_Bool m_multiLinePrompt;
  char m_stmt[32000];
  char m_curLine[1024];
  int m_curLen;

};
//----------------------------------------------------------------------
IFTools_sqlcliInput::IFTools_sqlcliInput(IFTools_sqlcliProperties *sqlcliProp):
m_sqlcliProp(sqlcliProp)
{}
//----------------------------------------------------------------------

class IFTools_sqlcliInputFromFile : public IFTools_sqlcliInput {
public:
  IFTools_sqlcliInputFromFile(IFTools_sqlcliProperties *sqlcliProp):
      IFTools_sqlcliInput(sqlcliProp){
      }

  ~IFTools_sqlcliInputFromFile   (){}

 virtual char* getnextCMD(){
    char* eof, *seppos, *cmdsep = m_sqlcliProp->getcmdsep();
    int cmdsepLen= strlen (cmdsep), tmpLen=0;
    m_curLen = 0;
    m_stmt[0]='\0';
    while(SQLDBC_TRUE){
      eof = fgets(m_stmt+m_curLen,sizeof(m_stmt)-2-m_curLen,m_sqlcliProp->getinfile()) ;
      tmpLen= strlen (m_stmt+m_curLen);
      seppos = strstr( &m_stmt[m_curLen], cmdsep );
      if (tmpLen >= cmdsepLen && seppos){
        m_curLen = (int)(seppos - m_stmt);
        break;
      }
      m_curLen += tmpLen;
      while ( m_curLen &&
              (   m_stmt[m_curLen-1] == '\n'
               || m_stmt[m_curLen-1] == '\r'
               ||  m_stmt[m_curLen-1] == '\0'
               ||  m_stmt[m_curLen-1] == ' '
               ||  m_stmt[m_curLen-1] == '\\'
              )
             ) {m_curLen-- ;}

      if (eof == NULL){
        if (m_curLen){
          seppos = strstr( &m_stmt[m_curLen], cmdsep );
          if (m_curLen >= cmdsepLen && seppos){
             m_curLen = (int)(seppos - m_stmt);
          }
          break;
        }
        m_cmdtype = cmdtype_exit;
        return (char*)"";
      }
      m_stmt[m_curLen++]=' ';
    }
    m_stmt[m_curLen]='\0';
    m_cmdtype = this->analyzeSQL(m_stmt);

      return m_stmt;
  }
  protected:
    char m_stmt[32000];
    int  m_curLen;
};
//----------------------------------------------------------------------

IFTools_sqlcliInput* IFTools_sqlcliInput::getNewInputHandler(
  IFTools_sqlcliProperties   *m_sqlcliProp){
    if ( m_sqlcliProp->getinfile() != stdin)
      return (new IFTools_sqlcliInputFromFile(m_sqlcliProp));
    else if (! m_sqlcliProp->getcommand()){
      return (new IFTools_sqlcliInputInteractive(m_sqlcliProp));
    }  
    else                  
      return (new IFTools_sqlcliSimpleInput(m_sqlcliProp));
  }

  //----------------------------------------------------------------------
  IFTools_sqlcliInput::sqlcliInput_cmdtype IFTools_sqlcliInput::analyzeSQL(char* pIdentifier){
    SQLDBC_Int4 currPos=0, symbolLen;
    /*clear internal state*/
    this->m_cmdtype = cmdtype_unknown;

    if (! cmdKeywordTabIsInit){
      initCMDKeywordTable (mycmdKeywordTab,      CMDKEYWORDTABLESIZE);
      initCMDKeywordTable (sqlmodeKeywordTab,    SQLMODEKEYWORDTABLESIZE);
      initCMDKeywordTable (autocommitKeywordTab, AUTOCOMMITKEYWORDTABLESIZE);
      initCMDKeywordTable (connectKeywordTab,    CONNECTKEYWORDTABLESIZE);
    } 
    pIdentifier = IFTools_sqlcliProperties::trimString(pIdentifier);
    symbolLen = this->nextSymbol(pIdentifier, &currPos);
    if (! this->isInternalCMD(&pIdentifier[currPos], &currPos, &symbolLen)){
      this->m_cmdtype = (IFTools_sqlcliInput::sqlcliInput_cmdtype) this->analyzeKeyword(&pIdentifier[currPos], symbolLen, mycmdKeywordTab, CMDKEYWORDTABLESIZE); 
      switch (this->m_cmdtype){
         /*allow only internal commands that doesn't start with '\' or ':'*/
         case(IFTools_sqlcliInput::cmdtype_help):
         case(IFTools_sqlcliInput::cmdtype_exit):{
           break;
         }
         default: {
          this->m_cmdtype = cmdtype_unknown;
         } 
      }
      return this->m_cmdtype;
    }
    this->m_cmdtype = (IFTools_sqlcliInput::sqlcliInput_cmdtype) this->analyzeKeyword(&pIdentifier[currPos], symbolLen, mycmdKeywordTab, CMDKEYWORDTABLESIZE); 
    switch (this->m_cmdtype){
      case(IFTools_sqlcliInput::cmdtype_sqlmode):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        pIdentifier[currPos+symbolLen]='\0';
        this->m_sqlcliProp->getifrprops().setProperty("SQLMODE", IFTools_sqlcliProperties::trimString(&pIdentifier[currPos]));
        break;
      } 
      case(IFTools_sqlcliInput::cmdtype_showUsers):
      case(IFTools_sqlcliInput::cmdtype_showSchemas):
      case(IFTools_sqlcliInput::cmdtype_showColumns):
      case(IFTools_sqlcliInput::cmdtype_showProcs):
      case(IFTools_sqlcliInput::cmdtype_showTables):
      case(IFTools_sqlcliInput::cmdtype_showIndices):
      case(IFTools_sqlcliInput::cmdtype_showViews): {
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        this->m_sqlcliProp->setsearchpattern(&pIdentifier[currPos],symbolLen);
        break;
     } 
     case(IFTools_sqlcliInput::cmdtype_shellcmd): {
        currPos+=symbolLen+1;
        this->m_sqlcliProp->setcommand(&pIdentifier[currPos], strlen (&pIdentifier[currPos]));
        break;
     } 
     case(IFTools_sqlcliInput::cmdtype_edit): {
        currPos+=symbolLen+1;
        if (strlen(&pIdentifier[currPos])){
          m_sqlcliProp->seteditfilename(IFTools_sqlcliProperties::trimString(&pIdentifier[currPos]));
        } else{
          m_sqlcliProp->seteditfilename(0);
        }
        break;

     } 
     case(IFTools_sqlcliInput::cmdtype_connect): {
        sqlcliInput_connecttype actyp;
        currPos+=symbolLen+1;
        while ((symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos))){ 
          actyp = (IFTools_sqlcliInput::sqlcliInput_connecttype)this->analyzeKeyword(&pIdentifier[currPos],symbolLen, connectKeywordTab, CONNECTKEYWORDTABLESIZE);        
          currPos+=symbolLen+1;
          symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
          if (symbolLen){
            switch (actyp){
              case(IFTools_sqlcliInput::connect_dbhost):{           
                this->m_sqlcliProp->setdbhost(&pIdentifier[currPos], symbolLen);
                break;
              }
                case(IFTools_sqlcliInput::connect_dbname):{           
                this->m_sqlcliProp->setdbname(&pIdentifier[currPos], symbolLen);
                break;
              }  case(IFTools_sqlcliInput::connect_user):{
                if (symbolLen && pIdentifier[currPos]=='\"' ){
                  SQLDBC_Int4 realPos=currPos;
                  currPos += symbolLen+1;
                  symbolLen= this->nextSymbol(&pIdentifier[currPos], &currPos); // should be a comma
                  if (symbolLen == 1){
                    currPos += symbolLen+1;
                    symbolLen= this->nextSymbol(&pIdentifier[currPos], &currPos);
                  } 
                  this->m_sqlcliProp->setuser(&pIdentifier[realPos], currPos+symbolLen-realPos);
                  currPos+=symbolLen+1;
                } else {           
                  this->m_sqlcliProp->setuser(&pIdentifier[currPos], symbolLen);
                }  
                break;
              }  case(IFTools_sqlcliInput::connect_xuser):{           
                this->m_sqlcliProp->setxuserkey(&pIdentifier[currPos], symbolLen);
                break;
              }
            }   
          }  
          currPos+=symbolLen+1;
        }
        break;
     } 
     case(IFTools_sqlcliInput::cmdtype_outfile): {
        currPos+=symbolLen+1;
        if (strlen(&pIdentifier[currPos])){
          m_sqlcliProp->setoutfile(IFTools_sqlcliProperties::trimString(&pIdentifier[currPos]));
        } else{
          m_sqlcliProp->setoutfile("stdout");
        }
        break;
     } 
     case(IFTools_sqlcliInput::cmdtype_input): {
        currPos+=symbolLen+1;
        if (strlen(&pIdentifier[currPos])){
          m_sqlcliProp->setinfile(IFTools_sqlcliProperties::trimString(&pIdentifier[currPos]));
        } else{
          m_sqlcliProp->setinfile("stdin");
        }
        break;
     } 
     case(IFTools_sqlcliInput::cmdtype_autocommit):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        if (! symbolLen){
          m_sqlcliProp->setAutocommit();
        } else {
          sqlcliInput_autocommittype actyp = (IFTools_sqlcliInput::sqlcliInput_autocommittype)this->analyzeKeyword(&pIdentifier[currPos],symbolLen, autocommitKeywordTab, AUTOCOMMITKEYWORDTABLESIZE);        
          m_sqlcliProp->setAutocommit((actyp == IFTools_sqlcliInput::autocommit_on)?SQLDBC_TRUE:SQLDBC_FALSE);
        }
        break;
      } 
     case(IFTools_sqlcliInput::cmdtype_usePreparedStmt):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        if (! symbolLen){
			m_sqlcliProp->setUsePreparedStmt();
        } else {
          sqlcliInput_autocommittype actyp = (IFTools_sqlcliInput::sqlcliInput_autocommittype)this->analyzeKeyword(&pIdentifier[currPos],symbolLen, autocommitKeywordTab, AUTOCOMMITKEYWORDTABLESIZE);        
          m_sqlcliProp->setUsePreparedStmt((actyp == IFTools_sqlcliInput::autocommit_on)?SQLDBC_TRUE:SQLDBC_FALSE);
        }
        break;
      } 
     case(IFTools_sqlcliInput::cmdtype_pager):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        if (! symbolLen){
			m_sqlcliProp->setNoPager();
        } else {
          sqlcliInput_autocommittype actyp = (IFTools_sqlcliInput::sqlcliInput_autocommittype)this->analyzeKeyword(&pIdentifier[currPos],symbolLen, autocommitKeywordTab, AUTOCOMMITKEYWORDTABLESIZE);        
          m_sqlcliProp->setNoPager((actyp == IFTools_sqlcliInput::autocommit_on)?SQLDBC_TRUE:SQLDBC_FALSE);
        }
        break;
      } 
     case(IFTools_sqlcliInput::cmdtype_multiline):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        if (! symbolLen){
			m_sqlcliProp->setMultiline();
        } else {
          sqlcliInput_autocommittype actyp = (IFTools_sqlcliInput::sqlcliInput_autocommittype)this->analyzeKeyword(&pIdentifier[currPos],symbolLen, autocommitKeywordTab, AUTOCOMMITKEYWORDTABLESIZE);        
          m_sqlcliProp->setMultiline((actyp == IFTools_sqlcliInput::autocommit_on)?SQLDBC_TRUE:SQLDBC_FALSE);
        }
        break;
      } 
      case(IFTools_sqlcliInput::cmdtype_status):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        break;
      }
      case(IFTools_sqlcliInput::cmdtype_alignment):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        if (! symbolLen){
          m_sqlcliProp->setAlignedOutput();
        } else {
          sqlcliInput_autocommittype actyp = (IFTools_sqlcliInput::sqlcliInput_autocommittype)this->analyzeKeyword(&pIdentifier[currPos],symbolLen, autocommitKeywordTab, AUTOCOMMITKEYWORDTABLESIZE);        
          m_sqlcliProp->setAlignedOutput((actyp == IFTools_sqlcliInput::autocommit_on)?SQLDBC_TRUE:SQLDBC_FALSE);
        }
        break;
      }
     case(IFTools_sqlcliInput::cmdtype_fieldsep):{
        currPos+=symbolLen+1;
        symbolLen = this->nextSymbol(&pIdentifier[currPos], &currPos);
        m_sqlcliProp->setfieldseparator(&pIdentifier[currPos],symbolLen);
        break;
      }
    } 
    return this->m_cmdtype;
  }

  //----------------------------------------------------------------------
  SQLDBC_Int4 IFTools_sqlcliInput::analyzeKeyword(char* pIdentifier, SQLDBC_Int4 IdentLength, cmdKeywordTable myCMDKeywTab[] ,SQLDBC_Int4 numElementsTab){

    SQLDBC_Int4 begin = 1;            /*lower bound of search scope*/
    SQLDBC_Int4 end = numElementsTab; /*upper bound of search scope*/ 
    SQLDBC_Int4 pos;                  /*actual position*/
    SQLDBC_Int4 encoding = 0;         /*stores position of keyword in KeywordTab depending on */
    SQLDBC_Int4 compareLength;        /*stores length in bytes for memcompare*/
    char UpperIdentifier[MAX_CMD_KEYWORD_LEN]; /*pIdentifier in upper cases*/

    if (! IdentLength || IdentLength > MAX_CMD_KEYWORD_LEN){
      return cmdtype_unknown;
    }  

    /*upper pidentifier for case insensitive compare*/
    memset (&UpperIdentifier[0],0,MAX_CMD_KEYWORD_LEN);
    for (int m=0; (m<IdentLength); m++) UpperIdentifier[m] = toupper(pIdentifier[m]);

    /* searching for keyword */ 
    do
    {
      SQLDBC_Int4 erg;                  /*result off keyword compare*/ 
      pos = (begin+end)/2;
      compareLength = (IdentLength > myCMDKeywTab[pos-1].keyword[encoding].length)?IdentLength:myCMDKeywTab[pos-1].keyword[encoding].length;
      erg = memcmp (&UpperIdentifier[0], 
        myCMDKeywTab[pos-1].keyword[encoding].rawString, 
        compareLength);
      if (erg == 0)
        /*(sub)keyword matches*/
      {
        if (IdentLength == myCMDKeywTab[pos-1].keyword[encoding].length)
        {
          return myCMDKeywTab[pos-1].entry;
        } 
        else
          /* identifier too small or too large for current keyword */ 
        {
          if (IdentLength > myCMDKeywTab[pos-1].keyword[encoding].length)
            erg = 1;
          else
            erg = -1;
        }    
      }

      if (erg < 0)
        /*continue serching in lower scope*/
      {
        end = pos-1;
      }
      else
        /*continue serching in upper scope*/
      {
        begin = pos+1;     
      }
    }while(begin <= end);  /*identifier not recognized as a keyword*/
    return cmdtype_unknown;
  }
  //--------------------------------------------------------------------------
  SQLDBC_Int4 IFTools_sqlcliInput::nextSymbol(char* pIdentifier, int* currPos){
    SQLDBC_Int4 IdentLength, len = strlen (pIdentifier);
    //remove leading blanks and opening parenthesis
    for (;(isspace(pIdentifier[0]) || pIdentifier[0]=='(');pIdentifier++, (*currPos)++){};    
    //getting length of keyword
    if (pIdentifier[0]=='\"') {
      for (IdentLength=1;(IdentLength<len && pIdentifier[IdentLength]!='\"'); IdentLength++){} 
      IdentLength++;   
    } else {
      for (IdentLength=0;(IdentLength<len && !isspace(pIdentifier[IdentLength]) ); IdentLength++){}    
    }
    return IdentLength;
  }
  //--------------------------------------------------------------------------
  SQLDBC_Int4 IFTools_sqlcliInput::isInternalCMD(char* pIdentifier, int* currPos, int* identLen){
    char internCMDPraefix[] ={'\\',':'};
    if (! strlen (pIdentifier)) return 0;
    for (SQLDBC_Int4 i=0; i < sizeof(internCMDPraefix); i++) {   
      if (pIdentifier[0]== internCMDPraefix[i]) {
         (*currPos)++;
         (*identLen)--;
         return 1;
      }
    } 
    return 0;
  }

  //--------------------------------------------------------------------------
	SQLDBC_SQLMode IFTools_sqlcliInput::getSQLModeFromString(char* sqlmode){
    if (! cmdKeywordTabIsInit){
      initCMDKeywordTable (mycmdKeywordTab,      CMDKEYWORDTABLESIZE);
      initCMDKeywordTable (sqlmodeKeywordTab,    SQLMODEKEYWORDTABLESIZE);
      initCMDKeywordTable (autocommitKeywordTab, AUTOCOMMITKEYWORDTABLESIZE);
      initCMDKeywordTable (connectKeywordTab,    CONNECTKEYWORDTABLESIZE);
    } 
  
   return (SQLDBC_SQLMode) analyzeKeyword(sqlmode, strlen(sqlmode), sqlmodeKeywordTab, SQLMODEKEYWORDTABLESIZE); 
	}
