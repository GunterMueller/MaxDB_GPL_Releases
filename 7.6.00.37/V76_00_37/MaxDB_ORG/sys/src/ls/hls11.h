/* 
  -----------------------------------------------------------------------------
 
  module: hls11.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Dispatcher module function definitions
                
  version:      7.6.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1997-2005 SAP AG-2003
 
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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


#ifndef HLS11
#define HLS11

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------

  function:     ls11ParseLoadCommand

  -----------------------------------------------------------------------------

  description:  Parses a command 
                Command separation has to be done by the client or the caller.

  arguments:    SvrCmd  [IN]  - structure with info on command to parse and execute  
                DBInfo  [IN]  - struct that contains all info on the running
                                 session and the db connected to.

  returnvalue:  errOK_els00  - in case of NO errors
                errYaccAlloc_els00 - error generating parser
                error generating scanner
                errors processing command
                Error messages will be logged by this function into the log file.
  -----------------------------------------------------------------------------
*/
int
ls11ParseLoadCommand(tls00_DBInfo  *pDBInfo, 
                     tls00_Command *& SvrCmd );

/*!
  -----------------------------------------------------------------------------

  function:     ls11ParseDispatchLoadCommand

  -----------------------------------------------------------------------------

  description:  Parses, dispatches and executes a command if no syntax errors found.
                Command separation has to be done by the client or the caller.

  arguments:    SvrCmd  [IN]  - UTF8String SvrCmdString
                DBInfo  [IN]  - struct that contains all info on the running
                                 session and the db connected to.

  returnvalue:  errOK_els00  - in case of NO errors
                errYaccAlloc_els00 - error generating parser
                error generating scanner
                errors processing command
                Error messages will be logged by this function into the log file.
  -----------------------------------------------------------------------------
*/
int
ls11ParseDispatchLoadCommand(Tools_DynamicUTF8String   SvrCmdString,
                             tls00_DBInfo             *pDBInfo,
                             tsp00_Addr                pszErrText);

/*!
  -----------------------------------------------------------------------------

  function:     ls11ParseDispatchLoadCommand

  -----------------------------------------------------------------------------

  description:  Parses, dispatches and executes a command if no syntax errors found.
                Command separation has to be done by the client or the caller.

  arguments:    SvrCmd  [IN]  - structure with info on command to parse and execute  
                DBInfo  [IN]  - struct that contains all info on the running
                                 session and the db connected to.

  returnvalue:  errOK_els00  - in case of NO errors
                errYaccAlloc_els00 - error generating parser
                error generating scanner
                errors processing command
                Error messages will be logged by this function into the log file.
  -----------------------------------------------------------------------------
*/
int
ls11ParseDispatchLoadCommand(tls00_Command *&SvrCmd, tls00_DBInfo *DBInfo);

#endif // HLS_11