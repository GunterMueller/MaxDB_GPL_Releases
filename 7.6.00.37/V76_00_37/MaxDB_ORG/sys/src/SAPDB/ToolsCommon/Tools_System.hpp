/*!
  -----------------------------------------------------------------------------

  module: Tools_System.hpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: universal stuff for tool development

  -----------------------------------------------------------------------------

  Copyright (c) 1998-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




  -----------------------------------------------------------------------------
*/

#ifndef _TOOLS_SYSTEM_HPP_
#define _TOOLS_SYSTEM_HPP_

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_sleep
  -------------------------------------------------------------------------
  description:  system independent sleep

  arguments:    nSeconds   [IN] - number of seconds to sleep
  -------------------------------------------------------------------------
 */
void ToolsSys_sleep(unsigned int nSeconds);

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_sysname
  -------------------------------------------------------------------------
  description:  system name

  return value: null terminated system name
  -------------------------------------------------------------------------
 */
const char * ToolsSys_sysname();

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_getcwd
  -------------------------------------------------------------------------
  description:  get current directory

  return value: null terminated directory
  -------------------------------------------------------------------------
 */
const char * ToolsSys_getcwd(bool bWithSlash = false);

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_fullpath
  -------------------------------------------------------------------------
  description:  get full path 

  arguments:    szRelpath [IN] - relative path

  return value: null terminated path
  -------------------------------------------------------------------------
 */
const char * ToolsSys_fullpath(const char * szRelpath);

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_joinpath
  -------------------------------------------------------------------------
  description:  make a path from two parts

  arguments:    sPart1 [IN] - first part to join
                sPart2 [IN] - second part to join

  return value: null terminated path
  -------------------------------------------------------------------------
 */
const char * ToolsSys_joinpath
      ( const char * sPart1,
        const char * sPart2 );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_createdir
  -------------------------------------------------------------------------
  description:  creates recursive a directory

  arguments:    szPath [IN] - directory to create

  return value: true in success otherwise false
  -------------------------------------------------------------------------
 */
bool ToolsSys_createdir
      ( const char * szPath );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_removeemptydir
  -------------------------------------------------------------------------
  description:  deletes a n empty directory

  arguments:    szPath [IN] - directory to delete

  return value: true in success otherwise false
  -------------------------------------------------------------------------
 */
bool ToolsSys_removeemptydir
      ( const char * szPath );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_fileexists
  -------------------------------------------------------------------------
  description:  check existence of file

  arguments:    szFile [IN] - file to check

  return value: true if exists
  -------------------------------------------------------------------------
 */
bool ToolsSys_fileexists
      ( const char * szFile );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_killprocess
  -------------------------------------------------------------------------
  description:  kills the process

  arguments:    nPid [IN] - process to kill

  return value: true if exists
  -------------------------------------------------------------------------
 */
bool ToolsSys_killprocess
      ( unsigned int nPid );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_errnotext
  -------------------------------------------------------------------------
  description:  returns a text of a errno

  arguments:    nErrno [IN] - errno to resolve

  return value: the error text

  prototypes:   2
  -------------------------------------------------------------------------
 */
const char * ToolsSys_errnotext
      ( int nErrno );
const char * ToolsSys_errnotext
      ( );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_errno
  -------------------------------------------------------------------------
  description:  returns the errno

  return value: the errno value
  -------------------------------------------------------------------------
 */
int ToolsSys_errno
      (  );


/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_closedir
  -------------------------------------------------------------------------
  description:  closes a directory handle

  arguments:    pHandle [IN] - handle to close
  -------------------------------------------------------------------------
 */
void ToolsSys_closedir
      (void * pHandle);

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_readdir
  -------------------------------------------------------------------------
  description:  reads a directory entry

  arguments:    pHandle [IN] - handle to directory
                szFile  [IN] - buffer for filename
                nMaxLen [IN] - length of buffer

  return value: success or not
  -------------------------------------------------------------------------
 */
bool ToolsSys_readdir
      ( void *  pHandle,
        char *  szFile,
        int     nMaxLen );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_opendir
  -------------------------------------------------------------------------
  description:  opens a directory and read the first entry

  arguments:    pHandle [OUT] - handle to directory
                szPath  [ON]  - name of directory
                szFile  [IN]  - buffer for filename
                nMaxLen [IN]  - length of buffer

  return value: success or not
  -------------------------------------------------------------------------
 */
bool ToolsSys_opendir
      (       void * & pHandle,
        const char *   szPath,
              char *   szFile,
              int      nMaxLen );

/*!
  -------------------------------------------------------------------------
  function:     ToolsSys_tmpname
  -------------------------------------------------------------------------
  description:  generates a unique temporary filename

  return value: pointer to the temporary filename
  -------------------------------------------------------------------------
 */
const char * ToolsSys_tmpname
      ( );

#endif // _TOOLS_SYSTEM_HPP_
