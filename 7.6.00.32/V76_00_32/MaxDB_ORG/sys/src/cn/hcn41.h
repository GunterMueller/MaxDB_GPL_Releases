/*! 
  -----------------------------------------------------------------------------
 
  module: hcn41.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Configuration
   
  description:  DBMServer Configuration - Specification

  version:      7.2.* / 7.1.* / 6.2.9.* 

  -----------------------------------------------------------------------------
 
  Copyright:   
 
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



  -----------------------------------------------------------------------------
*/

#ifndef _HCN41_H_
#define _HCN41_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gcn00.h"

/*!
  -----------------------------------------------------------------------------
  function:     cn41UserCreate
  -----------------------------------------------------------------------------

  description:  create a DBM-User 

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserCreate
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn41UserGet
  -----------------------------------------------------------------------------

  description:  get data of DBM-User 

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserGet
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn41UserGetRights
  -----------------------------------------------------------------------------

  description:  get rights of DBM-User 

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserGetRights
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn41UserGetAll
  -----------------------------------------------------------------------------

  description:  get names of all DBM-Users

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserGetAll
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn41UserLogon
  -----------------------------------------------------------------------------

  description:  logon to DBMServer

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserLogon
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn41UserSystem
  -----------------------------------------------------------------------------

  description:  logon to OS

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserSystem
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);
/*!
  -----------------------------------------------------------------------------
  function:     cn41UserSap
  -----------------------------------------------------------------------------

  description:  set sap user

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserSap
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);
/*!
  -----------------------------------------------------------------------------
  function:     cn41UserSysdba
  -----------------------------------------------------------------------------

  description:  set sysdba user

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserSysdba
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);
/*!
  -----------------------------------------------------------------------------
  function:     cn41UserPut
  -----------------------------------------------------------------------------

  description:  save data to DBMUser

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserPut
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn41UserDelete
  -----------------------------------------------------------------------------

  description:  save data to DBMUser

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn41UserDelete
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

#endif /* _HCN41_H_ */

