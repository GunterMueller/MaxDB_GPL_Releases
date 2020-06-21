/*! 
  \file    DBMSrvBHist_HistoryFile.hpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   a class for reading the backup history file

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_File.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp"
#include "SAPDB/ToolsCommon/Tools_List.hpp"


//-----------------------------------------------------------------------------
// class DBMSrvBHist_HistoryFile
//-----------------------------------------------------------------------------

class DBMSrvBHist_HistoryFile: public DBMSrvBHist_File
{
  public:
    DBMSrvBHist_HistoryFile();
    ~DBMSrvBHist_HistoryFile();

    Tools_List<DBMSrvBHist_Backup *> & getHistoryList();

  private:
    tcn00_Error useReadLine(DBMSrv_Reply & Reply);

    Tools_List<DBMSrvBHist_Backup *> historyList;
};
