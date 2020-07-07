/*!
  \file    DBMSrvCmdB_BackupLabelOffline.hpp
  \author  TiloH
  \ingroup DBM Server commands
  \brief   definition of a classes handling DBM Server
           command backup_labeloffline

\if EMIT_LICENCE

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


\endif
*/

#if !defined(DBMSRVCMDB_BACKUPLABELOFFLINE_HPP)
#define DBMSRVCMDB_BACKUPLABELOFFLINE_HPP

#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumLabelOffline.hpp"

class DBMSrvCmdB_BackupLabelOffline: public DBMSrvCmdMedium_MediumLabelOffline
{
  public:
    DBMSrvCmdB_BackupLabelOffline();

    static const char * getCommandName() {return "backup_labeloffline";}
};

#endif
