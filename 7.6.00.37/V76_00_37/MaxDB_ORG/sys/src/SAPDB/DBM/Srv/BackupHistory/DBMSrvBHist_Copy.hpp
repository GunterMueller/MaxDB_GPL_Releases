/*!
  \file    DBMSrvBHist_Copy.hpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   a class for managing the information of one copy of a backup

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


#if !defined (_DBMSRVBHIST_COPY_HPP_)
#define _DBMSRVBHIST_COPY_HPP_

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp"
#include "gcn003.h"


//-----------------------------------------------------------------------------
// class DBMSrvBHist_Copy
//-----------------------------------------------------------------------------

class DBMSrvBHist_Copy
{
  public:
    DBMSrvBHist_Copy();
    ~DBMSrvBHist_Copy();

    tcn00_Error AddPart(DBMSrvBHist_Part * Part);
    tcn00_Error AddMatchingPart(DBMSrvBHist_Part * Part, int & Matched);
    tcn00_Error CheckPart(DBMSrvBHist_Part * Part, int & Matched);

    bool PrintMediaAndEBIDsInto(char                                    *& Buffer,
                                size_t                                   & MaxUsableBufferSize,
                                bool                                       WithMedia,
                                bool                                       WithEBIDs,
                                const DBMSrvBHist_PartMediaColumnWidths  & ColumnWidths);
    void EnlargeColumnWidths(DBMSrvBHist_PartMediaColumnWidths & ColumnWidths);

    int MatchesFileNameElseReduce(const char * AbsoluteName, const char * DefaultDirectory);
    int ContainsFileBackup(const char *  BaseName);

    void MarkUsedExternalBackupTools(DBMSrvBTools_ToolEnum * BackupToolType);

    int          DBMReturnCode();
    const char * DBMReturnText();

    int         IsAvailable() const;
    int         IsAvailableExtended() const;
    tcn00_Error IsAvailableExtended(const char * DefaultDirectory, int & Available) const;
    char *      ConstructEBIDString() const;
    tcn00_Error ConstructMedium(tcn003_Medium *& Medium, tcn003_MediaGroup *& MediaGroup, const char * TheBackupType) const;

  private:
    Tools_List<DBMSrvBHist_Part *> Parts;

    char * InternalCopyID;
};

#endif
