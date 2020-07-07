/*!
  @file           RTE_SdbStarter.h
  @author         JoergM
  @brief          DBM RunTime: (UNIX only) Start a given program as if logged on as SdbOwner
  
    Other than setuid/setgid it resets the group list 

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#ifndef RTE_SDBSTARTER_H
#define RTE_SDBSTARTER_H

/*! the well known name of the starter program */
#define SDBSTARTER_PROGRAM    "sdbstarter"
/*! the well known name of the starter directory */
#define SDBSTARTER_DIRECTORY  "starter"

#include "RunTime/RTE_CompilerFeatures.h"

/*!
  @brief Calling sdbstarter program

  All 'setuid sdbown, setgid sdbadm' programs must call this function as first action item.
  It internally checks the following file rights before executing the call to 'sdbstarter':
  
    calling program must be                     -r-s--s--- sdbown sdbadm
    subdirectory starter must be                dr-x------ sdbown sdbadm
    sdbstarter program must be                  -r-s--x--- root   sdbadm

  and

    sdbstarter must be found as \<absolutePath for program\>/starter/sdbstarter

  The sdbstarter program is called via 'execve' without an extra fork and gets the absolut path
  of the program to call. Before sdbstarter does the second 'execve' needed to call the original
  program, it inserts '-sdbstarter' as first argument of the program which allows this function
  to detect the program was called for the second time. In such case the '-sdbstarter' argument
  is silently removed and the program works as wanted....

  Any problem leads to message output on stderr and additional a specific exit code.

  Since the original caller is completely overwritten (effectiv and real user and group ids...) 
  the original real user and group ids are returned.

  @param argc [inout] argument count (reduced by one if called with first argument '-sdbstarter')
  @param argv [inout] the argument vector
  @param callerUid [out] the orginal caller user id
  @param callerGid [out] the orginal caller group id
  @param isAdministrator [out] true if orginal caller is member of sdb administrator group
  @return none
 */
externC void RTE_CallSdbStarter(int VAR_VALUE_REF argc,
                                char *argv[], 
                                SAPDB_Int4 VAR_VALUE_REF callerUid,
                                SAPDB_Int4 VAR_VALUE_REF callerGid,
                                SAPDB_Bool VAR_VALUE_REF isAdministrator);

#endif /* RTE_SDBSTARTER_H */
