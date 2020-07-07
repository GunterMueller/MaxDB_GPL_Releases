/*!
  @file           geo31.h
  @author         JoergM
  @special area   RTE | Debugging Processes
  @brief          Enables Debugging
  @see            http://p26326.bea.sap-ag.de/Lzu/Specs/Processes.html

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


#ifndef GEO31_H
#define GEO31_H

#include "gsp00.h"

# if defined(__cplusplus)
extern "C" {
# endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
/*!
   @brief          This call will overwrites the name of the special text
'SAPDB_Debug_Env' normaly exists in the Rundirectory. The parameter
FileName should contain a fully qualified path plus a filename.
If FileName is set to NULL only the Enabled Parameter is assigned.


   The Value of Enabled will be used in a call to sqlDebugBreak().
   If Enabled is not NULL and the value of Enabled is set to 1
   the function sqlCallDebugger will start the debugger by calling
   eo31_CallDebugger.
   If Enbaled points the a shared memory segment you can switch debugging
   dynamicly on or off.
   If the Enabled parameter is set to NULL, debugging is switched off.
   If Enabled and FileName are set to NULL all parameters are set to the
   initial state (see below).
   On default FileName points to a static variable which contains
   'SAPDB_Debug_Env'.
   On default Enabled points to a static variable which contains 1 that means
   debugging is switched on.

 */


void sqlSetDebug(char *FileName, tsp00_Int4 *Enabled);

/*!
   @brief          This function calls eo31_CallDebugger if Enabled (set by
sqlSetDebug) points to an value of 1.
For additional information see description of sqlSetDebug and
eo31_CallDebugger.
   @return         none


   Without any other Calls to sqlSetDebug sqlDebugBreak will switch to the
   debugger (the default);

 */


void sqlDebugBreak();

/*!
   @brief          This call tries to call a debugger, if a debug environment is set.
   @Where Display must be set to your XDisplay identifier, i.e. p26216.0.
Make sure that Display is set to the correct value...
   @DebuggerCall depends on the Operating System<UL>
      <LI>AIX   - "dbx -a %d" No path to executable needed...
      <LI>HP    - "dde -ui line -attach %d path_to_exe"
      <LI>SUN   - "dbx path_to_exe %d"
      <LI>SOLARIS - "dbx path_to_exe %d"
      <LI>DEC   - "ladebug -pid %d path_to_exe"
               or "dbx path_to_exe %d"
      <LI>SNI   - "dbx %d" No path to executable needed...
      <LI>LINUX - "gdb path_to_exe %d"
      <LI>BSD44 - "gdb path_to_exe %d"
</UL>

Except for AIX and SNI you have to specify the path to the executable.
   @Normally this is "$DBROOT/pgm/dbBName". But you cannot use $DBROOT,
but must specify the expanded path.

Environment variable SAPDB_DEBUG_RUN_KERNEL if found will enable starting
debugger at kernel start (usefull for LZU debugging).
   @return         Calling done
      &lt;UL>
              &lt;LI>0: Did not try to call debugger
      &lt;LI>1: Debugger called
  &lt;/UL>


   Environment is setup in a special text 'SAPDB_Debug_Env' file found in the
   RunDirectory. This file consists of individual lines containing variable
   assignments. Comment line are introduced by a Hashmark '#'. Variable assignment
   lines are formed as
   Variable = Value up til end of line
   Environment variable SAPDB_DEBUG_CMD will have the effect, that a call to this
   function leads in executing a command prepared using this environment value
   as format string for sprintf() with argument current pid (returned by getpid() ).
   General form of such an environment entry is
   SAPDB_DEBUG_CMD="xterm -display Display -e DebuggerCall &amp; sleep 5"

 */


int eo31CallDebugger();

# if defined(__cplusplus)
}
# endif
#endif  /* GEO31_H */
