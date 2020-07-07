/*! 
  -----------------------------------------------------------------------------
 
  module: hcn32.h
 
  -----------------------------------------------------------------------------
 
  responsible:  TiloH
  
  description:  basic module for backup history

  Copyright (c) 2000-2005 SAP AG

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

#ifndef _HCN32_H_
#define _HCN32_H_


// ------------------------------------------------------------------
// includes
// ------------------------------------------------------------------

#include "gcn00.h"
#include "gcn003.h"


// ------------------------------------------------------------------
// specification public functions
// ------------------------------------------------------------------

tcn00_Error cn32GetEBIDs(VControlDataT * vcontrol,
                         CommandT      * command,
                         char          * replyData,
                         int           * replyLen,
                         int             replyLenMax);

extern const char * const LongHelp_GetEBIDS_cn32;

tcn00_Error cn32GetEBIDsFromTool(VControlDataT * vcontrol,
                                 CommandT      * command,
                                 char          * replyData,
                                 int           * replyLen,
                                 int             replyLenMax);

extern const char * const LongHelp_GetEBIDSFromTool_cn32;

tcn00_Error cn32ListNextEBIDs(VControlDataT * vcontrol,
                              CommandT      * command,
                              char          * replyData,
                              int           * replyLen,
                              int             replyLenMax);

tcn00_Error cn32ForgetEBIDs(VControlDataT * vcontrol,
                            CommandT      * command,
                            char          * replyData,
                            int           * replyLen,
                            int             replyLenMax);

void cn32FreeEBIDList(VControlDataT * vcontrol);

#endif /* _HCN32_H_ */
