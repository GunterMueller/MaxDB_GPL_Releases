/*! 
  -----------------------------------------------------------------------------
 
  module: hcn45.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer high level commands
   
  description:  DBMServer high level commands - Specification

  -----------------------------------------------------------------------------
 
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef _HCN45_H_
#define _HCN45_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gcn00.h"

/*!
  -----------------------------------------------------------------------------
  function:     cn45MigrateCatalog
  -----------------------------------------------------------------------------

  description:  migrates the catalog to unicode

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn45MigrateCatalog
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn45Reinstall
  -----------------------------------------------------------------------------

  description:  reinstall the database

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn45Reinstall
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn45DBInstall
  -----------------------------------------------------------------------------

  description:  install a database with template

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
/*
tcn00_Error cn45DBInstall
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);
*/
#endif /* _HCN45_H_ */
