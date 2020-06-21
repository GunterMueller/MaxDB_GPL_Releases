/*!
  \file    DBMSrvBMedia_MediaGroup.cpp
  \author  TiloH
  \ingroup backup media handling by the DBMServer
  \brief   declaring a class for handling backup media

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


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_MediaGroup.hpp"


// -----------------------------------------------------------------------------
// member functions of class DBMSrvBMedia_MediaGroup
// -----------------------------------------------------------------------------

DBMSrvBTools_ToolEnum DBMSrvBMedia_MediaGroup::BackupTool(const tcn003_Medium     * pMedium,
                                                          const tcn003_MediaGroup * pMediaGroup)
{
    const char * ExternProperty="";

    if(0!=pMedium)  //only one medium
        ExternProperty=pMedium->szExternal;
    else
        if(0!=pMediaGroup && 0!=pMediaGroup->pFirst) //we have a group of media with at least one element
            ExternProperty=pMediaGroup->pFirst->szExternal;
    
    return DBMSrvBTools_ToolEnum(ExternProperty);
}

int DBMSrvBMedia_MediaGroup::NeedsBackupTool(const tcn003_Medium     * pMedium,
                                             const tcn003_MediaGroup * pMediaGroup)
{
    return (DBMSrvBTools_ToolEnum::None!=BackupTool(pMedium, pMediaGroup));
}
