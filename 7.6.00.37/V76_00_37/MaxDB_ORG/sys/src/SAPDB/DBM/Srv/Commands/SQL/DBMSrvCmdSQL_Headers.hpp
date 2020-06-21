/*!
  \file    DBMSrvCmdSQL_Header.hpp
  \author  BerndV
  \ingroup SQL Kernel Access area
  \brief   This contains all headers from the SQL directory to avoid 
           "to long description line" errors

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

#if !defined(DBMSrvCmdSQL_Header_hpp)
#define DBMSrvCmdSQL_Header_hpp

#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlConnect.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlExecute.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetch.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlExecuteNice.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetchNice.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlInfo.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlRelease.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestat.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp"

#endif // DBMSrvCmdSQL_Header_hpp
