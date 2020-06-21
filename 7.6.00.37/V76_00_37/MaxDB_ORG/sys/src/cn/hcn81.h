/* module:        hcn81.h
 * ==================================================================
 *
 * responsible:   Martin Reinecke (D024853 - Martin.Reinecke@SAP-AG.de)
 *
 * created:       1998-12-04 by Martin Reinecke
 *
 * release :      6.2.8
 *
 * purpose:       utility connect/execute/release
 *
 * copyright:
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

 *
 * ==================================================================
 */

#ifndef _HCN81_H_
#define _HCN81_H_

/* IMPORT
 * ------------------------------------------------------------------
 */
#include "gcn00.h"

/* SPECIFICATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */
tcn00_Error cn81Info (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn81InfoNext (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

#endif /* _HCN81_H_ */

