/*! 
  -----------------------------------------------------------------------------
 
  module: hcn43.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Dbanalyzer Access
   
  description:  DBMServer Dbanalyzer Access - Specification

  -----------------------------------------------------------------------------
 
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

#ifndef _HCN43_H_
#define _HCN43_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gcn00.h"

/*!
  -----------------------------------------------------------------------------
  function:     cn43DBanalyzerStart
  -----------------------------------------------------------------------------

  description:  Starts the dbanalyzer

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn43DBanalyzerStart
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn43DBanalyzerStop
  -----------------------------------------------------------------------------

  description:  Stops the dbanalyzer

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn43DBanalyzerStop
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn43DBanalyzerState
  -----------------------------------------------------------------------------

  description:  Determines the state of the dbanalyzer

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn43DBanalyzerState
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);
/*!
  -----------------------------------------------------------------------------
  function:     cn43DBanalyzerDelete
  -----------------------------------------------------------------------------

  description:  Delete the protocols of the dbanalyzer

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn43DBanalyzerDelete
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);


#endif /* _HCN43_H_ */
