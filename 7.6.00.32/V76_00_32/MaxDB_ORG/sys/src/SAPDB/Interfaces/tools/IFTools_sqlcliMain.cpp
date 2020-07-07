/*!**************************************************************************

module      : IFTools_sqlcli.cpp

-------------------------------------------------------------------------

responsible : D031096

special area:
description : Interface Runtime 

description:  command line tool for querying the database

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
#include "SAPDB/Interfaces/tools/IFTools_sqlcliInput.h"
void mainFunc(int argc, char **argv)
{
  IFTools_sqlcliProperties sqlcliProp( argc, argv );
  if (sqlcliProp.getdebug()) sqlcliProp.dumpClass();

  IFTools_sqlcliInput*   in = IFTools_sqlcliInput::getNewInputHandler(&sqlcliProp);
  IFTools_sqlcliOutput   out(&sqlcliProp);
  IFTools_sqlcliDBAccess sqlcliDBA(&sqlcliProp, &out);

  if (sqlcliProp.isVersionInfoFlagSet()){
    out.printComment("SQLCLI version <MAJOR_VERSION>.<MINOR_VERSION>.<CORRECTION_LEVEL> BUILD <BUILD_NUMBER_EX_STRING>, "
    "the <PRODUCT_NAME> interactive terminal.\n"
    "<COPYRIGHT_STRING>.\n");
	sqlcliProp.exitProgram();
  }
  
  char * cmd=0;
  IFTools_sqlcliInput::sqlcliInput_cmdtype cmdtype;
  IFTools_sqlcliInput*   inInteractive= 0;

  while (SQLDBC_TRUE){
    if (sqlcliProp.isExitForced()){ 
      sqlcliDBA.doDisconnect();
      sqlcliProp.exitProgram();
    }
    cmd = in->getnextCMD();
    cmdtype =  in->getCMDType();
    if (cmdtype == IFTools_sqlcliInput::cmdtype_exit) {
      if (inInteractive){
        sqlcliProp.setinfile("stdin");
        delete in;
        in = inInteractive;
        inInteractive = 0;
      } else {
        break; 
      }
    }
    switch (cmdtype){
    case(IFTools_sqlcliInput::cmdtype_sqlmode):{
      sqlcliDBA.setSQLmode();
      break;
                                               } 
    case(IFTools_sqlcliInput::cmdtype_input):{
      inInteractive = in;
      in = IFTools_sqlcliInput::getNewInputHandler(&sqlcliProp);
      break;
                                               } 
    case(IFTools_sqlcliInput::cmdtype_alignment):{
      out.printComment(COM_CHANGE_ALIGNMODE,(sqlcliProp.getalignedOutput())?"ON":"OFF");
      break;
                                               } 
    case(IFTools_sqlcliInput::cmdtype_pager):{
      out.printComment(COM_CHANGE_PAGEROUTPUT,(sqlcliProp.getnoPager())?"OFF":"ON");
      break;
                                               } 
    case(IFTools_sqlcliInput::cmdtype_multiline):{
      out.printComment(COM_CHANGE_MULTILINE,(sqlcliProp.getmultilinemode())?"ON":"OFF");
      break;
                                               } 
    case(IFTools_sqlcliInput::cmdtype_outfile):{
      break;
                                              } 
    case(IFTools_sqlcliInput::cmdtype_connect):{
      sqlcliDBA.doConnect();
      break;
                                              } 
    case(IFTools_sqlcliInput::cmdtype_disconnect):{
      sqlcliDBA.doDisconnect();
      out.printComment(COM_DISCONNECT); 
      break;
                                              } 
    case(IFTools_sqlcliInput::cmdtype_shellcmd):{
      int erg = sqlcliProp.executeCommand (sqlcliProp.getcommand());
      out.printComment(COM_RETURNCODE, erg); 
      break;
                                              } 
    case(IFTools_sqlcliInput::cmdtype_status):{
      sqlcliDBA.displayStatus();
      break;
                                              } 
    case(IFTools_sqlcliInput::cmdtype_showUsers):{
      sqlcliDBA.displayUsers();
      break;
                                                 }
    case(IFTools_sqlcliInput::cmdtype_showSchemas):{
      sqlcliDBA.displaySchemas();
      break;
                                                 }
    case(IFTools_sqlcliInput::cmdtype_showTables):
    case(IFTools_sqlcliInput::cmdtype_showViews):{
      sqlcliDBA.displayTablesViews(cmdtype);
      break;
                                                 }  
    case(IFTools_sqlcliInput::cmdtype_showColumns):{
      sqlcliDBA.displayColumns();
      break;
                                                   } 
    case(IFTools_sqlcliInput::cmdtype_showIndices):{
      sqlcliDBA.displayIndices();
      break;
                                                   } 
    case(IFTools_sqlcliInput::cmdtype_showProcs):{
      sqlcliDBA.displayListDBProcs();
      break;
                                                   } 
    case(IFTools_sqlcliInput::cmdtype_autocommit):{
      sqlcliDBA.setAutocommit(sqlcliProp.getautocommit());
      break;
                                               } 
    case(IFTools_sqlcliInput::cmdtype_usePreparedStmt):{
      sqlcliDBA.setUsePreparedStmt(sqlcliProp.getUsePreparedStmt());
      break;
                                               } 
    case(IFTools_sqlcliInput::cmdtype_help):{
      out.printComment("%s",IFTools_sqlcliInput::getshortcutHelp());
      break;
                                            } 
    case(IFTools_sqlcliInput::cmdtype_unknown):{
      sqlcliDBA.executeCMD(cmd);
      break;
    } 
             
    }
  }
  sqlcliDBA.doDisconnect();
  sqlcliProp.exitProgram();
}

int main(int argc, char **argv){
   char **myargv = new char*[argc]; 
   myargv[0] = new char[strlen(argv[0])+1];
   strcpy(myargv[0],argv[0]);
   for (int i=1; i<argc; i++){
    size_t len = strlen(argv[i])+1;
    myargv[i] = new char[len];
    strcpy(myargv[i],argv[i]);
    memset(argv[i],0,len);
   }
   mainFunc(argc, myargv); 
} 
