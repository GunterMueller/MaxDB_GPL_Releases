/*!
  @file           hcn85.h
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          hot standy functions

\if EMIT_LICENCE
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


\endif
*/

#ifndef _HCN46_H_
#define _HCN46_H_

/* IMPORT
 * ------------------------------------------------------------------
 */
#include "gcn00.h"

/* 
  SPECIFICATION PUBLIC FUNCTIONS
  ------------------------------------------------------------------
 */

/*! @brief Add a standby node to the cluster */
tcn00_Error cn46AddStandby (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

/*! @brief Removes a standby node from the cluster */
tcn00_Error cn46RemoveStandby (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

/*! @brief List the nodes */
tcn00_Error cn46GetNodes (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

/*! @brief Bring node to standby */
tcn00_Error cn46DBStandby (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

/*! @brief Initialize the master configuration */
tcn00_Error cn46Enable (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

/*! @brief Is this a master or not? */
bool cn46IsMaster
    ( tsp00_DbNamec const szDBName );

#endif /* _HCN46_H_ */

