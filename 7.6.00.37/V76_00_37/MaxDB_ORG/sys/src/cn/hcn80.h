/* module:        hcn80.h
 * ==================================================================
 *
 * responsible:   Martin Reinecke (D024853 - Martin.Reinecke@SAP-AG.de)
 *
 * created:       1998-03-19 by Martin Reinecke
 *
 * release :      6.2.8
 *
 * purpose:       utility connect/execute/release
 *
 * copyright:     Copyright (c) 1998-2005 SAP AG
 *
 * last changed:  1998-03-20  - Martin Reinecke
 *                              Utility Connect/Execute/Release
 *                1998-04-21  - Bernd Vorsprach 
 *                              SQLConnect/Execute/Info/Fetch/Release
 * ==================================================================
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


 * ==================================================================
 */

#ifndef _HCN80_H_
#define _HCN80_H_

/* IMPORT
 * ------------------------------------------------------------------
 */
#include "gcn00.h"
#include "hcn50.h"

/* SPECIFICATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */
tcn00_Error cn80UtilConnect (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn80UtilExecute (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn80UtilRelease (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn80DBActivate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn80DBActivateRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn80ServiceConnect (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn80ServiceRelease (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

#endif /* _HCN80_H_ */

