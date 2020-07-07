/*!
  @file    Msg_OutputUtilProt.h
  @author  Robin
  @ingroup Common
  @brief   knldiag Message output 
  
  
  -------------------------------------------------------------------------

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

#ifndef MSG_OUTPUTUTILPROT_H
#define MSG_OUTPUTUTILPROT_H

#include "SAPDBCommon/SAPDB_C_Cplusplus.h"

void Msg_OutputToUtilProt(const SAPDB_Char *pFormatString,SAPDB_UInt length);

#endif /* MSG_OUTPUTKNLDIAG_H */
