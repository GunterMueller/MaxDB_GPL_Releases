/*! 
  \file    DBMSrvBHist_FileLineEBF.hpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   a class determining the layout of a line of dbm.ebf

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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


#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp"
#include "SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "SAPDB/ToolsCommon/Tools_List.hpp"


class DBMSrvBHist_FileLineEBF
{
  public:
    DBMSrvBHist_FileLineEBF(const DBMSrvBHist_Part & part);
    ~DBMSrvBHist_FileLineEBF();

    const char * asString();

  private:
    char * m_line;
};
