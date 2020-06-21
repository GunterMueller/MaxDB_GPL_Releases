/*!
  \file    DBMSrvCmdMedium_Headers.hpp
  \author  TiloH
  \ingroup DBM Server commands
  \brief   includes of all headers from DBM/Srv/Commands/Medium/

           This header file can be used to avoid limitations of the maximal
           number of includes per file, that is allowed by our build tools

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#if !defined(DBMSRVCMDMEDIUM_HEADERS_HPP)
#define DBMSRVCMDMEDIUM_HEADERS_HPP

#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumLabelOffline.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp"

#endif
