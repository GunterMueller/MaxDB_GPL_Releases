/*******************************************************************************
  module:       hcn382.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  function for handling Backint for Oracle
  last change:  2000-11-29 11:41
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

#if !defined (HCN382_H)
#define HCN382_H

#include "hcn38.h"
#include "hcn381.h"
#include "hcn386.h"

int cn382_UseBackint(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                     const char *                     PathOfBackint,
                     const char *                     UserID,
                     const char *                     FunctionString,
                           int                        ForPipes,
                           int                        MaxNumberOfFilesPerBackintCall,
                     const char *                     PathOfBackintParamFile,
                     const char *                     InFileName,
                     const char *                     OutFileName,
                     const char *                     ErrFileName,
                     tcn386_BackintOut              & Output,
                     const char *                     BI_CALLERValue);

#endif
