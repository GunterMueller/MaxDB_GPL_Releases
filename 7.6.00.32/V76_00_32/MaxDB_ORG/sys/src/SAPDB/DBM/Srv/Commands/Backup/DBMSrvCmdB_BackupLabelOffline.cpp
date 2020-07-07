/*!
  \file    DBMSrvCmdB_BackupLabelOffline.cpp
  \author  TiloH
  \ingroup DBM Server commands
  \brief   implementation of a classes handling DBM Server
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

#include "DBM/Srv/Commands/Backup/DBMSrvCmdB_BackupLabelOffline.hpp"

DBMSrvCmdB_BackupLabelOffline::DBMSrvCmdB_BackupLabelOffline()
  : DBMSrvCmdMedium_MediumLabelOffline() // this command is the obsolete version of medium_labeloffline
{
    m_isObsolete=true;
    m_longHelp=
        "@command backup_labeloffline This is an obsolete version of command backup_labeloffline."
        "@syntax backup_labeloffline (obsolete version of medium_labeloffline)";
}
