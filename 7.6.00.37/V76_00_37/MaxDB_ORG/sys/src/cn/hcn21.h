/*
    * responsible:   BerndV
    *
    * purpose:       - read and analyze old XParam help file
    *                - read and analyze new XParam help file

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

*/

#ifndef _HCN21_H_
#define _HCN21_H_

/* ------------------------------------------------------------------
 * INCLUDES
 * ------------------------------------------------------------------
 */
#include "gcn00.h"
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

/* ------------------------------------------------------------------
 * SPECIFICATION PUBLIC FUNCTION cn21ReadHelpFile
 * ------------------------------------------------------------------
 * purpose: - open the XParam help file (flag for old or new)
 *          - call processing of file contents
 *          - close the file
 */
tcn00_Error cn21ReadHelpFile (
      DBMSrvPar_Parameters * pXParamData,
      char                 * replyData,
      int                  * replyLen,
      char                 * szFile,
      bool                   bNewSyntax );

#endif /* _HCN21_H_ */

