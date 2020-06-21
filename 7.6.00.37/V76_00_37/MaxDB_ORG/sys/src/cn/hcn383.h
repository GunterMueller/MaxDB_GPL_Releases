/*!*****************************************************************************
  module:       hcn383.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  functions for handling history file of Backint for SAP DB
  last change:  2000-11-29 11:45
  version:      7.2.5
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2000-2005 SAP AG


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



*******************************************************************************/

#if !defined HCN383_H
#define HCN383_H

#include "hcn38.h"
#include "hcn381.h"
#include "hcn386.h"

int cn383_WriteInformationToHistoryFile(const char                     * HistoryFileName,
                                        ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                        const char                     * UserID);

int cn383_GetInformationFromOwnHistory(const char                     * PathOfHistoryFile,
                                       ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                       const char                     * UserID,
                                       const char                     * FunctionString,
                                       ni31_List<tcn381_InputEntry *> * BackUpsInQuestionList,
                                       tcn386_BackintOut              & Output);

#endif
