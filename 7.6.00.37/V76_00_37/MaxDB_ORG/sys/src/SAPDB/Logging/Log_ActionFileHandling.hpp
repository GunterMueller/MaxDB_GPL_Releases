/*!
  @file           Log_ActionFileHandling.hpp
  @author         UweH
  @ingroup        ActionHandling
  @brief          defines tools for file handling

\if EMIT_LICENCE

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



\endif
*/
#ifndef LOG_ACTION_FILE_HANDLING_H
#define LOG_ACTION_FILE_HANDLING_H

#include "ggg00.h"
#include "hbd01_1.h"

/// a pointer to a initialized RedoFileId or the pointer pCommandFile
inline tgg00_FileId *Log_GetFileId (const tgg00_Filename &Filename,
                                    tgg00_FileId         &RedoFileId,
                                    tgg00_FileId         *pCommandFileId)
{
    if (NULL != pCommandFileId)
    {
        return (pCommandFileId);
    }
    
    RedoFileId = b01niltree_id;
    
    RedoFileId.fileName_gg00() = Filename;
    RedoFileId.fileType_gg00().clear();
    RedoFileId.fileType_gg00().addElement (ftsConcurrent_egg00);
    
    if (tfnColumn_egg00 == Filename[0])
    {
        RedoFileId.fileType_gg00().addElement (ftsByteStr_egg00);
    }
    else
    {
        RedoFileId.fileType_gg00().addElement (ftsPerm_egg00);
    }
    return (&RedoFileId);
}

#endif  /* LOG_ACTION_FILE_HANDLING_H */
